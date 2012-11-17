/******************************************************************************
 *   Copyright (C) 2012  A.J. Admiraal                                        *
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

#ifndef PULSEAUDIOOUTPUT_H
#define PULSEAUDIOOUTPUT_H

#include <pulse/simple.h>
#include <QtCore>
#include <LXiStreamDevice>

namespace LXiStreamDevice {
namespace PulseAudioBackend {

class PulseAudioOutput : public SInterfaces::AudioOutput
{
Q_OBJECT
public:
                                PulseAudioOutput(const QString &, QObject *);
  virtual                       ~PulseAudioOutput();

  virtual bool                  start(void);
  virtual void                  stop(void);
  virtual STime                 latency(void) const;

  virtual void                  consume(const SAudioBuffer &);

private:
  void                          openCodec(const SAudioFormat &);

private:
  pa_simple                   * handle;

  STime                         outLatency;
  SAudioFormat                  inFormat;
};

} } // End of namespaces

#endif
