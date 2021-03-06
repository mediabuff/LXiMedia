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

#include "settings.h"
#include "platform/fstream.h"
#include "platform/path.h"
#include "platform/string.h"
#include "pupnp/client.h"
#include "vlc/instance.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <sstream>
#include <thread>

using std::chrono::duration_cast;

struct user_dirs { std::string download, music, pictures, videos; };
static struct user_dirs user_dirs();

static const char clean_exit_name[] = "_clean_exit";

settings::settings(class platform::messageloop_ref &messageloop, bool read_only)
    : messageloop(messageloop),
      read_only(read_only),
      inifile(platform::config_dir() + "/settings", read_only),
      general(inifile.open_section()),
      codecs(inifile.open_section("codecs")),
      formats(inifile.open_section("formats")),
      paths(inifile.open_section("paths")),
      timer(messageloop, std::bind(&settings::save, this)),
      save_delay(250),
      last_clean_exit(general.read(clean_exit_name, true))
{
    if (!read_only)
    {
        inifile.on_touched = [this] { timer.start(save_delay, true); };
        general.write(clean_exit_name, false);
    }
}

settings::~settings()
{
    if (!read_only)
        general.erase(clean_exit_name);
}

void settings::save()
{
    if (!read_only)
    {
        timer.stop();
        inifile.save();
    }
}

bool settings::was_clean_exit() const
{
    return last_clean_exit;
}

static const char is_configure_required_name[] = "is_configure_required";

bool settings::is_configure_required() const
{
    return general.read(is_configure_required_name, true);
}

void settings::set_configure_required(bool on)
{
    assert(!read_only);

    if (on)
        return general.erase(is_configure_required_name);
    else
        return general.write(is_configure_required_name, false);
}

static const char last_version_check_name[] = "last_version_check";
static const char latest_version_name[] = "latest_version";

std::string settings::latest_version()
{
    const auto now = duration_cast<std::chrono::hours>(
                std::chrono::system_clock::now().time_since_epoch()).count();

    auto last_version_check = general.read(last_version_check_name, 0);
    if ((now - last_version_check) >= 168)
    {
        std::ostringstream url;
        url << "http://www.admiraal.dds.nl/lximediaserver/version.php"
            << "?uuid=" << uuid()
            << "&version=" << VERSION
            << "&platform=" << PLATFORM;

        auto latest_version = pupnp::client::get(url.str());
        while (!latest_version.empty() && std::isspace(latest_version.back()))
            latest_version.pop_back();

        if (!latest_version.empty() && (latest_version.length() <= 8))
        {
            general.write(last_version_check_name, now);
            general.write(latest_version_name, latest_version);

            return latest_version;
        }
    }

    return general.read(latest_version_name, VERSION);
}

static const char uuid_name[] = "uuid";

platform::uuid settings::uuid()
{
    auto value = general.read(uuid_name);
    if (value.empty() && !read_only)
    {
        value = platform::uuid::generate();
        general.write(uuid_name, value);
    }

    return value;
}

static const char upnp_devicename_name[] = "upnp_devicename";

static std::string default_upnp_devicename();

std::string settings::upnp_devicename() const
{
    return general.read(upnp_devicename_name, default_upnp_devicename());
}

void settings::set_upnp_devicename(const std::string &upnp_devicename)
{
    assert(!read_only);

    if (upnp_devicename != default_upnp_devicename())
        return general.write(upnp_devicename_name, upnp_devicename);
    else
        return general.erase(upnp_devicename_name);
}

static const char http_port_name[] = "http_port";

static const uint16_t default_http_port = 4280;

uint16_t settings::http_port() const
{
    return uint16_t(general.read(http_port_name, default_http_port));
}

void settings::set_http_port(uint16_t http_port)
{
    assert(!read_only);

    if (http_port != default_http_port)
        return general.write(http_port_name, http_port);
    else
        return general.erase(http_port_name);
}

static const char bind_all_networks_name[] = "bind_all_networks";

bool settings::bind_all_networks() const
{
    return general.read(bind_all_networks_name, false);
}

void settings::set_bind_all_networks(bool on)
{
    assert(!read_only);

    if (on)
        return general.write(bind_all_networks_name, true);
    else
        return general.erase(bind_all_networks_name);
}

static const char republish_rootdevice_name[] = "republish_rootdevice";

bool settings::republish_rootdevice() const
{
    return general.read(republish_rootdevice_name, true);
}

void settings::set_republish_rootdevice(bool on)
{
    assert(!read_only);

    if (on)
        return general.erase(republish_rootdevice_name);
    else
        return general.write(republish_rootdevice_name, false);
}

static const char encode_mode_name[] = "encode_mode";

static const char * to_string(encode_mode e)
{
    switch (e)
    {
    case encode_mode::fast: return "fast";
    case encode_mode::slow: return "slow";
    }

    assert(false);
    return nullptr;
}

static encode_mode to_encode_mode(const std::string &e)
{
    if      (e == to_string(encode_mode::fast))   return encode_mode::fast;
    else if (e == to_string(encode_mode::slow))   return encode_mode::slow;

    assert(false);
    return encode_mode::slow;
}

static const enum encode_mode default_encode_mode = encode_mode::fast;

enum encode_mode settings::encode_mode() const
{
    return to_encode_mode(general.read(encode_mode_name, to_string(default_encode_mode)));
}

void settings::set_encode_mode(enum encode_mode encode_mode)
{
    assert(!read_only);

    if (encode_mode != default_encode_mode)
        return general.write(encode_mode_name, to_string(encode_mode));
    else
        return general.erase(encode_mode_name);
}

static const char video_mode_name[] = "video_mode";

static const char * to_string(video_mode e)
{
    switch (e)
    {
    case video_mode::auto_      : return "auto";
    case video_mode::vcd        : return "vcd";
    case video_mode::dvd        : return "dvd";
    case video_mode::hdtv_720   : return "720p";
    case video_mode::hdtv_1080  : return "1080p";
    }

    assert(false);
    return nullptr;
}

static video_mode to_video_mode(const std::string &e)
{
    if      (e == to_string(video_mode::auto_))     return video_mode::auto_;
    else if (e == to_string(video_mode::vcd))       return video_mode::vcd;
    else if (e == to_string(video_mode::dvd))       return video_mode::dvd;
    else if (e == to_string(video_mode::hdtv_720))  return video_mode::hdtv_720;
    else if (e == to_string(video_mode::hdtv_1080)) return video_mode::hdtv_1080;

    assert(false);
    return video_mode::auto_;
}

static const enum video_mode default_video_mode = video_mode::dvd;

enum video_mode settings::video_mode() const
{
    return to_video_mode(general.read(video_mode_name, to_string(default_video_mode)));
}

void settings::set_video_mode(enum video_mode video_mode)
{
    assert(!read_only);

    if (video_mode != default_video_mode)
        return general.write(video_mode_name, to_string(video_mode));
    else
        return general.erase(video_mode_name);
}

static const char canvas_mode_name[] = "canvas_mode";

static const char * to_string(canvas_mode e)
{
    switch (e)
    {
    case canvas_mode::none  : return "none";
    case canvas_mode::pad   : return "pad";
    }

    assert(false);
    return nullptr;
}

static canvas_mode to_canvas_mode(const std::string &e)
{
    if      (e == to_string(canvas_mode::none)) return canvas_mode::none;
    else if (e == to_string(canvas_mode::pad))  return canvas_mode::pad;

    assert(false);
    return canvas_mode::none;
}

bool settings::canvas_mode_enabled() const
{
    // Workaround for ticket https://trac.videolan.org/vlc/ticket/10148
    return compare_version(vlc::instance::version(), "2.1") != 0;
}

static enum canvas_mode default_canvas_mode = canvas_mode::pad;

enum canvas_mode settings::canvas_mode() const
{
    if (!canvas_mode_enabled())
        return ::canvas_mode::none;

    return to_canvas_mode(general.read(canvas_mode_name, to_string(default_canvas_mode)));
}

void settings::set_canvas_mode(enum canvas_mode canvas_mode)
{
    assert(!read_only);

    if (canvas_mode != default_canvas_mode)
        return general.write(canvas_mode_name, to_string(canvas_mode));
    else
        return general.erase(canvas_mode_name);
}

static const char surround_mode_name[] = "surround_mode";

static const char * to_string(surround_mode e)
{
    switch (e)
    {
    case surround_mode::stereo      : return "Stereo";
    case surround_mode::surround51  : return "5.1";
    }

    assert(false);
    return nullptr;
}

static surround_mode to_surround_mode(const std::string &e)
{
    if      (e == to_string(surround_mode::stereo))     return surround_mode::stereo;
    else if (e == to_string(surround_mode::surround51)) return surround_mode::surround51;

    assert(false);
    return surround_mode::stereo;
}

bool settings::surround_mode_enabled() const
{
    // Workaround for ticket https://trac.videolan.org/vlc/ticket/1897
    return compare_version(vlc::instance::version(), "2.2") >= 0;
}

static enum surround_mode default_surround_mode = surround_mode::stereo;

enum surround_mode settings::surround_mode() const
{
    if (!surround_mode_enabled())
        return ::surround_mode::stereo;

    return to_surround_mode(general.read(surround_mode_name, to_string(default_surround_mode)));
}

void settings::set_surround_mode(enum surround_mode surround_mode)
{
    assert(!read_only);

    if (surround_mode != default_surround_mode)
        return general.write(surround_mode_name, to_string(surround_mode));
    else
        return general.erase(surround_mode_name);
}

static const char font_size_name[] = "font_size";

static const char * to_string(font_size e)
{
    switch (e)
    {
    case font_size::small   : return "small";
    case font_size::normal  : return "normal";
    case font_size::large   : return "large";
    }

    assert(false);
    return nullptr;
}

static font_size to_font_size(const std::string &e)
{
    if      (e == to_string(font_size::small))  return font_size::small;
    else if (e == to_string(font_size::normal)) return font_size::normal;
    else if (e == to_string(font_size::large))  return font_size::large;

    assert(false);
    return font_size::normal;
}

static enum font_size default_font_size = font_size::normal;

enum font_size settings::font_size() const
{
    return to_font_size(general.read(font_size_name, to_string(default_font_size)));
}

void settings::set_font_size(enum font_size font_size)
{
    assert(!read_only);

    if (font_size != default_font_size)
        return general.write(font_size_name, to_string(font_size));
    else
        return general.erase(font_size_name);
}

static const char share_removable_media_name[] = "share_removable_media";

bool settings::share_removable_media() const
{
    return general.read(share_removable_media_name, true);
}

void settings::set_share_removable_media(bool on)
{
    assert(!read_only);

    if (!on)
        return general.write(share_removable_media_name, false);
    else
        return general.erase(share_removable_media_name);
}

static const char mp2v_name[] = "mp2v";

bool settings::mpeg2_enabled() const
{
    return codecs.read(mp2v_name, true);
}

void settings::set_mpeg2_enabled(bool on)
{
    assert(!read_only);

    if (on)
        return codecs.erase(mp2v_name);
    else
        return codecs.write(mp2v_name, false);
}

static const char h264_name[] = "h264";

static bool default_mpeg4_enabled()
{
    return std::thread::hardware_concurrency() > 3;
}

bool settings::mpeg4_enabled() const
{
    return codecs.read(h264_name, default_mpeg4_enabled());
}

void settings::set_mpeg4_enabled(bool on)
{
    assert(!read_only);

    if (on == default_mpeg4_enabled())
        return codecs.erase(h264_name);
    else
        return codecs.write(h264_name, on);
}

static const char mpeg_m2ts_name[] = "mpeg_m2ts";

bool settings::video_mpegm2ts_enabled() const
{
    return formats.read(mpeg_m2ts_name, true);
}

void settings::set_video_mpegm2ts_enabled(bool on)
{
    assert(!read_only);

    if (on)
        return formats.erase(mpeg_m2ts_name);
    else
        return formats.write(mpeg_m2ts_name, false);
}

static const char mpeg_ts_name[] = "mpeg_ts";

bool settings::video_mpegts_enabled() const
{
    return formats.read(mpeg_ts_name, true);
}

void settings::set_video_mpegts_enabled(bool on)
{
    assert(!read_only);

    if (on)
        return formats.erase(mpeg_ts_name);
    else
        return formats.write(mpeg_ts_name, false);
}

static const char mpeg_ps_name[] = "mpeg_ps";

bool settings::video_mpeg_enabled() const
{
    return formats.read(mpeg_ps_name, true);
}

void settings::set_video_mpeg_enabled(bool on)
{
    assert(!read_only);

    if (on)
        return formats.erase(mpeg_ps_name);
    else
        return formats.write(mpeg_ps_name, false);
}

static const char * to_string(path_type e)
{
    switch (e)
    {
    case path_type::auto_   : return "auto";
    case path_type::music   : return "music";
    case path_type::pictures: return "pictures";
    case path_type::videos  : return "videos";
    }

    assert(false);
    return nullptr;
}

static path_type to_path_type(const std::string &e)
{
    if      (e == to_string(path_type::auto_))      return path_type::auto_;
    else if (e == to_string(path_type::music))      return path_type::music;
    else if (e == to_string(path_type::pictures))   return path_type::pictures;
    else if (e == to_string(path_type::videos))     return path_type::videos;

    assert(false);
    return path_type::auto_;
}

static std::vector<root_path> default_root_paths()
{
    std::vector<root_path> result;

    const auto user_dirs = ::user_dirs();
    if (!user_dirs.download .empty()) result.emplace_back(root_path { path_type::auto_      , user_dirs.download  });
    if (!user_dirs.music    .empty()) result.emplace_back(root_path { path_type::music      , user_dirs.music     });
    if (!user_dirs.pictures .empty()) result.emplace_back(root_path { path_type::pictures   , user_dirs.pictures  });
    if (!user_dirs.videos   .empty()) result.emplace_back(root_path { path_type::videos     , user_dirs.videos    });

    return result;
}

std::vector<root_path> settings::root_paths() const
{
    const auto names = paths.names();
    if (!names.empty())
    {
        std::vector<root_path> result;
        for (auto &i : paths.names())
            result.emplace_back(root_path { to_path_type(paths.read(i, to_string(path_type::auto_))), i });

        return result;
    }

    return default_root_paths();
}

void settings::set_root_paths(const std::vector<root_path> &root_paths)
{
    assert(!read_only);

    const auto default_paths = default_root_paths();

    size_t defaults = 0;
    std::set<std::string> new_names;
    for (auto &i : root_paths)
        if (new_names.find(i.path) == new_names.end())
        {
            new_names.insert(i.path);
            paths.write(i.path, to_string(i.type));

            for (auto &j : default_paths)
                if ((j.type == i.type) && (j.path == i.path))
                {
                    defaults++;
                    break;
                }
        }

    if ((defaults == default_paths.size()) && (defaults == root_paths.size()))
        new_names.clear();

    for (auto &i : paths.names())
        if (new_names.find(i) == new_names.end())
            paths.erase(i);
}

#if defined(__unix__)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

static std::string unquote_and_replace_home(const std::string &src, const std::string &home)
{
    std::string result;
    const size_t fq = src.find_first_of('\"');
    const size_t sq = src.find_first_of('\"', fq + 1);
    if ((fq != src.npos) && (sq != src.npos))
        result = platform::clean_path(src.substr(fq + 1, sq - fq - 1));
    else
        result = platform::clean_path(src);

    const size_t h = result.find("$HOME");
    if (h != result.npos)
        result.replace(h, 5, home);

    if (!result.empty() && (result[result.length() - 1] != '/'))
        result.push_back('/');

    return result;
}

static struct user_dirs user_dirs()
{
    const std::string home = platform::home_dir();
    const class platform::inifile inifile(home + "/.config/user-dirs.dirs", true);
    const auto section = inifile.open_section();

    struct user_dirs user_dirs;
    user_dirs.download  = unquote_and_replace_home(section.read("XDG_DOWNLOAD_DIR"), home);
    user_dirs.music     = unquote_and_replace_home(section.read("XDG_MUSIC_DIR"), home);
    user_dirs.pictures  = unquote_and_replace_home(section.read("XDG_PICTURES_DIR"), home);
    user_dirs.videos    = unquote_and_replace_home(section.read("XDG_VIDEOS_DIR"), home);

    return user_dirs;
}

#include <cstring>
static std::string default_upnp_devicename()
{
    std::string default_devicename = "LXiMediaServer";

    char hostname[256] = { 0 };
    if (gethostname(hostname, sizeof(hostname) - 1) == 0)
        default_devicename = std::string(hostname) + " : " + default_devicename;

    return default_devicename;
}

#elif defined(__APPLE__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

static std::string get_user_dir(OSType type)
{
    FSRef ref;
    OSErr err = FSFindFolder(kUserDomain, type, false, &ref);
    if (!err)
    {
        std::string result;
        result.resize(2048);
        if (FSRefMakePath(&ref, reinterpret_cast<UInt8 *>(&result[0]), result.size()) == noErr)
        {
            result.resize(strnlen(result.data(), result.size()));
            if (!result.empty() && (result[result.length() - 1] != '/'))
                result.push_back('/');

            return result;
        }
    }

    return std::string();
}

static struct user_dirs user_dirs()
{
    struct user_dirs user_dirs;
    user_dirs.download  = get_user_dir(kDownloadsFolderType);
    user_dirs.music     = get_user_dir(kMusicDocumentsFolderType);
    user_dirs.pictures  = get_user_dir(kPictureDocumentsFolderType);
    user_dirs.videos    = get_user_dir(kMovieDocumentsFolderType);

    return user_dirs;
}

#include <cstring>
static std::string default_upnp_devicename()
{
    std::string default_devicename = "LXiMediaServer";

    char hostname[256] = { 0 };
    if (gethostname(hostname, sizeof(hostname) - 1) == 0)
        default_devicename = std::string(hostname) + " : " + default_devicename;

    return default_devicename;
}

#elif defined(WIN32)
#include <cstdlib>
#include <direct.h>
#include <shlobj.h>

static struct user_dirs user_dirs()
{
    struct user_dirs user_dirs;

    HMODULE shell32 = LoadLibrary(L"shell32.dll");
    if (shell32 != NULL)
    {
        typedef HRESULT WINAPI (* SHGetKnownFolderPathFunc)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
        auto SHGetKnownFolderPath = SHGetKnownFolderPathFunc(GetProcAddress(shell32, "SHGetKnownFolderPath"));
        if (SHGetKnownFolderPath)
        {
            PWSTR path = NULL;

            if (SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path) == S_OK)
            {
                user_dirs.download = platform::clean_path(platform::from_windows_path(path)) + '/';
                CoTaskMemFree(path);
                path = NULL;
            }

            if (SHGetKnownFolderPath(FOLDERID_Music, 0, NULL, &path) == S_OK)
            {
                user_dirs.music = platform::clean_path(platform::from_windows_path(path)) + '/';
                CoTaskMemFree(path);
                path = NULL;
            }

            if (SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &path) == S_OK)
            {
                user_dirs.pictures = platform::clean_path(platform::from_windows_path(path)) + '/';
                CoTaskMemFree(path);
                path = NULL;
            }

            if (SHGetKnownFolderPath(FOLDERID_Videos, 0, NULL, &path) == S_OK)
            {
                user_dirs.videos = platform::clean_path(platform::from_windows_path(path)) + '/';
                CoTaskMemFree(path);
                path = NULL;
            }
        }
        else // Windows XP and older
        {
            typedef HRESULT WINAPI(* SHGetFolderPathFunc)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);
            auto SHGetFolderPath = SHGetFolderPathFunc(GetProcAddress(shell32, "SHGetFolderPathW"));
            if (SHGetFolderPath)
            {
                wchar_t path[MAX_PATH];

                if (SHGetFolderPath(NULL, CSIDL_MYMUSIC, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
                    user_dirs.music = platform::clean_path(platform::from_windows_path(path)) + '/';

                if (SHGetFolderPath(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
                    user_dirs.pictures = platform::clean_path(platform::from_windows_path(path)) + '/';

                if (SHGetFolderPath(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
                    user_dirs.videos = platform::clean_path(platform::from_windows_path(path)) + '/';
            }
        }

        FreeLibrary(shell32);
    }

    return user_dirs;
}

static std::string default_upnp_devicename()
{
    std::string default_devicename = "LXiMediaServer";

    const wchar_t *hostname = _wgetenv(L"COMPUTERNAME");
    if (hostname)
        default_devicename = from_utf16(hostname) + " : " + default_devicename;

    return default_devicename;
}
#endif
