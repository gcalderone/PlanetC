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

#ifndef _PLANETC_VIDEOPLAYER_HPP_
#define _PLANETC_VIDEOPLAYER_HPP_

#include <QObject>
#include <QMap>
#include <QString>
#include <QMediaPlayer>
#include <QtGui/QMovie>
#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QGraphicsVideoItem>
#include <QVideoSurfaceFormat>


class PlanetC_VideoPlayer : public QMediaPlayer
{
	Q_OBJECT

public:
	PlanetC_VideoPlayer(QGraphicsView* p_view);
	~PlanetC_VideoPlayer();

	void openFile(const QString& filename);
	void close();

	void setGeometry(float posX=-1, float posY=-1, float sizeW=-1, float sizeH=-1);
	void setVisible(bool b);
	void setRotation(float rotationAngle);

private slots:
	void setOptions(QString,QVariant);

private:
	QGraphicsView* view;
	QGraphicsVideoItem* video;

	float posX;
	float posY;
	float sizeW;
	float sizeH;
	float rotationAngle;
	void setOptions();
};



#endif // _PLANETC_VIDEOPLAYER_HPP_
