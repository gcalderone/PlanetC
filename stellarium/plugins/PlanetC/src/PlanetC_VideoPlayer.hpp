/*
 * Planetarium Control (PlanetC) plug-in for Stellarium
 *
 * Copyright (C) 2016-2020 Giorgio Calderone
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PLANETC_VIDEOPLAYER_HPP_
#define _PLANETC_VIDEOPLAYER_HPP_

#include <QObject>
#include <QtWidgets>

#ifdef ENABLE_QTAV
#include <QtAV/AVPlayer.h>
#include <QtAVWidgets/GraphicsItemRenderer.h>
#else
#include <QMediaPlayer>
#include <QtGui/QMovie>
#include <QtWidgets/QWidget>
#include <QGraphicsVideoItem>
#include <QVideoSurfaceFormat>
#endif


class PlanetC_VideoPlayer 
#ifdef ENABLE_QTAV
    : public QtAV::AVPlayer
#else
    : public QMediaPlayer
#endif
{
    Q_OBJECT

public:
    PlanetC_VideoPlayer(QGraphicsView* p_view);
    ~PlanetC_VideoPlayer();

    void openFile(const QString& filename, bool twin);
    void close();
    void setScale(float scale);
    void setVisible(bool);
    void forceUpdate() { flagUpdate = true; }
    void setVolume(float vol);
#ifdef ENABLE_QTAV
    void play() {
      if (isPaused())
	pause(false);
      else
	QtAV::AVPlayer::play();
    }
#else
	bool isPaused()  { return (state() == QMediaPlayer::PausedState); }
	bool isPlaying() { return (state() == QMediaPlayer::PlayingState); }
    void seek(float pos) {
	setPosition(round(pos * duration()));
	if (twin) player2->setPosition(round(pos * duration()));
    }
    void play() {
      qDebug() << "play()";
      QMediaPlayer::play();
      if (twin) player2->play();
    }
    void pause() {
      qDebug() << "pause()";
	QMediaPlayer::pause();
	if (twin) player2->pause();
    }
#endif

private slots:
#ifdef ENABLE_QTAV
    void handleStateChange(QtAV::AVPlayer::State);
#else
    void handleStateChange(QMediaPlayer::State);
#endif
    void handlePosChange(qint64);

signals:
    void statechanged(int);

private:
    QGraphicsView* view;
#ifdef ENABLE_QTAV
    QtAV::GraphicsItemRenderer *videoItem1;
    QtAV::GraphicsItemRenderer *videoItem2;
#else
    QMediaPlayer* player2;
    QGraphicsVideoItem* videoItem1;
    QGraphicsVideoItem* videoItem2;
#endif
    float scale;
    bool flagUpdate;
    bool twin;
};

#endif // _PLANETC_VIDEOPLAYER_HPP_
