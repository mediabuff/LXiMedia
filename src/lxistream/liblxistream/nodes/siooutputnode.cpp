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

#include "nodes/siooutputnode.h"
#include "nodes/saudioencodernode.h"
#include "nodes/svideoencodernode.h"

namespace LXiStream {

const int SIOOutputNode::outBufferSize = 262144;

struct SIOOutputNode::Data
{
  inline Data(void) : mutex(QMutex::Recursive) { }

  QMutex                        mutex;
  QIODevice                   * ioDevice;
  SInterfaces::BufferWriter   * bufferWriter;
  bool                          autoClose;

  float                         streamingSpeed;
  STime                         streamingPreload;
  STimer                        streamTimer;
};

SIOOutputNode::SIOOutputNode(SGraph *parent, QIODevice *ioDevice)
  : SInterfaces::SinkNode(parent),
    d(new Data())
{
  d->ioDevice = ioDevice;
  d->bufferWriter = NULL;
  d->autoClose = false;

  d->streamingSpeed = 0.0f;
}

SIOOutputNode::~SIOOutputNode()
{
  delete d->bufferWriter;
  delete d;
  *const_cast<Data **>(&d) = NULL;
}

QStringList SIOOutputNode::formats(void)
{
  return SInterfaces::BufferWriter::available();
}

void SIOOutputNode::setIODevice(QIODevice *ioDevice, bool autoClose)
{
  d->ioDevice = ioDevice;
  d->autoClose = autoClose;
}

bool SIOOutputNode::hasIODevice(void) const
{
  return d->ioDevice != NULL;
}

bool SIOOutputNode::openFormat(const QString &format)
{
  delete d->bufferWriter;
  d->bufferWriter = SInterfaces::BufferWriter::create(this, format, false);

  return d->bufferWriter != NULL;
}

SInterfaces::BufferWriter * SIOOutputNode::bufferWriter(void)
{
  return d->bufferWriter;
}

void SIOOutputNode::enablePseudoStreaming(float speed, STime preload)
{
  d->streamingSpeed = speed;
  d->streamingPreload = preload;
}

bool SIOOutputNode::start(STimer *)
{
  if (d->ioDevice && d->bufferWriter)
  if (d->ioDevice->isOpen())
  {
    d->ioDevice->moveToThread(thread());
    d->ioDevice->setParent(this);

    connect(d->ioDevice, SIGNAL(readChannelFinished()), SLOT(close()));
    if (d->ioDevice->metaObject()->indexOfSignal("disconnected()") >= 0)
      connect(d->ioDevice, SIGNAL(disconnected()), SLOT(close()));

    return d->bufferWriter->start(this, d->ioDevice->isSequential());
  }

  return false;
}

void SIOOutputNode::stop(void)
{
  if (d->bufferWriter)
    d->bufferWriter->stop();

  QTime timer;
  timer.start();

  while (d->ioDevice && (d->ioDevice->bytesToWrite() >= 0))
  if (!d->ioDevice->waitForBytesWritten(qMax(0, 5000 - qAbs(timer.elapsed()))))
    break;

  close();
}

void SIOOutputNode::input(const SEncodedAudioBuffer &buffer)
{
  LXI_PROFILE_WAIT(d->mutex.lock());
  LXI_PROFILE_FUNCTION(TaskType_MiscProcessing);
  Q_ASSERT(QThread::currentThread() == thread());

  if (!qFuzzyCompare(d->streamingSpeed, 0.0f))
    blockUntil(buffer.decodingTimeStamp().isValid() ? buffer.decodingTimeStamp() : buffer.presentationTimeStamp());

  if (d->bufferWriter)
    d->bufferWriter->process(buffer);

  d->mutex.unlock();
}

void SIOOutputNode::input(const SEncodedVideoBuffer &buffer)
{
  LXI_PROFILE_WAIT(d->mutex.lock());
  LXI_PROFILE_FUNCTION(TaskType_MiscProcessing);
  Q_ASSERT(QThread::currentThread() == thread());

  if (!qFuzzyCompare(d->streamingSpeed, 0.0f))
    blockUntil(buffer.decodingTimeStamp().isValid() ? buffer.decodingTimeStamp() : buffer.presentationTimeStamp());

  if (d->bufferWriter)
    d->bufferWriter->process(buffer);

  d->mutex.unlock();
}

void SIOOutputNode::input(const SEncodedDataBuffer &buffer)
{
  LXI_PROFILE_WAIT(d->mutex.lock());
  LXI_PROFILE_FUNCTION(TaskType_MiscProcessing);
  Q_ASSERT(QThread::currentThread() == thread());

  if (!qFuzzyCompare(d->streamingSpeed, 0.0f))
    blockUntil(buffer.decodingTimeStamp().isValid() ? buffer.decodingTimeStamp() : buffer.presentationTimeStamp());

  if (d->bufferWriter)
    d->bufferWriter->process(buffer);

  d->mutex.unlock();
}

void SIOOutputNode::write(const uchar *buffer, qint64 size)
{
  if (d->ioDevice)
  while (d->ioDevice->bytesToWrite() >= outBufferSize)
  if (!d->ioDevice->waitForBytesWritten(-1))
    return;

  for (qint64 i=0; (i<size) && d->ioDevice; )
  {
    const qint64 r = d->ioDevice->write((char *)buffer + i, size - i);
    if (r > 0)
      i += r;
    else
      return;
  }
}

qint64 SIOOutputNode::seek(qint64 offset, int whence)
{
  if (d->ioDevice)
  {
    if (whence == SEEK_SET)
      return d->ioDevice->seek(offset) ? 0 : -1;
    else if (whence == SEEK_CUR)
      return d->ioDevice->seek(d->ioDevice->pos() + offset) ? 0 : -1;
    else if (whence == SEEK_END)
      return d->ioDevice->seek(d->ioDevice->size() + offset) ? 0 : -1;
    else if (whence == -1) // get size
      return d->ioDevice->size();
  }

  return -1;
}

void SIOOutputNode::close(void)
{
  LXI_PROFILE_WAIT(d->mutex.lock());
  LXI_PROFILE_FUNCTION(TaskType_MiscProcessing);

  if (d->ioDevice)
  {
    QIODevice * const device = d->ioDevice; // May be called recursively from close().
    d->ioDevice = NULL;

    if (d->autoClose)
    {
      device->close();
      device->deleteLater();
    }

    //qDebug() << "SIOOutputNode: Client disconnected";
    emit closed();
  }

  d->mutex.unlock();
}

void SIOOutputNode::blockUntil(STime timeStamp)
{
  // Hack to get access to msleep()
  struct T : QThread { static inline void msleep(unsigned long msec) { QThread::msleep(msec); } };

  static const int maxDelay = 250;

  if (timeStamp >= d->streamingPreload)
  {
    const STime correctedTime = STime::fromMSec(qint64(float(timeStamp.toMSec()) / d->streamingSpeed));

    // This blocks the thread until it is time to process the buffer.
    const STime duration = d->streamTimer.correctOffset(correctedTime, STime::fromMSec(maxDelay));
    if (duration.isPositive())
      T::msleep(qBound(0, int(duration.toMSec()), maxDelay));
  }
}


} // End of namespace
