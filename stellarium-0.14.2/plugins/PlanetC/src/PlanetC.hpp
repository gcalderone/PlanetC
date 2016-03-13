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

#ifndef _PLANETC_HPP_
#define _PLANETC_HPP_



#include "StelPluginInterface.hpp"
#include "StelFileMgr.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelGui.hpp"
#include "StelMainView.hpp"
#include "StelMovementMgr.hpp"
#include "StelActionMgr.hpp"
#include "LandscapeMgr.hpp"
#include "StelScriptMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StarMgr.hpp"
#include "NebulaMgr.hpp"
#include "SolarSystem.hpp"
#include "ConstellationMgr.hpp"
#include "GridLinesMgr.hpp"
#include "StelTexture.hpp"
#include "StelTextureMgr.hpp"
#include "MilkyWay.hpp"
#include "StelSkyLayerMgr.hpp"
#include "../../MeteorShowers/src/MeteorShowers.hpp"
#include "../../AngleMeasure/src/AngleMeasure.hpp"
#include "../../Satellites/src/Satellites.hpp"



struct StelModulesP
{
	StelApp* app;
	StelCore* core;
	QSettings* conf;
	StelGui* gui;
	StelMainView* view;
	StelMovementMgr* mvmgr;
	LandscapeMgr* landscape;
	StelScriptMgr* scrmgr;
	StelObjectMgr* objmgr;
	StarMgr* starmgr;
	NebulaMgr* nebulamgr;
	SolarSystem* solar;
	ConstellationMgr* constmgr;
	GridLinesMgr* gridmgr;
	MeteorShowersMgr* meteor;
	AngleMeasure* anglemeas;
	Satellites* satel;
	MilkyWay* milky;
	StelSkyLayerMgr* skylayer;
};


class PlanetC_UI;



//====================================================================
/*!
  class PlanetCStelPluginInterface

  This class is used by Qt to manage a plug-in interface
*/
class PlanetCStelPluginInterface : public QObject, public StelPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "stellarium.StelGuiPluginInterface/1.0")
	Q_INTERFACES(StelPluginInterface)

public:
	virtual StelModule* getStelModule() const;
	virtual StelPluginInfo getPluginInfo() const;
};





//====================================================================
/*!
  class PlanetC

  Main PlanetC plugin class.
*/
class PlanetC : public StelModule
{
	Q_OBJECT

	Q_PROPERTY(bool enablePlanetC
	           READ isEnabled
	           WRITE enablePlanetC)

public:
	PlanetC();
	virtual ~PlanetC()
	{
		enablePlanetC(false);
	}

	static PlanetC* getInstance();

	//! Initialize plugin
	virtual void init();
	// Deinitialize plugin (currently does nothing)
	virtual void deinit() {}

	virtual double getCallOrder(StelModuleActionName actionName) const;
	//! Is the plugin enabled?
	bool isEnabled() const
	{
		return flagEnabled;
	}

	static struct StelModulesP getStelModules();

	//! Returns the complete path of a file under the PlanetC user directory
	//! @param name File name
	//! @param checkExists Check if the file exists and if not returns an empty string.
	static QString filePath(QString name, bool checkExists=true);

	//! Simple function to display a message to the user.
	static void msgBox(QString s);

	//! Run a script
	void runScript(QString);

	virtual void update(double delta) {}
	virtual void draw(StelCore *core);

	PlanetC_UI* getUI()
	{
		return UI;
	}
	bool getEqMountTrackAppPos();
	void cloneView(QOpenGLFramebufferObject* buf);

public slots:
	//! Enable plugin usage
	void enablePlanetC(bool b);

private:
	PlanetC_UI* UI;
	struct StelModulesP stel;
	bool flagEnabled;
	StelAction* action;
};


#endif /* _PLANETC_HPP_ */
