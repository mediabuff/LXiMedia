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

#include "server.h"
#include "platform/string.h"
#include "platform/translator.h"
#include "files.h"
#include "recommended.h"
#include "setup.h"
#include "test.h"
#include <cassert>

std::function<void()> server::recreate_server;

server::server(
        class platform::messageloop_ref &messageloop,
        class settings &settings,
        class pupnp::upnp &upnp,
        const std::string &logfilename,
        class platform::inifile &media_cache_file,
        class platform::inifile &watchlist_file)
    : messageloop(messageloop),
      settings(settings),
      media_cache_file(media_cache_file),
      watchlist_file(watchlist_file),
      upnp(upnp),
      rootdevice(messageloop, upnp, settings.uuid(), "urn:schemas-upnp-org:device:MediaServer:1"),
      connection_manager(messageloop, rootdevice),
      content_directory(messageloop, upnp, rootdevice, connection_manager),
      mediareceiver_registrar(messageloop, rootdevice),
      files(),
      setup(),
      test(),
      mainpage(messageloop, upnp, connection_manager, settings),
      settingspage(mainpage, settings, std::bind(&server::apply_settings, this)),
      logpage(mainpage, logfilename),
      helppage(mainpage),
      setuppage(mainpage, settings, test, std::bind(&server::force_apply_settings, this)),
      republish_timer(messageloop, std::bind(&server::republish_rootdevice, this)),
      republish_timeout(15),
      republish_required(false),
      recreate_server_timer(messageloop, [this] { if (recreate_server) this->messageloop.post(recreate_server); }),
      recreate_server_timeout(500)
{
    if (settings.is_configure_required())
        setuppage.activate_setup();
}

server::~server()
{
    rootdevice.handled_action.erase(this);
    connection_manager.numconnections_changed.erase(this);
}


bool server::initialize()
{
    const std::string upnp_devicename = settings.upnp_devicename();
    rootdevice.set_devicename(upnp_devicename);
    mainpage.set_devicename(upnp_devicename);

    add_audio_protocols();
    add_video_protocols();
    add_image_protocols();

    if (upnp.initialize(settings.http_port(), settings.bind_all_networks()))
    {
        switch (html::setuppage::setup_mode())
        {
        case html::setup_mode::disabled:
        case html::setup_mode::name:
            update.reset(new check_for_update(*this));
            recommended.reset(new class recommended(content_directory));

            files.reset(new class files(
                            messageloop,
                            connection_manager,
                            content_directory,
                            *recommended,
                            settings,
                            media_cache_file,
                            watchlist_file));

            setup.reset(new class setup(
                            messageloop,
                            content_directory));
            break;

        case html::setup_mode::network:
        case html::setup_mode::codecs:
        case html::setup_mode::high_definition:
            test.reset(new class test(
                           messageloop,
                           connection_manager,
                           content_directory,
                           settings));
            break;
        }

        republish_required = settings.republish_rootdevice();
        if (republish_required)
        {
            rootdevice.handled_action[this] = [this]
            {
                if (republish_required)
                    republish_timer.start(republish_timeout * 8);
            };

            connection_manager.numconnections_changed[this] = [this](size_t count)
            {
                if (count > 0)
                {
                    republish_required = false;
                    republish_timer.stop();
                }
                else
                {
                    republish_required = true;
                    republish_timer.start(republish_timeout * 4);
                }
            };

            republish_timer.start(republish_timeout);
        }

        return true;
    }

    return false;
}

const std::set<std::string> & server::bound_addresses() const
{
    return upnp.bound_addresses();
}

uint16_t server::bound_port() const
{
    return upnp.bound_port();
}

void server::republish_rootdevice()
{
    assert(republish_required);
    if (republish_required)
    {
        rootdevice.close();
        rootdevice.initialize();
    }
}

bool server::apply_settings()
{
    if (connection_manager.output_connections().empty())
    {
        force_apply_settings();
        return true;
    }

    return false;
}

void server::force_apply_settings()
{
    settings.save();

    // Wait a bit before restarting to handle pending requests from the webbrowser.
    recreate_server_timer.start(recreate_server_timeout, true);
}


server::check_for_update::check_for_update(server &parent)
    : parent(parent),
      basedir('/' + tr("Update available") + '/')
{
    parent.content_directory.item_source_register(basedir, *this);
}

server::check_for_update::~check_for_update()
{
    parent.content_directory.item_source_unregister(basedir);
}

std::vector<pupnp::content_directory::item> server::check_for_update::list_contentdir_items(
        const std::string &client,
        const std::string &,
        size_t start, size_t &count)
{
    const bool return_all = count == 0;

    std::vector<pupnp::content_directory::item> items;
    if (compare_version(VERSION, parent.settings.latest_version()) < 0)
        items.emplace_back(get_contentdir_item(client, basedir + "update"));

    std::vector<pupnp::content_directory::item> result;
    for (size_t i=start, n=0; (i<items.size()) && (return_all || (n<count)); i++, n++)
        result.emplace_back(std::move(items[i]));

    count = items.size();

    return result;
}

pupnp::content_directory::item server::check_for_update::get_contentdir_item(
        const std::string &,
        const std::string &path)
{
    pupnp::content_directory::item item;

    if (ends_with(path, "/update"))
    {
        item.is_dir = false;
        item.path = path;
        item.mrl = "file:///";
        item.type = pupnp::content_directory::item_type::none;
        item.title = tr("Version") + ' ' + parent.settings.latest_version();
    }

    return item;
}

bool server::check_for_update::correct_protocol(
        const pupnp::content_directory::item &,
        pupnp::connection_manager::protocol &)
{
    return true;
}

int server::check_for_update::play_item(
        const std::string &,
        const pupnp::content_directory::item &,
        const std::string &,
        std::string &,
        std::shared_ptr<std::istream> &)
{
    return pupnp::upnp::http_not_found;
}

