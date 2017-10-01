/*
 * Stellarium
 * Copyright (C) 2006 Fabien Chereau
 * Copyright (C) 2010 Bogdan Marinov (add/remove landscapes feature)
 * Copyright (C) 2011 Alexander Wolf
 * Copyright (C) 2012 Timothy Reaves
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "StelActionMgr.hpp"
#include "LandscapeMgr.hpp"
#include "Landscape.hpp"
#include "Atmosphere.hpp"
#include "StelApp.hpp"
#include "SolarSystem.hpp"
#include "StelCore.hpp"
#include "StelLocaleMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelFileMgr.hpp"
#include "Planet.hpp"
#include "StelIniParser.hpp"
#include "StelSkyDrawer.hpp"
#include "StelPainter.hpp"
#include "qzipreader.h"

#include <QDebug>
#include <QSettings>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTemporaryFile>
#include <QMouseEvent>

#include <stdexcept>

// Class which manages the cardinal points displaying
class Cardinals
{
public:
	Cardinals(float _radius = 1.);
	virtual ~Cardinals();
	void draw(const StelCore* core, double latitude) const;
	void setColor(const Vec3f& c) {color = c;}
	Vec3f get_color() {return color;}
	void updateI18n();
	void update(double deltaTime) {fader.update((int)(deltaTime*1000));}
	void set_fade_duration(float duration) {fader.setDuration((int)(duration*1000.f));}
	void setFlagShow(bool b){fader = b;}
	bool getFlagShow() const {return fader;}
private:
	float radius;
	QFont font;
	Vec3f color;
	QString sNorth, sSouth, sEast, sWest;
	LinearFader fader;
};


Cardinals::Cardinals(float _radius) : radius(_radius), color(0.6,0.2,0.2), sNorth("N"), sSouth("S"), sEast("E"), sWest("W")
{
	// Font size is 30
	font.setPixelSize(StelApp::getInstance().getBaseFontSize()+17);	
}

Cardinals::~Cardinals()
{
}

// Draw the cardinals points : N S E W
// handles special cases at poles
void Cardinals::draw(const StelCore* core, double latitude) const
{
	const StelProjectorP prj = core->getProjection(StelCore::FrameAltAz, StelCore::RefractionOff);
	StelPainter sPainter(prj);
	sPainter.setFont(font);

	if (!fader.getInterstate()) return;

	// direction text
	QString d[4];

	d[0] = sNorth;
	d[1] = sSouth;
	d[2] = sEast;
	d[3] = sWest;

	// fun polar special cases
	if (latitude ==  90.0 ) d[0] = d[1] = d[2] = d[3] = sSouth;
	if (latitude == -90.0 ) d[0] = d[1] = d[2] = d[3] = sNorth;

	sPainter.setColor(color[0],color[1],color[2],fader.getInterstate());
	sPainter.setBlending(true);

	Vec3f pos;
	Vec3f xy;

	float shift = sPainter.getFontMetrics().width(sNorth)/2.;
	if (core->getProjection(StelCore::FrameJ2000)->getMaskType() == StelProjector::MaskDisk)
		shift = 0;

	// N for North
	pos.set(-1.f, 0.f, 0.f);
	if (prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[0], 0., -shift, -shift, false);

	// S for South
	pos.set(1.f, 0.f, 0.f);
	if (prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[1], 0., -shift, -shift, false);

	// E for East
	pos.set(0.f, 1.f, 0.f);
	if (prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[2], 0., -shift, -shift, false);

	// W for West
	pos.set(0.f, -1.f, 0.f);
	if (prj->project(pos,xy)) sPainter.drawText(xy[0], xy[1], d[3], 0., -shift, -shift, false);

}

// Translate cardinal labels with gettext to current sky language and update font for the language
void Cardinals::updateI18n()
{
	const StelTranslator& trans = StelApp::getInstance().getLocaleMgr().getAppStelTranslator();
	sNorth = trans.qtranslate("N");
	sSouth = trans.qtranslate("S");
	sEast = trans.qtranslate("E");
	sWest = trans.qtranslate("W");
}


LandscapeMgr::LandscapeMgr()
	: StelModule()
	, atmosphere(Q_NULLPTR)
	, cardinalsPoints(Q_NULLPTR)
	, landscape(Q_NULLPTR)
	, oldLandscape(Q_NULLPTR)
	, flagLandscapeSetsLocation(false)
	, flagLandscapeAutoSelection(false)
	, flagLightPollutionFromDatabase(false)
	, flagLandscapeUseMinimalBrightness(false)
	, defaultMinimalBrightness(0.01)
	, flagLandscapeSetsMinimalBrightness(false)
	, flagAtmosphereAutoEnabling(false)
{
	setObjectName("LandscapeMgr"); // should be done by StelModule's constructor.

	//Note: The first entry in the list is used as the default 'default landscape' in removeLandscape().
	packagedLandscapeIDs = (QStringList() << "guereins");
	QDirIterator directories(StelFileMgr::getInstallationDir()+"/landscapes/", QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while(directories.hasNext())
	{
		directories.next();
		packagedLandscapeIDs << directories.fileName();
	}
	packagedLandscapeIDs.removeDuplicates();
	landscapeCache.clear();
}

LandscapeMgr::~LandscapeMgr()
{
	delete atmosphere;
	delete cardinalsPoints;
	if (oldLandscape)
	{
		delete oldLandscape;
		oldLandscape=Q_NULLPTR;
	}
	delete landscape;
	landscape = Q_NULLPTR;
	qDebug() << "LandscapeMgr: Clearing cache of" << landscapeCache.size() << "landscapes totalling about " << landscapeCache.totalCost() << "MB.";
	landscapeCache.clear(); // deletes all objects within.
}

/*************************************************************************
 Reimplementation of the getCallOrder method
*************************************************************************/
double LandscapeMgr::getCallOrder(StelModuleActionName actionName) const
{
	if (actionName==StelModule::ActionDraw)
		return StelApp::getInstance().getModuleMgr().getModule("SporadicMeteorMgr")->getCallOrder(actionName)+20;
	if (actionName==StelModule::ActionUpdate)
		return StelApp::getInstance().getModuleMgr().getModule("SolarSystem")->getCallOrder(actionName)+10;
	// GZ The next 2 lines are only required to test landscape transparency. They should be commented away for releases.
	if (actionName==StelModule::ActionHandleMouseClicks)
		return StelApp::getInstance().getModuleMgr().getModule("StelMovementMgr")->getCallOrder(actionName)-1;
	return 0;
}

void LandscapeMgr::update(double deltaTime)
{
	atmosphere->update(deltaTime);

	if (oldLandscape)
	{
		// This is only when transitioning to newly loaded landscape. We must draw the old one until the new one is faded in completely.
		oldLandscape->update(deltaTime);
		if (getIsLandscapeFullyVisible())
		{
			oldLandscape->setFlagShow(false);

			if (oldLandscape->getEffectiveLandFadeValue()< 0.01f)
			{
				// new logic: try to put old landscape to cache.
				//qDebug() << "LandscapeMgr::update: moving oldLandscape " << oldLandscape->getId() << "to Cache. Cost:" << oldLandscape->getMemorySize()/(1024*1024)+1;
				landscapeCache.insert(oldLandscape->getId(), oldLandscape, oldLandscape->getMemorySize()/(1024*1024)+1);
				//qDebug() << "--> LandscapeMgr::update(): cache now contains " << landscapeCache.size() << "landscapes totalling about " << landscapeCache.totalCost() << "MB.";
				oldLandscape=Q_NULLPTR;
			}
		}
	}
	landscape->update(deltaTime);
	cardinalsPoints->update(deltaTime);

	// Compute the atmosphere color and intensity
	// Compute the sun position in local coordinate
	SolarSystem* ssystem = (SolarSystem*)StelApp::getInstance().getModuleMgr().getModule("SolarSystem");

	StelCore* core = StelApp::getInstance().getCore();
	Vec3d sunPos = ssystem->getSun()->getAltAzPosAuto(core);
	// Compute the moon position in local coordinate
	Vec3d moonPos = ssystem->getMoon()->getAltAzPosAuto(core);
	float lunarPhaseAngle=ssystem->getMoon()->getPhaseAngle(ssystem->getEarth()->getHeliocentricEclipticPos());
	float lunarMagnitude=ssystem->getMoon()->getVMagnitudeWithExtinction(core);
	// LP:1673283 no lunar brightening if not on Earth!
	if (core->getCurrentLocation().planetName != "Earth")
	{
		moonPos=sunPos;
		lunarPhaseAngle=0.0f;
	}
	// GZ: First parameter in next call is used for particularly earth-bound computations in Schaefer's sky brightness model. Difference DeltaT makes no difference here.
	atmosphere->computeColor(core->getJDE(), sunPos, moonPos, lunarPhaseAngle, lunarMagnitude,
		core, core->getCurrentLocation().latitude, core->getCurrentLocation().altitude,
		15.f, 40.f);	// Temperature = 15c, relative humidity = 40%

	core->getSkyDrawer()->reportLuminanceInFov(3.75+atmosphere->getAverageLuminance()*3.5, true);


	// NOTE: Simple workaround for brightness of landscape when observing from the Sun.
	if (core->getCurrentLocation().planetName == "Sun")
	{
		landscape->setBrightness(1.0f, 1.0f);
		return;
	}


	// Compute the ground luminance based on every planets around
	// TBD: Reactivate and verify this code!? Source, reference?
//	float groundLuminance = 0;
//	const vector<Planet*>& allPlanets = ssystem->getAllPlanets();
//	for (vector<Planet*>::const_iterator i=allPlanets.begin();i!=allPlanets.end();++i)
//	{
//		Vec3d pos = (*i)->getAltAzPos(core);
//		pos.normalize();
//		if (pos[2] <= 0)
//		{
//			// No need to take this body into the landscape illumination computation
//			// because it is under the horizon
//		}
//		else
//		{
//			// Compute the Illuminance E of the ground caused by the planet in lux = lumen/m^2
//			float E = pow10(((*i)->get_mag(core)+13.988)/-2.5);
//			//qDebug() << "mag=" << (*i)->get_mag(core) << " illum=" << E;
//			// Luminance in cd/m^2
//			groundLuminance += E/0.44*pos[2]*pos[2]; // 1m^2 from 1.5 m above the ground is 0.44 sr.
//		}
//	}
//	groundLuminance*=atmosphere->getFadeIntensity();
//	groundLuminance=atmosphere->getAverageLuminance()/50;
//	qDebug() << "Atmosphere lum=" << atmosphere->getAverageLuminance() << " ground lum=" <<  groundLuminance;
//	qDebug() << "Adapted Atmosphere lum=" << eye->adaptLuminance(atmosphere->getAverageLuminance()) << " Adapted ground lum=" << eye->adaptLuminance(groundLuminance);

	// compute global ground brightness in a simplistic way, directly in RGB
	sunPos.normalize();
	moonPos.normalize();

	float landscapeBrightness=0.0f;
	if (getFlagLandscapeUseMinimalBrightness())
	{
		// Setting from landscape.ini has priority if enabled
		if (getFlagLandscapeSetsMinimalBrightness() && landscape->getLandscapeMinimalBrightness()>=0)
			landscapeBrightness = landscape->getLandscapeMinimalBrightness();
		else
			landscapeBrightness = getDefaultMinimalBrightness();
	}

	// With atmosphere on, we define the solar brightness contribution zero when the sun is 8 degrees below the horizon.
	// The multiplier of 1.5 just looks better, it somehow represents illumination by scattered sunlight.
	// Else, we should account for sun's diameter but else just apply Lambertian Cos-rule and check with landscape opacity.
	float sinSunAngle = 0.0f;
	if(atmosphere->getFlagShow())
	{
		sinSunAngle=sin(qMin(M_PI_2, asin(sunPos[2])+8.*M_PI/180.));
		if(sinSunAngle > -0.1/1.5 )
			landscapeBrightness +=  1.5*(sinSunAngle+0.1/1.5);
	}
	else
	{
		// In case we have exceptionally deep horizons ("Little Prince planet"), the sun will rise somehow over that line and demand light on the landscape.
		sinSunAngle=sin(qMin(M_PI_2, asin(qBound(-1.0, sunPos[2]-landscape->getSinMinAltitudeLimit(), 1.0) ) + (0.25f *M_PI/180.)));
		if(sinSunAngle > 0.0f)
			landscapeBrightness +=  (1.0f-landscape->getOpacity(sunPos))*sinSunAngle;

	}

	// GZ: 2013-09-25 Take light pollution into account!
	StelSkyDrawer* drawer=StelApp::getInstance().getCore()->getSkyDrawer();
	float pollutionAddonBrightness=(drawer->getBortleScaleIndex()-1.0f)*0.025f; // 0..8, so we assume empirical linear brightening 0..0.02
	float lunarAddonBrightness=0.f;
	if (moonPos[2] > -0.1/1.5)
		lunarAddonBrightness = qMax(0.2/-12.*ssystem->getMoon()->getVMagnitudeWithExtinction(core),0.)*moonPos[2];

	landscapeBrightness += qMax(lunarAddonBrightness, pollutionAddonBrightness);

	// TODO make this more generic for non-atmosphere planets
	if(atmosphere->getFadeIntensity() == 1)
	{
		// If the atmosphere is on, a solar eclipse might darken the sky
		// otherwise we just use the sun position calculation above
		landscapeBrightness *= (atmosphere->getRealDisplayIntensityFactor()+0.1);
	}
	// TODO: should calculate dimming with solar eclipse even without atmosphere on

	// Brightness can't be over 1.f (see https://bugs.launchpad.net/stellarium/+bug/1115364)
	if (landscapeBrightness>0.95)
		landscapeBrightness = 0.95;

	// GZ's rules and intentions for lightscape brightness:
	// lightscapeBrightness >0 makes sense only for sun below horizon.
	// If atmosphere on, we mix it in with darkening twilight. If atmosphere off, we can switch on more apruptly.
	// Note however that lightscape rendering does not per se depend on atmosphere on/off.
	// This allows for illuminated windows or light panels on spaceships. If a landscape's lightscape
	// contains light smog of a city, it should also be shown if atmosphere is switched off.
	// (Configure another landscape without light smog to avoid, or just switch off lightscape.)
	float lightscapeBrightness=0.0f;
	float sinSunAlt = sunPos[2];
	if (atmosphere->getFlagShow())
	{
		// light pollution layer is mixed in at -3...-8 degrees.
		if (sinSunAlt<-0.14f)
			lightscapeBrightness=1.0f;
		else if (sinSunAlt<-0.05f)
			lightscapeBrightness = 1.0f-(sinSunAlt+0.14)/(-0.05+0.14);
	}
	else
	{
		// If we have no atmosphere, we can assume windows and panels on spaceships etc. are switched on whenever the sun does not shine, i.e. when sun is blocked by landscape.
		lightscapeBrightness= landscape->getOpacity(sunPos);
	}

	landscape->setBrightness(landscapeBrightness, lightscapeBrightness);
}

void LandscapeMgr::draw(StelCore* core)
{
	// Draw the atmosphere
	atmosphere->draw(core);

	// Draw the landscape
	if (oldLandscape)
		oldLandscape->draw(core);
	landscape->draw(core);

	// Draw the cardinal points
	cardinalsPoints->draw(core, StelApp::getInstance().getCore()->getCurrentLocation().latitude);
}

void LandscapeMgr::init()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);

	landscapeCache.setMaxCost(conf->value("landscape/cache_size_mb", 100).toInt());
	qDebug() << "LandscapeMgr: initialized Cache for" << landscapeCache.maxCost() << "MB.";

	atmosphere = new Atmosphere();
	defaultLandscapeID = conf->value("init_location/landscape_name").toString();
	setCurrentLandscapeID(defaultLandscapeID);
	setFlagLandscape(conf->value("landscape/flag_landscape", conf->value("landscape/flag_ground", true).toBool()).toBool());
	setFlagFog(conf->value("landscape/flag_fog",true).toBool());
	setFlagAtmosphere(conf->value("landscape/flag_atmosphere", true).toBool());
	setAtmosphereFadeDuration(conf->value("landscape/atmosphere_fade_duration",0.5).toFloat());
	setAtmosphereLightPollutionLuminance(conf->value("viewing/light_pollution_luminance",0.0).toFloat());
	setFlagUseLightPollutionFromDatabase(conf->value("viewing/flag_light_pollution_database", false).toBool());
	cardinalsPoints = new Cardinals();
	cardinalsPoints->setFlagShow(conf->value("viewing/flag_cardinal_points",true).toBool());
	setFlagLandscapeSetsLocation(conf->value("landscape/flag_landscape_sets_location",false).toBool());
	setFlagLandscapeAutoSelection(conf->value("viewing/flag_landscape_autoselection", false).toBool());
	// Set minimal brightness for landscape. This feature has been added for folks which say "landscape is super dark, please add light". --AW
	setDefaultMinimalBrightness(conf->value("landscape/minimal_brightness", 0.01).toFloat());
	setFlagLandscapeUseMinimalBrightness(conf->value("landscape/flag_minimal_brightness", false).toBool());
	setFlagLandscapeSetsMinimalBrightness(conf->value("landscape/flag_landscape_sets_minimal_brightness",false).toBool());
	setFlagAtmosphereAutoEnable(conf->value("viewing/flag_atmosphere_auto_enable",true).toBool());
	setFlagIllumination(conf->value("landscape/flag_enable_illumination_layer", true).toBool());
	setFlagLabels(conf->value("landscape/flag_enable_labels", true).toBool());

	// Load colors from config file
	QString defaultColor = conf->value("color/default_color").toString();
	setColorCardinalPoints(StelUtils::strToVec3f(conf->value("color/cardinal_color", defaultColor).toString()));

	StelApp *app = &StelApp::getInstance();
	//Bortle scale is managed by SkyDrawer
	StelSkyDrawer* drawer = app->getCore()->getSkyDrawer();
	setAtmosphereBortleLightPollution(drawer->getBortleScaleIndex());
	connect(app->getCore(), SIGNAL(locationChanged(StelLocation)), this, SLOT(updateLocationBasedPollution(StelLocation)));
	connect(drawer, SIGNAL(bortleScaleIndexChanged(int)), this, SLOT(setAtmosphereBortleLightPollution(int)));
	connect(app, SIGNAL(languageChanged()), this, SLOT(updateI18n()));

	QString displayGroup = N_("Display Options");
	addAction("actionShow_Atmosphere", displayGroup, N_("Atmosphere"), "atmosphereDisplayed", "A");
	addAction("actionShow_Fog", displayGroup, N_("Fog"), "fogDisplayed", "F");
	addAction("actionShow_Cardinal_Points", displayGroup, N_("Cardinal points"), "cardinalsPointsDisplayed", "Q");
	addAction("actionShow_Ground", displayGroup, N_("Ground"), "landscapeDisplayed", "G");
	addAction("actionShow_LandscapeIllumination", displayGroup, N_("Landscape illumination"), "illuminationDisplayed", "Shift+G");
	addAction("actionShow_LandscapeLabels", displayGroup, N_("Landscape labels"), "labelsDisplayed", "Ctrl+Shift+G");
	addAction("actionShow_LightPollutionFromDatabase", displayGroup, N_("Light pollution data from locations database"), "flagUseLightPollutionFromDatabase");
}

bool LandscapeMgr::setCurrentLandscapeID(const QString& id, const double changeLocationDuration)
{
	if (id.isEmpty())
		return false;

	//prevent unnecessary changes/file access
	if(id==currentLandscapeID)
		return false;

	Landscape* newLandscape;

	// There is a slight chance that we switch back to oldLandscape while oldLandscape is still fading away.
	// in this case it is not yet stored in cache, but obviously available. So we just swap places.
	if (oldLandscape && oldLandscape->getId()==id)
	{
		newLandscape=oldLandscape;
	}
	else
	{
		// We want to lookup the landscape ID (dir) from the name.
		newLandscape= landscapeCache.take(id);

		if (newLandscape)
		{
#ifndef NDEBUG
			qDebug() << "LandscapeMgr::setCurrentLandscapeID():: taken " << id << "from cache...";
			qDebug() << ".-->LandscapeMgr::setCurrentLandscapeID(): cache contains " << landscapeCache.size() << "landscapes totalling about " << landscapeCache.totalCost() << "MB.";
#endif
		}
		else
		{
#ifndef NDEBUG
			qDebug() << "LandscapeMgr::setCurrentLandscapeID: Loading from file:" << id ;
#endif
			newLandscape = createFromFile(StelFileMgr::findFile("landscapes/" + id + "/landscape.ini"), id);
		}

		if (!newLandscape)
		{
			qWarning() << "ERROR while loading landscape " << "landscapes/" + id + "/landscape.ini";
			return false;
		}
	}

	// Keep current landscape for a while, while new landscape fades in!
	// This prevents subhorizon sun or grid becoming briefly visible.
	if (landscape)
	{
		// Copy display parameters from previous landscape to new one
		newLandscape->setFlagShow(landscape->getFlagShow());
		newLandscape->setFlagShowFog(landscape->getFlagShowFog());
		newLandscape->setFlagShowIllumination(landscape->getFlagShowIllumination());
		newLandscape->setFlagShowLabels(landscape->getFlagShowLabels());

		// If we have an oldLandscape that is not just swapped back, put that into cache.
		if (oldLandscape && oldLandscape!=newLandscape)
		{
#ifndef NDEBUG
			qDebug() << "LandscapeMgr::setCurrent: moving oldLandscape " << oldLandscape->getId() << "to Cache. Cost:" << oldLandscape->getMemorySize()/(1024*1024)+1;
#endif
			landscapeCache.insert(oldLandscape->getId(), oldLandscape, oldLandscape->getMemorySize()/(1024*1024)+1);
#ifndef NDEBUG
			qDebug() << "-->LandscapeMgr::setCurrentLandscapeId(): cache contains " << landscapeCache.size() << "landscapes totalling about " << landscapeCache.totalCost() << "MB.";
#endif
		}
		oldLandscape = landscape; // keep old while transitioning!
	}
	landscape=newLandscape;
	currentLandscapeID = id;

	if (getFlagLandscapeSetsLocation() && landscape->hasLocation())
	{
		StelCore *core = StelApp::getInstance().getCore();
		core->moveObserverTo(landscape->getLocation(), changeLocationDuration);
		StelSkyDrawer* drawer=core->getSkyDrawer();

		if (landscape->getDefaultFogSetting() >-1)
		{
			setFlagFog((bool) landscape->getDefaultFogSetting());
			landscape->setFlagShowFog((bool) landscape->getDefaultFogSetting());
		}
		if (landscape->getDefaultBortleIndex() > 0)
		{
			drawer->setBortleScaleIndex(landscape->getDefaultBortleIndex());
		}
		if (landscape->getDefaultAtmosphericExtinction() >= 0.0f)
		{
			drawer->setExtinctionCoefficient(landscape->getDefaultAtmosphericExtinction());
		}
		if (landscape->getDefaultAtmosphericTemperature() > -273.15f)
		{
			drawer->setAtmosphereTemperature(landscape->getDefaultAtmosphericTemperature());
		}
		if (landscape->getDefaultAtmosphericPressure() >= 0.0f)
		{
			drawer->setAtmospherePressure(landscape->getDefaultAtmosphericPressure());
		}
		else if (landscape->getDefaultAtmosphericPressure() == -1.0f)
		{
			// compute standard pressure for standard atmosphere in given altitude if landscape.ini coded as atmospheric_pressure=-1
			// International altitude formula found in Wikipedia.
			double alt=landscape->getLocation().altitude;
			double p=1013.25*std::pow(1-(0.0065*alt)/288.15, 5.255);
			drawer->setAtmospherePressure(p);
		}
	}

	emit currentLandscapeChanged(currentLandscapeID,getCurrentLandscapeName());

	// else qDebug() << "Will not set new location; Landscape location: planet: " << landscape->getLocation().planetName << "name: " << landscape->getLocation().name;
	return true;
}

bool LandscapeMgr::setCurrentLandscapeName(const QString& name, const double changeLocationDuration)
{
	if (name.isEmpty())
		return false;
	
	QMap<QString,QString> nameToDirMap = getNameToDirMap();
	if (nameToDirMap.find(name)!=nameToDirMap.end())
	{
		return setCurrentLandscapeID(nameToDirMap[name], changeLocationDuration);
	}
	else
	{
		qWarning() << "Can't find a landscape with name=" << name << endl;
		return false;
	}
}

// Load a landscape into cache.
// @param id the ID of a landscape
// @param replace true if existing landscape entry should be replaced (useful during development to reload after edit)
// @return false if landscape could not be found, or existed already and replace was false.
bool LandscapeMgr::precacheLandscape(const QString& id, const bool replace)
{
	if (landscapeCache.contains(id) && (!replace))
		return false;

	Landscape* newLandscape = createFromFile(StelFileMgr::findFile("landscapes/" + id + "/landscape.ini"), id);
	if (!newLandscape)
	{
		qWarning() << "ERROR while preloading landscape " << "landscapes/" + id + "/landscape.ini";
		return false;
	}

	bool res=landscapeCache.insert(id, newLandscape, newLandscape->getMemorySize()/(1024*1024)+1);
#ifndef NDEBUG
	if (res)
	{
		qDebug() << "LandscapeMgr::precacheLandscape(): Successfully added landscape with ID " << id << "to cache";
	}
	qDebug() << "LandscapeMgr::precacheLandscape(): cache contains " << landscapeCache.size() << "landscapes totalling about " << landscapeCache.totalCost() << "MB.";
#endif
	return res;
}

// Remove a landscape from the cache of loaded landscapes.
// @param id the ID of a landscape
// @return false if landscape could not be found
bool LandscapeMgr::removeCachedLandscape(const QString& id)
{
	bool res= landscapeCache.remove(id);
#ifndef NDEBUG
	qDebug() << "LandscapeMgr::removeCachedLandscape(): cache contains " << landscapeCache.size() << "landscapes totalling about " << landscapeCache.totalCost() << "MB.";
#endif
	return res;
}


// Change the default landscape to the landscape with the ID specified.
bool LandscapeMgr::setDefaultLandscapeID(const QString& id)
{
	if (id.isEmpty())
		return false;
	defaultLandscapeID = id;
	QSettings* conf = StelApp::getInstance().getSettings();
	conf->setValue("init_location/landscape_name", id);
	emit defaultLandscapeChanged(id);
	return true;
}

void LandscapeMgr::updateI18n()
{
	// Translate all labels with the new language
	if (cardinalsPoints) cardinalsPoints->updateI18n();
	landscape->loadLabels(getCurrentLandscapeID());
}

void LandscapeMgr::setFlagLandscape(const bool displayed)
{
	if (oldLandscape && !displayed)
		oldLandscape->setFlagShow(false);
	if(landscape->getFlagShow() != displayed) {
		landscape->setFlagShow(displayed);
		emit landscapeDisplayedChanged(displayed);
	}
}

bool LandscapeMgr::getFlagLandscape() const
{
	return landscape->getFlagShow();
}

bool LandscapeMgr::getIsLandscapeFullyVisible() const
{
	return landscape->getIsFullyVisible();
}

float LandscapeMgr::getLandscapeSinMinAltitudeLimit() const
{
	return landscape->getSinMinAltitudeLimit();
}

bool LandscapeMgr::getFlagUseLightPollutionFromDatabase() const
{
	return flagLightPollutionFromDatabase;
}

void LandscapeMgr::setFlagUseLightPollutionFromDatabase(const bool usage)
{
	if (flagLightPollutionFromDatabase != usage)
	{
		flagLightPollutionFromDatabase = usage;

		StelCore* core = StelApp::getInstance().getCore();

		//this was previously logic in ViewDialog, but should really be on a non-GUI layer
		if (usage)
		{
			StelLocation loc = core->getCurrentLocation();
			updateLocationBasedPollution(loc);
		}

		emit flagUseLightPollutionFromDatabaseChanged(usage);
	}
}

void LandscapeMgr::updateLocationBasedPollution(StelLocation loc)
{
	if(flagLightPollutionFromDatabase)
	{
		//this was previously logic in ViewDialog, but should really be on a non-GUI layer
		StelCore* core = StelApp::getInstance().getCore();
		int bIdx = loc.bortleScaleIndex;
		if (!loc.planetName.contains("Earth")) // location not on Earth...
			bIdx = 1;
		if (bIdx<1) // ...or it observatory, or it unknown location
			bIdx = loc.DEFAULT_BORTLE_SCALE_INDEX;

		core->getSkyDrawer()->setBortleScaleIndex(bIdx);
	}
}

void LandscapeMgr::setFlagFog(const bool displayed)
{
	if (landscape->getFlagShowFog() != displayed) {
		landscape->setFlagShowFog(displayed);
		emit fogDisplayedChanged(displayed);
	}
}

bool LandscapeMgr::getFlagFog() const
{
	return landscape->getFlagShowFog();
}

void LandscapeMgr::setFlagIllumination(const bool displayed)
{
	if (landscape->getFlagShowIllumination() != displayed) {
		landscape->setFlagShowIllumination(displayed);
		emit illuminationDisplayedChanged(displayed);
	}
}

bool LandscapeMgr::getFlagIllumination() const
{
	return landscape->getFlagShowIllumination();
}

void LandscapeMgr::setFlagLabels(const bool displayed)
{
	if (landscape->getFlagShowLabels() != displayed) {
		landscape->setFlagShowLabels(displayed);
		emit labelsDisplayedChanged(displayed);
	}
}

bool LandscapeMgr::getFlagLabels() const
{
	return landscape->getFlagShowLabels();
}

void LandscapeMgr::setFlagLandscapeAutoSelection(bool enableAutoSelect)
{
	if(enableAutoSelect != flagLandscapeAutoSelection)
	{
		flagLandscapeAutoSelection = enableAutoSelect;
		emit flagLandscapeAutoSelectionChanged(enableAutoSelect);
	}
}

bool LandscapeMgr::getFlagLandscapeAutoSelection() const
{
	return flagLandscapeAutoSelection;
}

void LandscapeMgr::setFlagAtmosphereAutoEnable(bool b)
{
	if(b != flagAtmosphereAutoEnabling)
	{
		flagAtmosphereAutoEnabling = b;
		emit setFlagAtmosphereAutoEnableChanged(b);
	}

}

bool LandscapeMgr::getFlagAtmosphereAutoEnable() const
{
	return flagAtmosphereAutoEnabling;
}

/*********************************************************************
 Retrieve list of the names of all the available landscapes
 *********************************************************************/
QStringList LandscapeMgr::getAllLandscapeNames() const
{
	return getNameToDirMap().keys();
}

QStringList LandscapeMgr::getAllLandscapeIDs() const
{
	return getNameToDirMap().values();
}

QStringList LandscapeMgr::getUserLandscapeIDs() const
{
	QStringList result;
	foreach (QString id, getNameToDirMap().values())
	{
		if(!packagedLandscapeIDs.contains(id))
		{
			result.append(id);
		}
	}
	return result;
}

QString LandscapeMgr::getCurrentLandscapeName() const
{
	return landscape->getName();
}

QString LandscapeMgr::getCurrentLandscapeHtmlDescription() const
{
	QString desc = getDescription();
	desc+="<p>";
	desc+="<b>"+q_("Author: ")+"</b>";
	desc+=landscape->getAuthorName();
	desc+="<br>";
	// This previously showed 0/0 for locationless landscapes!
	if (landscape->hasLocation())
	{
		desc+="<b>"+q_("Location: ")+"</b>";
		desc += StelUtils::radToDmsStrAdapt(landscape->getLocation().longitude * M_PI/180.);
		desc += "/" + StelUtils::radToDmsStrAdapt(landscape->getLocation().latitude *M_PI/180.);
		desc += QString(q_(", %1 m")).arg(landscape->getLocation().altitude);
		QString planetName = landscape->getLocation().planetName;
		if (!planetName.isEmpty())
		{
			const StelTranslator& trans = StelApp::getInstance().getLocaleMgr().getSkyTranslator();
			desc += "<br><b>"+q_("Celestial body:")+"</b> "+ trans.qtranslate(planetName);
		}
		desc += "<br><br>";
	}
	// TBD: Activate this or delete?
	//else
	//	desc+="<b>"+q_("Location: ")+"</b>" + q_("not specified (just decoration)") + "<br><br>";
	return desc;
}

//! Set flag for displaying Cardinals Points
void LandscapeMgr::setFlagCardinalsPoints(const bool displayed)
{
	if (cardinalsPoints->getFlagShow() != displayed) {
		cardinalsPoints->setFlagShow(displayed);
		emit cardinalsPointsDisplayedChanged(displayed);
	}
}

//! Get flag for displaying Cardinals Points
bool LandscapeMgr::getFlagCardinalsPoints() const
{
	return cardinalsPoints->getFlagShow();
}

//! Set Cardinals Points color
void LandscapeMgr::setColorCardinalPoints(const Vec3f& v)
{
	if(v != getColorCardinalPoints())
	{
		cardinalsPoints->setColor(v);
		emit cardinalsPointsColorChanged(v);
	}
}

//! Get Cardinals Points color
Vec3f LandscapeMgr::getColorCardinalPoints() const
{
	return cardinalsPoints->get_color();
}

///////////////////////////////////////////////////////////////////////////////////////
// Atmosphere
//! Set flag for displaying Atmosphere
void LandscapeMgr::setFlagAtmosphere(const bool displayed)
{
	if (atmosphere->getFlagShow() != displayed) {
		atmosphere->setFlagShow(displayed);
		StelApp::getInstance().getCore()->getSkyDrawer()->setFlagHasAtmosphere(displayed);
		emit atmosphereDisplayedChanged(displayed);
		//if (StelApp::getInstance().getSettings()->value("landscape/flag_fog", true).toBool())
		//	setFlagFog(displayed); // sync of visibility of fog because this is atmospheric phenomena
		// GZ This did not work as it may have been intended. Switch off fog, switch off atmosphere. Switch on atmosphere, and you have fog?
		// --> Fog is only drawn in Landscape if atmosphere is switched on!
	}
}

//! Get flag for displaying Atmosphere
bool LandscapeMgr::getFlagAtmosphere() const
{
	return atmosphere->getFlagShow();
}

//! Set atmosphere fade duration in s
void LandscapeMgr::setAtmosphereFadeDuration(const float f)
{
	atmosphere->setFadeDuration(f);
}

//! Get atmosphere fade duration in s
float LandscapeMgr::getAtmosphereFadeDuration() const
{
	return atmosphere->getFadeDuration();
}

//! Set light pollution luminance level
void LandscapeMgr::setAtmosphereLightPollutionLuminance(const float f)
{
	atmosphere->setLightPollutionLuminance(f);
}

//! Get light pollution luminance level
float LandscapeMgr::getAtmosphereLightPollutionLuminance() const
{
	return atmosphere->getLightPollutionLuminance();
}

//! Set the light pollution following the Bortle Scale
void LandscapeMgr::setAtmosphereBortleLightPollution(const int bIndex)
{
	// This is an empirical formula
	setAtmosphereLightPollutionLuminance(qMax(0.,0.0004*std::pow(bIndex-1, 2.1)));
}

void LandscapeMgr::setZRotation(const float d)
{
	if (landscape)
		landscape->setZRotation(d);
}

float LandscapeMgr::getLuminance() const
{
	return atmosphere->getRealDisplayIntensityFactor();
}

float LandscapeMgr::getAtmosphereAverageLuminance() const
{
	return atmosphere->getAverageLuminance();
}

// Override auto-computed luminance. Only use when you know what you are doing, and don't forget to unfreeze the average by calling this function with a negative value.
void LandscapeMgr::setAtmosphereAverageLuminance(const float overrideLum)
{
	atmosphere->setAverageLuminance(overrideLum);
}

Landscape* LandscapeMgr::createFromFile(const QString& landscapeFile, const QString& landscapeId)
{
	QSettings landscapeIni(landscapeFile, StelIniFormat);
	QString s;
	if (landscapeIni.status() != QSettings::NoError)
	{
		qWarning() << "ERROR parsing landscape.ini file: " << QDir::toNativeSeparators(landscapeFile);
		s = "";
	}
	else
		s = landscapeIni.value("landscape/type").toString();

	Landscape* landscape = Q_NULLPTR;
	if (s=="old_style")
		landscape = new LandscapeOldStyle();
	else if (s=="spherical")
		landscape = new LandscapeSpherical();
	else if (s=="fisheye")
		landscape = new LandscapeFisheye();
	else if (s=="polygonal")
		landscape = new LandscapePolygonal();
	else
	{
		qDebug() << "Unknown landscape type: \"" << s << "\"";

		// to avoid making this a fatal error, will load as a fisheye
		// if this fails, it just won't draw
		landscape = new LandscapeFisheye();
	}

	landscape->load(landscapeIni, landscapeId);
	QSettings *conf=StelApp::getInstance().getSettings();
	landscape->setLabelFontSize(conf->value("landscape/label_font_size", 15).toInt());
	return landscape;
}


QString LandscapeMgr::nameToID(const QString& name) const
{
	QMap<QString,QString> nameToDirMap = getNameToDirMap();

	if (nameToDirMap.find(name)!=nameToDirMap.end())
	{
		Q_ASSERT(0);
		return "error";
	}
	else
	{
		return nameToDirMap[name];
	}
}

/****************************************************************************
 get a map of landscape names (from landscape.ini name field) to ID (dir name)
 ****************************************************************************/
QMap<QString,QString> LandscapeMgr::getNameToDirMap() const
{
	QMap<QString,QString> result;
	QSet<QString> landscapeDirs = StelFileMgr::listContents("landscapes",StelFileMgr::Directory);

	foreach (const QString& dir, landscapeDirs)
	{
		QString fName = StelFileMgr::findFile("landscapes/" + dir + "/landscape.ini");
		if (!fName.isEmpty())
		{
			QSettings landscapeIni(fName, StelIniFormat);
			QString k = landscapeIni.value("landscape/name").toString();
			result[k] = dir;
		}
	}
	return result;
}

QString LandscapeMgr::installLandscapeFromArchive(QString sourceFilePath, const bool display, const bool toMainDirectory)
{
	Q_UNUSED(toMainDirectory);
	if (!QFile::exists(sourceFilePath))
	{
		qDebug() << "LandscapeMgr: File does not exist:" << QDir::toNativeSeparators(sourceFilePath);
		emit errorUnableToOpen(sourceFilePath);
		return QString();
	}

	QDir parentDestinationDir;
	parentDestinationDir.setPath(StelFileMgr::getUserDir());

	if (!parentDestinationDir.exists("landscapes"))
	{
		//qDebug() << "LandscapeMgr: No 'landscapes' subdirectory exists in" << parentDestinationDir.absolutePath();
		if (!parentDestinationDir.mkdir("landscapes"))
		{
			qWarning() << "LandscapeMgr: Unable to install landscape: Unable to create sub-directory 'landscapes' in" << QDir::toNativeSeparators(parentDestinationDir.absolutePath());
			emit errorUnableToOpen(QDir::cleanPath(parentDestinationDir.filePath("landscapes")));//parentDestinationDir.absolutePath()
			return QString();
		}
	}
	QDir destinationDir (parentDestinationDir.absoluteFilePath("landscapes"));

	Stel::QZipReader reader(sourceFilePath);
	if (reader.status() != Stel::QZipReader::NoError)
	{
		qWarning() << "LandscapeMgr: Unable to open as a ZIP archive:" << QDir::toNativeSeparators(sourceFilePath);
		emit errorNotArchive();
		return QString();
	}

	//Detect top directory
	QString topDir, iniPath;
	QList<Stel::QZipReader::FileInfo> infoList = reader.fileInfoList();
	foreach(Stel::QZipReader::FileInfo info, infoList)
	{
		QFileInfo fileInfo(info.filePath);
		if (fileInfo.fileName() == "landscape.ini")
		{
			iniPath = info.filePath;
			topDir = fileInfo.dir().path();
			break;
		}
	}
	if (topDir.isEmpty())
	{
		qWarning() << "LandscapeMgr: Unable to install landscape. There is no directory that contains a 'landscape.ini' file in the source archive.";
		emit errorNotArchive();
		return QString();
	}
	//Determine the landscape's identifier
	QString landscapeID = QFileInfo(topDir).fileName();
	if (landscapeID.length() < 2)
	{
		// If the archive has no top level directory
		// use the first 65 characters of its file name for an identifier
		QFileInfo sourceFileInfo(sourceFilePath);
		landscapeID = sourceFileInfo.baseName().left(65);
	}

	//Check for duplicate IDs
	if (getAllLandscapeIDs().contains(landscapeID))
	{
		qWarning() << "LandscapeMgr: Unable to install landscape. A landscape with the ID" << landscapeID << "already exists.";
		emit errorNotUnique(landscapeID);
		return QString();
	}

	//Read the .ini file and check if the landscape name is unique
	QTemporaryFile tempLandscapeIni("landscapeXXXXXX.ini");
	if (tempLandscapeIni.open())
	{
		QByteArray iniData = reader.fileData(iniPath);
		tempLandscapeIni.write(iniData);
		tempLandscapeIni.close();
		QSettings confLandscapeIni(tempLandscapeIni.fileName(), StelIniFormat);
		QString landscapeName = confLandscapeIni.value("landscape/name").toString();
		if (getAllLandscapeNames().contains(landscapeName))
		{
			qWarning() << "LandscapeMgr: Unable to install landscape. There is already a landscape named" << landscapeName;
			emit errorNotUnique(landscapeName);
			return QString();
		}
	}

	//Copy the landscape directory to the target
	//This case already has been handled - and commented out - above. :)
	if(destinationDir.exists(landscapeID))
	{
		qWarning() << "LandscapeMgr: A subdirectory" << landscapeID << "already exists in" << QDir::toNativeSeparators(destinationDir.absolutePath()) << "Its contents may be overwritten.";
	}
	else if(!destinationDir.mkdir(landscapeID))
	{
		qWarning() << "LandscapeMgr: Unable to install landscape. Unable to create" << landscapeID << "directory in" << QDir::toNativeSeparators(destinationDir.absolutePath());
		emit errorUnableToOpen(QDir::cleanPath(destinationDir.filePath(landscapeID)));
		return QString();
	}
	destinationDir.cd(landscapeID);
	foreach(Stel::QZipReader::FileInfo info, infoList)
	{
		QFileInfo fileInfo(info.filePath);
		if (info.isFile && fileInfo.dir().path() == topDir)
		{
			QByteArray data = reader.fileData(info.filePath);
			QFile out(destinationDir.filePath(fileInfo.fileName()));
			if (out.open(QIODevice::WriteOnly))
			{
				out.write(data);
				out.close();
			}
			else
			{
				qWarning() << "LandscapeMgr: cannot open " << QDir::toNativeSeparators(fileInfo.absoluteFilePath());
			}
		}
	}
	reader.close();
	//If necessary, make the new landscape the current landscape
	if (display)
	{
		setCurrentLandscapeID(landscapeID);
	}

	//Make sure that everyone knows that the list of available landscapes has changed
	emit landscapesChanged();

	qDebug() << "LandscapeMgr: Successfully installed landscape directory" << landscapeID << "to" << QDir::toNativeSeparators(destinationDir.absolutePath());
	return landscapeID;
}

bool LandscapeMgr::removeLandscape(const QString landscapeID)
{
	if (landscapeID.isEmpty())
	{
		qWarning() << "LandscapeMgr: Error! No landscape ID passed to removeLandscape().";
		return false;
	}

	if (packagedLandscapeIDs.contains(landscapeID))
	{
		qWarning() << "LandscapeMgr: Landscapes that are part of the default installation cannot be removed.";
		return false;
	}

	qDebug() << "LandscapeMgr: Trying to remove landscape" << landscapeID;

	QString landscapePath = getLandscapePath(landscapeID);
	if (landscapePath.isEmpty())
		return false;

	QDir landscapeDir(landscapePath);
	foreach (QString fileName, landscapeDir.entryList(QDir::Files | QDir::NoDotAndDotDot))
	{
		if(!landscapeDir.remove(fileName))
		{
			qWarning() << "LandscapeMgr: Unable to remove" << QDir::toNativeSeparators(fileName);
			emit errorRemoveManually(landscapeDir.absolutePath());
			return false;
		}
	}
	landscapeDir.cdUp();
	if(!landscapeDir.rmdir(landscapeID))
	{
		qWarning() << "LandscapeMgr: Error! Landscape" << landscapeID
				   << "could not be removed. "
				   << "Some files were deleted, but not all."
				   << endl
				   << "LandscapeMgr: You can delete manually" << QDir::cleanPath(landscapeDir.filePath(landscapeID));
		emit errorRemoveManually(QDir::cleanPath(landscapeDir.filePath(landscapeID)));
		return false;
	}

	qDebug() << "LandscapeMgr: Successfully removed" << QDir::toNativeSeparators(landscapePath);

	//If the landscape has been selected, revert to the default one
	//TODO: Make this optional?
	if (getCurrentLandscapeID() == landscapeID)
	{
		if(getDefaultLandscapeID() == landscapeID)
		{
			setDefaultLandscapeID(packagedLandscapeIDs.first());
			//TODO: Find what happens if a missing landscape is specified in the configuration file
		}

		setCurrentLandscapeID(getDefaultLandscapeID());
	}

	//Make sure that everyone knows that the list of available landscapes has changed
	emit landscapesChanged();

	return true;
}

QString LandscapeMgr::getLandscapePath(const QString landscapeID) const
{
	QString result;
	//Is this necessary? This function is private.
	if (landscapeID.isEmpty())
		return result;

	result = StelFileMgr::findFile("landscapes/" + landscapeID, StelFileMgr::Directory);
	if (result.isEmpty())
	{
		qWarning() << "LandscapeMgr: Error! Unable to find" << landscapeID;
		return result;
	}

	return result;
}

QString LandscapeMgr::loadLandscapeName(const QString landscapeID)
{
	QString landscapeName;
	if (landscapeID.isEmpty())
	{
		qWarning() << "LandscapeMgr: Error! No landscape ID passed to loadLandscapeName().";
		return landscapeName;
	}

	QString landscapePath = getLandscapePath(landscapeID);
	if (landscapePath.isEmpty())
		return landscapeName;

	QDir landscapeDir(landscapePath);
	if (landscapeDir.exists("landscape.ini"))
	{
		QString landscapeSettingsPath = landscapeDir.filePath("landscape.ini");
		QSettings landscapeSettings(landscapeSettingsPath, StelIniFormat);
		landscapeName = landscapeSettings.value("landscape/name").toString();
	}
	else
	{
		qWarning() << "LandscapeMgr: Error! Landscape directory" << QDir::toNativeSeparators(landscapePath) << "does not contain a 'landscape.ini' file";
	}

	return landscapeName;
}

quint64 LandscapeMgr::loadLandscapeSize(const QString landscapeID) const
{
	quint64 landscapeSize = 0;
	if (landscapeID.isEmpty())
	{
		qWarning() << "LandscapeMgr: Error! No landscape ID passed to loadLandscapeSize().";
		return landscapeSize;
	}

	QString landscapePath = getLandscapePath(landscapeID);
	if (landscapePath.isEmpty())
		return landscapeSize;

	QDir landscapeDir(landscapePath);
	foreach (QFileInfo file, landscapeDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot))
	{
		//qDebug() << "name:" << file.baseName() << "size:" << file.size();
		landscapeSize += file.size();
	}

	return landscapeSize;
}

QString LandscapeMgr::getDescription() const
{
	QString lang, desc, descFile, locDescriptionFile, engDescriptionFile;
	bool hasFile = true;

	lang = StelApp::getInstance().getLocaleMgr().getAppLanguage();
	locDescriptionFile = StelFileMgr::findFile("landscapes/" + getCurrentLandscapeID(), StelFileMgr::Directory) + "/description." + lang + ".utf8";
	engDescriptionFile = StelFileMgr::findFile("landscapes/" + getCurrentLandscapeID(), StelFileMgr::Directory) + "/description.en.utf8";

	// OK. Check the file with full name of locale
	if (!QFileInfo(locDescriptionFile).exists())
	{
		// Oops...  File not exists! What about short name of locale?
		lang = lang.split("_").at(0);
		locDescriptionFile = StelFileMgr::findFile("landscapes/" + getCurrentLandscapeID(), StelFileMgr::Directory) + "/description." + lang + ".utf8";
	}

	// Check localized description for landscape
	if (!locDescriptionFile.isEmpty() && QFileInfo(locDescriptionFile).exists())
	{		
		descFile = locDescriptionFile;
	}
	// OK. Localized description of landscape not exists. What about english description of its?
	else if (!engDescriptionFile.isEmpty() && QFileInfo(engDescriptionFile).exists())
	{
		descFile = engDescriptionFile;
	}
	// That file not exists too? OK. Will be used description from landscape.ini file.
	else
	{
		hasFile = false;
	}

	if (hasFile)
	{
		QFile file(descFile);
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			in.setCodec("UTF-8");
			desc = in.readAll();
			file.close();
		}
	}
	else
	{
		desc = QString("<h2>%1</h2>").arg(q_(landscape->getName()));
		desc += landscape->getDescription();
	}

	return desc;
}

/*
// GZ: Addition to identify landscape transparency. Used for development and debugging only, should be commented out in release builds.
// Also, StelMovementMgr l.382 event->accept() must be commented out for this here to work!
void LandscapeMgr::handleMouseClicks(QMouseEvent *event)
{
	switch (event->button())
	{
	case Qt::LeftButton :
		if (event->type()==QEvent::MouseButtonRelease)
		{
			Vec3d v;
			StelApp::getInstance().getCore()->getProjection(StelCore::FrameAltAz, StelCore::RefractionOff)->unProject(event->x(),event->y(),v);
			v.normalize();
			float trans=landscape->getOpacity(v);
			qDebug() << "Landscape opacity at screen X=" << event->x() << ", Y=" << event->y() << ": " << trans;
		}
		break;
	default: break;

	}
	// do not event->accept(), so that it is forwarded to other modules.
	return;
}
*/