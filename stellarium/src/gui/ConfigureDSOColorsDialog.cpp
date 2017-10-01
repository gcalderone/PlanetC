/*
 * Stellarium
 * Copyright (C) 2016 Alexander Wolf
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
*/

#include "StelApp.hpp"
#include "StelCore.hpp"
#include "NebulaMgr.hpp"
#include "StelPropertyMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelTranslator.hpp"
#include "StelUtils.hpp"
#include "ConfigureDSOColorsDialog.hpp"
#include "ui_dsoColorsDialog.h"

#include <QSettings>
#include <QColorDialog>

ConfigureDSOColorsDialog::ConfigureDSOColorsDialog() : StelDialog("ConfigureDSOColorsDialog")
{
	ui = new Ui_ConfigureDSOColorsDialogForm;
}

ConfigureDSOColorsDialog::~ConfigureDSOColorsDialog()
{
	delete ui;
}

void ConfigureDSOColorsDialog::retranslate()
{
	if (dialog)
		ui->retranslateUi(dialog);
}


void ConfigureDSOColorsDialog::createDialogContent()
{
	ui->setupUi(dialog);
	
	//Signals and slots
	connect(&StelApp::getInstance(), SIGNAL(languageChanged()), this, SLOT(retranslate()));
	connect(ui->closeStelWindow, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->TitleBar, SIGNAL(movedTo(QPoint)), this, SLOT(handleMovedTo(QPoint)));

	colorButton(ui->colorDSOLabels,				"NebulaMgr.labelsColor");
	colorButton(ui->colorDSOMarkers,			"NebulaMgr.circlesColor");
	colorButton(ui->colorDSOGalaxies,			"NebulaMgr.galaxiesColor");
	colorButton(ui->colorDSOActiveGalaxies,			"NebulaMgr.activeGalaxiesColor");
	colorButton(ui->colorDSORadioGalaxies,			"NebulaMgr.radioGalaxiesColor");
	colorButton(ui->colorDSOInteractingGalaxies,		"NebulaMgr.interactingGalaxiesColor");
	colorButton(ui->colorDSOQuasars,			"NebulaMgr.quasarsColor");
	colorButton(ui->colorDSOPossibleQuasars,		"NebulaMgr.possibleQuasarsColor");
	colorButton(ui->colorDSOStarClusters,			"NebulaMgr.clustersColor");
	colorButton(ui->colorDSOOpenStarClusters,		"NebulaMgr.openClustersColor");
	colorButton(ui->colorDSOGlobularStarClusters,		"NebulaMgr.globularClustersColor");
	colorButton(ui->colorDSOStellarAssociations,		"NebulaMgr.stellarAssociationsColor");
	colorButton(ui->colorDSOStarClouds,			"NebulaMgr.starCloudsColor");
	colorButton(ui->colorDSOStars,				"NebulaMgr.starsColor");
	colorButton(ui->colorDSONebulae,			"NebulaMgr.nebulaeColor");
	colorButton(ui->colorDSOPlanetaryNebulae,		"NebulaMgr.planetaryNebulaeColor");
	colorButton(ui->colorDSODarkNebulae,			"NebulaMgr.darkNebulaeColor");
	colorButton(ui->colorDSOReflectionNebulae,		"NebulaMgr.reflectionNebulaeColor");
	colorButton(ui->colorDSOBipolarNebulae,			"NebulaMgr.bipolarNebulaeColor");
	colorButton(ui->colorDSOEmissionNebulae,		"NebulaMgr.emissionNebulaeColor");
	colorButton(ui->colorDSONebulosityClusters,		"NebulaMgr.clusterWithNebulosityColor");
	colorButton(ui->colorDSOPossiblePlanetaryNebulae,	"NebulaMgr.possiblePlanetaryNebulaeColor");
	colorButton(ui->colorDSOProtoplanetaryNebulae,		"NebulaMgr.protoplanetaryNebulaeColor");
	colorButton(ui->colorDSOHydrogenRegions,		"NebulaMgr.hydrogenRegionsColor");
	colorButton(ui->colorDSOInterstellarMatter,		"NebulaMgr.interstellarMatterColor");
	colorButton(ui->colorDSOEmissionObjects,		"NebulaMgr.emissionObjectsColor");
	colorButton(ui->colorDSOMolecularClouds,		"NebulaMgr.molecularCloudsColor");
	colorButton(ui->colorDSOBLLacObjects,			"NebulaMgr.blLacObjectsColor");
	colorButton(ui->colorDSOBlazars,			"NebulaMgr.blazarsColor");
	colorButton(ui->colorDSOYoungStellarObjects,		"NebulaMgr.youngStellarObjectsColor");
	colorButton(ui->colorDSOSupernovaRemnants,		"NebulaMgr.supernovaRemnantsColor");

	connect(ui->colorDSOLabels,				SIGNAL(released()), this, SLOT(askDSOLabelsColor()));
	connect(ui->colorDSOMarkers,				SIGNAL(released()), this, SLOT(askDSOMarkersColor()));
	connect(ui->colorDSOGalaxies,				SIGNAL(released()), this, SLOT(askDSOGalaxiesColor()));
	connect(ui->colorDSOActiveGalaxies,			SIGNAL(released()), this, SLOT(askDSOActiveGalaxiesColor()));
	connect(ui->colorDSORadioGalaxies,			SIGNAL(released()), this, SLOT(askDSORadioGalaxiesColor()));
	connect(ui->colorDSOInteractingGalaxies,		SIGNAL(released()), this, SLOT(askDSOInteractingGalaxiesColor()));
	connect(ui->colorDSOQuasars,				SIGNAL(released()), this, SLOT(askDSOQuasarsColor()));
	connect(ui->colorDSOPossibleQuasars,			SIGNAL(released()), this, SLOT(askDSOPossibleQuasarsColor()));
	connect(ui->colorDSOStarClusters,			SIGNAL(released()), this, SLOT(askDSOStarClustersColor()));
	connect(ui->colorDSOOpenStarClusters,			SIGNAL(released()), this, SLOT(askDSOOpenStarClustersColor()));
	connect(ui->colorDSOGlobularStarClusters,		SIGNAL(released()), this, SLOT(askDSOGlobularStarClustersColor()));
	connect(ui->colorDSOStellarAssociations,		SIGNAL(released()), this, SLOT(askDSOStellarAssociationsColor()));
	connect(ui->colorDSOStarClouds,				SIGNAL(released()), this, SLOT(askDSOStarCloudsColor()));
	connect(ui->colorDSOStars,				SIGNAL(released()), this, SLOT(askDSOStarsColor()));
	connect(ui->colorDSONebulae,				SIGNAL(released()), this, SLOT(askDSONebulaeColor()));
	connect(ui->colorDSOPlanetaryNebulae,			SIGNAL(released()), this, SLOT(askDSOPlanetaryNebulaeColor()));
	connect(ui->colorDSODarkNebulae,			SIGNAL(released()), this, SLOT(askDSODarkNebulaeColor()));
	connect(ui->colorDSOReflectionNebulae,			SIGNAL(released()), this, SLOT(askDSOReflectionNebulaeColor()));
	connect(ui->colorDSOBipolarNebulae,			SIGNAL(released()), this, SLOT(askDSOBipolarNebulaeColor()));
	connect(ui->colorDSOEmissionNebulae,			SIGNAL(released()), this, SLOT(askDSOEmissionNebulaeColor()));
	connect(ui->colorDSONebulosityClusters,			SIGNAL(released()), this, SLOT(askDSONebulosityClustersColor()));
	connect(ui->colorDSOPossiblePlanetaryNebulae,		SIGNAL(released()), this, SLOT(askDSOPossiblePlanetaryNebulaeColor()));
	connect(ui->colorDSOProtoplanetaryNebulae,		SIGNAL(released()), this, SLOT(askDSOProtoplanetaryNebulaeColor()));
	connect(ui->colorDSOHydrogenRegions,			SIGNAL(released()), this, SLOT(askDSOHydrogenRegionsColor()));
	connect(ui->colorDSOInterstellarMatter,			SIGNAL(released()), this, SLOT(askDSOInterstellarMatterColor()));
	connect(ui->colorDSOEmissionObjects,			SIGNAL(released()), this, SLOT(askDSOEmissionObjectsColor()));
	connect(ui->colorDSOMolecularClouds,			SIGNAL(released()), this, SLOT(askDSOMolecularCloudsColor()));
	connect(ui->colorDSOBLLacObjects,			SIGNAL(released()), this, SLOT(askDSOBLLacObjectsColor()));
	connect(ui->colorDSOBlazars,				SIGNAL(released()), this, SLOT(askDSOBlazarsColor()));
	connect(ui->colorDSOYoungStellarObjects,		SIGNAL(released()), this, SLOT(askDSOYoungStellarObjectsColor()));
	connect(ui->colorDSOSupernovaRemnants,			SIGNAL(released()), this, SLOT(askDSOSupernovaRemnantsColor()));

}

void ConfigureDSOColorsDialog::colorButton(QToolButton* toolButton, QString propName)
{
	StelProperty* prop = StelApp::getInstance().getStelPropertyManager()->getProperty(propName);
	Vec3f vColor = prop->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	// Use style sheet for create a nice buttons :)
	toolButton->setStyleSheet("QToolButton { background-color:" + color.name() + "; }");
	toolButton->setFixedSize(QSize(18, 18));
}

void ConfigureDSOColorsDialog::askDSOLabelsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.labelsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOLabels->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setLabelsColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_label_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOLabels->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOMarkersColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.circlesColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOMarkers->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setCirclesColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_circle_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOMarkers->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOGalaxiesColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.galaxiesColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOGalaxies->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setGalaxyColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_galaxy_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOGalaxies->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOActiveGalaxiesColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.activeGalaxiesColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOActiveGalaxies->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setActiveGalaxyColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_active_galaxy_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOActiveGalaxies->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSORadioGalaxiesColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.radioGalaxiesColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSORadioGalaxies->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setRadioGalaxyColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_radio_galaxy_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSORadioGalaxies->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOInteractingGalaxiesColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.interactingGalaxiesColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOInteractingGalaxies->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setInteractingGalaxyColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_interacting_galaxy_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOInteractingGalaxies->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOQuasarsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.quasarsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOQuasars->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setQuasarColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_quasar_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOQuasars->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOPossibleQuasarsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.possibleQuasarsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOPossibleQuasars->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setPossibleQuasarColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_possible_quasar_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOPossibleQuasars->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOStarClustersColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.clustersColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOStarClusters->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setClusterColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_cluster_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOStarClusters->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOOpenStarClustersColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.openClustersColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOOpenStarClusters->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setOpenClusterColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_open_cluster_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOOpenStarClusters->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOGlobularStarClustersColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.globularClustersColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOGlobularStarClusters->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setGlobularClusterColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_globular_cluster_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOGlobularStarClusters->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOStellarAssociationsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.stellarAssociationsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOStellarAssociations->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setStellarAssociationColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_stellar_association_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOStellarAssociations->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOStarCloudsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.starCloudsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOStarClouds->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setStarCloudColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_star_cloud_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOStarClouds->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOPlanetaryNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.planetaryNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOPlanetaryNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setPlanetaryNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_planetary_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOPlanetaryNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSODarkNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.darkNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSODarkNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setDarkNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_dark_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSODarkNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOReflectionNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.reflectionNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOReflectionNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setReflectionNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_reflection_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOReflectionNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOBipolarNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.bipolarNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOBipolarNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setBipolarNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_bipolar_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOBipolarNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOEmissionNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.emissionNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOEmissionNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setEmissionNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_emission_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOEmissionNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOPossiblePlanetaryNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.possiblePlanetaryNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOPossiblePlanetaryNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setPossiblePlanetaryNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_possible_planetary_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOPossiblePlanetaryNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOProtoplanetaryNebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.protoplanetaryNebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOProtoplanetaryNebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setProtoplanetaryNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_protoplanetary_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOProtoplanetaryNebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSONebulosityClustersColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.clusterWithNebulosityColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSONebulosityClusters->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setClusterWithNebulosityColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_cluster_with_nebulosity_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSONebulosityClusters->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSONebulaeColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.nebulaeColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSONebulae->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setNebulaColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_nebula_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSONebulae->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOHydrogenRegionsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.hydrogenRegionsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOHydrogenRegions->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setHydrogenRegionColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_hydrogen_region_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOHydrogenRegions->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOInterstellarMatterColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.interstellarMatterColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOInterstellarMatter->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setInterstellarMatterColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_interstellar_matter_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOInterstellarMatter->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOEmissionObjectsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.emissionObjectsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOEmissionObjects->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setEmissionObjectColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_emission_object_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOEmissionObjects->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOMolecularCloudsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.molecularCloudsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOMolecularClouds->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setMolecularCloudColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_molecular_cloud_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOMolecularClouds->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOBLLacObjectsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.blLacObjectsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOBLLacObjects->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setBlLacObjectColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_bl_lac_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOBLLacObjects->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOBlazarsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.blazarsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOBlazars->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setBlazarColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_blazar_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOBlazars->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOYoungStellarObjectsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.youngStellarObjectsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOYoungStellarObjects->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setYoungStellarObjectColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_young_stellar_object_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOYoungStellarObjects->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOSupernovaRemnantsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.supernovaRemnantsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOSupernovaRemnants->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setSupernovaRemnantColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_supernova_remnant_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOSupernovaRemnants->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}

void ConfigureDSOColorsDialog::askDSOStarsColor()
{
	Vec3f vColor = StelApp::getInstance().getStelPropertyManager()->getProperty("NebulaMgr.starsColor")->getValue().value<Vec3f>();
	QColor color(0,0,0);
	color.setRgbF(vColor.v[0], vColor.v[1], vColor.v[2]);
	QColor c = QColorDialog::getColor(color, Q_NULLPTR, q_(ui->colorDSOStars->toolTip()));
	if (c.isValid())
	{
		vColor = Vec3f(c.redF(), c.greenF(), c.blueF());
		GETSTELMODULE(NebulaMgr)->setStarColor(vColor);
		StelApp::getInstance().getSettings()->setValue("color/dso_star_color", StelUtils::vec3fToStr(vColor));
		ui->colorDSOStars->setStyleSheet("QToolButton { background-color:" + c.name() + "; }");
	}
}
