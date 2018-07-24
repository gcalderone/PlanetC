/*
 * Planetarium Control (PlanetC) plug-in for Stellarium
 *
 * Copyright (C) 2016-2018 Giorgio Calderone
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

#include "PlanetC_VideoPlayer.hpp"
#include "StelMainView.hpp"


PlanetC_VideoPlayer::PlanetC_VideoPlayer(QGraphicsView* p_view) 
#ifdef ENABLE_QTAV
    : AVPlayer(0)
#else
    : QMediaPlayer(0, QMediaPlayer::VideoSurface)
#endif
{
#ifdef ENABLE_QTAV
    QtAV::Widgets::registerRenderers();
    videoItem1 = new QtAV::GraphicsItemRenderer();
    videoItem2 = new QtAV::GraphicsItemRenderer();    
    videoItem1->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
    videoItem2->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
    setRenderer(videoItem1);
    addVideoRenderer(videoItem2);
#else
    player2 = new QMediaPlayer(0, QMediaPlayer::VideoSurface);
    videoItem1 = new QGraphicsVideoItem();
    videoItem2 = new QGraphicsVideoItem();
    videoItem1->setAspectRatioMode(Qt::KeepAspectRatio);
    videoItem1->setAspectRatioMode(Qt::KeepAspectRatio);
    setVideoOutput(videoItem1);
    player2->setVideoOutput(videoItem2);
    videoItem1->show();
    videoItem1->show();
#endif
    videoItem1->setVisible(false);
    videoItem2->setVisible(false);
    videoItem1->setOpacity(1.);
    videoItem2->setOpacity(1.);

    //Add object to the main scene
    view = p_view;
    view->scene()->addItem(videoItem1);
    view->scene()->addItem(videoItem2);
    scale = 1.;
    
#ifdef ENABLE_QTAV
    connect(this, SIGNAL(stateChanged(QtAV::AVPlayer::State)), this, SLOT(handleStateChange(QtAV::AVPlayer::State)));
#else
    connect(this, SIGNAL(stateChanged(QMediaPlayer::State))  , this, SLOT(handleStateChange(QMediaPlayer::State)));
#endif
    connect(this, SIGNAL(positionChanged(qint64))            , this, SLOT(handlePosChange(qint64)));
}

PlanetC_VideoPlayer::~PlanetC_VideoPlayer()
{
    close();
#ifndef ENABLE_QTAV
    delete player2;
#endif
    delete videoItem1;
    delete videoItem2;
}


void PlanetC_VideoPlayer::openFile(const QString& filename, bool twin)
{
    this->twin = twin;
    scale = 1;

#ifdef ENABLE_QTAV
    setFile(filename);
    load();
#else
    //video->setVisible(false);
    setMedia(QUrl::fromLocalFile(filename));
    pause();
    if (twin) {
	player2->setMedia(QUrl::fromLocalFile(filename));
	player2->pause();
    }
#endif
}

void PlanetC_VideoPlayer::close()
{
    stop();
#ifndef ENABLE_QTAV
    player2->stop();
    setMedia(QMediaContent());
    player2->setMedia(QMediaContent());
#endif
}

void PlanetC_VideoPlayer::setScale(float scale)
{
    this->scale = scale;
    flagUpdate = true;
}

void PlanetC_VideoPlayer::setVisible(bool b)
{
    videoItem1->setVisible(b);
    if (twin)
	videoItem2->setVisible(b);
}

void PlanetC_VideoPlayer::setVolume(float vol)
{
#ifdef ENABLE_QTAV
    audio()->setVolume(vol);
#else
    QMediaPlayer::setVolume(vol);
    player2->setVolume(0.);
#endif
}

void PlanetC_VideoPlayer::handlePosChange(qint64 timepos)
{
    if (!flagUpdate)
	if (position() > 5000)
	    return;
    flagUpdate = false;
    
#ifdef ENABLE_QTAV
    //videoItem1->setOpenGL(true);
    QSize videoSize = videoItem1->videoFrameSize();
#else
    QSize videoSize = QSize(videoItem1->nativeSize().width(), videoItem1->nativeSize().height());
#endif
    QRectF viewRect = view->mapToScene(view->viewport()->rect()).boundingRect();
    qDebug() << "Sizes " << videoSize << "  "  << viewRect;
    int maxW = viewRect.width();
    int maxH = viewRect.height();
    if (twin) maxH /= 2.;
 
    float scale2W = (scale * maxW) / videoSize.width();
    float scale2H = (scale * maxH) / videoSize.height();
    float scale2 = (scale2W < scale2H  ?  scale2H  :  scale2W);

    int finalW = videoSize.width()  * scale2;
    int finalH = videoSize.height() * scale2;
    qDebug() << "Final " << finalW << "  "  << finalH;
    
#ifdef ENABLE_QTAV
    videoItem1->resizeRenderer(finalW, finalH);
    videoItem2->resizeRenderer(finalW, finalH);
#else
    videoItem1->setSize(QSize(finalW, finalH));
    videoItem2->setSize(QSize(finalW, finalH));
#endif

    //Rotation occurs wrt the OriginPoint.  Set the latter at the center of Item.
    videoItem1->setTransformOriginPoint(videoItem1->boundingRect().center());
    videoItem1->setRotation(0);
    videoItem2->setTransformOriginPoint(videoItem2->boundingRect().center());
    videoItem2->setRotation(180);
	
    float posX = 0.5;
    float posY = 0.5;
    if (twin) posY = 0.75;
    QPointF pos1(posX * viewRect.width(), posY * viewRect.height());
    videoItem1->setPos(pos1 - videoItem1->transformOriginPoint());		
    qDebug() << "Pos " << pos1 << "  "  << pos1 - videoItem1->transformOriginPoint();
    videoItem1->setVisible(true);
    if (twin) {
	posY = 0.25;
	QPointF pos2(posX * viewRect.width(), posY * viewRect.height());
	videoItem2->setPos(pos2 - videoItem2->transformOriginPoint());
	videoItem2->setVisible(true);
    }
}


#ifdef ENABLE_QTAV
void PlanetC_VideoPlayer::handleStateChange(QtAV::AVPlayer::State state) {
#else
void PlanetC_VideoPlayer::handleStateChange(QMediaPlayer::State state) {
#endif
  qDebug() << "State: " << state;
    int iState = 0;
    switch (state){
    case StoppedState: iState = 1; setVisible(false); break;
    case PlayingState: iState = 2; setVisible(true);  break;
    case PausedState:  iState = 3; break;
    default: ;
    }
    emit statechanged(iState);
}
