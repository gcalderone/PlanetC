/*
 * Planetarium Control (PlanetC) plug-in for Stellarium
 *
 * Copyright (C) 2016 Giorgio Calderone
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
	: QMediaPlayer(0, QMediaPlayer::VideoSurface)
{
	posX = 0;
	posY = 0;
	sizeW = 100;
	sizeH = 100;
	rotationAngle = 0.;

	//Create a QGraphicsVideoItem object to display the videow
	video = new QGraphicsVideoItem();
	video->setVisible(true);
	video->setAspectRatioMode(Qt::KeepAspectRatio);

	//Add QGraphicsVideoItem object to the main scene
	view = p_view;
	view->scene()->addItem(video);

	//Connect the player to the video item
	setVideoOutput(video);
	video->setVisible(true);

	//Once geometry properties are available (width, height) set the GraphicsVideoItem geometry
	connect(this, SIGNAL(metaDataChanged(QString,QVariant)), this, SLOT(setOptions(QString,QVariant)));
}

PlanetC_VideoPlayer::~PlanetC_VideoPlayer()
{
	stop();
}

void PlanetC_VideoPlayer::openFile(const QString& filename)
{
	//Set default options
	posX = 0;
	posY = 0;
	sizeW = 0.64;
	sizeH = 0.48;
	rotationAngle = 0;

	//Load file into QMediaPlayer and pause
	//video->setVisible(false);
	setMedia(QUrl::fromLocalFile(filename));
	pause();
}

void PlanetC_VideoPlayer::close()
{
	stop();
	setMedia(QMediaContent());
}

void PlanetC_VideoPlayer::setGeometry(float posX, float posY, float sizeW, float sizeH)
{
	//Save input values into private state
	if(posX >= 0)  this->posX = posX;
	if(posY >= 0)  this->posY = posY;
	if(sizeW >= 0) this->sizeW = sizeW;
	if(sizeH >= 0) this->sizeH = sizeH;
	setOptions();
}

void PlanetC_VideoPlayer::setVisible(bool b)
{
	//video->setOpacity(1.);
	video->setVisible(b);
}

void PlanetC_VideoPlayer::setRotation(float rotationAngle)
{
	this->rotationAngle = rotationAngle;
	setOptions();
}

void PlanetC_VideoPlayer::setOptions(QString dummy1, QVariant dummy2)
{
	setOptions();
}

void PlanetC_VideoPlayer::setOptions()
{
	//Check if geometry data are already available
	if(video->nativeSize().width() <= 0) return;

	//Get current View size in Scene coordinates
	//view->viewport();
	//view->viewport()->rect();
	QRectF viewbb = view->mapToScene(view->viewport()->rect()).boundingRect();

	//Compute video size in Scene coordinates.
	//Width and height are given as fraction of the entire View area
	float WW = 0;
	float HH = 0;
	if((sizeW > 0) && (sizeH > 0))
	{
		WW = sizeW * viewbb.width();
		HH = sizeH * viewbb.height();
	}
	else if(sizeW > 0)
	{
		WW = sizeW * viewbb.width();
		HH = WW * video->nativeSize().height() / video->nativeSize().width();
	}
	else if(sizeH > 0)
	{
		HH = sizeH * viewbb.height();
		WW = HH * video->nativeSize().width() / video->nativeSize().height();
	}

	if((WW > 0) && (HH > 0))
	{
		video->setSize(QSize(WW, HH));
	}
	else
		qDebug() << "WARNING: no size specified for video";

	//Rotation occurs wrt the OriginPoint.  Set the latter at the center of Item.
	video->setTransformOriginPoint(video->boundingRect().center());
	video->setRotation(rotationAngle);

	//Set video at the center of the view
	//QPointF a = view->mapToScene( view->viewport()->rect().center() );
	//QPointF b = video->transformOriginPoint();
	QPointF pos(posX * viewbb.width(), posY * viewbb.height());
	video->setPos(pos - video->transformOriginPoint());
}

