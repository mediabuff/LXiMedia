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

#ifndef LXSTREAM_SDATADECODERNODE_H
#define LXSTREAM_SDATADECODERNODE_H

#include <QtCore>
#include <LXiCore>
#include "../sinterfaces.h"
#include "../export.h"

namespace LXiStream {

class SSubtitleBuffer;
class SIOInputNode;

class LXISTREAM_PUBLIC SDataDecoderNode : public SInterfaces::Node
{
Q_OBJECT
public:
  typedef SInterfaces::DataDecoder::Flags Flags;

public:
  explicit                      SDataDecoderNode(SGraph *);
  virtual                       ~SDataDecoderNode();

  static QStringList            codecs(void);

  bool                          open(SIOInputNode *, Flags = SInterfaces::DataDecoder::Flag_None);

  Flags                         flags(void) const;
  void                          setFlags(Flags);

public: // From SInterfaces::Node
  virtual bool                  start(void);
  virtual void                  stop(void);

public slots:
  void                          input(const SEncodedDataBuffer &);

signals:
  void                          output(const SSubtitleBuffer &);
  void                          output(const SSubpictureBuffer &);

private:
  struct Data;
  Data                  * const d;
};

} // End of namespace

#endif
