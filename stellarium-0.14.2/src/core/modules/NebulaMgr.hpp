/*
 * Stellarium
 * Copyright (C) 2002 Fabien Chereau
 * Copyright (C) 2011 Alexander Wolf
 * Copyright (C) 2015 Georg Zotti
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

#ifndef _NEBULAMGR_HPP_
#define _NEBULAMGR_HPP_

#include "StelObjectType.hpp"
#include "StelFader.hpp"
#include "StelSphericalIndex.hpp"
#include "StelObjectModule.hpp"
#include "StelTextureTypes.hpp"
#include "Nebula.hpp"

#include <QString>
#include <QStringList>
#include <QFont>

class StelTranslator;
class StelToneReproducer;
class QSettings;
class StelPainter;

typedef QSharedPointer<Nebula> NebulaP;

//! @class NebulaMgr
//! Manage a collection of nebulae. This class is used
//! to display the NGC catalog with information, and textures for some of them.
// GZ: This doc seems outdated/misleading - photo textures are not mamaged here but in StelSkyImageTile

class NebulaMgr : public StelObjectModule
{
	Q_OBJECT
	Q_PROPERTY(bool flagHintDisplayed
		   READ getFlagHints
		   WRITE setFlagHints)

public:
	NebulaMgr();
	virtual ~NebulaMgr();

	///////////////////////////////////////////////////////////////////////////
	// Methods defined in the StelModule class
	//! Initialize the NebulaMgr object.
	//!  - Load the font into the Nebula class, which is used to draw Nebula labels.
	//!  - Load the texture used to draw nebula locations into the Nebula class (for
	//!     those with no individual texture).
	//!  - Load the pointer texture.
	//!  - Set flags values from ini parser which relate to nebula display.
	//!  - call updateI18n() to translate names.
	virtual void init();

	//! Draws all nebula objects.
	virtual void draw(StelCore* core);

	//! Update state which is time dependent.
	virtual void update(double deltaTime) {hintsFader.update((int)(deltaTime*1000)); flagShow.update((int)(deltaTime*1000));}

	//! Determines the order in which the various modules are drawn.
	virtual double getCallOrder(StelModuleActionName actionName) const;

	///////////////////////////////////////////////////////////////////////////
	// Methods defined in StelObjectManager class
	//! Used to get a vector of objects which are near to some position.
	//! @param v a vector representing the position in th sky around which to search for nebulae.
	//! @param limitFov the field of view around the position v in which to search for nebulae.
	//! @param core the StelCore to use for computations.
	//! @return an list containing the nebulae located inside the limitFov circle around position v.
	virtual QList<StelObjectP> searchAround(const Vec3d& v, double limitFov, const StelCore* core) const;

	//! Return the matching nebula object's pointer if exists or NULL.
	//! @param nameI18n The case in-sensistive nebula name or NGC M catalog name : format can
	//! be M31, M 31, NGC31, NGC 31
	virtual StelObjectP searchByNameI18n(const QString& nameI18n) const;

	//! Return the matching nebula if exists or NULL.
	//! @param name The case in-sensistive standard program name
	virtual StelObjectP searchByName(const QString& name) const;

	//! Find and return the list of at most maxNbItem objects auto-completing the passed object I18n name.
	//! @param objPrefix the case insensitive first letters of the searched object
	//! @param maxNbItem the maximum number of returned object names
	//! @param useStartOfWords the autofill mode for returned objects names
	//! @return a list of matching object name by order of relevance, or an empty list if nothing match
	virtual QStringList listMatchingObjectsI18n(const QString& objPrefix, int maxNbItem=5, bool useStartOfWords=false) const;
	//! Find and return the list of at most maxNbItem objects auto-completing the passed object English name.
	//! @param objPrefix the case insensitive first letters of the searched object
	//! @param maxNbItem the maximum number of returned object names
	//! @param useStartOfWords the autofill mode for returned objects names
	//! @return a list of matching object name by order of relevance, or an empty list if nothing match
	virtual QStringList listMatchingObjects(const QString& objPrefix, int maxNbItem=5, bool useStartOfWords=false) const;
	//! @note Loading deep-sky objects with the proper names only.
	virtual QStringList listAllObjects(bool inEnglish) const;
	virtual QStringList listAllObjectsByType(const QString& objType, bool inEnglish) const;
	virtual QString getName() const { return "Deep-sky objects"; }

	//! Compute the maximum magntiude for which hints will be displayed.
	float computeMaxMagHint(const class StelSkyDrawer* skyDrawer) const;
	
	void setCatalogFilters(const Nebula::CatalogGroup& cflags);
	const Nebula::CatalogGroup& getCatalogFilters() const { return Nebula::catalogFilters; }

	void setTypeFilters(const Nebula::TypeGroup& tflags) { Nebula::typeFilters=tflags; }
	const Nebula::TypeGroup& getTypeFilters() const { return Nebula::typeFilters; }

	bool objectInDisplayedCatalog(NebulaP n);

	///////////////////////////////////////////////////////////////////////////
	// Properties setters and getters
public slots:
	//! Set the default color used to draw the nebula symbols (default circles, etc).
	//! @param c The color of the nebula symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setCirclesColor(Vec3f(0.6,0.8,0.0));
	//! @endcode
	void setCirclesColor(const Vec3f& c);
	//! Get current value of the nebula circle color.
	const Vec3f& getCirclesColor(void) const;

	//! Set the color used to draw the galaxy symbols (ellipses).
	//! @param c The color of the galaxy symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setGalaxyColor(Vec3f(1.0,0.0,0.0));
	//! @endcode
	void setGalaxyColor(const Vec3f& c);
	//! Get current value of the galaxy symbol color.
	const Vec3f& getGalaxyColor(void) const;

	//! Set the color used to draw the active galaxy symbols (ellipses).
	//! @param c The color of the active galaxy symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setActiveGalaxyColor(Vec3f(1.0,0.0,0.0));
	//! @endcode
	void setActiveGalaxyColor(const Vec3f& c);
	//! Get current value of the active galaxy symbol color.
	const Vec3f& getActiveGalaxyColor(void) const;

	//! Set the color used to draw the interacting galaxy symbols (ellipses).
	//! @param c The color of the interacting galaxy symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setInteractingGalaxyColor(Vec3f(1.0,0.0,0.0));
	//! @endcode
	void setInteractingGalaxyColor(const Vec3f& c);
	//! Get current value of the interacting galaxy symbol color.
	const Vec3f& getInteractingGalaxyColor(void) const;

	//! Set the color used to draw the radio galaxy symbols (ellipses).
	//! @param c The color of the radio galaxy symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setRadioGalaxyColor(Vec3f(1.0,0.0,0.0));
	//! @endcode
	void setRadioGalaxyColor(const Vec3f& c);
	//! Get current value of the radio galaxy symbol color.
	const Vec3f& getRadioGalaxyColor(void) const;

	//! Set the color used to draw the quasars symbols (ellipses).
	//! @param c The color of the quasars symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setQuasarColor(Vec3f(1.0,0.0,0.0));
	//! @endcode
	void setQuasarColor(const Vec3f& c);
	//! Get current value of the quasar symbol color.
	const Vec3f& getQuasarColor(void) const;

	//! Set the color used to draw the bright nebula symbols (emission nebula boxes, planetary nebulae circles).
	//! @param c The color of the nebula symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setBrightNebulaColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	//! @deprecated
	void setBrightNebulaColor(const Vec3f& c);
	//! Get current value of the nebula circle color.
	//! @deprecated
	const Vec3f& getBrightNebulaColor(void) const;

	//! Set the color used to draw the bright nebula symbols (emission nebula boxes, planetary nebulae circles).
	//! @param c The color of the nebula symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setNebulaColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setNebulaColor(const Vec3f& c);
	//! Get current value of the nebula circle color.
	const Vec3f& getNebulaColor(void) const;

	//! Set the color used to draw the planetary nebulae symbols.
	//! @param c The color of the planetary nebulae symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setPlanetaryNebulaColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setPlanetaryNebulaColor(const Vec3f& c);
	//! Get current value of the planetary nebula circle color.
	const Vec3f& getPlanetaryNebulaColor(void) const;

	//! Set the color used to draw the reflection nebulae symbols.
	//! @param c The color of the reflection nebulae symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setReflectionNebulaColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setReflectionNebulaColor(const Vec3f& c);
	//! Get current value of the reflection nebula circle color.
	const Vec3f& getReflectionNebulaColor(void) const;

	//! Set the color used to draw the bipolar nebulae symbols.
	//! @param c The color of the bipolar nebulae symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setBipolarNebulaColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setBipolarNebulaColor(const Vec3f& c);
	//! Get current value of the bipolar nebula circle color.
	const Vec3f& getBipolarNebulaColor(void) const;

	//! Set the color used to draw the emission nebulae symbols.
	//! @param c The color of the emission nebulae symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setEmissionNebulaColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setEmissionNebulaColor(const Vec3f& c);
	//! Get current value of the emission nebula circle color.
	const Vec3f& getEmissionNebulaColor(void) const;

	//! Set the color used to draw the ionized hydrogen region symbols.
	//! @param c The color of the ionized hydrogen region symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setHydrogenRegionColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setHydrogenRegionColor(const Vec3f& c);
	//! Get current value of the hydrogen region symbol color.
	const Vec3f& getHydrogenRegionColor(void) const;

	//! Set the color used to draw the supernova remnant symbols.
	//! @param c The color of the supernova remnant symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setSupernovaRemnantColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setSupernovaRemnantColor(const Vec3f& c);
	//! Get current value of the supernova remnant symbol color.
	const Vec3f& getSupernovaRemnantColor(void) const;

	//! Set the color used to draw the interstellar matter symbols.
	//! @param c The color of the interstellar matter symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setInterstellarMatterColor(Vec3f(0.0,1.0,0.0));
	//! @endcode
	void setInterstellarMatterColor(const Vec3f& c);
	//! Get current value of the interstellar matter symbol color.
	const Vec3f& getInterstellarMatterColor(void) const;

	//! Set the color used to draw the dark nebula symbols (gray boxes).
	//! @param c The color of the dark nebula symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setDarkNebulaColor(Vec3f(0.2,0.2,0.2));
	//! @endcode
	void setDarkNebulaColor(const Vec3f& c);
	//! Get current value of the dark nebula color.
	const Vec3f& getDarkNebulaColor(void) const;

	//! Set the color used to draw the cluster associated with nebulosity symbols.
	//! @param c The color of the cluster associated with nebulosity symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setClusterWithNebulosityColor(Vec3f(0.2,0.2,0.2));
	//! @endcode
	void setClusterWithNebulosityColor(const Vec3f& c);
	//! Get current value of the cluster associated with nebulosity color.
	const Vec3f& getClusterWithNebulosityColor(void) const;

	//! Set the color used to draw the star cluster symbols (Open/Globular).
	//! @param c The color of the cluster symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setClusterColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setClusterColor(const Vec3f& c);
	//! Get current value of the star cluster symbol color.
	const Vec3f& getClusterColor(void) const;

	//! Set the color used to draw the open star cluster symbols.
	//! @param c The color of the open star cluster symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setOpenClusterColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setOpenClusterColor(const Vec3f& c);
	//! Get current value of the open star cluster symbol color.
	const Vec3f& getOpenClusterColor(void) const;

	//! Set the color used to draw the globular star cluster symbols.
	//! @param c The color of the globular star cluster symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setGlobularClusterColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setGlobularClusterColor(const Vec3f& c);
	//! Get current value of the globular star cluster symbol color.
	const Vec3f& getGlobularClusterColor(void) const;

	//! Set the color used to draw the stellar associations symbols.
	//! @param c The color of the stellar associations symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setStellarAssociationColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setStellarAssociationColor(const Vec3f& c);
	//! Get current value of the stellar association symbol color.
	const Vec3f& getStellarAssociationColor(void) const;

	//! Set the color used to draw the star clouds symbols.
	//! @param c The color of the star clouds symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setStarCloudColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setStarCloudColor(const Vec3f& c);
	//! Get current value of the star cloud symbol color.
	const Vec3f& getStarCloudColor(void) const;

	//! Set the color used to draw the emission objects symbols.
	//! @param c The color of the emission objects symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setEmissionObjectColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setEmissionObjectColor(const Vec3f& c);
	//! Get current value of the emission object symbol color.
	const Vec3f& getEmissionObjectColor(void) const;

	//! Set the color used to draw the BL Lac objects symbols.
	//! @param c The color of the BL Lac objects symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setBlLacObjectColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setBlLacObjectColor(const Vec3f& c);
	//! Get current value of the BL Lac object symbol color.
	const Vec3f& getBlLacObjectColor(void) const;

	//! Set the color used to draw the blazars symbols.
	//! @param c The color of the blazars symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setBlazarColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setBlazarColor(const Vec3f& c);
	//! Get current value of the blazar symbol color.
	const Vec3f& getBlazarColor(void) const;

	//! Set the color used to draw the molecular clouds symbols.
	//! @param c The color of the molecular clouds symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setMolecularCloudColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setMolecularCloudColor(const Vec3f& c);
	//! Get current value of the molecular cloud symbol color.
	const Vec3f& getMolecularCloudColor(void) const;

	//! Set the color used to draw the young stellar objects symbols.
	//! @param c The color of the young stellar objects symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setYoungStellarObjectColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setYoungStellarObjectColor(const Vec3f& c);
	//! Get current value of the young stellar object symbol color.
	const Vec3f& getYoungStellarObjectColor(void) const;

	//! Set the color used to draw the possible quasars symbols.
	//! @param c The color of the possible quasars symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setPossibleQuasarColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setPossibleQuasarColor(const Vec3f& c);
	//! Get current value of the possible quasar symbol color.
	const Vec3f& getPossibleQuasarColor(void) const;

	//! Set the color used to draw the possible planetary nebulae symbols.
	//! @param c The color of the possible planetary nebulae symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setPossiblePlanetaryNebulaColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setPossiblePlanetaryNebulaColor(const Vec3f& c);
	//! Get current value of the possible planetary nebula symbol color.
	const Vec3f& getPossiblePlanetaryNebulaColor(void) const;

	//! Set the color used to draw the protoplanetary nebulae symbols.
	//! @param c The color of the protoplanetary nebulae symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setProtoplanetaryNebulaColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setProtoplanetaryNebulaColor(const Vec3f& c);
	//! Get current value of the protoplanetary nebula symbol color.
	const Vec3f& getProtoplanetaryNebulaColor(void) const;

	//! Set the color used to draw the stars symbols.
	//! @param c The color of the stars symbols
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setStarColor(Vec3f(1.0,1.0,0.0));
	//! @endcode
	void setStarColor(const Vec3f& c);
	//! Get current value of the star symbol color.
	const Vec3f& getStarColor(void) const;

	//! Set Nebulae Hints circle scale.
	void setCircleScale(float scale);
	//! Get Nebulae Hints circle scale.
	float getCircleScale(void) const;

	//! Set how long it takes for nebula hints to fade in and out when turned on and off.
	//! @param duration given in seconds
	void setHintsFadeDuration(float duration) {hintsFader.setDuration((int) (duration * 1000.f));}

	//! Set flag for displaying Nebulae Hints.
	void setFlagHints(bool b) {hintsFader=b;}
	//! Get flag for displaying Nebulae Hints.
	bool getFlagHints(void) const {return hintsFader;}

	//! Set whether hints (symbols) should be scaled according to nebula size.
	void setHintsProportional(const bool proportional);
	//! Get whether hints (symbols) are scaled according to nebula size.
	bool getHintsProportional(void) const;

	//! Set whether hints (symbols) should be visible according to surface brightness value.
	void setFlagSurfaceBrightnessUsage(const bool usage) { Nebula::surfaceBrightnessUsage=usage; }
	//! Get whether hints (symbols) are visible according to surface brightness value.
	bool getFlagSurfaceBrightnessUsage(void) const { return Nebula::surfaceBrightnessUsage; }

	//! Set flag used to turn on and off Nebula rendering.
	void setFlagShow(bool b) { flagShow = b; }
	//! Get value of flag used to turn on and off Nebula rendering.
	bool getFlagShow(void) const { return flagShow; }

	//! Set flag used to turn on and off DSO type filtering.
	void setFlagTypeFiltersUsage(bool b) { Nebula::flagUsageTypeFilter=b; }
	//! Get value of flag used to turn on and off DSO type filtering.
	bool getFlagTypeFiltersUsage(void) const { return Nebula::flagUsageTypeFilter; }

	//! Set the color used to draw nebula labels.
	//! @param c The color of the nebula labels
	//! @code
	//! // example of usage in scripts
	//! NebulaMgr.setLabelsColor(Vec3f(1.0,0.0,0.0));
	//! @endcode
	void setLabelsColor(const Vec3f& c);
	//! Get current value of the nebula label color.
	const Vec3f& getLabelsColor(void) const;

	//! Set the amount of nebulae labels. The real amount is also proportional with FOV.
	//! The limit is set in function of the nebulae magnitude
	//! @param a the amount between 0 and 10. 0 is no labels, 10 is maximum of labels
	void setLabelsAmount(float a) {labelsAmount=a;}
	//! Get the amount of nebulae labels. The real amount is also proportional with FOV.
	//! @return the amount between 0 and 10. 0 is no labels, 10 is maximum of labels
	float getLabelsAmount(void) const {return labelsAmount;}

	//! Set the amount of nebulae hints. The real amount is also proportional with FOV.
	//! The limit is set in function of the nebulae magnitude
	//! @param f the amount between 0 and 10. 0 is no hints, 10 is maximum of hints
	void setHintsAmount(float f) {hintsAmount = f;}
	//! Get the amount of nebulae labels. The real amount is also proportional with FOV.
	//! @return the amount between 0 and 10. 0 is no hints, 10 is maximum of hints
	float getHintsAmount(void) const {return hintsAmount;}

private slots:
	//! Sets the colors of the Nebula labels and markers according to the
	//! values in a configuration object
	void setStelStyle(const QString& section);

	//! Update i18 names from English names according to passed translator.
	//! The translation is done using gettext with translated strings defined
	//! in translations.h
	void updateI18n();
	

private:

	//! Search for a nebula object by name. e.g. M83, NGC 1123, IC 1234.
	NebulaP search(const QString& name);

	//! Search the Nebulae by position
	NebulaP search(const Vec3d& pos);

	//! Load a set of nebula images.
	//! Each sub-directory of the INSTALLDIR/nebulae directory contains a set of
	//! nebula textures.  The sub-directory is the setName.  Each set has its
	//! own nebula_textures.fab file and corresponding image files.
	//! This function loads a set of textures.
	//! @param setName a string which corresponds to the directory where the set resides
	void loadNebulaSet(const QString& setName);

	//! Draw a nice animated pointer around the object
	void drawPointer(const StelCore* core, StelPainter& sPainter);

	NebulaP searchDSO(unsigned int DSO);
	NebulaP searchM(unsigned int M);
	NebulaP searchNGC(unsigned int NGC);
	NebulaP searchIC(unsigned int IC);
	NebulaP searchC(unsigned int C);
	NebulaP searchB(unsigned int B);
	NebulaP searchSh2(unsigned int Sh2);
	NebulaP searchVdB(unsigned int VdB);
	NebulaP searchRCW(unsigned int RCW);
	NebulaP searchLDN(unsigned int LDN);
	NebulaP searchLBN(unsigned int LBN);
	NebulaP searchCr(unsigned int Cr);
	NebulaP searchMel(unsigned int Mel);
	NebulaP searchPGC(unsigned int PGC);
	NebulaP searchUGC(unsigned int UGC);
	NebulaP searchCed(QString Ced);	

	// Load catalog of DSO
	bool loadDSOCatalog(const QString& filename);
	void convertDSOCatalog(const QString& in, const QString& out, bool decimal);
	// Load proper names for DSO
	bool loadDSONames(const QString& filename);

	QVector<NebulaP> dsoArray;		// The DSO list
	QHash<unsigned int, NebulaP> dsoIndex;

	LinearFader hintsFader;
	LinearFader flagShow;

	//! The internal grid for fast positional lookup
	StelSphericalIndex nebGrid;

	//! The amount of hints (between 0 and 10)
	float hintsAmount;
	//! The amount of labels (between 0 and 10)
	float labelsAmount;

	//! The selection pointer texture
	StelTextureSP texPointer;
	
	QFont nebulaFont;      // Font used for names printing

	// For DSO convertor
	bool flagConverter;
	bool flagDecimalCoordinates;
};

#endif // _NEBULAMGR_HPP_
