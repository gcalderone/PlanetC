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

#include "StelModuleMgr.hpp"
#include "PlanetC.hpp"
#include "PlanetC_UI.hpp"


StelModule* PlanetCStelPluginInterface::getStelModule() const
{
	return new PlanetC();
}

StelPluginInfo PlanetCStelPluginInterface::getPluginInfo() const
{
	// Allow to load the resources when used as a static plugin
	Q_INIT_RESOURCE(PlanetC);

	StelPluginInfo info;
	info.id = "PlanetC";

	info.displayedName = N_("PlanetC");
	info.authors = "Giorgio Calderone";
	info.contact = "https://github.com/gcalderone/planetc/wiki/";
	info.description = N_("This plugin enables the Planetarium Control (PlanetC) interface.");
	info.version = PLANETC_PLUGIN_VERSION;
	return info;
}







bool PlanetC::hasInstance = false;

PlanetC::PlanetC()
{
	flagEnabled = false;
	hasInstance = true;
	cloneEnabled = false;

	UI = NULL;
	setObjectName("PlanetC");
	stel = getStelModules();

	qDebug() << "";
	qDebug() << "PlanetC plugin ver. " << PLANETC_PLUGIN_VERSION;
	qDebug() << "";
}


PlanetC* PlanetC::getInstance()
{
	if (!hasInstance) return NULL;
	static PlanetC* singleton = GETSTELMODULE(PlanetC);
	return singleton;
}



void PlanetC::init()
{
	//Link the action "actionShow_PlanetC" with a call to "enablePlanetC".
	action = addAction("actionShow_PlanetC", N_("PlanetC"), N_("PlanetC"), "enablePlanetC");

	static StelButton* toolbarButton = NULL;
	if(toolbarButton==NULL)
	{
		// Create the button. Link the action "actionShow_PlanetC"
		// with the state of the toolbar button.
		toolbarButton = new StelButton(NULL,
		                               QPixmap(":/PlanetC/bt_PlanetC_On.png"),
		                               QPixmap(":/PlanetC/bt_PlanetC_Off.png"),
		                               QPixmap(":/graphicGui/glow32x32.png"),
		                               "actionShow_PlanetC");
	}
 
	stel.gui->getButtonBar()->addButton(toolbarButton, "065-pluginsGroup");
}



double PlanetC::getCallOrder(StelModuleActionName actionName) const
{
	if(actionName==StelModule::ActionDraw)
		return stel.landscape->getCallOrder(actionName)+10.;
	return 0;
}


struct StelModulesP PlanetC::getStelModules()
{
	StelModulesP stel;
	stel.app       = &(StelApp::getInstance());
	stel.core      = stel.app->getCore();
	stel.conf      = stel.app->getSettings();
	stel.gui       = dynamic_cast<StelGui*>(stel.app->getGui());
	stel.view      = &(StelMainView::getInstance());
	stel.mvmgr     = GETSTELMODULE(StelMovementMgr);
	stel.landscape = GETSTELMODULE(LandscapeMgr);
	stel.scrmgr    = &(stel.app->getScriptMgr());
	stel.objmgr    = &(stel.app->getStelObjectMgr());
	stel.starmgr   = GETSTELMODULE(StarMgr);
	stel.nebulamgr = GETSTELMODULE(NebulaMgr);
	stel.solar     = GETSTELMODULE(SolarSystem);
	stel.constmgr  = GETSTELMODULE(ConstellationMgr);
	stel.gridmgr   = GETSTELMODULE(GridLinesMgr);
	stel.satel     = GETSTELMODULE(Satellites);
	stel.meteor    = (MeteorShowersMgr*) stel.app->getModuleMgr().getModule("MeteorShowers", false);
	stel.anglemeas = (AngleMeasure*) stel.app->getModuleMgr().getModule("AngleMeasure", false);
	stel.milky     = GETSTELMODULE(MilkyWay);
	stel.skylayer  = GETSTELMODULE(StelSkyLayerMgr);
	stel.imgmgr    = GETSTELMODULE(ScreenImageMgr);
	return stel;
}

QString PlanetC::filePath(QString name, bool checkExists)
{

	QString file = QDir::toNativeSeparators(StelFileMgr::getUserDir() + "/planetc/" + name);

	if(checkExists)
	{
		if(!StelFileMgr::exists(file))
		{
			qDebug() << "File " + file + " does not exists";
			return "";
		}
	}

	return file;
}

void PlanetC::msgBox(QString s)
{
	QMessageBox msg;
	msg.setText(s);
	msg.exec();
}

void PlanetC::runScript(QString script)
{
	//qDebug();
	//qDebug() << "SCRIPT: " << script;
	//qDebug();

	//Create a temporary file containing the script, then run it.
	QTemporaryFile file(QDir::tempPath() + "/planetc_XXXXXX.ssc");
	QString fileName;

	if(file.open())
	{
		QTextStream out(&file);
		//out << ui->scriptEdit->toPlainText() << "\n";
		out << script << "\n";
		fileName = file.fileName();
		file.close();
	}
	else
	{
		QString msg = "ERROR - cannot open temporary file for writing script text";
		qWarning() << __FILE__ << ":" << __LINE__ << ": " + msg;
		msgBox(msg);
	}

	if(!stel.scrmgr->runScript(fileName))
	{
		QString msg = QString("ERROR - cannot run script from temp file: \"%1\"").arg(fileName);
		qWarning() << __FILE__ << ":" << __LINE__ << ": " + msg;
		msgBox(msg);
	}
}


void PlanetC::update(double delta)
{
	if(!flagEnabled) return;
	UI->update(delta);
}

void PlanetC::draw(StelCore* core)
{
	if(!flagEnabled) return;
	UI->draw();
}

void PlanetC::enablePlanetC(bool b)
{
	if(b)
	{
		bool quit = false;

		//Check MeteorShower plugin is enabled
		if(!stel.conf->value("plugins_load_at_startup/MeteorShowers", false).toBool())
		{
			msgBox("PlanetC INFO: Meteor plugin is disabled, you must restart stellarium");
			stel.conf->setValue("plugins_load_at_startup/MeteorShowers", "true");
			quit = true;
		}

		//Check Satellites plugin is enabled
		if(!stel.conf->value("plugins_load_at_startup/Satellites", false).toBool())
		{
			msgBox("PlanetC INFO: Satellites plugin is disabled, you must restart stellarium");
			stel.conf->setValue("plugins_load_at_startup/Satellites", "true");
			quit = true;
		}

		//Check AngleMEasure plugin is enabled
		if(!stel.conf->value("plugins_load_at_startup/AngleMeasure", false).toBool())
		{
			msgBox("PlanetC INFO: AngleMeasure plugin is disabled, you must restart stellarium");
			stel.conf->setValue("plugins_load_at_startup/AngleMeasure", "true");
			quit = true;
		}

		//Should we quit stellarium?
		if(quit)
		{
			stel.app->quit();
			return;
		}

		//Make PlanetC directories
		QDir dir(StelFileMgr::getUserDir());
		dir.mkpath("planetc/images");
		dir.mkpath("planetc/scripts");
		dir.mkpath("planetc/video");


		//Copy example script file
		QString exampleFile = PlanetC::filePath("scripts/example.ssc", false);
		QFile exampleF(exampleFile);
		if(!exampleF.exists())
		{
			QFile::copy(":PlanetC/example.ssc", exampleFile);
			QFile::setPermissions(exampleFile, QFile::permissions(exampleFile) | QFileDevice::WriteOwner);
		}


		//Disable full screen to allow user to "see"
		//the PlanetC window
		stel.view->setFullScreen(false);

		//Disable Stellarium GUI
		stel.gui->setGuiVisible(false);

		//Create and show the PlanetC interface
		UI = new PlanetC_UI(this);
		UI->setVisible(true);

		//Compile PlanetC scripts
		QString f = PlanetC::filePath("scripts/planetc.ssc", false);
		if(StelFileMgr::exists(f))
		{
			stel.scrmgr->runScript(f); //use real file
		}
		else
		{
			//use file in resources
			QFile f(":/PlanetC/planetc.ssc");
			f.open(QFile::ReadOnly);
			QByteArray bytearr = f.readAll();
			QString script(bytearr);
			runScript(script);
		}

		flagEnabled = true;
	}
	else
	{
		if(!flagEnabled) return;
		flagEnabled = false;
		if(UI)
		{
			UI->timer.stop();
			QThread::msleep(2 * UI->timer.interval() * 1.2);
			UI->close();
			delete UI;
			UI = NULL;
		}
		stel.gui->setGuiVisible(true);
		//if(action->isChecked())
		//	action->setChecked(false);
	}
	cloneEnabled = flagEnabled;
}


bool PlanetC::getEqMountTrackAppPos()
{
	if(!UI) return false;
	return UI->pOpt->getEqMountTrackAppPos();
}

//Called from StelApp::applyRenderBuffer()
void PlanetC::cloneView(QOpenGLFramebufferObject* buf)
{
	if(flagEnabled  &&  cloneEnabled) {
		UI->glWidget->cloneView(buf);
	}
}

void PlanetC::cloneView(bool f)
{
	cloneEnabled = f;
}


void PlanetC::quit()
{
	enablePlanetC(false);
	stel.app->quit();
}
