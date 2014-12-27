/******************************************************************************
 *   Copyright (C) 2014  A.J. Admiraal                                        *
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

#ifndef PLATFORM_PROCESS_H
#define PLATFORM_PROCESS_H

#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
#if defined(__unix__) || defined(__APPLE__)
# include <sys/types.h>
#elif defined(WIN32)
# include <thread>
#endif

namespace platform {

class process : public std::istream
{
public:
    process();

    process(
            const std::function<void(process &, int)> &,
            bool background_task = false);

    process(
            const std::function<void(process &, std::ostream &)> &,
            bool background_task = false);

    process(process &&);
    process & operator=(process &&);

    ~process();

    process(const process &) = delete;
    process & operator=(const process &) = delete;

    void send_term();
    bool term_pending() const;
    void close();

    bool joinable() const;
    void join();

private:
    int pipe_desc[2];

#if defined(__unix__) || defined(__APPLE__)
    pid_t child;
#elif defined(WIN32)
    std::thread thread;
    volatile bool term_received;
#endif
};

} // End of namespace

#endif
