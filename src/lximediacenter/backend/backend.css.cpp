/***************************************************************************
 *   Copyright (C) 2010 by A.J. Admiraal                                   *
 *   code@admiraal.dds.nl                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 as     *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.           *
 ***************************************************************************/

#include "backend.h"

const char * const Backend::cssMain =
    "body {\n"
    " background-color: {_PALETTE_BASE};\n"
    " color: {_PALETTE_TEXT};\n"
    " font-family: sans-serif;\n"
    " font-size: 1em;\n"
    " font-weight: normal;\n"
    " font-style: normal;\n"
    " text-align: left;\n"
    "}\n"
    "img {\n"
    " border: none;\n"
    "}\n"
    "fieldset {\n"
    " padding: 0.5em;\n"
    "}\n"
// Head
    "div.head {\n"
    " padding: 0.5em;\n"
    " margin: 0;\n"
    " background: {_PALETTE_WINDOW};\n"
    " float: top;\n"
    "}\n"
    "table.head {\n"
    " padding: 0;\n"
    " border-spacing: 0;\n"
    " border: none;\n"
    "}\n"
    "td.heada {\n"
    " text-align: left;\n"
    "}\n"
    "td.headb {\n"
    " text-align: right;\n"
    " vertical-align: bottom;\n"
    "}\n"
    "span.headlogoa { color: {_PALETTE_BUTTONTEXT}; font-size: 3em; font-weight: bold; }\n"
    "span.headlogob { color: {_PALETTE_BUTTONTEXT}; font-size: 3em; font-weight: bold; font-style: italic; }\n"
    "span.headlogoc { color: {_PALETTE_TEXT}; font-size: 3em; font-weight: bold; }\n"
// Menu
    "ul.menu {\n"
    " padding: 0;\n"
    " margin: 0.5em 0.5em 0 0;\n"
    " width: 14em;\n"
    " overflow: hidden;\n"
    " float: left;\n"
    " clear: left;\n"
    " list-style-type: none;\n"
    " border: 1px solid {_PALETTE_WINDOW};\n"
    "}\n"
    ".menu li:first-child {\n"
    " padding: 0.3em;\n"
    " list-style: none;\n"
    " font-weight: bold;\n"
    " background-color: {_PALETTE_WINDOW};\n"
    " color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    ".menu li {\n"
    " padding: 0.3em 0.3em 0.3em 1em;\n"
    " list-style: none;\n"
    "}\n"
    ".menu a:link    { color: {_PALETTE_TEXT}; text-decoration: none; }\n"
    ".menu a:visited { color: {_PALETTE_TEXT}; text-decoration: none; }\n"
    ".menu a:active  { color: {_PALETTE_TEXT}; text-decoration: underline; }\n"
    ".menu a:hover   { color: {_PALETTE_TEXT}; text-decoration: underline; }\n"
// Content
    "div.content {\n"
    " display: table-cell;\n"
    "}\n"
// Search
    "table.searchresultlist {\n"
    " padding: 0;\n"
    " border-spacing: 0;\n"
    " border: none;\n"
    "}\n"
    "td.searchresultlistitemhead {\n"
    " padding: 0;\n"
    " height: 1.5em;\n"
    " vertical-align: bottom;\n"
    " text-align: left;\n"
    "}\n"
    "td.searchresultlistitemthumb {\n"
    " padding: 0;\n"
    " text-align: center;\n"
    " width: 68\n"
    "}\n"
    "td.searchresultlistitem {\n"
    " padding: 0;\n"
    " text-align: left;\n"
    " width: 100%\n"
    "}\n"
// Error log files
    "ul.errorlogfiles {\n"
    " padding: 0;\n"
    " list-style-type: none;\n"
    "}\n"
    "li.errorlogfilehead {\n"
    " padding: 0;\n"
    " list-style: none;\n"
    " font-weight: bold;\n"
    "}\n"
    "li.errorlogfile {\n"
    " padding: 0 0 0 1em;\n"
    " list-style: none;\n"
    "}\n"
// List
    "div.thumbnaillist {\n"
    " display: block;\n"
    "}\n"
    "div.thumbnaillistitem {\n"
    " padding: 1em;\n"
    " margin: 0.5em;\n"
    " width: 256px;\n"
    " height: 284px;\n"
    " overflow: hidden;\n"
    " float: left;\n"
    " text-align:center;\n"
    " background-color: {_PALETTE_ALTBASE};\n"
    " color: {_PALETTE_TEXT};\n"
    "}\n"
    "div.thumbnail{\n"
    " padding: 0;\n"
    " margin: 0;\n"
    " display: table-cell;\n"
    " width: 256px;\n"
    " height: 256px;\n"
    " overflow: hidden;\n"
    " text-align: center;\n"
    " vertical-align: middle;\n"
    "}\n"
    "div.pageselector {\n"
    " padding: 0;\n"
    " margin: 0;\n"
    " display: table-cell;\n"
    "}\n"
    ".pageselector ul {\n"
    " padding: 0;\n"
    " list-style-type: none;\n"
    " background-color: {_PALETTE_WINDOW};\n"
    " color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    ".pageselector li {\n"
    " padding: 0.3em;\n"
    " float: left;\n"
    " list-style: none;\n"
    " font-weight: bold;\n"
    " background-color: {_PALETTE_WINDOW};\n"
    " color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    ".pageselector a:link    { color: {_PALETTE_TEXT}; text-decoration: none; }\n"
    ".pageselector a:visited { color: {_PALETTE_TEXT}; text-decoration: none; }\n"
    ".pageselector a:active  { color: {_PALETTE_TEXT}; text-decoration: underline; }\n"
    ".pageselector a:hover   { color: {_PALETTE_TEXT}; text-decoration: underline; }\n"
// Player
    "div.player {\n"
    " padding: 0;\n"
    " margin: 0.5em;\n"
    " display: block;\n"
    " text-align: center;\n"
    "}\n"
    ;

/*
    "p {\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "}\n"
    "\n"
    "p.head {\n"
    "  font-size: 1.5em;\n"
    "  font-weight: bold;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "}\n"
    "p.head2 {\n"
    "  font-size: 1em;\n"
    "  font-weight: bold;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "}\n"
    "p.light {\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "}\n"
    "\n"
    "a {\n"
    "  color: {_PALETTE_TEXT};\n"
    "  text-decoration: underline;\n"
    "}\n"
    "\n"
    "a.light {\n"
    "  color: {_PALETTE_TEXT};\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "  text-decoration: underline;\n"
    "}\n"
    "\n"
    "a.bookmark {\n"
    "  text-decoration: none;\n"
    "}\n"
    "\n"
    "table {\n"
    "  padding: 0;\n"
    "  border-spacing: 0;\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "table.full {\n"
    "  width: 100%;\n"
    "  padding: 0;\n"
    "  border-spacing: 0;\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "tr {\n"
    "  vertical-align: top;\n"
    "}\n"
    "\n"
    "th {\n"
    "  text-align: left;\n"
    "}\n"
    "\n"
    "td {\n"
    "  text-align: left;\n"
    "}\n"
    "\n"
    "td.left {\n"
    "  text-align: left;\n"
    "}\n"
    "\n"
    "td.center {\n"
    "  text-align: center;\n"
    "}\n"
    "\n"
    "td.right {\n"
    "  text-align: right;\n"
    "}\n"
    "\n"
    "table.main {\n"
    "  padding: 0;\n"
    "  border-spacing: 0;\n"
    "  width: 100%;\n"
    "  margin-top: 2em;\n"
    "  margin-bottom: 2em;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "  color: {_PALETTE_TEXT};\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "tr.main {\n"
    "  vertical-align: top;\n"
    "}\n"
    "\n"
    "td.mainleft {\n"
    "  padding: 1em;\n"
    "  text-align: left;\n"
    "}\n"
    "\n"
    "td.maincenter {\n"
    "  padding: 1em;\n"
    "  text-align: center;\n"
    "}\n"
    "\n"
    "td.mainright {\n"
    "  padding: 1em;\n"
    "  text-align: right;\n"
    "}\n"
    "\n"
    "img {\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "iframe {\n"
    "  margin: 0;\n"
    "  padding: 0;\n"
    "  border: solid 1px;\n"
    "}\n"
    "\n"
    "iframe.noborder {\n"
    "  margin: 0;\n"
    "  padding: 0;\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "span.logoa {\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  font-size: 3em;\n"
    "  font-weight: bold;\n"
    "}\n"
    "\n"
    "span.logob {\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  font-size: 3em;\n"
    "  font-weight: bold;\n"
    "  font-style: italic;\n"
    "}\n"
    "\n"
    "span.logoc {\n"
    "  color: {_PALETTE_TEXT};\n"
    "  font-size: 3em;\n"
    "  font-weight: bold;\n"
    "}\n"
    "\n"
    "span.logoversion {\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  font-size: 0.9em;\n"
    "}\n"
    "\n"
    "table.widgets {\n"
    "  padding: 1em;\n"
    "  border-spacing: 2em;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "}\n"
    "\n"
    "table.widgetsfull {\n"
    "  width: 100%;\n"
    "  padding: 1em;\n"
    "  border-spacing: 2em;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "}\n"
    "\n"
    "tr.widgets {\n"
    "  vertical-align: top;\n"
    "}\n"
    "\n"
    "td.widget {\n"
    "  padding: 1em;\n"
    "  text-align: center;\n"
    "  background-color: {_PALETTE_WINDOW};\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "  border: 1px solid {_PALETTE_TEXT};\n"
    "}\n"
    "\n"
    "td.nowidget {\n"
    "  padding: 1em;\n"
    "  text-align: center;\n"
    "}\n"
    "\n"
    "a.widget {\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "  text-decoration: underline;\n"
    "}\n"
    "\n"
    "a.widgetlight {\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "  text-decoration: underline;\n"
    "}\n"
    "\n"
    "table.widgetbuttons {\n"
    "  border-spacing: 0;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "}\n"
    "\n"
    "tr.widgetbuttons {\n"
    "  vertical-align: middle;\n"
    "}\n"
    "\n"
    "td.widgetbutton {\n"
    "  padding: 1em;\n"
    "  text-align: center;\n"
    "}\n"
    "\n"
    "div.widgetbuttonsmall {\n"
    "  width: 64px;\n"
    "  height: 64px;\n"
    "  text-align: center;\n"
    "  display: table-cell;\n"
    "  padding: 4px;\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "div.widgetbutton {\n"
    "  width: 128px;\n"
    "  height: 128px;\n"
    "  text-align: center;\n"
    "  display: table-cell;\n"
    "  padding: 8px;\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "a.widgetbutton {\n"
    "  text-decoration: none;\n"
    "}\n"
    "\n"
    "div.widgetbuttonsmalltitle {\n"
    "  color: {_PALETTE_TEXT};\n"
    "  text-align: center;\n"
    "  font-weight: bold;\n"
    "  margin-top: 0.2em;\n"
    "  width: 72px;\n"
    "  height: 1.4em;\n"
    "  overflow: hidden;\n"
    "}\n"
    "\n"
    "div.widgetbuttontitle {\n"
    "  color: {_PALETTE_TEXT};\n"
    "  text-align: center;\n"
    "  font-weight: bold;\n"
    "  margin-top: 0.4em;\n"
    "  width: 142px;\n"
    "  height: 1.4em;\n"
    "  overflow: hidden;\n"
    "}\n"
    "\n"
    "a.widgetbuttontitle {\n"
    "  font-weight: bold;\n"
    "  text-decoration: none;\n"
    "  color: {_PALETTE_TEXT};\n"
    "}\n"
    "\n"
    "input\n"
    "{\n"
    "  background: {_PALETTE_BUTTON};\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  border: 1px solid {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "textarea\n"
    "{\n"
    "  background: {_PALETTE_BUTTON};\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  border: 1px solid {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "select\n"
    "{\n"
    "  background: {_PALETTE_BUTTON};\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  border: 1px solid {_PALETTE_BUTTONTEXT};\n"
    "}\n";*/

const char * const Backend::cssLogin =
    "p.loginspacer {\n"
    "  height: 10em;\n"
    "}\n"
    "\n"
    "table.login {\n"
    "  border-spacing: 0;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "}\n"
    "\n"
    "tr.login {\n"
    "  vertical-align: middle;\n"
    "}\n"
    "\n"
    "td.loginlogo {\n"
    "  padding: 0.3em;\n"
    "  text-align: center;\n"
    "}\n"
    "\n"
    "td.loginmain {\n"
    "  width: 23em;\n"
    "  padding: 3em;\n"
    "  text-align: left;\n"
    "  background-color: {PALETTE_WINDOWDARK};\n"
    "  color: {PALETTE_WINDOWTEXTDARK};\n"
    "}\n"
    "\n"
    "td.loginabout {\n"
    "  padding: 0.3em;\n"
    "  text-align: center;\n"
    "  background-color: {PALETTE_BUTTON};\n"
    "  color: {PALETTE_BUTTONTEXT};\n"
    "  font-size: 0.9em;\n"
    "}\n"
    "\n"
    "label.login\n"
    "{\n"
    "  width: 7em;\n"
    "  float: left;\n"
    "  text-align: right;\n"
    "  margin-right: 0.5em;\n"
    "  display: block;\n"
    "}\n";

const char * const Backend::cssMenu =
    "table.head {\n"
    "  width: 100%;\n"
    "  border-spacing: 0;\n"
    "  background-color: {_PALETTE_WINDOW};\n"
    "}\n"
    "\n"
    "tr.head {\n"
    "  vertical-align: middle;\n"
    "}\n"
    "\n"
    "td.headlogo {\n"
    "  padding: 0.3em;\n"
    "  text-align: left;\n"
    "}\n"
    "\n"
    "td.headmenuitem {\n"
    "  width: 10em;\n"
    "  padding: 0;\n"
    "  text-align: center;\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "a.headmenuitem {\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "  text-decoration: none;\n"
    "}\n"
    "\n"
    "td.headmenuitemsel {\n"
    "  width: 10em;\n"
    "  padding: 0.3em;\n"
    "  text-align: center;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "table.submenu {\n"
    "  width: 100%;\n"
    "  border-spacing: 0;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "}\n"
    "\n"
    "tr.submenu {\n"
    "  vertical-align: middle;\n"
    "}\n"
    "\n"
    "td.submenuitem {\n"
    "  width: 10em;\n"
    "  padding: 0;\n"
    "  text-align: center;\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "a.submenuitem {\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  text-decoration: none;\n"
    "}\n"
    "\n"
    "td.submenuitemsel {\n"
    "  width: 10em;\n"
    "  padding: 0.3em;\n"
    "  text-align: center;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "}\n"
    "\n"
    "table.subsubmenu {\n"
    "  width: 100%;\n"
    "  border-spacing: 0;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "}\n"
    "\n"
    "tr.subsubmenu {\n"
    "  vertical-align: middle;\n"
    "}\n"
    "\n"
    "td.subsubmenuitem {\n"
    "  width: 10em;\n"
    "  padding: 0;\n"
    "  text-align: center;\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "a.subsubmenuitem {\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "  text-decoration: none;\n"
    "}\n"
    "\n"
    "td.subsubmenuitemsel {\n"
    "  width: 10em;\n"
    "  padding: 0.3em;\n"
    "  text-align: center;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "}\n";

const char * const Backend::csslist =
    "table.list {\n"
    "  padding: 2em;\n"
    "  border-spacing: 0;\n"
    "  width: 100%;\n"
    "  margin-top: 2em;\n"
    "  margin-bottom: 2em;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "table.listiframe {\n"
    "  padding: 0;\n"
    "  border-spacing: 0;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "  width: 100%;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "body.listiframe {\n"
    "  margin: 0;\n"
    "  padding: 0;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "  font-family: sans-serif;\n"
    "  font-size: 1em;\n"
    "  font-weight: normal;\n"
    "  font-style: normal;\n"
    "  text-align: center;\n"
    "  }\n"
    "\n"
    "tr.listitem {\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "}\n"
    "\n"
    "td.listitem {\n"
    "  padding: 0;\n"
    "  text-align: left;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "}\n"
    "\n"
    "th.listitem {\n"
    "  padding: 0;\n"
    "  font-weight: bold;\n"
    "  text-align: center;\n"
    "  background-color: {_PALETTE_WINDOW};\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "td.listitemright {\n"
    "  padding: 0;\n"
    "  text-align: right;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "}\n"
    "\n"
    "tr.listaltitem {\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_ALTBASE};\n"
    "}\n"
    "\n"
    "td.listaltitem {\n"
    "  padding: 0;\n"
    "  text-align: left;\n"
    "  background-color: {_PALETTE_ALTBASE};\n"
    "}\n"
    "\n"
    "th.listaltitem {\n"
    "  padding: 0;\n"
    "  font-weight: bold;\n"
    "  text-align: center;\n"
    "  background-color: {_PALETTE_WINDOW};\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "td.listaltitemright {\n"
    "  padding: 0;\n"
    "  text-align: right;\n"
    "  background-color: {_PALETTE_ALTBASE};\n"
    "}\n"
    "\n"
    "p.listitem {\n"
    "  font-weight: bold;\n"
    "  text-decoration: none;\n"
    "  margin-top: 0.3em;\n"
    "  margin-bottom: 0;\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "a.listitem {\n"
    "  font-weight: bold;\n"
    "  text-decoration: none;\n"
    "  margin-top: 0.3em;\n"
    "  margin-bottom: 0;\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "p.listitemlight {\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0.3em;\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "a.listitemlight {\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0.3em;\n"
    "  text-decoration: underline;\n"
    "  color: {_PALETTE_WINDOWTEXT};\n"
    "}\n"
    "\n"
    "tr.listselitem {\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "}\n"
    "\n"
    "td.listselitem {\n"
    "  padding: 0;\n"
    "  text-align: left;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "}\n"
    "\n"
    "td.listselitemright {\n"
    "  padding: 0;\n"
    "  text-align: right;\n"
    "  background-color: {_PALETTE_BUTTON};\n"
    "}\n"
    "\n"
    "p.listselitem {\n"
    "  font-weight: bold;\n"
    "  text-decoration: none;\n"
    "  margin-top: 0.3em;\n"
    "  margin-bottom: 0;\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "a.listselitem {\n"
    "  font-weight: bold;\n"
    "  text-decoration: none;\n"
    "  margin-top: 0.3em;\n"
    "  margin-bottom: 0;\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "p.listselitemlight {\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0.3em;\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "a.listselitemlight {\n"
    "  font-size: 0.9em;\n"
    "  font-weight: lighter;\n"
    "  margin-top: 0;\n"
    "  margin-bottom: 0.3em;\n"
    "  text-decoration: underline;\n"
    "  color: {_PALETTE_BUTTONTEXT};\n"
    "}\n"
    "\n"
    "table.thumbnaillist {\n"
    "  padding: 0;\n"
    "  border-spacing: 0;\n"
    "  width: 100%;\n"
    "  margin-top: 2em;\n"
    "  margin-bottom: 2em;\n"
    "  margin-left: auto;\n"
    "  margin-right: auto;\n"
    "  color: {_PALETTE_TEXT};\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "tr.thumbnaillist {\n"
    "  vertical-align: middle;\n"
    "}\n"
    "\n"
    "td.thumbnaillistitem {\n"
    "  padding: 1em;\n"
    "  text-align: center;\n"
    "}\n"
    "\n"
    "div.thumbnaillistitem {\n"
    "  width: 256px;\n"
    "  height: 256px;\n"
    "  text-align: center;\n"
    "  display: table-cell;\n"
    "  padding: 16px;\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_ALTBASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "}\n"
    "\n"
    "div.thumbnaillistitemplayed {\n"
    "  width: 256px;\n"
    "  height: 256px;\n"
    "  text-align: center;\n"
    "  display: table-cell;\n"
    "  padding: 16px;\n"
    "  vertical-align: middle;\n"
    "  background-color: {_PALETTE_BASE};\n"
    "  color: {_PALETTE_TEXT};\n"
    "}\n"
    "\n"
    "a.thumbnaillistitem {\n"
    "  text-decoration: none;\n"
    "}\n"
    "\n"
    "div.thumbnaillistitemtitle {\n"
    "  color: {_PALETTE_TEXT};\n"
    "  text-align: center;\n"
    "  font-weight: bold;\n"
    "  margin-top: 0.6em;\n"
    "  width: 280px;\n"
    "  height: 1.4em;\n"
    "  overflow: hidden;\n"
    "}\n"
    "\n"
    "a.thumbnaillistitemtitle {\n"
    "  font-weight: bold;\n"
    "  text-decoration: none;\n"
    "  color: {_PALETTE_TEXT};\n"
    "}\n"
    "\n"
    "div.thumbnaillistitemnote {\n"
    "  color: {_PALETTE_TEXT_LIGHT};\n"
    "  text-align: center;\n"
    "  font-size: 0.9em;\n"
    "  width: 280px;\n"
    "  height: 1.2em;\n"
    "  overflow: hidden;\n"
    "}\n"
    "\n"
    "table.searchresultlistbase {\n"
    "  padding: 0;\n"
    "  border-spacing: 2em;\n"
    "  width: 100%;\n"
    "  border: none;\n"
    "}\n"
    "\n"
    "div.pageselector {\n"
    "  margin-top: 0.3em;\n"
    "  font-weight: bold;\n"
    "}\n"
    "\n"
    "a.pageselector {\n"
    "  font-weight: bold;\n"
    "  text-decoration: underline;\n"
    "}\n";

const char * const Backend::csslog =
    "table.log {\n"
    " padding: 0;\n"
    " border-spacing: 0;\n"
    " border: none;\n"
    "}\n"
    "\n"
    "tr.log {\n"
    " vertical-align: middle;\n"
    " font-family: monospace;\n"
    " font-size: 1em;\n"
    " font-weight: normal;\n"
    " font-style: normal;\n"
    "}\n"
    "\n"
    "th.log {\n"
    " font-weight: bold;\n"
    " text-align: center;\n"
    "}\n"
    "td.log {\n"
    " text-align: left;\n"
    "}\n"
    "\n"
    "td.loginf {\n"
    " color: {_PALETTE_WINDOWTEXT};\n"
    " text-align: left;\n"
    "}\n"
    "\n"
    "td.logdbg {\n"
    " color: {_PALETTE_WINDOWTEXT_LIGHT};\n"
    " text-align: left;\n"
    "}\n"
    "\n"
    "td.logwrn {\n"
    " color: {_PALETTE_WINDOWTEXT};\n"
    " text-align: left;\n"
    "}\n"
    "\n"
    "td.logerr {\n"
    " color: {_PALETTE_WINDOWTEXT};\n"
    " text-align: left;\n"
    "}\n";


SHttpServer::SocketOp Backend::handleCssRequest(const SHttpServer::RequestMessage &request, QAbstractSocket *socket, const QString &file)
{
  const char * cssFile = NULL;
  if (file == "main.css")
    cssFile = cssMain;
  else if (file == "login.css")
    cssFile = cssLogin;
  else if (file == "menu.css")
    cssFile = cssMenu;
  else if (file == "list.css")
    cssFile = csslist;
  else if (file == "log.css")
    cssFile = csslog;

  if (cssFile)
  {
    SHttpServer::ResponseHeader response(request, SHttpServer::Status_Ok);
    response.setContentType("text/css;charset=utf-8");
    response.setField("Cache-Control", "max-age=1800");

    socket->write(response);
    socket->write(cssParser.parse(cssFile));
  }
  else
    return SHttpServer::sendResponse(request, socket, SHttpServer::Status_NotFound, this);

  return SHttpServer::SocketOp_Close;
}
