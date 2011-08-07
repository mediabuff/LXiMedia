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

#include "filetester.h"
#include <iostream>

void FileTester::testFile(const QString &fileName)
{
  const SMediaInfo mediaInfo(fileName);
  if (mediaInfo.containsAudio())
  {
    FileTester * const tester = new FileTester(fileName);

    if (tester->setup())
    {
      QTime timer; timer.start();

      std::cout << fileName.toAscii().data() << " ... " << std::flush;

      tester->start();
      tester->wait();

      std::cout << QByteArray::number(float(timer.elapsed()) / 1000.0f, 'f', 1).data() << " sec" << std::endl;
    }

    delete tester;
  }
}

FileTester::FileTester(const QString &fileName)
  : SGraph(),
    file(this, fileName),
    audioDecoder(this),
    videoDecoder(this),
    dataDecoder(this),
    timeStampResampler(this),
    audioMatrix(this),
    audioResampler(this),
    letterboxDetectNode(this),
    subpictureRenderer(this),
    subtitleRenderer(this),
    sync(this)
{
  // File
  connect(&file, SIGNAL(finished()), SLOT(stop()));
  connect(&file, SIGNAL(output(SEncodedAudioBuffer)), &audioDecoder, SLOT(input(SEncodedAudioBuffer)));
  connect(&file, SIGNAL(output(SEncodedVideoBuffer)), &videoDecoder, SLOT(input(SEncodedVideoBuffer)));
  connect(&file, SIGNAL(output(SEncodedDataBuffer)), &dataDecoder, SLOT(input(SEncodedDataBuffer)));

  // Audio
  connect(&audioDecoder, SIGNAL(output(SAudioBuffer)), &timeStampResampler, SLOT(input(SAudioBuffer)));
  connect(&timeStampResampler, SIGNAL(output(SAudioBuffer)), &audioMatrix, SLOT(input(SAudioBuffer)));
  connect(&audioMatrix, SIGNAL(output(SAudioBuffer)), &audioResampler, SLOT(input(SAudioBuffer)));
  connect(&audioResampler, SIGNAL(output(SAudioBuffer)), &sync, SLOT(input(SAudioBuffer)));

  // Video
  connect(&videoDecoder, SIGNAL(output(SVideoBuffer)), &timeStampResampler, SLOT(input(SVideoBuffer)));
  connect(&timeStampResampler, SIGNAL(output(SVideoBuffer)), &letterboxDetectNode, SLOT(input(SVideoBuffer)));
  connect(&letterboxDetectNode, SIGNAL(output(SVideoBuffer)), &subpictureRenderer, SLOT(input(SVideoBuffer)));
  connect(&subpictureRenderer, SIGNAL(output(SVideoBuffer)), &subtitleRenderer, SLOT(input(SVideoBuffer)));
  connect(&subtitleRenderer, SIGNAL(output(SVideoBuffer)), &sync, SLOT(input(SVideoBuffer)));

  // Data
  connect(&dataDecoder, SIGNAL(output(SSubpictureBuffer)), &timeStampResampler, SLOT(input(SSubpictureBuffer)));
  connect(&timeStampResampler, SIGNAL(output(SSubpictureBuffer)), &subpictureRenderer, SLOT(input(SSubpictureBuffer)));
  connect(&dataDecoder, SIGNAL(output(SSubtitleBuffer)), &timeStampResampler, SLOT(input(SSubtitleBuffer)));
  connect(&timeStampResampler, SIGNAL(output(SSubtitleBuffer)), &subtitleRenderer, SLOT(input(SSubtitleBuffer)));
}

bool FileTester::setup(void)
{
  // Select streams
  if (file.open(0))
  {
    const QList<SIOInputNode::AudioStreamInfo> audioStreams = file.audioStreams();
    const QList<SIOInputNode::VideoStreamInfo> videoStreams = file.videoStreams();
    const QList<SIOInputNode::DataStreamInfo>  dataStreams  = file.dataStreams();

    QList<SIOInputNode::StreamId> selectedStreams;
    if (!audioStreams.isEmpty())
      selectedStreams += audioStreams.first();

    if (!videoStreams.isEmpty())
      selectedStreams += videoStreams.first();

    if (!dataStreams.isEmpty())
      selectedStreams += dataStreams.first();

    file.selectStreams(selectedStreams);

    // Set stream properties
    if (!audioStreams.isEmpty() && !videoStreams.isEmpty())
    {
      // Graph options.
      timeStampResampler.setFrameRate(SInterval::fromFrequency(15));
      audioDecoder.setFlags(SInterfaces::AudioDecoder::Flag_DownsampleToStereo);
      audioMatrix.guessMatrices(SAudioFormat::Channels_Stereo);
      audioResampler.setSampleRate(48000);

      return true;
    }
    else if (!audioStreams.isEmpty())
    {
      audioMatrix.guessMatrices(SAudioFormat::Channels_Stereo);
      audioResampler.setSampleRate(48000);

      return true;
    }
  }

  return false;
}
