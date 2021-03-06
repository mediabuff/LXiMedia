/******************************************************************************
 *   Copyright (C) 2015  A.J. Admiraal                                        *
 *   code@admiraal.dds.nl                                                     *
 *                                                                            *
 *   This program is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License version 3 as        *
 *   published by the Free Software Foundation.                               *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 ******************************************************************************/

#include <upnp/upnp.h> // Include first to make sure off_t is the correct size.
#include "upnp.h"
#include "platform/string.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

namespace pupnp {

const char  upnp::mime_audio_ac3[]          = "audio/x-ac3";
const char  upnp::mime_audio_lpcm_48000_2[] = "audio/L16; rate=48000; channels=2";
const char  upnp::mime_audio_mp3[]          = "audio/mp3";
const char  upnp::mime_audio_mpeg[]         = "audio/mpeg";
const char  upnp::mime_image_jpeg[]         = "image/jpeg";
const char  upnp::mime_image_png[]          = "image/png";
const char  upnp::mime_image_svg[]          = "image/svg+xml";
const char  upnp::mime_video_mpeg[]         = "video/mpeg";
const char  upnp::mime_video_mpegm2ts[]     = "video/vnd.dlna.mpeg-tts";
const char  upnp::mime_video_mpegts[]       = "video/x-mpegts";
const char  upnp::mime_text_css_utf8[]      = "text/css; charset=utf-8";
const char  upnp::mime_text_html_utf8[]     = "text/html; charset=utf-8";
const char  upnp::mime_text_xml_utf8[]      = "text/xml; charset=utf-8";

upnp * upnp::me = nullptr;

std::string upnp::hostname()
{
    char buffer[64] = { '\0' };

    // On Windows this will ensure Winsock is initialized properly.
    if (::UpnpGetAvailableIpAddresses() != nullptr)
        gethostname(buffer, sizeof(buffer) - 1);

    return buffer;
}

upnp::upnp(class platform::messageloop_ref &messageloop)
    : messageloop(messageloop),
      basedir("/upnp/"),
      update_interfaces_timer(messageloop, std::bind(&upnp::update_interfaces, this)),
      update_interfaces_interval(10),
      clear_responses_timer(messageloop, std::bind(&upnp::clear_responses, this)),
      clear_responses_interval(15)
{
    assert(me == nullptr);

    me = this;
    port = 0;
    bind_public = false;
    initialized = false;
    webserver_enabled = false;
}

upnp::~upnp()
{
    upnp::close();

    assert(me != nullptr);
    me = nullptr;
}

const std::string &upnp::http_basedir() const
{
    return basedir;
}

void upnp::child_add(struct child &child)
{
    children.insert(&child);
    if (initialized)
    {
        messageloop.post([this, &child]
        {
            if (children.find(&child) != children.end())
                child.initialize();
        });
    }
}

void upnp::child_remove(struct child &child)
{
    children.erase(&child);
}

void upnp::http_callback_register(const std::string &path, const http_callback &callback)
{
    if (initialized && !webserver_enabled)
        enable_webserver();

    std::string p = path;
    if (!p.empty() && (p[p.length() - 1] != '/'))
        p += '/';

    if (http_callbacks.find(p) == http_callbacks.end())
    {
        http_callbacks[p] = callback;
        if (initialized)
            ::UpnpAddVirtualDir(p.c_str());
    }
}

void upnp::http_callback_unregister(const std::string &path)
{
    std::string p = path;
    if (!p.empty() && (p[p.length() - 1] != '/'))
        p += '/';

    auto i = http_callbacks.find(p);
    if (i != http_callbacks.end())
        http_callbacks.erase(i);
}

bool upnp::initialize(uint16_t port, bool bind_public)
{
    if (!initialized || ((this->port != port) && (port != 0)) || (this->bind_public != bind_public))
    {
        if (initialized)
            close();

        this->port = port;
        this->bind_public = bind_public;

        std::vector<const char *> addresses;
        for (char **i = ::UpnpGetAvailableIpAddresses(); i && *i; i++)
        {
            available_address_set.insert(*i);
            if (bind_public || !is_public_address(*i))
            {
                bound_address_set.insert(*i);
                addresses.push_back(*i);
            }
        }

        addresses.push_back(nullptr);
        int result = ::UpnpInit3(&(addresses[0]), port);
        initialized = result == UPNP_E_SUCCESS;
        if (!initialized)
            initialized = (result = ::UpnpInit3(&(addresses[0]), 0)) == UPNP_E_SUCCESS;

        if (initialized)
        {
            if (!http_callbacks.empty())
                enable_webserver();

            for (auto i = http_callbacks.begin(); i != http_callbacks.end(); i++)
                ::UpnpAddVirtualDir(i->first.c_str());
        }
        else
            std::cerr << "pupnp::upnp: failed to initialize libupnp:" << result << std::endl;

        update_interfaces_timer.start(update_interfaces_interval);

        if (initialized)
            for (auto *child : children)
                initialized &= child->initialize();
    }

    return initialized;
}

void upnp::close(void)
{
    update_interfaces_timer.stop();

    for (auto *child : children)
        child->close();

    if (initialized)
    {
        initialized = false;

        if (webserver_enabled)
        {
            webserver_enabled = false;
            ::UpnpRemoveAllVirtualDirs();
        }

        // Ugly, but needed as UpnpFinish waits for callbacks from the HTTP server.
        bool finished = false;
        messageloop.process_events(std::chrono::milliseconds(16));
        std::thread finish([&finished] { ::UpnpFinish(); finished = true; });
        do messageloop.process_events(std::chrono::milliseconds(16)); while (!finished);
        finish.join();

        clear_responses();
    }
}

bool upnp::is_private_address(const char *address)
{
    return
            (strncmp(address, "10.", 3) == 0) ||
            (strncmp(address, "172.16.", 7) == 0) ||
            (strncmp(address, "172.17.", 7) == 0) ||
            (strncmp(address, "172.18.", 7) == 0) ||
            (strncmp(address, "172.19.", 7) == 0) ||
            (strncmp(address, "172.20.", 7) == 0) ||
            (strncmp(address, "172.21.", 7) == 0) ||
            (strncmp(address, "172.22.", 7) == 0) ||
            (strncmp(address, "172.23.", 7) == 0) ||
            (strncmp(address, "172.24.", 7) == 0) ||
            (strncmp(address, "172.25.", 7) == 0) ||
            (strncmp(address, "172.26.", 7) == 0) ||
            (strncmp(address, "172.27.", 7) == 0) ||
            (strncmp(address, "172.28.", 7) == 0) ||
            (strncmp(address, "172.29.", 7) == 0) ||
            (strncmp(address, "172.30.", 7) == 0) ||
            (strncmp(address, "172.31.", 7) == 0) ||
            (strncmp(address, "192.168.", 8) == 0);
}

bool upnp::is_loopback_address(const char *address)
{
    return (strncmp(address, "127.", 4) == 0);
}

bool upnp::is_auto_address(const char *address)
{
    return (strncmp(address, "169.254.", 8) == 0);
}

bool upnp::is_public_address(const char *address)
{
    return !is_private_address(address) && !is_loopback_address(address) && !is_auto_address(address);
}

bool upnp::is_my_address(const std::string &address) const
{
    const size_t colon = address.find_first_of(':');

    return available_address_set.find((colon != address.npos) ? address.substr(0, colon) : address) != available_address_set.end();
}

const std::set<std::string> & upnp::bound_addresses() const
{
    return bound_address_set;
}

uint16_t upnp::bound_port() const
{
    return ::UpnpGetServerPort();
}

int upnp::handle_http_request(const struct request &request, std::string &content_type, std::shared_ptr<std::istream> &response)
{
    for (std::string path = request.url.path;;)
    {
        auto i = http_callbacks.find(path);
        if (i != http_callbacks.end())
            return i->second(request, content_type, response);

        const size_t slash = path.find_last_of('/', path.length() - 2);
        if (slash != path.npos)
            path = path.substr(0, slash + 1);
        else
            break;
    }

    return http_not_found;
}

void upnp::clear_responses()
{
    std::lock_guard<std::mutex> _(responses_mutex);

    responses.clear();
}

void upnp::update_interfaces()
{
    if (handles.empty())
        for (char **i = ::UpnpGetAvailableIpAddresses(); i && *i; i++)
            if (available_address_set.find(*i) == available_address_set.end())
                if (bind_public || !is_public_address(*i))
                {
                    std::clog << "pupnp::upnp: found new network interface: " << *i << std::endl;
                    close();
                    initialize(port, bind_public);
                    break;
                }
}

void upnp::enable_webserver()
{
    struct T
    {
        static int get_info(::Request_Info *request_info, const char *url, ::File_Info *info)
        {
            struct request request;
            request.user_agent = request_info->userAgent;
            request.source_address = request_info->sourceAddress;
            request.url = upnp::url("http://" + std::string(request_info->host) + url);

            std::string content_type;
            std::shared_ptr<std::istream> response;
            if (me->get_response(request, content_type, response, false) != http_not_found)
            {
                info->file_length = -1;
                if (response)
                {
                    auto i = response->tellg();
                    if ((i != decltype(i)(-1)) && response->seekg(0, std::ios_base::end))
                    {
                        info->file_length = response->tellg();
                        response->seekg(i);
                    }

                    response->clear();
                }

                info->last_modified = 0;
                info->is_directory = FALSE;
                info->is_readable = TRUE;
                info->is_cacheable = FALSE;
                info->content_type = ::ixmlCloneDOMString(content_type.c_str());

                return 0;
            }

            std::clog << "pupnp::upnp: webserver get_info(\"" << url << "\") not found" << std::endl;
            return -1;
        }

        static ::UpnpWebFileHandle open(::Request_Info *request_info, const char *url, ::UpnpOpenFileMode mode)
        {
            struct request request;
            request.user_agent = request_info->userAgent;
            request.source_address = request_info->sourceAddress;
            request.url = upnp::url("http://" + std::string(request_info->host) + url);

            std::string content_type;
            std::shared_ptr<std::istream> response;
            if (me->get_response(request, content_type, response, true) == http_ok)
            {
                me->messageloop.send([&response]
                {
                    me->handles[response.get()] = response;
                });

                return response.get();
            }

            std::clog << "pupnp::upnp: webserver open(\"" << url << "\") failed" << std::endl;
            return nullptr;
        }

        static int read(::UpnpWebFileHandle fileHnd, char *buf, size_t len)
        {
            if (fileHnd)
            {
                std::istream * const stream = reinterpret_cast<std::istream *>(fileHnd);
                stream->read(buf, len);
                return std::max(int(stream->gcount()), 0);
            }

            return UPNP_E_INVALID_HANDLE;
        }

        static int write(::UpnpWebFileHandle /*fileHnd*/, char */*buf*/, size_t /*len*/)
        {
            return UPNP_E_INVALID_HANDLE;
        }

        static int seek(::UpnpWebFileHandle fileHnd, off_t offset, int origin)
        {
            if (fileHnd)
            {
                std::istream * const stream = reinterpret_cast<std::istream *>(fileHnd);
                switch (origin)
                {
                case SEEK_SET:  return stream->seekg(offset, std::ios_base::beg) ? 0 : -1;
                case SEEK_CUR:  return stream->seekg(offset, std::ios_base::cur) ? 0 : -1;
                case SEEK_END:  return stream->seekg(offset, std::ios_base::end) ? 0 : -1;
                default:        return -1;
                }
            }

            return -1;
        }

        static int close(::UpnpWebFileHandle fileHnd)
        {
            if (fileHnd)
            {
                me->messageloop.post([fileHnd]
                {
                    auto i = me->handles.find(fileHnd);
                    if (i != me->handles.end())
                        me->handles.erase(i);
                });

                return UPNP_E_SUCCESS;
            }

            return UPNP_E_INVALID_HANDLE;
        }
    };

    const int rc = ::UpnpEnableWebserver(TRUE);
    if (rc == UPNP_E_SUCCESS)
    {
        static const ::UpnpVirtualDirCallbacks callbacks = { &T::get_info, &T::open, &T::read, &T::write, &T::seek, &T::close };
        const int rc = ::UpnpSetVirtualDirCallbacks(const_cast< ::UpnpVirtualDirCallbacks * >(&callbacks));
        if (rc == UPNP_E_SUCCESS)
            webserver_enabled = true;
        else
            std::cerr << "pupnp::upnp: UpnpSetVirtualDirCallbacks failed:" << rc << std::endl;
    }
    else
        std::cerr << "pupnp::upnp: UpnpEnableWebserver() failed:" << rc << std::endl;
}

int upnp::get_response(const struct request &request, std::string &content_type, std::shared_ptr<std::istream> &stream, bool erase)
{
    {
        std::lock_guard<std::mutex> _(responses_mutex);

        auto i = responses.find(request.url.path);
        if (i != responses.end())
        {
            content_type = i->second.type;
            stream = i->second.stream;
            if (erase)
                responses.erase(i);

            return http_ok;
        }
    }

    int result = 0;
    messageloop.send([this, &request, &content_type, &stream, erase, &result]
    {
        if (initialized)
        {
            result = handle_http_request(request, content_type, stream);
            if (stream && !erase && (result == http_ok))
                clear_responses_timer.start(clear_responses_interval, true);
        }
        else
            result = http_internal_server_error;
    });

    if (!erase && (result != http_not_found))
    {
        std::lock_guard<std::mutex> _(responses_mutex);
        responses[request.url.path] = response { content_type, stream };
    }

    return result;
}


upnp::url::url()
{
}

upnp::url::url(const std::string &url)
{
    size_t s = 0;
    if (starts_with(url, "http://"))
    {
        s = url.find_first_of('/', 7);
        if (s != url.npos)
            host = url.substr(7, s - 7);
        else
            s = 7;
    }

    const size_t q = url.find_first_of('?', s);
    if (q != url.npos)
    {
        path = url.substr(s, q - s);

        for (size_t i = q; i != url.npos; )
        {
            const size_t n = url.find_first_of('&', i + 1);
            const size_t e = url.find_first_of('=', i + 1);
            if (e != url.npos)
                query[url.substr(i + 1, e - i - 1)] = url.substr(e + 1, n - e - 1);
            else
                query[url.substr(i + 1, n - i - 1)] = std::string();

            i = n;
        }
    }
    else
        path = url.substr(s);
}

upnp::url::operator std::string() const
{
    std::string result = "http://" + host + path;

    char querysep = '?';
    for (auto &i : query)
    {
        result += querysep + i.first;
        if (!i.second.empty())
            result += '=' + i.second;

        querysep = '&';
    }

    return result;
}

} // End of namespace
