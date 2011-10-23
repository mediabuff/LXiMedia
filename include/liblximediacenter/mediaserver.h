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

#ifndef LXIMEDIACENTER_MEDIASERVER_H
#define LXIMEDIACENTER_MEDIASERVER_H

#include <QtCore>
#include <QtNetwork>
#include <LXiServer>
#include <LXiStream>
#include "backendserver.h"
#include "mediaprofiles.h"
#include "export.h"

namespace LXiMediaCenter {

class MediaStream;

class LXIMEDIACENTER_PUBLIC MediaServer : public BackendServer,
                                          protected SHttpServer::Callback,
                                          private SUPnPContentDirectory::Callback
{
Q_OBJECT
friend class MediaServerDir;
public:
  struct LXIMEDIACENTER_PUBLIC Item : SUPnPContentDirectory::Item
  {
                                Item(void);
                                ~Item();

    SAudioFormat                audioFormat;
    SVideoFormat                videoFormat;
    SSize                       imageSize;
  };

  class LXIMEDIACENTER_PUBLIC File
  {
  public:
                                File(const SHttpServer::RequestMessage &);
                                ~File();

    inline const QString      & fileName(void) const                            { return d.fileName; }
    inline const QString      & parentDir(void) const                           { return d.parentDir; }
    inline const QUrl         & url(void) const                                 { return d.url; }

  private:
    struct
    {
      QString                   fileName;
      QString                   parentDir;
      QUrl                      url;
    }                           d;
  };

  class LXIMEDIACENTER_PUBLIC Stream
  {
  public:
                                Stream(MediaServer *parent, const QString &url);
    virtual                     ~Stream();

  public:
    MediaServer         * const parent;
    const QString               url;
    SHttpStreamProxy            proxy;
  };

  struct LXIMEDIACENTER_PUBLIC ThumbnailListItem
  {
                                ThumbnailListItem(void);
                                ~ThumbnailListItem();

    QString                     title;
    QString                     subtitle;
    QUrl                        iconurl;
    QUrl                        url;
    bool                        played;
  };

  typedef QList<ThumbnailListItem> ThumbnailListItemList;

  struct LXIMEDIACENTER_PUBLIC DetailedListItem
  {
    struct LXIMEDIACENTER_PUBLIC Column
    {
                                Column(QString = QString::null, QUrl = QUrl(), QUrl = QUrl());
                                ~Column();

      QString                   title;
      QUrl                      iconurl; 
      QUrl                      url;
    };

                                DetailedListItem(void);
                                ~DetailedListItem();

    QList<Column>               columns;
    bool                        played;
  };

  typedef QList<DetailedListItem> DetailedListItemList;

public:
  explicit                      MediaServer(QObject * = NULL);
  virtual                       ~MediaServer();

  virtual void                  initialize(MasterServer *);
  virtual void                  close(void);

  _lxi_pure static MediaProfiles & mediaProfiles(void);
  _lxi_pure static QSet<QString> & activeClients(void);

protected:
  virtual Stream              * streamVideo(const SHttpServer::RequestMessage &) = 0;

  virtual int                   countItems(const QString &path) = 0;
  virtual QList<Item>           listItems(const QString &path, unsigned start = 0, unsigned count = 0) = 0;

protected slots:
  virtual void                  cleanStreams(void);

protected: // From SHttpServer::Callback
  virtual SHttpServer::ResponseMessage httpRequest(const SHttpServer::RequestMessage &, QIODevice *);

private: // From UPnPContentDirectory::Callback
  virtual int                   countContentDirItems(const QString &client, const QString &path);
  virtual QList<SUPnPContentDirectory::Item> listContentDirItems(const QString &client, const QString &path, unsigned start, unsigned count);

private:
  _lxi_internal static SAudioFormat audioFormatFor(const QString &client, const Item &item, int &addVideo);
  _lxi_internal static SVideoFormat videoFormatFor(const QString &client, const Item &item);
  _lxi_internal static void     setQueryItemsFor(const QString &client, QUrl &);
  _lxi_internal void            addStream(Stream *);
  _lxi_internal void            removeStream(Stream *);

public:
  _lxi_internal static const qint32 defaultDirSortOrder;
  _lxi_internal static const qint32 defaultFileSortOrder;
  _lxi_internal static const int seekBySecs;

private:
  struct Data;
  Data                  * const d;

public: // Implemented in mediaserver.html.cpp
  static void                   enableHtml5(bool = true);

protected: // Implemented in mediaserver.html.cpp
  static bool                   html5Enabled;
  static const char             audioTimeFormat[];
  static const char             videoTimeFormat[];

  static const char             m3uPlaylist[];
  static const char             m3uPlaylistItem[];
  static const char             htmlThumbnailList[];
  static const char             htmlThumbnailLoader[];
  static const char             htmlThumbnailItem[];
  static const char             htmlThumbnailItemNoTitle[];
  static const char             htmlDetailedList[];
  static const char             htmlDetailedLoader[];
  static const char             htmlDetailedListRow[];
  static const char             htmlDetailedListHead[];
  static const char             htmlDetailedListColumn[];
  static const char             htmlDetailedListColumnIcon[];
  static const char             htmlDetailedListColumnLink[];
  static const char             htmlDetailedListColumnLinkIcon[];
  static const char             htmlPlayerAudioItem[];
  static const char             htmlPlayerAudioItemHtml5[];
  static const char             htmlPlayerAudioSourceItemHtml5[];
  static const char             htmlPlayerAudioItemFlv[];
  static const char             htmlPlayerVideoItem[];
  static const char             htmlPlayerVideoItemHtml5[];
  static const char             htmlPlayerVideoSourceItemHtml5[];
  static const char             htmlPlayerVideoItemFlv[];
  static const char             htmlPlayerThumbItem[];
  static const char             htmlPlayerThumbItemOption[];
  static const char             htmlDetail[];

  static const char             headList[];
  static const char             headPlayer[];

  static QString                audioFormatString(const SMediaInfo::Program &);
  static QString                videoFormatString(const SMediaInfo::Program &);
  QByteArray                    buildThumbnailView(const QString &title, const ThumbnailListItemList &);
  QByteArray                    buildThumbnailLoader(const QString &title);
  QByteArray                    buildThumbnailItems(const ThumbnailListItemList &);
  QByteArray                    buildDetailedView(const QString &title, const QList< QPair<QString, bool> > &columns, const DetailedListItemList &);
  QByteArray                    buildDetailedLoader(const QString &title, const QList< QPair<QString, bool> > &columns);
  QByteArray                    buildDetailedItems(const DetailedListItemList &);
  QByteArray                    buildVideoPlayer(const QByteArray &item, const QString &title, const SMediaInfo::Program &, const QUrl &, const QSize & = QSize(768, 432), SAudioFormat::Channels = SAudioFormat::Channels_Stereo);
  QByteArray                    buildVideoPlayer(const QByteArray &item, const QString &title, const QUrl &, const QSize & = QSize(768, 432), SAudioFormat::Channels = SAudioFormat::Channels_Stereo);
};

} // End of namespace

#endif
