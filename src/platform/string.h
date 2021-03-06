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

#ifndef STRING_H
#define STRING_H

#include <string>

bool starts_with(const std::string &text, const std::string &find);
bool ends_with(const std::string &text, const std::string &find);

std::string to_upper(const std::string &);
std::string to_lower(const std::string &);

struct alphanum_less { bool operator()(const std::string &, const std::string &) const; };

std::string from_base64(const std::string &);
std::string to_base64(const std::string &, bool pad = false);

std::string from_percent(const std::string &);
std::string to_percent(const std::string &);
std::string escape_xml(const std::string &);

/*! returns:
 *  <0 if the actual version is older than the specified version.
 *   0 if the actual version equals the specified version.
 *  >0 if the actual version is newer than the specified version.
 */
int compare_version(const std::string &actual, const std::string &specified);

bool is_utf8(const std::string &);

#if defined(WIN32)
std::wstring to_utf16(const std::string &);
std::string from_utf16(const std::wstring &);
#endif

std::string to_utf8(const std::string &, const std::string &encoding);
std::u32string to_utf32(const std::string &, const char *encoding = nullptr);

#endif
