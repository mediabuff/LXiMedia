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

#include "tvshowserver.h"
#include "mediaplayersandbox.h"

namespace LXiMediaCenter {
namespace MediaPlayerBackend {

TvShowServer::TvShowServer(MediaDatabase::Category category, QObject *parent)
  : PlaylistServer(category, parent),
    seasonText(tr("Season"))
{
}

TvShowServer::~TvShowServer()
{
}

TvShowServer::Stream * TvShowServer::streamVideo(const SHttpServer::RequestMessage &request)
{
  const MediaServer::File file(request);
  if (file.fileName().startsWith("playlist."))
  {
    SSandboxClient::Priority priority = SSandboxClient::Priority_Normal;
    if (file.url().queryItemValue("priority") == "low")
      priority = SSandboxClient::Priority_Low;
    else if (file.url().queryItemValue("priority") == "high")
      priority = SSandboxClient::Priority_High;

    SSandboxClient * const sandbox = masterServer->createSandbox(priority);
    connect(sandbox, SIGNAL(consoleLine(QString)), SLOT(consoleLine(QString)));
    sandbox->ensureStarted();

    const QString path = request.directory().mid(serverPath().length() - 1);
    if (!mediaDatabase->hasAlbum(category, path))
    {
      const QString dir = path.mid(path.left(path.length() - 1).lastIndexOf('/'));
      if (dir.startsWith("/" + seasonText + " "))
      {
        const unsigned season = dir.mid(seasonText.length() + 1, dir.length() - seasonText.length() - 2).toUInt();

        QMap<unsigned, QVector<MediaDatabase::UniqueID> > seasons;
        bool useSeasons;
        categorizeSeasons(path.left(path.length() - dir.length() + 1), seasons, useSeasons);

        QMultiMap<QString, QByteArray> files;

        const QMap<unsigned, QVector<MediaDatabase::UniqueID> >::ConstIterator s = seasons.find(season);
        if (s != seasons.end())
        for (QVector<MediaDatabase::UniqueID>::ConstIterator i=s->begin(); i!=s->end(); i++)
        {
          const FileNode node = mediaDatabase->readNode(*i);
          if (!node.isNull())
          {
            const QDateTime lastPlayed = mediaDatabase->lastPlayed(node.filePath());
            const QString key =
                (lastPlayed.isValid() ? lastPlayed.toString("yyyyMMddhhmmss") : QString("00000000000000")) +
                ("000000000" + QString::number(node.track())).right(10) +
                SStringParser::toRawName(node.title());

            files.insert(key, node.toByteArray(-1));
          }
        }

        QUrl rurl;
        rurl.setPath(MediaPlayerSandbox::path);
        rurl.addQueryItem("playlist", QString::null);
        typedef QPair<QString, QString> QStringPair;
        foreach (const QStringPair &queryItem, file.url().queryItems())
          rurl.addQueryItem(queryItem.first, queryItem.second);

        QByteArray content;
        foreach (const QByteArray &line, files)
          content += line + '\n';

        Stream *stream = new Stream(this, sandbox, request.path());
        if (stream->setup(rurl, content))
          return stream;

        delete stream;
      }
    }

    disconnect(sandbox, SIGNAL(consoleLine(QString)), this, SLOT(consoleLine(QString)));
    masterServer->recycleSandbox(sandbox);
  }

  return PlaylistServer::streamVideo(request);
}

int TvShowServer::countItems(const QString &path)
{
  const bool hasAlbum = mediaDatabase->hasAlbum(category, path);
  const int numAlbums = countAlbums(path);

  if ((numAlbums == 0) && !hasAlbum)
  {
    const QString dir = path.mid(path.left(path.length() - 1).lastIndexOf('/'));
    if (dir.startsWith("/" + seasonText + " "))
    {
      const unsigned season = dir.mid(seasonText.length() + 1, dir.length() - seasonText.length() - 2).toUInt();

      QMap<unsigned, QVector<MediaDatabase::UniqueID> > seasons;
      bool useSeasons;
      categorizeSeasons(path.left(path.length() - dir.length() + 1), seasons, useSeasons);

      const QMap<unsigned, QVector<MediaDatabase::UniqueID> >::ConstIterator s = seasons.find(season);
      if (s != seasons.end())
        return s->count();
      else
        return 0;
    }
  }

  if (hasAlbum)
  {
    QMap<unsigned, QVector<MediaDatabase::UniqueID> > seasons;
    bool useSeasons;
    categorizeSeasons(path, seasons, useSeasons);

    const QMap<unsigned, QVector<MediaDatabase::UniqueID> >::ConstIterator r = seasons.find(0);
    if (r != seasons.end())
      return numAlbums + (seasons.count() - 1) + r->count() + 1;
    else
      return numAlbums + seasons.count() + 1;
  }
  else
    return numAlbums;
}

QList<TvShowServer::Item> TvShowServer::listItems(const QString &path, unsigned start, unsigned count)
{
  const bool returnAll = count == 0;
  const bool hasAlbum = mediaDatabase->hasAlbum(category, path);
  QList<Item> result = listAlbums(path, start, count);

  if (result.isEmpty() && !hasAlbum)
  {
    const QString dir = path.mid(path.left(path.length() - 1).lastIndexOf('/'));
    if (dir.startsWith("/" + seasonText + " "))
    {
      const unsigned season = dir.mid(seasonText.length() + 1, dir.length() - seasonText.length() - 2).toUInt();

      QMap<unsigned, QVector<MediaDatabase::UniqueID> > seasons;
      bool useSeasons;
      categorizeSeasons(path.left(path.length() - dir.length() + 1), seasons, useSeasons);

      const QMap<unsigned, QVector<MediaDatabase::UniqueID> >::ConstIterator s = seasons.find(season);
      if ((s != seasons.end()) && !s->isEmpty())
      {
        QList<Item> result = listPlayAllItem(path, start, count, s->first());

        if (returnAll || (count > 0))
        for (int i=start, n=0; (i<s->count()) && (returnAll || (n<int(count))); i++, n++)
          result.append(makeSeasonItem(s->at(i)));

        return result;
      }
    }

    return QList<Item>();
  }

  if (hasAlbum && (returnAll || (count > 0)))
  {
    QMap<unsigned, QVector<MediaDatabase::UniqueID> > seasons;
    bool useSeasons;
    categorizeSeasons(path, seasons, useSeasons);

    for (QMap<unsigned, QVector<MediaDatabase::UniqueID> >::ConstIterator i=seasons.begin(); i!=seasons.end(); i++)
    if ((i.key() != 0) && !i.value().isEmpty())
    {
      if (start == 0)
      {
        if (returnAll || (count > 0))
        {
          Item item;
          item.isDir = true;
          item.type = defaultItemType();
          item.title = seasonText + " " + QString::number(i.key());
          item.iconUrl = MediaDatabase::toUidString(i.value().first()) + "-thumb.png?overlay=folder-video";
          result.append(item);

          if (count > 0)
            count--;
        }
      }
      else
        start--;
    }

    if (returnAll || (count > 0))
    {
      result += listPlayAllItem(path, start, count, 0, result);

      if (returnAll || (count > 0))
      {
        const QMap<unsigned, QVector<MediaDatabase::UniqueID> >::ConstIterator r = seasons.find(0);
        if (r != seasons.end())
        for (int i=start, n=0; (i<r->count()) && (returnAll || (n<int(count))); i++, n++)
          result.append(useSeasons ? makeSeasonItem(r->at(i)) : makePlainItem(r->at(i)));
      }
    }
  }

  return result;
}

void TvShowServer::categorizeSeasons(const QString &path, QMap<unsigned, QVector<MediaDatabase::UniqueID> > &seasons, bool &useSeasons)
{
  const QVector<MediaDatabase::UniqueID> files = mediaDatabase->getAlbumFiles(category, path);

  seasons.clear();
  useSeasons = true;
  foreach (MediaDatabase::UniqueID uid, files)
  {
    const FileNode node = mediaDatabase->readNode(uid);
    if ((node.track() > 0) && (node.track() < SMediaInfo::tvShowSeason))
    {
      // Don't use seasons
      seasons.clear();
      useSeasons = false;
      foreach (MediaDatabase::UniqueID uid, files)
        seasons[0].append(uid);

      return;
    }

    seasons[node.track() / SMediaInfo::tvShowSeason].append(uid);
  }

  if (seasons.count() == 1)
  {
    // Only one season
    const QVector<MediaDatabase::UniqueID> s = *(seasons.begin());
    seasons.erase(seasons.begin());
    seasons[0] = s;
  }
}

TvShowServer::Item TvShowServer::makePlainItem(MediaDatabase::UniqueID uid)
{
  Item item = makeItem(uid);
  if (item.track > 0)
    item.title = QString::number(item.track) + " " + item.title;

  return item;
}

TvShowServer::Item TvShowServer::makeSeasonItem(MediaDatabase::UniqueID uid)
{
  Item item = makeItem(uid);
  if (item.track > 0)
    item.title = toTvShowNumber(item.track) + " " + item.title;

  return item;
}

QString TvShowServer::toTvShowNumber(unsigned episode)
{
  QString text = QString::number(episode % SMediaInfo::tvShowSeason);
  if (text.length() < 2)
    text = "0" + text;

  return QString::number(episode / SMediaInfo::tvShowSeason) + "x" + text;
}

} } // End of namespaces
