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

#ifndef _PLANETC_UI_HPP_
#define _PLANETC_UI_HPP_

#include <QOpenGLWidget>
#include <QOpenGLFramebufferObject>

#include "PlanetC_DateTimeDialog.hpp"
#include "PlanetC_VideoPlayer.hpp"
#include "ScreenImageMgr.hpp"
#include "ConfigurationDialog.hpp"
#include "LocationDialog.hpp"
#include "ViewDialog.hpp"
#include "SearchDialog.hpp"
#include "ui_searchDialogGui.h"

#include "PlanetC.hpp"
#include "PlanetC_Settings.hpp"

#include "ui_planetc_UI.h"
class Ui_PlanetC_UI_Form;




/*
  A simplemacro to define the classes for the Stellarium dialogs
  used in PlanetC.

  NOTE: MOC do not allow to define signal/slots using this macro,
  hence the "window" member is public.  It will be used to connect
  signals to the button in the main PlanetC window.

  Also, when clicking on the "closeStelButton" in the dialog the whole
  window should disappear, not just the "dialog" widget.  Therefore I
  modified the StelDialog::close as virtual.

  Finally, I modified the BarFrame class to allow move the whole
  window when grabbing the title bar.
 */
#define PLANETC_DIALOG(name)                                       \
class PlanetC_##name: public name                                  \
{                                                                  \
        Q_OBJECT                                                   \
        friend class PlanetC_UI;                                   \
public:                                                            \
        PlanetC_##name(QWidget* parent) : name(parent) {           \
                window = new PlanetC_Dialog(parent);               \
                dialog = new QWidget(window);                      \
                createDialogContent();                             \
                window->setCentralWidget(dialog);                  \
        }                                                          \
        ~PlanetC_##name() {                                        \
                delete dialog;                                     \
                delete window;                                     \
        }                                                          \
        PlanetC_Dialog* window;                                    \
                                                                   \
protected:                                                         \
        void close() {                                             \
                window->setVisible(false);                         \
        }                                                          \
}


/*
   The ConfigurationDialog class requires two parameters in the
   constructor, the first being simply the address of the StelGui
   object.

   The following class wraps the ConfigurationDialog class and require
   only one parameter, like the other dialog used here, hence it can
   be used with the PLANETC_DIALOG macro.
 */
class myConfigurationDialog : public ConfigurationDialog
{
public:
	myConfigurationDialog(QWidget* parent) :
		ConfigurationDialog(dynamic_cast<StelGui*>(StelApp::getInstance().getGui()),
		                    parent)
	{}
};


//Definition of the PlanetC_* classes to handle Stellarium dialogs.
PLANETC_DIALOG(ViewDialog);
PLANETC_DIALOG(LocationDialog);
PLANETC_DIALOG(SearchDialog);
PLANETC_DIALOG(myConfigurationDialog);




//====================================================================
/*!
  class gStopWatch
 */

class gStopWatch : public QObject
{
	Q_OBJECT

private:
	bool running;
	double elapsedPast;
	QElapsedTimer timer;
	double elapsedCurrent()
	{
		return timer.nsecsElapsed() / ((double) 1.e9);
	}

public:
	gStopWatch()
	{
		elapsedPast = 0;
		running = false;
	}

	bool isRunning()
	{
		return running;
	}

	double elapsed()
	{
		if(running)
			return elapsedPast + elapsedCurrent();
		else
			return elapsedPast;
	}

	QString elapsedTime()
	{
		QTime time(0, 0, 0);
		time = time.addMSecs(elapsed() * 1000.);
		return time.toString();
	}

public slots:
	void start()
	{
		timer.start();
		running = true;
	}

	void stop()
	{
		if(running)
		{
			elapsedPast += elapsedCurrent();
			running = false;
		}
	}

	void toggle()
	{
		if(running)
			stop();
		else
			start();
	}

	void reset()
	{
		elapsedPast = 0;
		start();
	}
};




#include <QOpenGLWidget>

class PlanetC_GLWidget : public QOpenGLWidget
{
	Q_OBJECT
	friend class PlanetC_QGraphicsView;

public:
	PlanetC_GLWidget(class PlanetC_UI*, QWidget*);
	~PlanetC_GLWidget() {}
	void cloneView(QOpenGLFramebufferObject* cloneFBO);

protected:
	virtual void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE {}
	void paintGL() Q_DECL_OVERRIDE {}

private:
	class PlanetC_UI* UI;
	bool sizeUpdated;
	QRect from;   //rectangle of main Stellarium view
	QRectF fromF; //rectangle of main Stellarium view normalized in the range 0:1
	QRect to;     //rectangle of the currently used area of this widget
};



/*!
  class PlanetC_UI

*/
class PlanetC_UI : public QMainWindow
{
	Q_OBJECT
	friend class PlanetC;

public:
	PlanetC_UI(PlanetC*);
	~PlanetC_UI();

	//! Reads a CSS file and scale of pixel size according to the given sale
	//! @param Scale Numerical factor to scale sizes
	static QString scaleStyleSheet(QString cssFileName, float scale);
	PlanetC* getPlanetC()
	{
		return planetc;
	}
	inline bool getDomeMode()
	{
		return domeMode;
	}

	void getViewAltAz(float&, float&);
	void getObjAltAz(const QSharedPointerNoDelete<StelObject>&, float&, float&);
	Ui_PlanetC_UI_Form* getUI()
	{
		return ui;
	}
	PlanetC_Settings* getOpt()
	{
		return pOpt;
	}

	void setPointerPos(float x = -1, float y = -1)
	{
		pointerX = x;
		pointerY = y;
	}

public slots:
	//! Change the pixel scale size of all elements described in
	//! the PlanetC style sheets.  This applies to both the
	//! "normal" and "night" styles.
	void setUIStyle(double scale=0);

	void setUIRefreshRate(double rate);

protected:
	void keyPressEvent(QKeyEvent* event);


private slots:
	void setDomeMode(bool);
	void saveState();
	void restoreState();
	void resetView();
	void setTimeNow();
	void setTimeNormalRate();
	void stopTime();
	void setTimeRelative(int sign=1);
	void setTimeRelativeMinus();
	void setTimeRate(int);
	void setCustomTimeRate();
	void showTimeRate(double);
	void enableMouseKeys(bool);
	void setLocation();
	void incrStarTwinkle(bool);
	void toggleMeteor(bool);
	void toggleMeteorRadiant(bool);
	void toggleMeteorIncrRate(bool);

	void autoZoom();
	void sliZoomMoved(int);

	void scriptPlay();
	void videoPlay();
	void videoStop();
	void imageShow();
	void imageHide();
	void imageNext();

	void setImageAlpha(int);
	void setImageSize(int);
	void setVideoSize(int);
	void setVideoVolume(int);
	void setVideoPos(int pos=0);
	void updateVideoCurTime(qint64);
	void updateVideoStatus(int);

	void updateBtn_ScriptRunning();
	void updateBtn_ScriptStopped();
	void updateBtn_ScriptPaused();

	void quickSelObject();
	void quickSelConstel();

	void selectNone();
	void setCurrentDateAsStartDate();

	void setFullScreen(bool);

	void updateFromTimer();
	void update(double delta);
	void draw();

	void closeEvent(QCloseEvent *event);

	void selectedObjChanged(StelModule::StelModuleSelectAction);
	void openUserDir();
	void refreshUserDir();
	void saveScreenshot();

private:
	PlanetC* planetc;
	struct StelModulesP stel;
	Ui_PlanetC_UI_Form* ui;
	PlanetC_Settings* pOpt;
	PlanetC_GLWidget* glWidget;
	gStopWatch watch;
	bool domeMode;

	//Timers
	QTimer timer;

	//Auto Zoom
	float autoZoomAlt, autoZoomAz, autoZoomFOV;

	//Mouse pointer
	StelTextureSP texPointer;
	float pointerX;
	float pointerY;

	ScreenImage* image[2];
	PlanetC_VideoPlayer* video;
	QMap<QWidget*, float> mapCustomTimeRate;

	PlanetC_DateTimeDialog* dateTimeDialog;
	PlanetC_SearchDialog* searchdialog;
	PlanetC_ViewDialog* viewdialog;
	PlanetC_myConfigurationDialog* configurationdialog;
	PlanetC_LocationDialog* locationdialog;

	float currentFOV;
	QString currentObjInfo;
	QString currentLocDateTime;
	bool flagHomeLocation;
	QMap<QWidget*, QString> mapLocations;

	QMap<QWidget*, QString> mapQuickSel;
	void addQuickButtons(QString pattern, QVBoxLayout* grid, const char* slot);
	void addLocationButtons();
	void refreshUserDirList(QListWidget* list, QString path);
};


#endif /* _PLANETC_UI_HPP_ */
