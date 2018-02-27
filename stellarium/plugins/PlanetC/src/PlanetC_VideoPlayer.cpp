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
	scale = 1;
#ifdef ENABLE_QTAV
	QtAV::Widgets::registerRenderers();
	videoItem1 = new QtAV::GraphicsItemRenderer();
	videoItem2 = new QtAV::GraphicsItemRenderer();
#else
    videoItem1 = new QGraphicsVideoItem();
    videoItem2 = new QGraphicsVideoItem();
#endif
	videoItem1->setVisible(false);
	videoItem2->setVisible(false);
	videoItem1->setOpacity(1.);
	videoItem2->setOpacity(1.);

#ifdef ENABLE_QTAV
    videoItem1->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
    videoItem2->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
#endif

	//Add object to the main scene
	view = p_view;
	view->scene()->addItem(videoItem1);
	view->scene()->addItem(videoItem2);

#ifdef ENABLE_QTAV
    setRenderer(videoItem1);
    addVideoRenderer(videoItem2);
#else
	setVideoOutput(videoItem1);
#endif

	//Once geometry properties are available (width, height) set the GraphicsVideoItem geometry
#ifdef ENABLE_QTAV
	connect(this, SIGNAL(positionChanged(qint64))            , this, SLOT(update(qint64)));
	connect(this, SIGNAL(stateChanged(QtAV::AVPlayer::State)), this, SLOT(handleStateChange(QtAV::AVPlayer::State)));
#endif
}

PlanetC_VideoPlayer::~PlanetC_VideoPlayer()
{
	stop();
}

#ifdef ENABLE_QTAV
void PlanetC_VideoPlayer::handleStateChange(QtAV::AVPlayer::State state)
{
	int iState = 0;
	switch (state)
	{
	case StoppedState: iState = 1; setVisible(false); break;
	case PlayingState: iState = 2; setVisible(true);  break;
	case PausedState:  iState = 3; break;
	default: ;
	}

	emit statechanged(iState);
}
#else
void PlanetC_VideoPlayer::handleStateChange(int state)
{
	emit statechanged(state);
}
#endif

void PlanetC_VideoPlayer::openFile(const QString& filename, bool twin)
{
	this->twin = twin;
	scale = 1;

#ifdef ENABLE_QTAV
	setFile(filename);
	load();
#else
	setMedia(QUrl::fromLocalFile(filename));
	pause();
#endif
}

void PlanetC_VideoPlayer::stop()
{
#ifdef ENABLE_QTAV
	QtAV::AVPlayer::stop();
#else
	stop();
	setMedia(QMediaContent());
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
	if(twin)
		videoItem2->setVisible(b);
}

void PlanetC_VideoPlayer::setVolume(float vol)
{
#ifdef ENABLE_QTAV
	audio()->setVolume(vol);
#else
	setVolume(vol);
	#endif
}

void PlanetC_VideoPlayer::update(qint64 timepos)
{
	if ((!flagUpdate) &&
		(position() > 5000))
		return;
	flagUpdate = false;

#ifdef ENABLE_QTAV
	//videoItem1->setOpenGL(true);
	QSize videoSize = videoItem1->videoFrameSize();
#else
	QSize videoSize;// = videoItem1->size();
#endif
	QRectF viewRect = view->mapToScene(view->viewport()->rect()).boundingRect();

	int maxW = viewRect.width();
	int maxH = viewRect.height();
	if (twin) maxH /= 2.;

	float scale2W = (scale * maxW) / videoSize.width();
	float scale2H = (scale * maxH) / videoSize.height();
	float scale2 = (scale2W < scale2H  ?  scale2H  :  scale2W);
#ifdef ENABLE_QTAV
	videoItem1->resizeRenderer(videoSize.width() * scale2, videoSize.height() * scale2);
	videoItem2->resizeRenderer(videoSize.width() * scale2, videoSize.height() * scale2);
#endif

	//Rotation occurs wrt the OriginPoint.  Set the latter at the center of Item.
	videoItem1->setTransformOriginPoint(videoItem1->boundingRect().center());
	videoItem1->setRotation(0);
	videoItem2->setTransformOriginPoint(videoItem2->boundingRect().center());
	videoItem2->setRotation(180);
	
	float posX = 0.5;
	float posY = 0.5;
	if(twin) posY = 0.75;
	
	QPointF pos1(posX * viewRect.width(), posY * viewRect.height());
	videoItem1->setPos(pos1 - videoItem1->transformOriginPoint());
		
	if(twin)
	{
		posY = 0.25;
		QPointF pos2(posX * viewRect.width(), posY * viewRect.height());
		videoItem2->setPos(pos2 - videoItem2->transformOriginPoint());
	}
}


