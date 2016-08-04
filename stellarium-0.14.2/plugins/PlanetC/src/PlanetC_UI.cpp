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


#include <QtGlobal>

#ifdef Q_OS_WIN
//The following is only required on windows, and must be included
//before all the other #include.  Copied from the Scenery3d plugin.

#include <QOpenGLContext>
#include <QOpenGLFunctions_1_0>

#ifndef QT_OPENGL_ES_2
class myGL: public QOpenGLFunctions_1_0
{
public:
	void init(QOpenGLContext* ctx)
	{
		if(!ctx->isOpenGLES())
			initializeOpenGLFunctions();
	}
};

myGL* mygl = NULL;
#endif
#endif

#include "PlanetC_UI.hpp"

PlanetC_QGLWidget::PlanetC_QGLWidget(PlanetC_UI* UI, QWidget* parent, QGLWidget* share) : QGLWidget(parent, share)
{
	this->UI = UI;
	firstResize = false;
	cloneFBO = NULL;
	setMouseTracking(true);

	QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sp.setHorizontalStretch(0);
	sp.setVerticalStretch(0);
	sp.setHeightForWidth(sizePolicy().hasHeightForWidth());
	setSizePolicy(sp);
}

void PlanetC_QGLWidget::updateFBO(QOpenGLFramebufferObject* cloneFBO)
{
	this->cloneFBO = cloneFBO;
	updateGL();
}

void PlanetC_QGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	float x = event->pos().x();
	float y = height() - event->pos().y();
	x = (x - to.left()) / ((float) to.width()) * from.width() + from.left();
	y = (y - to.top()) / ((float) to.height()) * from.height() + from.top();
	UI->setPointerPos(x, y);
}

void PlanetC_QGLWidget::leaveEvent(QEvent* event)
{
	UI->setPointerPos();
}

void PlanetC_QGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if(event->button() == Qt::RightButton)
	{
		QPushButton* btn = UI->getUI()->btnPointer;
		btn->setChecked(!btn->isChecked());
	}
}


void PlanetC_QGLWidget::initializeGL()
{
#ifdef Q_OS_WIN
	QOpenGLContext* ctx = QOpenGLContext::currentContext();
#ifndef QT_OPENGL_ES_2
	mygl = new myGL();
	mygl->init(ctx);
#endif
#endif
}

void PlanetC_QGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);

#ifdef Q_OS_WIN
	if(!mygl)
	{
		qDebug() << __FUNCTION__;
		qDebug() << "mygl not istantiayed";
		return;
	}
	mygl->glMatrixMode(GL_PROJECTION);
	mygl->glLoadIdentity();
	mygl->glOrtho(0, width, height, 0, 1, -1);
	mygl->glMatrixMode(GL_MODELVIEW);
	mygl->glLoadIdentity();
#else
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif
	glEnable(GL_TEXTURE_2D);

	firstResize = true;
}

void PlanetC_QGLWidget::paintGL()
{
	//wait until the widget is first shown and appropriately sized
	if(!firstResize) return;
	if(!cloneFBO) return;

	//Initialize "from" and "fromF" rectangle
	from.setRect(0, 0, cloneFBO->size().width(), cloneFBO->size().height());
	QRectF fromF(0, 0, 1, 1); //normalized "from" rectangle (in the range 0:1)
	if(UI->getDomeMode())
	{
		//Consider only the central squared region of the main view
		if(from.width() > from.height())
		{
			int offset = (from.width() - from.height()) / 2.;
			fromF.moveLeft(offset   / ((float) from.width()));
			fromF.setWidth(from.height() / ((float) from.width()));
			from.moveLeft(offset);
			from.setWidth(from.height());
		}
		else
		{
			int offset = (from.height() - from.width()) / 2.;
			fromF.moveTop(offset  / ((float) from.height()));
			fromF.setHeight(from.width() / ((float) from.height()));
			from.moveTop(offset);
			from.setHeight(from.width());
		}
	}

	//The cloned view should have the same aspect ratio as the main view
	to = rect();
	float fromRatio = from.width() / ((float) from.height());
	float toRatio   =   to.width() / ((float)   to.height());

	if(fromRatio > toRatio)
	{
		to.setHeight(to.width() / fromRatio);
		to.moveTop((fromRatio / toRatio - 1) * to.height() / 2.);
	}
	else
	{
		to.moveLeft((1 - fromRatio / toRatio) * to.width() / 2.);
		to.setWidth(to.height() * fromRatio);
	}


	//Draw texture on the widget
	qglClearColor(Qt::black); //(Qt::blue);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, cloneFBO->texture());
#ifdef Q_OS_WIN
	mygl->glColor3f(1, 1, 1);
	mygl->glBegin(GL_QUADS);
	mygl->glTexCoord2f(fromF.left() , fromF.bottom());
	mygl->glVertex2f(to.left() , to.top());
	mygl->glTexCoord2f(fromF.right(), fromF.bottom());
	mygl->glVertex2f(to.right(), to.top());
	mygl->glTexCoord2f(fromF.right(), fromF.top());
	mygl->glVertex2f(to.right(), to.bottom());
	mygl->glTexCoord2f(fromF.left() , fromF.top());
	mygl->glVertex2f(to.left() , to.bottom());
	mygl->glEnd();
#else
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(fromF.left() , fromF.bottom());
	glVertex2f(to.left() , to.top());    //glTexCoord2f(0, 1); glVertex2f(      0,        0);
	glTexCoord2f(fromF.right(), fromF.bottom());
	glVertex2f(to.right(), to.top());    //glTexCoord2f(1, 1); glVertex2f(width(),        0);
	glTexCoord2f(fromF.right(), fromF.top());
	glVertex2f(to.right(), to.bottom()); //glTexCoord2f(1, 0); glVertex2f(width(), height());
	glTexCoord2f(fromF.left() , fromF.top());
	glVertex2f(to.left() , to.bottom()); //glTexCoord2f(0, 0); glVertex2f(      0, height());
	glEnd();
#endif
}




#define PLANETC_MINFPS 25
#define PLANETC_MAXFPS 200

PlanetC_UI::PlanetC_UI(PlanetC* planetc) : QMainWindow(), glWidget(NULL), timer(this)
{
	this->planetc = planetc;
	stel = PlanetC::getStelModules();

	//Setup PlanetC GUI
	ui = new Ui_PlanetC_UI_Form();
	ui->setupUi(this);
	ui->stelWindowTitle->setText("PlanetC  (v" PLANETC_PLUGIN_VERSION ")");
	setWindowIcon(QIcon(":/PlanetC/bt_PlanetC_On.png"));

	image[0] = NULL;
	image[1] = NULL;
	video[0] = new PlanetC_VideoPlayer(stel.view);
	video[1] = new PlanetC_VideoPlayer(stel.view);


	addLocationButtons();
	ui->btnVideoPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
	ui->btnVideoStop->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));

	addQuickButtons("planetc_quickselect_object", ui->layoutQuickSel, "1quickSelObject()");
	addQuickButtons("planetc_quickselect_constellation", ui->layoutQuickConstell, "1quickSelConstel()");

	//Setup clone view in PlanetC_QGLWidget
	glWidget = new PlanetC_QGLWidget(this, ui->frameMiddle, (QGLWidget*) stel.view->viewport());
	ui->layoutCloneView->addWidget(glWidget);


	//Set the remaining variables
	mapCustomTimeRate[ui->btnTimeRate1] = 1.e1;
	mapCustomTimeRate[ui->btnTimeRate2] = 1.e2;
	mapCustomTimeRate[ui->btnTimeRate3] = 1.e3;
	mapCustomTimeRate[ui->btnTimeRate4] = 1.e4;
	domeMode = false;


	//Setup PlanetC settings GUI
	pOpt = new PlanetC_Settings(this);

	//Create dialog windows.
	dateTimeDialog = new PlanetC_DateTimeDialog(NULL, ui->layoutDateTime);
	viewdialog = new PlanetC_ViewDialog(this);
	searchdialog = new PlanetC_SearchDialog(this);
	configurationdialog = new PlanetC_myConfigurationDialog(this);
	locationdialog = new PlanetC_LocationDialog(this);


	//Connect signals
	connect(pOpt->ui->btnSetStartDate, SIGNAL(clicked())      , this          , SLOT(setCurrentDateAsStartDate()));
	connect(pOpt->ui->spinUIScale  , SIGNAL(valueChanged(double)), this       , SLOT(setUIStyle(double)));
	connect(ui->btnOpenUserDir     , SIGNAL(clicked())        , this          , SLOT(openUserDir()));
	connect(ui->btnRefreshUserDir  , SIGNAL(clicked())        , this          , SLOT(refreshUserDir()));

	connect(ui->btnDialogPlanetCOpt, SIGNAL(clicked())        , pOpt                       , SLOT(show()));
	connect(ui->btnDialogLocation  , SIGNAL(clicked())        , locationdialog->window     , SLOT(show()));
	connect(ui->btnDialogConfig    , SIGNAL(clicked())        , configurationdialog->window, SLOT(show()));
	connect(ui->btnDialogView      , SIGNAL(clicked())        , viewdialog->window         , SLOT(show()));
	connect(ui->btnDialogSearch    , SIGNAL(clicked())        , searchdialog->window       , SLOT(show()));

	connect(ui->btnMouseKeys       , SIGNAL(toggled(bool))    , this          , SLOT(enableMouseKeys(bool)));
	connect(ui->btnFullScreen      , SIGNAL(toggled(bool))    , this          , SLOT(setFullScreen(bool)));

	connect(ui->btnTimerToggle     , SIGNAL(clicked())        , &watch        , SLOT(toggle()));
	connect(ui->btnTimerReset      , SIGNAL(clicked())        , &watch        , SLOT(reset()));
	connect(ui->btnQuit            , SIGNAL(clicked())        , stel.app      , SLOT(quit()));
#ifdef Q_OS_WIN
        //On windows the QFileDialog::getSaveFileName method fails with a SIGSEGV, hence we disable the button until we understand what's going on.
        ui->btnScreenshot->setEnabled(false);
#else
        connect(ui->btnScreenshot      , SIGNAL(clicked())        , this          , SLOT(saveScreenshot()));
#endif

	connect(ui->btnInit            , SIGNAL(toggled(bool))    , this          , SLOT(setDomeMode(bool)));
	connect(ui->btnResetView       , SIGNAL(clicked())        , this          , SLOT(resetView()));
	connect(ui->btnSaveState       , SIGNAL(clicked())        , this          , SLOT(saveState()));
	connect(ui->btnRestoreState    , SIGNAL(clicked())        , this          , SLOT(restoreState()));

	connect(ui->btnTimeNow         , SIGNAL(clicked())        , this          , SLOT(setTimeNow()));
	connect(ui->btnTimeStop        , SIGNAL(clicked())        , this          , SLOT(stopTime()));
	connect(ui->btnTimeRate0       , SIGNAL(clicked())        , this          , SLOT(setTimeNormalRate()));
	connect(ui->btnTimeRate1       , SIGNAL(clicked())        , this          , SLOT(setCustomTimeRate()));
	connect(ui->btnTimeRate2       , SIGNAL(clicked())        , this          , SLOT(setCustomTimeRate()));
	connect(ui->btnTimeRate3       , SIGNAL(clicked())        , this          , SLOT(setCustomTimeRate()));
	connect(ui->btnTimeRate4       , SIGNAL(clicked())        , this          , SLOT(setCustomTimeRate()));

	connect(ui->btnTimeRelPlus     , SIGNAL(clicked())        , this          , SLOT(setTimeRelative()));
	connect(ui->btnTimeRelMinus    , SIGNAL(clicked())        , this          , SLOT(setTimeRelativeMinus()));

	connect(ui->sliTimeRate        , SIGNAL(valueChangeRequested(int)), this  , SLOT(setTimeRate(int)));
	connect(stel.core              , SIGNAL(timeRateChanged(double)), this    , SLOT(showTimeRate(double)));


	connect(ui->btnAtmosphere      , SIGNAL(toggled(bool))    , stel.landscape, SLOT(setFlagAtmosphere(bool)));
	connect(ui->btnGround          , SIGNAL(toggled(bool))    , stel.landscape, SLOT(setFlagLandscape(bool)));
	connect(ui->btnFog             , SIGNAL(toggled(bool))    , stel.landscape, SLOT(setFlagFog(bool)));

	connect(ui->btnStars           , SIGNAL(toggled(bool))    , stel.starmgr  , SLOT(setFlagStars(bool)));
	connect(ui->btnStarNames       , SIGNAL(toggled(bool))    , stel.starmgr  , SLOT(setFlagLabels(bool)));
	connect(ui->btnStarTwinkle     , SIGNAL(toggled(bool))    , this          , SLOT(incrStarTwinkle(bool)));
	//connect(ui->btnNebulae         , SIGNAL(toggled(bool))    , stel.nebulamgr, SLOT(setFlagShow(bool)));
	stel.nebulamgr->setFlagShow(true); //
	connect(ui->btnNebNames        , SIGNAL(toggled(bool))    , stel.nebulamgr, SLOT(setFlagShow(bool)));
	connect(ui->btnNebNames        , SIGNAL(toggled(bool))    , stel.nebulamgr, SLOT(setFlagHints(bool)));
	connect(ui->btnSkyImage        , SIGNAL(toggled(bool))    , stel.skylayer , SLOT(setFlagShow(bool)));



	connect(ui->btnPlanets         , SIGNAL(toggled(bool))    , stel.solar    , SLOT(setFlagPlanets(bool)));
	connect(ui->btnPlanetNames     , SIGNAL(toggled(bool))    , stel.solar    , SLOT(setFlagLabels(bool)));
	connect(ui->btnPlanetOrbits    , SIGNAL(toggled(bool))    , stel.solar    , SLOT(setFlagOrbits(bool)));

	connect(ui->btnMeteor          , SIGNAL(toggled(bool))    , this          , SLOT(toggleMeteor(bool)));
	connect(ui->btnMeteorRadiant   , SIGNAL(toggled(bool))    , this          , SLOT(toggleMeteorRadiant(bool)));
	connect(ui->btnMeteorIncrRate  , SIGNAL(toggled(bool))    , this          , SLOT(toggleMeteorIncrRate(bool)));

	connect(ui->btnSatellites     , SIGNAL(toggled(bool))     , stel.satel    , SLOT(setFlagHints(bool)));
	connect(ui->btnSatelliteNames , SIGNAL(toggled(bool))     , stel.satel    , SLOT(setFlagLabels(bool)));
	connect(ui->btnSatelliteTrails, SIGNAL(toggled(bool))     , stel.satel    , SLOT(setOrbitLinesFlag(bool)));

	connect(ui->btnConstel         , SIGNAL(toggled(bool))    , stel.constmgr , SLOT(setFlagLines(bool)));
	connect(ui->btnConstelNames    , SIGNAL(toggled(bool))    , stel.constmgr , SLOT(setFlagLabels(bool)));
	connect(ui->btnConstelArt      , SIGNAL(toggled(bool))    , stel.constmgr , SLOT(setFlagArt(bool)));
	connect(ui->btnConstelBound    , SIGNAL(toggled(bool))    , stel.constmgr , SLOT(setFlagBoundaries(bool)));

	//connect(ui->btnGridRaDec       , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEquatorJ2000Grid(bool)));
	connect(ui->btnGridRaDec       , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEquatorGrid(bool)));
	connect(ui->btnGridAltAz       , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagAzimuthalGrid(bool)));
	//connect(ui->btnGridGal        , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagGalacticGrid(bool)));
	//connect(ui->btnGridEcliptic    , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEclipticJ2000Grid(bool)));
	connect(ui->btnGridEcliptic    , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEclipticGrid(bool)));
	connect(ui->btnCardPoints      , SIGNAL(toggled(bool))    , stel.landscape, SLOT(setFlagCardinalsPoints(bool)));

	connect(ui->btnEclipticLine    , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEclipticLine(bool)));
	//connect(ui->btnEquatorLine     , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEquatorJ2000Line(bool)));
	connect(ui->btnEquatorLine     , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagEquatorLine(bool)));
	connect(ui->btnMeridianLine    , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagMeridianLine(bool)));
	connect(ui->btnPrecCircles     , SIGNAL(toggled(bool))    , stel.gridmgr  , SLOT(setFlagPrecessionCircles(bool)));
	connect(ui->btnAngle           , SIGNAL(toggled(bool))    , stel.anglemeas, SLOT(enableAngleMeasure(bool)));
	//connect(ui->btnMilky         , SIGNAL(toggled(bool))    , stel.milky    , SLOT(setFlagShow(bool)));
	connect(ui->btnEqMount         , SIGNAL(toggled(bool))    , stel.mvmgr    , SLOT(setEquatorialMount(bool)));

	connect(ui->btnZoom            , SIGNAL(clicked())        , this          , SLOT(autoZoom()));
	connect(ui->sliZoom            , SIGNAL(valueChangeRequested(int)), this  , SLOT(sliZoomMoved(int)));


	connect(stel.scrmgr            , SIGNAL(scriptRunning())  , this          , SLOT(updateBtn_ScriptRunning()));
	connect(stel.scrmgr            , SIGNAL(scriptStopped())  , this          , SLOT(updateBtn_ScriptStopped()));
	connect(stel.scrmgr            , SIGNAL(scriptPaused())   , this          , SLOT(updateBtn_ScriptPaused()));

	connect(ui->btnScriptPlay      , SIGNAL(clicked())        , this          , SLOT(scriptPlay()));
	connect(ui->btnScriptNext      , SIGNAL(clicked())        , stel.scrmgr   , SLOT(resumeScript()));
	connect(ui->btnScriptNext      , SIGNAL(clicked())        , this          , SLOT(updateBtn_ScriptRunning()));
	connect(ui->btnScriptStop      , SIGNAL(clicked())        , stel.scrmgr   , SLOT(stopScript()));

	connect(ui->btnImageShow       , SIGNAL(clicked())        , this          , SLOT(imageShow()));
	connect(ui->btnImageNext       , SIGNAL(clicked())        , this          , SLOT(imageNext()));
	connect(ui->btnImageHide       , SIGNAL(clicked())        , this          , SLOT(imageHide()));
	connect(ui->btnSelectNone      , SIGNAL(clicked())        , this          , SLOT(selectNone()));
	connect(ui->btnTrack           , SIGNAL(toggled(bool))    , stel.mvmgr    , SLOT(setFlagTracking(bool)));
	connect(ui->btnVideoPlay       , SIGNAL(clicked())        , this          , SLOT(videoPlay()));
	connect(ui->btnVideoStop       , SIGNAL(clicked())        , this          , SLOT(videoStop()));

	connect(ui->sliVideoSize       , SIGNAL(valueChanged(int)), this          , SLOT(setVideoSize(int)));
	connect(ui->sliVideoVolume     , SIGNAL(valueChanged(int)), this          , SLOT(setVideoVolume(int)));
	connect(ui->sliImageSize       , SIGNAL(valueChanged(int)), this          , SLOT(setImageSize(int)));
	connect(ui->sliImageTransp     , SIGNAL(valueChanged(int)), this          , SLOT(setImageAlpha(int)));

	connect(&timer                 , SIGNAL(timeout())        , this          , SLOT(update()));

	connect(stel.objmgr            , SIGNAL(selectedObjectChanged(StelModule::StelModuleSelectAction)),
	        this, SLOT(selectedObjChanged(StelModule::StelModuleSelectAction)));

	//Fill the lists with User dir content
	refreshUserDir();

	timer.start(200);
	watch.start();
	setTimeRate(0);

	pointerX = -1;
	pointerY = -1;
	texPointer =
	    StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/telescope.png");

	setUIStyle();

	//Set min FPS for a smoother view
	stel.view->setMinFps(PLANETC_MINFPS);

	setDomeMode(false);
}


void PlanetC_UI::closeEvent(QCloseEvent *event)
{
	planetc->enablePlanetC(false);
}

PlanetC_UI::~PlanetC_UI()
{
	timer.stop();

	//Restore original min FPS
	stel.view->setMinFps(stel.conf->value("video/minimum_fps", 18).toFloat());

	//Destroy glWidget object
	QGLWidget* tmp = glWidget;
	glWidget = NULL;
	delete tmp;

	delete dateTimeDialog;
	delete locationdialog;
	delete configurationdialog;
	delete viewdialog;
	delete searchdialog;
	delete video[0];
	delete video[1];
	delete pOpt;
	delete ui;
}

void PlanetC_UI::keyPressEvent(QKeyEvent* event)
{
	switch(event->key())
	{
	case Qt::Key_Escape:
	case Qt::Key_Space:
		break; //ignored key events
	default:
		//Do not works: QCoreApplication::instance()->notify(stel.view, event);
		QMainWindow::keyPressEvent(event);
	}
}



void PlanetC_UI::setDomeMode(bool b)
{
	ui->btnShowClock->setEnabled(b);
	ui->btnShowDate->setEnabled(b);
	ui->btnShowLoc->setEnabled(b);
	ui->btnShowCard->setEnabled(b);
	ui->btnResetView->setEnabled(b);
	ui->btnCardPoints->setEnabled(!b);
	ui->btnEqMount->setEnabled(!b);
	ui->btnTrack->setEnabled(!b);

	ui->btnShowClock->setChecked(b);
	ui->btnShowDate->setChecked(b);
	ui->btnShowCard->setChecked(b);
	ui->btnCardPoints->setChecked(b);

	ui->btnVideoDouble->setChecked(b);
	ui->btnVideoDouble->setEnabled(b);
	ui->btnImageDouble->setChecked(b);
	ui->btnImageDouble->setEnabled(b);


	domeMode = b;

	if(b)
	{
		planetc->runScript("planetc_initializeDomeMode(\"" + pOpt->getStartDate() + "\");");
		enableMouseKeys(false);
		stel.landscape->setFlagCardinalsPoints(false);
		ui->btnMouseKeys->setChecked(false);
		ui->btnTrack->setChecked(false);
		flagHomeLocation = true;
	}
	else
	{
		ui->btnEqMount->setChecked(false);
		planetc->runScript("core.moveToAltAzi(20, 180 , 0);");
		stel.mvmgr->zoomTo(120, 0);
		stel.core->setMaskType(StelProjector::MaskNone);
		planetc->runScript("planetc_domeMode = 0;");
	}
}


void PlanetC_UI::selectedObjChanged(StelModule::StelModuleSelectAction)
{
	const QList<StelObjectP>& selected = stel.objmgr->getSelectedObject();
	if(selected.isEmpty())
	{
		ui->tabQuick->setCurrentIndex(0);
		ui->btnSelectNone->setEnabled(false);
		ui->btnZoom->setEnabled(false);
		ui->btnTrack->setEnabled(false);
	}
	else
	{
		ui->btnSelectNone->setEnabled(true);
		if(selected[0]->getType() != "Constellation")
		{
			//ui->tabQuick->setTabText(2, selected[0]->getNameI18n());
			ui->tabQuick->setCurrentIndex(2);
			ui->btnZoom->setEnabled(true);
			ui->btnTrack->setEnabled(true);
		}
	}
}



void PlanetC_UI::draw()
{
	if(video[0]->state() == QMediaPlayer::PlayingState)
		return; //to achieve the highest possible frame rate


	const StelProjectorP prj = stel.core->getProjection(StelCore::FrameAltAz, StelCore::RefractionOff);
	StelPainter sPainter(prj);

	//Draw a pointer in the main view
	if(pointerX > 0   &&
	        pointerY > 0   &&
	        ui->btnPointer->isChecked())
	{
		sPainter.setColor(0.8, 0.1, 0.1);
		texPointer->bind();
		sPainter.enableTexture2d(true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Normal transparency mode
		sPainter.drawSprite2dMode(pointerX, pointerY, 13.f, StelApp::getInstance().getTotalRunTime()*40.);
	}


	if(!domeMode) return;
	if(!ui->sliZoom->isEnabled()) return;  //nothing to draw while zooming
	if(currentFOV < 179) return;  //Nothig to draw if the horizon is not entirely visible


	Vec3f pos;
	Vec3f xy;
	float radius = stel.view->size().height() / 2.;
	float angle;
	float shift;
	static QFont placts_font;
	placts_font.setPixelSize(pOpt->getClockFontSize());
	sPainter.setFont(placts_font);



	//Draw location/date/time labels
	if(currentLocDateTime != "")
	{
		static Vec3f color = StelUtils::strToVec3f(stel.conf->value("planetc/clock_color", "0.9,0.5,0.1").toString());
		sPainter.setColor(color[0],color[1],color[2],0.8);

		shift = pOpt->getShiftClock();
		angle = sPainter.getFontMetrics().width(currentLocDateTime)/2. /
		        (radius*(1-shift/2.))  //effective radius
		        + 0.7854; //add PI/4.

		pos.set(sin(angle), -cos(angle), shift);
		if(prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], currentLocDateTime, 0., 0, 0, false);

		angle += 3.1416;
		pos.set(sin(angle), -cos(angle), shift);
		if(prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], currentLocDateTime, 0., 0, 0, false);
	}

	//Draw cardinal points
	if(ui->btnShowCard->isChecked())
	{
		static Vec3f color = StelUtils::strToVec3f(stel.conf->value("color/cardinal_color", "0.8,0.2,0.1").toString());
		sPainter.setColor(color[0],color[1],color[2]);

		QString d[4];
		//d[0] = q_("Nord");
		//d[1] = q_("Sud");
		//d[2] = q_("Est");
		//d[3] = q_("Ovest");
		d[0] = q_("North");
		d[1] = q_("South");
		d[2] = q_("East");
		d[3] = q_("West");

		shift = pOpt->getShiftCardPo();
		radius *= 1-shift/2.;

		angle = sPainter.getFontMetrics().width(d[0])/2. / radius - 1.5708;
		pos.set(sin(angle), -cos(angle), shift);
		if(prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[0], 0., 0, 0, false);

		angle = sPainter.getFontMetrics().width(d[1])/2. / radius + 1.5708;
		pos.set(sin(angle), -cos(angle), shift);
		if(prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[1], 0., 0, 0, false);

		angle = sPainter.getFontMetrics().width(d[2])/2. / radius - 3.1416;
		pos.set(sin(angle), -cos(angle), shift);
		if(prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[2], 0., 0, 0, false);

		angle = sPainter.getFontMetrics().width(d[3])/2. / radius;
		pos.set(sin(angle), -cos(angle), shift);
		if(prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[3], 0., 0, 0, false);
	}
}



void PlanetC_UI::update()
{
	if(!timer.isActive()) return;

	//Set scroll areas to show the whole horizontal size of their
	//contents
	ui->scrollLeft->setMinimumSize(ui->scrollLeftWidget->width() +
	                               ui->scrollLeft->verticalScrollBar()->width(),
	                               ui->scrollLeft->minimumSize().height());

	ui->scrollRight->setMinimumSize(ui->scrollRightWidget->width() +
	                                ui->scrollRight->verticalScrollBar()->width(),
	                                ui->scrollRight->minimumSize().height());


	//Apply options on video(s)
	if(video[0]->state() == QMediaPlayer::PlayingState)
	{
		//Continue to set geometry after 1.5 seconds of playing
		if(video[0]->position() < 1500)
		{
			video[0]->setGeometry();
			if(video[1]->state() == QMediaPlayer::PlayingState)
				video[1]->setGeometry();
		}
	}


	//Update stopwatch
	ui->labelTimer->setText("Timer: " + watch.elapsedTime());
	ui->btnTimerToggle->setText(watch.isRunning()   ?   "Pause"   :   "Go");

	//Update clock
	ui->labelClock->setText("Clock: " + QDateTime::currentDateTime().toString("hh:mm:ss"));


	//Update DateTimeDialog
	double jday = stel.core->getJD();
	dateTimeDialog->setDateTime(jday);
	QDateTime currentTime = dateTimeDialog->current();


	//Update currentLocDateTime
	StelLocation loc = stel.core->getCurrentLocation();
	QString location;
	if(!loc.name.isEmpty())
	{
		location = loc.name;
		ui->labelLocation->setText("Loc.: " + loc.name + " (" + loc.planetName + ")");
	}
	else
	{
		ui->labelLocation->setText("Loc.: " + loc.planetName);
		location = loc.planetName;
	}
	ui->labelCoord->setText("Coord: " +
	                        StelUtils::decDegToDmsStr(loc.latitude) + ", " +
	                        StelUtils::decDegToDmsStr(loc.longitude));

	currentLocDateTime = "";
	if(ui->btnShowLoc->isChecked())   currentLocDateTime += location;
	if(ui->btnShowDate->isChecked())  currentLocDateTime += currentTime.toString("  dd MMM yyyy");
	if(ui->btnShowClock->isChecked()) currentLocDateTime += currentTime.toString("  hh:mm");
	currentLocDateTime = currentLocDateTime.trimmed();


	//Update FOV
	static float lastFOV = -1;
	currentFOV = stel.core->getMovementMgr()->getCurrentFov();
	if(lastFOV != currentFOV)
	{
		ui->labelFOV->setText("FOV:" + StelUtils::decDegToDmsStr(currentFOV));
		ui->sliZoom->setUValue(currentFOV); //set slider position
		lastFOV = currentFOV;
	}


	//Update FPS
	ui->labelFPS->setText("FPS = " + QString::number(stel.app->getFps(), 'f', 1));


	//Display object info
	stel.objmgr->setFlagSelectedObjectPointer( ui->chkShowMarker->isChecked() );
	const QList<StelObjectP>& selected = stel.objmgr->getSelectedObject();
	if(selected.isEmpty())
	{
		ui->txtObjInfo->setHtml("");
		currentObjInfo = "";
		ui->chkShowMarker->setEnabled(false);
		ui->labelSelectedObject->setText("none");
		ui->labelSelectedObject->setStyleSheet("font: bold;");
		ui->labelVisibility->setText("");
	}
	else
	{
		ui->chkShowMarker->setEnabled(true);
		QString s = selected[0]->getInfoString(stel.core, StelObject::InfoStringGroup(StelObject::AllInfo));
		if(s != currentObjInfo)
		{
			ui->labelSelectedObject->setText( selected[0]->getNameI18n() + " (" + selected[0]->getType() + ")");
			ui->labelSelectedObject->setStyleSheet("font: bold;");

			currentObjInfo = s;
			//During daylight the font color is black against a dark background. Change color to white.
			if(stel.core->isBrightDaylight()   &&
			        stel.core->getSkyDrawer()->getFlagHasAtmosphere()==true)
				s.replace(13, 6, "eeeeee");
			ui->txtObjInfo->setHtml(s);

			float alt, az;
			getObjAltAz(selected[0], alt, az);
			if(alt > 10)
			{
				ui->labelVisibility->setText("Object is above the horizon");
				ui->labelVisibility->setStyleSheet("color : rgb(130, 180, 180);");
			}
			else if(alt > 0)
			{
				ui->labelVisibility->setText("Object is close to the horizon");
				ui->labelVisibility->setStyleSheet("color : rgb(150, 150, 0);");
			}
			else
			{
				ui->labelVisibility->setText("Object is not visible");
				ui->labelVisibility->setStyleSheet("color : rgb(150, 0, 0);");
			}
		}
	}


	//Update button state
	ui->btnAtmosphere->setChecked(stel.landscape->getFlagAtmosphere());
	ui->btnGround->setChecked(stel.landscape->getFlagLandscape());
	ui->btnFog->setChecked(stel.landscape->getFlagFog());

	ui->btnStars->setChecked(stel.starmgr->getFlagStars());
	ui->btnStarNames->setChecked(stel.starmgr->getFlagLabels());
	//ui->btnNebulae->setChecked(stel.nebulamgr->getFlagShow());
	ui->btnNebNames->setChecked(stel.nebulamgr->getFlagHints());
	ui->btnSkyImage->setChecked(stel.skylayer->getFlagShow());

	ui->btnPlanets->setChecked(stel.solar->getFlagPlanets());
	ui->btnPlanetNames->setChecked(stel.solar->getFlagLabels());
	ui->btnPlanetOrbits->setChecked(stel.solar->getFlagOrbits());

	ui->btnMeteor->setChecked(stel.meteor->getEnablePlugin());
	ui->btnMeteorRadiant->setChecked(stel.meteor->getEnableMarker());

	ui->btnSatellites->setChecked(stel.satel->getFlagHints());
	ui->btnSatelliteNames->setChecked(stel.satel->getFlagLabels());
	ui->btnSatelliteTrails->setChecked(stel.satel->getOrbitLinesFlag());

	ui->btnConstel->setChecked(stel.constmgr->getFlagLines());
	ui->btnConstelNames->setChecked(stel.constmgr->getFlagLabels());
	ui->btnConstelArt->setChecked(stel.constmgr->getFlagArt());
	ui->btnConstelBound->setChecked(stel.constmgr->getFlagBoundaries());

	//ui->btnGridRaDec->setChecked(stel.gridmgr->getFlagEquatorJ2000Grid());
	ui->btnGridRaDec->setChecked(stel.gridmgr->getFlagEquatorGrid());
	ui->btnGridAltAz->setChecked(stel.gridmgr->getFlagAzimuthalGrid());
	//ui->btnGridGal->setChecked(stel.gridmgr->getFlagGalacticGrid());
	//ui->btnGridEcliptic->setChecked(stel.gridmgr->getFlagEclipticJ2000Grid());
	ui->btnGridEcliptic->setChecked(stel.gridmgr->getFlagEclipticGrid());
	ui->btnCardPoints->setChecked(stel.landscape->getFlagCardinalsPoints());

	ui->btnEclipticLine->setChecked(stel.gridmgr->getFlagEclipticLine());
	//ui->btnEquatorLine->setChecked(stel.gridmgr->getFlagEquatorJ2000Line());
	ui->btnEquatorLine->setChecked(stel.gridmgr->getFlagEquatorLine());
	ui->btnMeridianLine->setChecked(stel.gridmgr->getFlagMeridianLine());
	ui->btnPrecCircles->setChecked(stel.gridmgr->getFlagPrecessionCircles());
	ui->btnAngle->setChecked(stel.anglemeas->isEnabled());
	//ui->btnMilky->setChecked(stel.milky->getFlagShow());
	ui->btnEqMount->setChecked(stel.mvmgr->getEquatorialMount());

	ui->btnTrack->setChecked(stel.mvmgr->getFlagTracking());
}




void PlanetC_UI::setFullScreen(bool b)
{
	int nScreens = QGuiApplication::screens().size();
	if(b)
	{
		if(nScreens != 2)
		{
			planetc->msgBox("The automatic fullscreen facility requires 2 screens!");
			ui->btnFullScreen->setChecked(false);
			return;
		}

		/*
		            qDebug() << "Number of screens:" << QGuiApplication::screens().size();
		            qDebug() << "Primary screen:" << QGuiApplication::primaryScreen()->name();
		            foreach (QScreen *screen, QGuiApplication::screens()) {
		                    qDebug() << "Information for screen:" << screen->name();
		                    qDebug() << "  Available geometry:" << screen->availableGeometry().x() << screen->availableGeometry().y() << screen->availableGeometry().width() << "x" << screen->availableGeometry().height();
		                    qDebug() << "  Available size:" << screen->availableSize().width() << "x" << screen->availableSize().height();
		                    qDebug() << "  Available virtual geometry:" << screen->availableVirtualGeometry().x() << screen->availableVirtualGeometry().y() << screen->availableVirtualGeometry().width() << "x" << screen->availableVirtualGeometry().height();
		                    qDebug() << "  Available virtual size:" << screen->availableVirtualSize().width() << "x" << screen->availableVirtualSize().height();
		                    qDebug() << "  Depth:" << screen->depth() << "bits";
		                    qDebug() << "  Geometry:" << screen->geometry().x() << screen->geometry().y() << screen->geometry().width() << "x" << screen->geometry().height();
		                    qDebug() << "  Logical DPI:" << screen->logicalDotsPerInch();
		                    qDebug() << "  Logical DPI X:" << screen->logicalDotsPerInchX();
		                    qDebug() << "  Logical DPI Y:" << screen->logicalDotsPerInchY();
		                    qDebug() << "  Orientation:" << screen->orientation();
		                    qDebug() << "  Physical DPI:" << screen->physicalDotsPerInch();
		                    qDebug() << "  Physical DPI X:" << screen->physicalDotsPerInchX();
		                    qDebug() << "  Physical DPI Y:" << screen->physicalDotsPerInchY();
		                    qDebug() << "  Physical size:" << screen->physicalSize().width() << "x" << screen->physicalSize().height() << "mm";
		                    qDebug() << "  Primary orientation:" << screen->primaryOrientation();
		                    qDebug() << "  Refresh rate:" << screen->refreshRate() << "Hz";
		                    qDebug() << "  Size:" << screen->size().width() << "x" << screen->size().height();
		                    qDebug() << "  Virtual geometry:" << screen->virtualGeometry().x() << screen->virtualGeometry().y() << screen->virtualGeometry().width() << "x" << screen->virtualGeometry().height();
		                    qDebug() << "  Virtual size:" << screen->virtualSize().width() << "x" << screen->virtualSize().height();
		            }
		*/

		foreach(QScreen *screen, QGuiApplication::screens())
		{
			bool isPrimary = screen->name() == QGuiApplication::primaryScreen()->name();

			if(isPrimary)
				move(screen->geometry().topLeft());

			if(!isPrimary || nScreens == 1)
				stel.view->move(screen->geometry().topLeft());
		}


		QThread::msleep(500);

		if(nScreens > 1)
		{
			showFullScreen();
		}
		stel.view->setFullScreen(true);
	}
	else
	{
		if(nScreens != 2) return;
		stel.view->setFullScreen(false);
		showNormal();
	}
}


void PlanetC_UI::saveState()
{
	planetc->runScript("planetc_home_state = planetc_get_state();");
}

void PlanetC_UI::restoreState()
{
	planetc->runScript("planetc_set_state( planetc_home_state );");
	flagHomeLocation = true;
}

void PlanetC_UI::resetView()
{
	stel.core->setTimeRate(0);
	stel.mvmgr->setFlagTracking(false);
	stel.mvmgr->setFlagLockEquPos(false);
	stel.mvmgr->setEquatorialMount(false);
	stel.mvmgr->zoomTo(180, 0);
	planetc->runScript("core.moveToAltAzi(89.9, 180 , 0);");
}

void PlanetC_UI::incrStarTwinkle(bool b)
{
	float amount = (b ? 5 : 0.2) * stel.core->getSkyDrawer()->getTwinkleAmount();
	stel.core->getSkyDrawer()->setTwinkleAmount(amount);
}

void PlanetC_UI::toggleMeteor(bool b)
{
	stel.meteor->setEnablePlugin(b);

	stel.meteor->setEnableMarker(false);
	stel.meteor->setEnableLabels(false);
	ui->btnMeteorRadiant->setChecked(false);
	ui->btnMeteorIncrRate->setChecked(false);

	if(b)
	{
		setTimeNormalRate(); //otherwise meteors won't be visible
		stel.meteor->setActiveRadiantOnly(true);
	}
}

void PlanetC_UI::toggleMeteorRadiant(bool b)
{
	stel.meteor->setEnableMarker(b);
	stel.meteor->setEnableLabels(b);
}


void PlanetC_UI::toggleMeteorIncrRate(bool b)
{
	MeteorShower::zhrFactor = (b ? 5000: 1);
}


void PlanetC_UI::showTimeRate(double value)
{
	value *= 86400.;

	QString tmp;
	tmp.sprintf("Time rate: %8.3gx", value);
	ui->groupTimeRate->setTitle(tmp);

	ui->sliTimeRate->setUValue(value);
}

void PlanetC_UI::setTimeRate(int v)
{
	float sign = 1;//(ui->radioTimeFwd->isChecked()  ?  1  : -1.);
	float timeSpeed = sign * ui->sliTimeRate->uvalue(v);
	stel.core->setTimeRate(timeSpeed / 86400.);
}

void PlanetC_UI::setTimeNormalRate()
{
	//float sign = (ui->radioTimeFwd->isChecked()  ?  1  : -1.);
	float sign = (ui->sliTimeRate->value() >= 0  ?  1  :  -1);
	stel.core->setTimeRate(sign / 86400.);
}

void PlanetC_UI::setCustomTimeRate()
{
	//float sign = (ui->radioTimeFwd->isChecked()  ?  1  : -1.);
	float sign = (ui->sliTimeRate->value() >= 0  ?  1  :  -1);
	float timeSpeed = sign * mapCustomTimeRate[(QWidget*) sender()];
	stel.core->setTimeRate(timeSpeed / 86400.);
}

void PlanetC_UI::stopTime()
{
	stel.core->setTimeRate(0);
}

void PlanetC_UI::setTimeNow()
{
	stel.core->setTimeNow();
}

void PlanetC_UI::setTimeRelativeMinus()
{
	setTimeRelative(-1);
}

void PlanetC_UI::setTimeRelative(int sign)
{
	if(stel.scrmgr->scriptIsRunning())
	{
		planetc->msgBox("A script is running: can not use the time shift facility");
		return;
	}

	QString s = ui->lineTimeRelAmount->text();
	float amount = s.toFloat();

	QString csign = "-";
	if(sign > 0) csign = "+";

	QString p;
	if(ui->radioTimeRel1->isChecked())  p = "minutes" ;
	if(ui->radioTimeRel2->isChecked())  p = "hours" ;
	if(ui->radioTimeRel3->isChecked())  p = "days" ;
	if(ui->radioTimeRel4->isChecked())
	{
		p = "days" ;
		amount *= 365;
	}

	if(ui->checkSidereal->isChecked())
		p += " sidereal";

	//TODO: this must be done without script
	planetc->runScript("core.setDate('" + csign + QString::number(amount) + " " + p + "', '', false)");
}

void PlanetC_UI::enableMouseKeys(bool b)
{
	stel.mvmgr->setFlagEnableZoomKeys(b);
	stel.mvmgr->setFlagEnableMouseNavigation(b);
	stel.mvmgr->setFlagEnableMoveKeys(b);

	if(b) stel.view->setCursor(Qt::ArrowCursor);     // Qt::CrossCursor
	else  stel.view->setCursor(Qt::BlankCursor);
}

void PlanetC_UI::addLocationButtons()
{
	QStringList keys = stel.conf->allKeys();
	int ibtnEarth = 0;
	int ibtnOther = 0;

	for(int i=0; i<keys.count(); i++)
	{
		if(keys[i].indexOf("planetc_locations") != -1)
		{
			QString s = stel.conf->value(keys[i]).toString();
			QStringList list = s.split("|");

			if(list.count() != 4)
			{
				s = "Error while reading entry: " + keys[i];
				qFatal(s.toLatin1().data());
			}

			QPushButton* button = new QPushButton(list[0].trimmed());
			if(list[2].trimmed().toLower() == "earth")
			{
				ui->layoutLocEarth->addWidget(button, ibtnEarth/2, ibtnEarth % 2);
				ibtnEarth += 1;
			}
			else
			{
				ui->layoutLocOther->addWidget(button, ibtnOther/2, ibtnOther % 2);
				ibtnOther += 1;
			}
			connect(button, SIGNAL(clicked()), this, SLOT(setLocation()));

			mapLocations[button] = s;
		}
	}
}

void PlanetC_UI::setLocation()
{
	if(flagHomeLocation)
	{
		saveState();
		flagHomeLocation = false;
	}

	stel.landscape->setFlagLandscapeUseMinimalBrightness(true);
	stel.landscape->setFlagLandscapeSetsMinimalBrightness(true);
	stel.landscape->setFlagLandscapeSetsLocation(true);
	stel.landscape->setFlagAtmosphereAutoEnable(true);

	QString s = mapLocations[(QWidget*) sender() ];
	QStringList list = s.split("|");
	QString landscapeID = list[1];

	if(!stel.landscape->setCurrentLandscapeID(landscapeID))
	{
		qDebug() << "Problems with landscape: " << list[1];
		((QPushButton*) sender())->setDisabled(true);
	}

	int v = list[3].toInt();
	stel.landscape->setFlagAtmosphere(v);
}


//Add "quick selection" buttons
void PlanetC_UI::addQuickButtons(QString section, QVBoxLayout* parent, const char* slot)
{
	QStringList keys = stel.conf->allKeys();
	QRegExp re_group(section + "_[0-9]+_(.*)/");
	QString group("");

	int ibtn = 0;

	QGroupBox* box = NULL;
	QGridLayout *layout = NULL;
	QPushButton* buttonGrp = NULL;

	int nColumn = 3;
	if(QString("1quickSelConstel()") == slot)
		nColumn = 2;

	for(int i=0; i<keys.count(); i++)
	{
		if(re_group.indexIn(keys[i]) != -1)
		{
			if(layout)
			{
				box->setLayout(layout);
				parent->addWidget(box);
			}

			if(group != re_group.cap(1))
			{
				group = re_group.cap(1);
				//qDebug() << "New group: " << group;

				ibtn = 0; //((ibtn / 3)+1)*3;
				box = new QGroupBox(group);
				layout = new QGridLayout();
				box->setLayout(layout);

				if(QString("1quickSelConstel()") == slot)
				{
					buttonGrp = new QPushButton("Select all");
					layout->addWidget(buttonGrp, 0, 0, 1, 2);
					connect(buttonGrp, SIGNAL(clicked()), this, SLOT(quickSelConstel()));
					ibtn = nColumn;
				}
			}

			QString s = stel.conf->value(keys[i]).toString();
			QStringList list = s.split("|");
			if(list.count() != 2)
			{
				s = "Error while reading entry: " + keys[i];
				qFatal(s.toLatin1().data());
			}

			QPushButton* button = new QPushButton(list[0].trimmed());
			layout->addWidget(button, ibtn/nColumn, ibtn % nColumn);

			if(QString("1quickSelConstel()") == slot)
			{
				connect(button, SIGNAL(clicked()), this, SLOT(quickSelConstel()));
			}
			else
			{
				connect(button, SIGNAL(clicked()), this, SLOT(quickSelObject()));
			}

			mapQuickSel[button] = list[1].trimmed();
			if(QString("1quickSelConstel()") == slot)
				mapQuickSel[buttonGrp] += list[1].trimmed() + "|";

			ibtn += 1;
		}
	}
}

void PlanetC_UI::setCurrentDateAsStartDate()
{
	QDateTime dt = dateTimeDialog->current();
	pOpt->setStartDate(dt.toString(Qt::ISODate));
}

void PlanetC_UI::quickSelConstel()
{
	stel.constmgr->setFlagIsolateSelected(true);
	stel.constmgr->setFlagBoundaries(false);
	stel.constmgr->setFlagLines(ui->btnSelConstLine->isChecked());
	stel.constmgr->setFlagLabels(ui->btnSelConstName->isChecked());
	stel.constmgr->setFlagArt(ui->btnSelConstArt->isChecked());

	QString obj = mapQuickSel[(QWidget*) sender() ];
	QStringList list = obj.split("|", QString::SkipEmptyParts);

	for(int i=0; i<list.count(); i++)
	{
		//Remember: search is case sensitive!!
		//qDebug() << "Selecting constell: " << list[i];
		if(!stel.objmgr->findAndSelectI18n(list[i]))
		{
			if(!stel.objmgr->findAndSelect(list[i]))
			{
				qDebug() << "Could not find: " << obj;
				((QPushButton*) sender())->setDisabled(true);
			}
		}
	}
}

void PlanetC_UI::quickSelObject()
{
	if(stel.constmgr->getFlagIsolateSelected())
	{
		stel.constmgr->setFlagIsolateSelected(false);
		stel.constmgr->setFlagBoundaries(false);
		stel.constmgr->setFlagLines(false);
		stel.constmgr->setFlagLabels(false);
		stel.constmgr->setFlagArt(false);
	}

	QString obj = mapQuickSel[(QWidget*) sender() ];

	//NOTE: search is case sensitive!!
	qDebug() << " searching for " << obj;
	if(!stel.objmgr->findAndSelectI18n(obj))
	{
		if(!stel.objmgr->findAndSelect(obj))
		{
			qDebug() << "Could not find: " << obj;
			((QPushButton*) sender())->setDisabled(true);
		}
	}
}

void PlanetC_UI::selectNone()
{
	stel.constmgr->setFlagIsolateSelected(false);
	stel.constmgr->setFlagBoundaries(false);
	stel.constmgr->setFlagLines(false);
	stel.constmgr->setFlagLabels(false);
	stel.constmgr->setFlagArt(false);
	stel.objmgr->findAndSelect("Sun");
	stel.objmgr->unSelect();
}

/* ******************************  SCRIPT HANDLING  ****************************** */
void PlanetC_UI::updateBtn_ScriptRunning()
{
	ui->btnScriptPlay->setEnabled(false);
	ui->btnScriptNext->setEnabled(false);
	ui->btnScriptStop->setEnabled(true);
}
void PlanetC_UI::updateBtn_ScriptPaused()
{
	ui->btnScriptPlay->setEnabled(false);
	ui->btnScriptNext->setEnabled(true);
	ui->btnScriptStop->setEnabled(true);
}
void PlanetC_UI::updateBtn_ScriptStopped()
{
	ui->btnScriptPlay->setEnabled(true);
	ui->btnScriptNext->setEnabled(false);
	ui->btnScriptStop->setEnabled(false);
}

void PlanetC_UI::scriptPlay()
{
	if(stel.scrmgr->scriptIsRunning())
	{
		planetc->msgBox("A script is already running!");
		return;
	}

	QString file = PlanetC::filePath("scripts/" + ui->listScript->currentItem()->text());
	if(file != "")
	{
		saveState();
		stel.view->setMinFps(PLANETC_MAXFPS);
		qDebug() << "Running script: " << file;
		stel.scrmgr->runScript(file);
		qDebug() << "Script finished";
		restoreState();
		flagHomeLocation = true;
		stel.view->setMinFps(PLANETC_MINFPS);
	}
}


/* ******************************  IMAGE HANDLING  ****************************** */
void PlanetC_UI::setImageAlpha(int pos)
{
	if(pos < 1) pos = 1;   //avoid transparency=0 otherwise image will disappear forever
	if(image[0] != NULL) image[0]->setAlpha(pos / 100.);
	if(image[1] != NULL) image[1]->setAlpha(pos / 100.);
}

float PlanetC_UI::imageSize2scale(int pos, int screenHeight, int imgW, int imgH, bool twin)
{
	float max = imgH;

	if(twin)
	{
		if(imgW/2. > max)
		{
			max = imgW;
			twin = false;
		}
	}
	else
	{
		if(imgW > max)
			max = imgW;
	}

	float scale = pos / 100. * screenHeight / max;
	if(twin) scale /= 2.;
	return scale;
}

void PlanetC_UI::setImageSize(int pos)
{
	bool Twin = (image[1] != NULL);

	if(image[0] != NULL)
	{
		float scale = imageSize2scale(pos
		                              , stel.view->height()
		                              , image[0]->imageWidth()
		                              , image[0]->imageHeight()
		                              , Twin);

		image[0]->setScale(scale);
		if(Twin)
			image[1]->setScale(scale);
	}
}

void PlanetC_UI::imageNext()
{
	int row = ui->listImage->currentRow();
	if(row < ui->listImage->count()-1)
	{
		ui->listImage->setCurrentRow(ui->listImage->currentRow() + 1);
		imageShow();
	}
	else
		imageHide();
}

void PlanetC_UI::imageShow()
{
	StelMainView* view = stel.view;
	bool Twin = ui->btnImageDouble->isChecked() ? true : false;

	imageHide();

	QString file = PlanetC::filePath("images/" + ui->listImage->currentItem()->text());
	qDebug() << "Loading image: " << file;
	if(file == "") return;

	//Get image size
	QPixmap pm(file);
	int imgW = pm.size().width();
	int imgH = pm.size().height();
	float scale = imageSize2scale(ui->sliImageSize->value(), stel.view->height(), imgW, imgH, Twin);

	if(Twin)
	{
		image[0] = new ScreenImage(file, (view->width() - imgW)/2., (view->height()/2.   - imgH)/2., true, 1, ui->sliImageTransp->value()/100., 1, 180.);
		image[1] = new ScreenImage(file, (view->width() - imgW)/2.,  view->height()*0.75 - imgH /2., true, 1, ui->sliImageTransp->value()/100., 1.,  0.);
		image[0]->setScale(scale);
		image[1]->setScale(scale);
	}
	else
	{
		image[0] = new ScreenImage(file, (view->width() - imgW)/2., (view->height() - imgH)/2., true, 1, ui->sliImageTransp->value()/100.);
		image[0]->setScale(scale);
	}
}

void PlanetC_UI::imageHide()
{
	if(image[0] != NULL) delete image[0];
	if(image[1] != NULL) delete image[1];
	image[0] = NULL;
	image[1] = NULL;
}

/* ******************************  VIDEO HANDLING  ****************************** */
void PlanetC_UI::updateVideoCurTime(qint64 pos)
{
	static QTime time0(0, 0, 0);
	QTime time = time0.addMSecs(pos);

	qint64 dur_msec = video[0]->duration();
	if(dur_msec < 100)
	{
		ui->labelVideoTime->setText("Time: " + time.toString());
	}
	else
	{
		QTime dur  = time0.addMSecs(dur_msec);
		ui->labelVideoTime->setText("Time: " + time.toString() + " / " + dur.toString());
	}
}

void PlanetC_UI::setVideoVolume(int v)
{
	video[0]->setVolume(v);
	video[1]->setVolume(0);
}

void PlanetC_UI::updateVideoStatus(QMediaPlayer::MediaStatus status)
{
	if(status == QMediaPlayer::EndOfMedia)
		videoStop();
}

void PlanetC_UI::setVideoSize(int size)
{
	if(video[0]->media().isNull())
		return;

	int Twin = !video[1]->media().isNull();

	if(!Twin)   //single video
	{
		video[0]->setGeometry(-1, -1, 0, size / 100.);
	}
	else     //twin videos
	{
		video[0]->setGeometry(-1, -1, 0, size / 200.);
		video[1]->setGeometry(-1, -1, 0, size / 200.);
	}
}

void PlanetC_UI::videoPlay()
{
	int Twin;

	if(video[0]->state() == QMediaPlayer::PlayingState)
	{
		Twin = !video[1]->media().isNull();

		video[0]->pause();
		if(Twin)
		{
			video[1]->setPosition(video[0]->position());
			video[1]->pause();
		}
		ui->btnVideoPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
		ui->btnVideoPlay->setText("Play");
		return;
	}

	if(video[0]->media().isNull())
	{
		Twin = ui->btnVideoDouble->isChecked() ? 1 : 0;
		QString file = PlanetC::filePath("video/" + ui->listVideo->currentItem()->text());
		if(file == "") return;


		imageHide(); //Hide images before starting the video

		qDebug();
		qDebug() << "Playing video: " << file;
		video[0]->openFile(file);
		if(Twin)
		{
			video[1]->openFile(file);
			video[0]->setGeometry(0.5, 0.75, 0, ui->sliVideoSize->value()/200.);
			video[1]->setGeometry(0.5, 0.25, 0, ui->sliVideoSize->value()/200.);
			video[1]->setRotation(180);
		}
		else
		{
			video[0]->setGeometry(0.5, 0.5, 0, ui->sliVideoSize->value()/100.);
		}

		connect(video[0], SIGNAL(positionChanged(qint64))                      , this, SLOT(updateVideoCurTime(qint64)));
		connect(video[0], SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(updateVideoStatus(QMediaPlayer::MediaStatus)));
	}
	else
	{
		Twin = !video[1]->media().isNull();
	}

	setVideoVolume(ui->sliVideoVolume->value());
	video[0]->play();
	if(Twin) video[1]->play();
	ui->btnVideoPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPause));
	ui->btnVideoPlay->setText("Pause");

	setTimeRate(0); //avoid overhead during video playing
	stel.view->setMinFps(PLANETC_MAXFPS);
}

void PlanetC_UI::videoStop()
{
	ui->btnVideoPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
	ui->btnVideoPlay->setText("Play");
	video[0]->close();
	video[1]->close();
	stel.view->setMinFps(PLANETC_MINFPS);
}



void PlanetC_UI::getViewAltAz(float& Alt, float& Az)
{
	const Vec3d& current = stel.core->j2000ToAltAz(stel.mvmgr->getViewDirectionJ2000(), StelCore::RefractionOff);
	double alt, azi;
	StelUtils::rectToSphe(&azi, &alt, current);
	Alt = alt*180/M_PI; // convert to degrees from radians
	Az  = std::fmod(((azi*180/M_PI)*-1)+180., 360.);
}


void PlanetC_UI::getObjAltAz(const QSharedPointerNoDelete<StelObject>& obj, float& Alt, float& Az)
{
	Vec3d current;

	if(stel.landscape->getFlagAtmosphere())
		current = obj->getAltAzPosApparent(stel.core);
	else
		current = obj->getAltAzPosGeometric(stel.core);
	double alt, azi;
	StelUtils::rectToSphe(&azi, &alt, current);
	Alt = alt*180/M_PI; // convert to degrees from radians
	Az  = std::fmod(((azi*180/M_PI)*-1)+180., 360.);
}






void PlanetC_UI::autoZoom()
{
	if(stel.scrmgr->scriptIsRunning())
	{
		planetc->msgBox("A script is running: can not use autozoom facility");
		return;
	}

	const QList<StelObjectP>& obj = stel.objmgr->getSelectedObject();
	if(obj.isEmpty()) return;

	if(obj[0]->getType() == "Constellation")
		return; //avoid zooming on constellations

	char buf[200];
	double timeMove = pOpt->getTimeMove();
	double timeZoom = pOpt->getTimeZoom();
	bool zoomIn = ui->btnZoom->text() == "Auto zoom in";

	stopTime();
	ui->btnZoom->setEnabled(false);
	ui->sliZoom->setEnabled(false);
	stel.view->setMinFps(PLANETC_MAXFPS);
	if(zoomIn)
	{
		sprintf(buf, "planetc_AutoZoomIn(%f, %f);", timeMove, timeZoom);
		planetc->runScript(buf);

		ui->btnZoom->setText("Auto zoom out");
		ui->btnZoom->setEnabled(true);
	}
	else  	//Zoom out
	{
		sprintf(buf, "planetc_AutoZoomOut(%f, %f);", timeMove, timeZoom);
		planetc->runScript(buf);

		ui->btnZoom->setText("Auto zoom in");
	}
	stel.view->setMinFps(PLANETC_MINFPS);
	ui->sliZoom->setEnabled(true);
}


void PlanetC_UI::sliZoomMoved(int v)
{
	stel.mvmgr->zoomTo(ui->sliZoom->uvalue(v), 0.5);
}





QString PlanetC_UI::scaleStyleSheet(QString cssFileName, float scale)
{
	//Note: there must be a space between ":" and "_num_px"
	QFile cssFile(cssFileName);
	cssFile.open(QIODevice::ReadOnly);

	QString out;
	char buf[1000];
	while(cssFile.readLine(buf, 1000) > -1)
	{
		QString line = buf;
		QStringList list = line.split(" ");
		for(int i=0; i<list.count(); i++)
		{
			QString entry = list[i];

			if(entry.indexOf("px") != -1)
			{
				QRegExp re;
				float f_val;
				QString s_val;

				re.setPattern("([\\-0-9]+)pxs(.*)");
				if(re.indexIn(entry) != -1)
				{
					f_val = round(re.cap(1).toFloat() * sqrt(scale));
					s_val = QString::number(f_val, 'f', 0);
					entry = s_val + "px" + re.cap(2);
				}
				else
				{
					re.setPattern("([\\-0-9]+)px(.*)");
					if(re.indexIn(entry) != -1)
					{
						f_val = round(re.cap(1).toFloat() * scale);
						s_val = QString::number(f_val, 'f', 0);
						entry = s_val + "px" + re.cap(2);
					}
				}
			}

			list[i] = entry;
		}

		line = list.join(" ");
		out += line;
	}
	cssFile.close();

	return out;
}




void PlanetC_UI::setUIStyle(double scale)
{
	QByteArray stelStyle = stel.gui->getStelStyle().qtStyleSheet;
	locationdialog->dialog->setStyleSheet(stelStyle);
	locationdialog->dialog->adjustSize();

	configurationdialog->dialog->setStyleSheet(stelStyle);
	configurationdialog->dialog->adjustSize();

	viewdialog->dialog->setStyleSheet(stelStyle);
	viewdialog->dialog->adjustSize();

	searchdialog->dialog->setStyleSheet(stelStyle);
	searchdialog->dialog->adjustSize();


	QString style = scaleStyleSheet(pOpt->editor.loadCurrentFile(),
	                                pOpt->getUIScale());
	pOpt->setStyleSheet(style);
	pOpt->adjustSize();

	showNormal();
	setStyleSheet(style);
	adjustSize();
}


void PlanetC_UI::openUserDir()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(PlanetC::filePath("")));
}




void PlanetC_UI::refreshUserDirList(QListWidget* list, QString path)
{
	list->clear();
	list->setSortingEnabled(true);
	path = PlanetC::filePath(path);
	QDirIterator it(path, QStringList() << "*",
	                QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while(it.hasNext())
		list->addItem(it.next().mid(path.length()+1));
}


void PlanetC_UI::refreshUserDir()
{
	refreshUserDirList(ui->listScript, "scripts");
	refreshUserDirList(ui->listImage , "images");
	refreshUserDirList(ui->listVideo , "video");
}



void PlanetC_UI::saveScreenshot()
{
	static QString path = QDir::homePath();

	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Open Image"), path, tr("Image Files (*.png)"));
	if(fileName != "")
	{
		QFileInfo fi(fileName);
		path = fi.absolutePath();
		stel.view->saveScreenShot(fi.completeBaseName(), path, true);
	}
}