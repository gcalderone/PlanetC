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



/*
  This file contains the definition of widget subclasses used in PlanetC.
 */


#ifndef _PLANETC_WIDGETS_HPP_
#define _PLANETC_WIDGETS_HPP_

#include <QDial>
#include <QMainWindow>
#include <QTimer>
#include <math.h>




/*
   This class is used in all PlanetC dialog.  It simply inherits
   QMainWindow, and catch the "show" events to put the window above
   the main PlanetC window (to avoid disturbing the main Stellarium
   window) and set the window as modal.

   NOTE: the code is ready to show the dialogs as "frameless" window
   (by passing Qt::FramelessWindowHint in the constructor).  In this
   case the window can be moved using the "title bar" defined in the
   Stellarium dialogs.  However this results in a very strange
   behaviour of the window, likely caused by the QGLWidget at the
   center of the main PlanetC window.

   I don't know how to fix this, hence I keep the border in the
   dialogs.  The title bar is automatically disabled in the
   BarFrame::mousePressEvent() routine.
 */
class PlanetC_Dialog : public QMainWindow
{
	Q_OBJECT

public:
	PlanetC_Dialog(QWidget* parent) :
		QMainWindow(parent,
					Qt::WindowStaysOnTopHint)
	{}

	void showEvent(QShowEvent * event) {
		//Move the dialog to cover the PlanetC window.
		move(((QMainWindow*) parent())->geometry().topLeft() + QPoint(50, 50));

		//Set the window as modal otherwise it will go to
		//background when it looses the focus and  the main
		//PlanetC window is in fullscreen mode.
		setWindowModality(Qt::ApplicationModal);
	}
};





/*
 */
class PlanetC_Dial : public QDial
{
	Q_OBJECT

public:
	PlanetC_Dial(QWidget* parent = NULL) : QDial(parent) 
	{
		setTracking(false);     //emit signal while being dragged
		setMouseTracking(true); //receives signals only when at least one button is pressed
		timerShot = 0;
		reqValue = value();
		setStyleSheet("background: rgb(30, 20, 20);");
	}

public slots:
	void setValue(int v) 
	{
		//float c = ((float) v - minimum()) / (maximum() - minimum()) * 255.;
		//if (invertedAppearance()) c = 255 - c;
		//QString s = "background: rgb(" +
		//        QString::number(0.5 * c, 'f', 0) + ", " +
		//        QString::number(0.3 * c, 'f', 0) + ", 0);";
		//setStyleSheet(s);
		QDial::setValue(v);
		if(timerShot == 0)
			{
				reqValue = v;
			}
	}

protected slots:
	void wheelEvent(QWheelEvent* e)
	{
		if(!isEnabled()) return;
		e->accept();

		float delta = (e->angleDelta()).y() / 8.;
		if(invertedControls()) delta *= -1;
		reqValue += singleStep() * delta;
		if(reqValue > maximum()) reqValue = maximum();
		if(reqValue < minimum()) reqValue = minimum();

		timerShot += 1;
		QTimer::singleShot(1000, this, SLOT(reset()));
		emit valueChangeRequested( reqValue );
	}


	//Reimplement these method to avoid changing value by click and keyboard
	void mousePressEvent(QMouseEvent* e)   {}
	void mouseReleaseEvent(QMouseEvent* e) {}
	void keyPressEvent(QKeyEvent* e)       {}
	void keyReleaseEvent(QKeyEvent* e)     {}
	//void mouseMoveEvent(QMouseEvent* e) {}

private slots:
	void reset()
	{
		timerShot -= 1;
		if(timerShot == 0)
			{
				reqValue = value();
				clearFocus();
			}
	}

private:
	int reqValue;
	int timerShot;

signals:
	void valueChangeRequested(int);
};




/*
 */
class PlanetC_sliZoom : public PlanetC_Dial {
public:
	PlanetC_sliZoom(QWidget* parent = NULL) : PlanetC_Dial(parent)
	{
		setMinimum(100);
		setMaximum(3e3);
		setSingleStep(1);
		setNotchTarget(100);
	}

public:
	float uvalue(int v)
	{
		float vExt = 180. * pow(((float) v) / maximum(), 4.);
		return vExt;
	}

	void setUValue(float v)
	{
		float vInt = maximum() * pow(((float) v) / 180., 0.25);
		setValue(vInt);
	}
};



/*
 */
class PlanetC_sliTimeRate : public PlanetC_Dial
{
public:
	PlanetC_sliTimeRate(QWidget* parent = NULL) : PlanetC_Dial(parent) {
		setMinimum(-600);
		setMaximum( 600);
		setSingleStep(10);
		setNotchTarget(5);
	}

public:
	float uvalue(int v)
	{
		//float vExt = pow((double) 10., v/100.);

		if (v == 0) return 0;
		if (abs(v) == 1) return v;
		float sign = (v > 0  ?  1  : -1.);
		float vExt = sign * pow((double) 10., abs(v)/100.);

		return vExt;
	}

	void setUValue(float v)
	{
		//float vInt = ( log10(abs(v)) * 100. );
		//setValue(sign * vInt);

		if(abs(v) < 1)
			{
				setValue(0);
				return;
			}

		float sign = (v > 0  ?  1  : -1.);
		float vInt = ( log10(abs(v)) * 100. );
		setValue(sign * vInt);
	}
};

#endif // _PLANETC_WIDGETS_HPP_
