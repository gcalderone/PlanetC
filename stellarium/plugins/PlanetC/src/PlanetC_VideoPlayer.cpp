/*
 * Planetarium Control (PlanetC) plug-in for Stellarium
 *
 * Copyright (C) 2016-2017 Giorgio Calderone
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
	posX = 0;
	posY = 0;
	scale = 100;
	rotationAngle = 0.;
#ifdef ENABLE_QTAV
	QtAV::Widgets::registerRenderers();
	videoItem = new QtAV::GraphicsItemRenderer();
#else
    videoItem = new QGraphicsVideoItem();
#endif
	videoItem->setVisible(false);
	videoItem->setOpacity(1.);

#ifdef ENABLE_QTAV
    videoItem->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
#endif

	//Add object to the main scene
	view = p_view;
	view->scene()->addItem(videoItem);

#ifdef ENABLE_QTAV
    setRenderer(videoItem);
#else
	setVideoOutput(videoItem);
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

void PlanetC_VideoPlayer::openFile(const QString& filename)
{
	//Set default options
	posX = 0;
	posY = 0;
	scale = 1;
	rotationAngle = 0;

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

void PlanetC_VideoPlayer::setGeometry(float posX, float posY, float scale)
{
	//Save input values into private state
	if(posX >= 0)  this->posX = posX;
	if(posY >= 0)  this->posY = posY;
	if(scale >= 0) this->scale = scale;
	flagUpdate = true;
}

void PlanetC_VideoPlayer::setVisible(bool b)
{
	videoItem->setVisible(b);
}

void PlanetC_VideoPlayer::setRotation(float rotationAngle)
{
	this->rotationAngle = rotationAngle;
	flagUpdate = true;
}

void PlanetC_VideoPlayer::setVolume(float vol)
{
#ifdef ENABLE_QTAV
	audio()->setVolume(vol);
#else
	setVolume(vol);
	#endif
}

void PlanetC_VideoPlayer::forceUpdate()
{
	flagUpdate = true;
}

void PlanetC_VideoPlayer::update(qint64 timepos)
{
	if (!flagUpdate) return;
	flagUpdate = false;

	if(scale > 0)
	{
#ifdef ENABLE_QTAV
		//videoItem->setOpenGL(true);
		QSize videoSize = videoItem->videoFrameSize();
#else
		QSize videoSize;// = videoItem->size();
#endif
		QRectF viewRect = view->mapToScene(view->viewport()->rect()).boundingRect();

		int maxW = viewRect.width();
		int maxH = viewRect.height();
		if (posY != 0.5) 
			maxH /= 2.;

		float scale2 = (scale * maxW) / videoSize.width();
		if (scale2 * videoSize.height() > maxH)
			scale2 = (scale * maxH) / videoSize.height();
#ifdef ENABLE_QTAV
		videoItem->resizeRenderer(videoSize.width() * scale2, videoSize.height() * scale2);
#endif

		//Rotation occurs wrt the OriginPoint.  Set the latter at the center of Item.
		videoItem->setTransformOriginPoint(videoItem->boundingRect().center());
		videoItem->setRotation(rotationAngle);
	
		QPointF pos(posX * viewRect.width(), posY * viewRect.height());
		videoItem->setPos(pos - videoItem->transformOriginPoint());
	}
	else
		qDebug() << "WARNING: no size specified for video";
}

