/*
 * Stellarium
 * Copyright (C) 2008 Fabien Chereau
 * Copyright (C) 2012 Timothy Reaves
 * Copyright (C) 2012 Bogdan Marinov
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

#include "Dialog.hpp"
#include "ConfigurationDialog.hpp"
#include "CustomDeltaTEquationDialog.hpp"
#include "StelMainView.hpp"
#include "ui_configurationDialog.h"
#include "StelApp.hpp"
#include "StelFileMgr.hpp"
#include "StelCore.hpp"
#include "StelLocaleMgr.hpp"
#include "StelProjector.hpp"
#include "StelObjectMgr.hpp"
#include "StelActionMgr.hpp"
#include "StelProgressController.hpp"

#include "StelUtils.hpp"
#include "StelCore.hpp"
#include "StelMovementMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelGui.hpp"
#include "StelGuiItems.hpp"
#include "StelLocation.hpp"
#include "StelSkyCultureMgr.hpp"
#include "StelSkyLayerMgr.hpp"
#include "ConstellationMgr.hpp"
#include "AsterismMgr.hpp"
#include "StarMgr.hpp"
#include "NebulaMgr.hpp"
#include "Planet.hpp"
#ifndef DISABLE_SCRIPTING
#include "StelScriptMgr.hpp"
#endif
#include "LabelMgr.hpp"
#include "ScreenImageMgr.hpp"
#include "SkyGui.hpp"
#include "StelJsonParser.hpp"
#include "StelTranslator.hpp"
#include "EphemWrapper.hpp"
#include "ToastMgr.hpp"

#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QComboBox>
#include <QDir>
#include <QDesktopWidget>

ConfigurationDialog::ConfigurationDialog(StelGui* agui, QObject* parent)
	: StelDialog("Configuration", parent)
	, isDownloadingStarCatalog(false)
	, nextStarCatalogToDownloadIndex(0)
	, starCatalogsCount(0)
	, hasDownloadedStarCatalog(false)
	, starCatalogDownloadReply(Q_NULLPTR)
	, currentDownloadFile(Q_NULLPTR)
	, progressBar(Q_NULLPTR)
	, gui(agui)
	, customDeltaTEquationDialog(Q_NULLPTR)
	, savedProjectionType(StelApp::getInstance().getCore()->getCurrentProjectionType())
{
	ui = new Ui_configurationDialogForm;
}

ConfigurationDialog::~ConfigurationDialog()
{
	delete ui;
	ui = Q_NULLPTR;
	delete customDeltaTEquationDialog;
	customDeltaTEquationDialog = Q_NULLPTR;
}

void ConfigurationDialog::retranslate()
{
	if (dialog) {
		ui->retranslateUi(dialog);

		//Initial FOV and direction on the "Main" page
		updateConfigLabels();
		
		//Star catalog download button and info
		updateStarCatalogControlsText();

		//Script information
		//(trigger re-displaying the description of the current item)
		#ifndef DISABLE_SCRIPTING
		scriptSelectionChanged(ui->scriptListWidget->currentItem()->text());
		#endif

		//Plug-in information
		populatePluginsList();

		populateDeltaTAlgorithmsList();
		populateDateFormatsList();
		populateTimeFormatsList();

		//Hack to shrink the tabs to optimal size after language change
		//by causing the list items to be laid out again.
		updateTabBarListWidgetWidth();
	}
}

void ConfigurationDialog::styleChanged()
{
	// Nothing for now
}

void ConfigurationDialog::createDialogContent()
{
	const StelProjectorP proj = StelApp::getInstance().getCore()->getProjection(StelCore::FrameJ2000);
	StelCore* core = StelApp::getInstance().getCore();

	StelMovementMgr* mvmgr = GETSTELMODULE(StelMovementMgr);

	ui->setupUi(dialog);
	connect(&StelApp::getInstance(), SIGNAL(languageChanged()), this, SLOT(retranslate()));

	// Set the main tab activated by default
	ui->configurationStackedWidget->setCurrentIndex(0);
	ui->stackListWidget->setCurrentRow(0);

	connect(ui->closeStelWindow, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->TitleBar, SIGNAL(movedTo(QPoint)), this, SLOT(handleMovedTo(QPoint)));

	// Main tab
	// Fill the language list widget from the available list
	QComboBox* cb = ui->programLanguageComboBox;
	cb->clear();
	cb->addItems(StelTranslator::globalTranslator->getAvailableLanguagesNamesNative(StelFileMgr::getLocaleDir()));
	cb->model()->sort(0);
	updateCurrentLanguage();
	connect(cb->lineEdit(), SIGNAL(editingFinished()), this, SLOT(updateCurrentLanguage()));
	connect(cb, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectLanguage(const QString&)));
	// Do the same for sky language:
	cb = ui->skycultureLanguageComboBox;
	cb->clear();
	cb->addItems(StelTranslator::globalTranslator->getAvailableLanguagesNamesNative(StelFileMgr::getLocaleDir(), "skycultures"));
	cb->model()->sort(0);
	updateCurrentSkyLanguage();
	connect(cb->lineEdit(), SIGNAL(editingFinished()), this, SLOT(updateCurrentSkyLanguage()));
	connect(cb, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectSkyLanguage(const QString&)));

	connect(ui->getStarsButton, SIGNAL(clicked()), this, SLOT(downloadStars()));
	connect(ui->downloadCancelButton, SIGNAL(clicked()), this, SLOT(cancelDownload()));
	connect(ui->downloadRetryButton, SIGNAL(clicked()), this, SLOT(downloadStars()));
	resetStarCatalogControls();

	connect(ui->de430checkBox, SIGNAL(clicked()), this, SLOT(de430ButtonClicked()));
	connect(ui->de431checkBox, SIGNAL(clicked()), this, SLOT(de431ButtonClicked()));
	resetEphemControls();

	ui->nutationCheckBox->setChecked(core->getUseNutation());
	connect(ui->nutationCheckBox, SIGNAL(toggled(bool)), core, SLOT(setUseNutation(bool)));
	ui->topocentricCheckBox->setChecked(core->getUseTopocentricCoordinates());
	connect(ui->topocentricCheckBox, SIGNAL(toggled(bool)), core, SLOT(setUseTopocentricCoordinates(bool)));
#ifdef Q_OS_WIN
	//Kinetic scrolling for tablet pc and pc
	QList<QWidget *> addscroll;
	addscroll << ui->pluginsListWidget << ui->scriptListWidget;
	installKineticScrolling(addscroll);
#endif

	// Selected object info
	if (gui->getInfoTextFilters() == StelObject::InfoStringGroup(0))
	{
		ui->noSelectedInfoRadio->setChecked(true);
	}
	else if (gui->getInfoTextFilters() == StelObject::ShortInfo)
	{
		ui->briefSelectedInfoRadio->setChecked(true);	
	}
	else if (gui->getInfoTextFilters() == StelObject::AllInfo)
	{
		ui->allSelectedInfoRadio->setChecked(true);
	}
	else
	{
		ui->customSelectedInfoRadio->setChecked(true);
	}
	updateSelectedInfoCheckBoxes();

	// Additional settings for selected object info
	connectBoolProperty(ui->checkBoxUMSurfaceBrightness, "NebulaMgr.flagSurfaceBrightnessArcsecUsage");
	
	connect(ui->noSelectedInfoRadio, SIGNAL(released()), this, SLOT(setNoSelectedInfo()));
	connect(ui->allSelectedInfoRadio, SIGNAL(released()), this, SLOT(setAllSelectedInfo()));
	connect(ui->briefSelectedInfoRadio, SIGNAL(released()), this, SLOT(setBriefSelectedInfo()));
	connect(ui->buttonGroupDisplayedFields, SIGNAL(buttonClicked(int)), this, SLOT(setSelectedInfoFromCheckBoxes()));
	
	// Navigation tab
	// Startup time
	if (core->getStartupTimeMode()=="actual")
		ui->systemTimeRadio->setChecked(true);
	else if (core->getStartupTimeMode()=="today")
		ui->todayRadio->setChecked(true);
	else
		ui->fixedTimeRadio->setChecked(true);
	connect(ui->systemTimeRadio, SIGNAL(clicked(bool)), this, SLOT(setStartupTimeMode()));
	connect(ui->todayRadio, SIGNAL(clicked(bool)), this, SLOT(setStartupTimeMode()));
	connect(ui->fixedTimeRadio, SIGNAL(clicked(bool)), this, SLOT(setStartupTimeMode()));

	ui->todayTimeSpinBox->setTime(core->getInitTodayTime());
	connect(ui->todayTimeSpinBox, SIGNAL(timeChanged(QTime)), core, SLOT(setInitTodayTime(QTime)));
	ui->fixedDateTimeEdit->setMinimumDate(QDate(100,1,1));
	ui->fixedDateTimeEdit->setDateTime(StelUtils::jdToQDateTime(core->getPresetSkyTime()));
	connect(ui->fixedDateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), core, SLOT(setPresetSkyTime(QDateTime)));

	ui->enableKeysNavigationCheckBox->setChecked(mvmgr->getFlagEnableMoveKeys() || mvmgr->getFlagEnableZoomKeys());
	ui->enableMouseNavigationCheckBox->setChecked(mvmgr->getFlagEnableMouseNavigation());
	connect(ui->enableKeysNavigationCheckBox, SIGNAL(toggled(bool)), mvmgr, SLOT(setFlagEnableMoveKeys(bool)));
	connect(ui->enableKeysNavigationCheckBox, SIGNAL(toggled(bool)), mvmgr, SLOT(setFlagEnableZoomKeys(bool)));
	connect(ui->enableMouseNavigationCheckBox, SIGNAL(toggled(bool)), mvmgr, SLOT(setFlagEnableMouseNavigation(bool)));
	connect(ui->fixedDateTimeCurrentButton, SIGNAL(clicked()), this, SLOT(setFixedDateTimeToCurrent()));
	connect(ui->editShortcutsPushButton, SIGNAL(clicked()), this, SLOT(showShortcutsWindow()));

	StelLocaleMgr & localeManager = StelApp::getInstance().getLocaleMgr();
	// Display formats of date
	populateDateFormatsList();
	int idx = ui->dateFormatsComboBox->findData(localeManager.getDateFormatStr(), Qt::UserRole, Qt::MatchCaseSensitive);
	if (idx==-1)
	{
		// Use system_default as default
		idx = ui->dateFormatsComboBox->findData(QVariant("system_default"), Qt::UserRole, Qt::MatchCaseSensitive);
	}
	ui->dateFormatsComboBox->setCurrentIndex(idx);
	connect(ui->dateFormatsComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setDateFormat()));

	// Display formats of time
	populateTimeFormatsList();
	idx = ui->timeFormatsComboBox->findData(localeManager.getTimeFormatStr(), Qt::UserRole, Qt::MatchCaseSensitive);
	if (idx==-1)
	{
		// Use system_default as default
		idx = ui->timeFormatsComboBox->findData(QVariant("system_default"), Qt::UserRole, Qt::MatchCaseSensitive);
	}
	ui->timeFormatsComboBox->setCurrentIndex(idx);
	connect(ui->timeFormatsComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setTimeFormat()));
	if (StelApp::getInstance().getSettings()->value("gui/flag_time_jd", false).toBool())
		ui->jdRadioButton->setChecked(true);
	else
		ui->dtRadioButton->setChecked(true);
	connect(ui->jdRadioButton, SIGNAL(clicked(bool)), this, SLOT(setButtonBarDTFormat()));
	connect(ui->dtRadioButton, SIGNAL(clicked(bool)), this, SLOT(setButtonBarDTFormat()));

	// Delta-T
	populateDeltaTAlgorithmsList();	
	idx = ui->deltaTAlgorithmComboBox->findData(core->getCurrentDeltaTAlgorithmKey(), Qt::UserRole, Qt::MatchCaseSensitive);
	if (idx==-1)
	{
		// Use Espenak & Meeus (2006) as default
		idx = ui->deltaTAlgorithmComboBox->findData(QVariant("EspenakMeeus"), Qt::UserRole, Qt::MatchCaseSensitive);
	}
	ui->deltaTAlgorithmComboBox->setCurrentIndex(idx);
	connect(ui->deltaTAlgorithmComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDeltaTAlgorithm(int)));
	connect(ui->pushButtonCustomDeltaTEquationDialog, SIGNAL(clicked()), this, SLOT(showCustomDeltaTEquationDialog()));
	if (core->getCurrentDeltaTAlgorithm()==StelCore::Custom)
		ui->pushButtonCustomDeltaTEquationDialog->setEnabled(true);

	// Tools tab
	ui->sphericMirrorCheckbox->setChecked(StelApp::getInstance().getViewportEffect() == "sphericMirrorDistorter");
	connect(ui->sphericMirrorCheckbox, SIGNAL(toggled(bool)), this, SLOT(setSphericMirror(bool)));
	ui->gravityLabelCheckbox->setChecked(proj->getFlagGravityLabels());
	connect(ui->gravityLabelCheckbox, SIGNAL(toggled(bool)), StelApp::getInstance().getCore(), SLOT(setFlagGravityLabels(bool)));
	connectBoolProperty(ui->selectSingleConstellationButton, "ConstellationMgr.isolateSelected");
	ui->diskViewportCheckbox->setChecked(proj->getMaskType() == StelProjector::MaskDisk);
	connect(ui->diskViewportCheckbox, SIGNAL(toggled(bool)), this, SLOT(setDiskViewport(bool)));
	connectBoolProperty(ui->autoZoomResetsDirectionCheckbox, "StelMovementMgr.flagAutoZoomOutResetsDirection");

	ui->showFlipButtonsCheckbox->setChecked(gui->getFlagShowFlipButtons());
	connect(ui->showFlipButtonsCheckbox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowFlipButtons(bool)));

	ui->showNebulaBgButtonCheckbox->setChecked(gui->getFlagShowNebulaBackgroundButton());
	connect(ui->showNebulaBgButtonCheckbox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowNebulaBackgroundButton(bool)));

	ui->showToastSurveyButtonCheckbox->setChecked(gui->getFlagShowToastSurveyButton());
	connect(ui->showToastSurveyButtonCheckbox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowToastSurveyButton(bool)));

	ui->showBookmarksButtonCheckBox->setChecked(gui->getFlagShowBookmarksButton());
	connect(ui->showBookmarksButtonCheckBox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowBookmarksButton(bool)));

	ui->showICRSGridButtonCheckBox->setChecked(gui->getFlagShowICRSGridButton());
	connect(ui->showICRSGridButtonCheckBox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowICRSGridButton(bool)));

	ui->showGalacticGridButtonCheckBox->setChecked(gui->getFlagShowGalacticGridButton());
	connect(ui->showGalacticGridButtonCheckBox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowGalacticGridButton(bool)));

	ui->showEclipticGridButtonCheckBox->setChecked(gui->getFlagShowEclipticGridButton());
	connect(ui->showEclipticGridButtonCheckBox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowEclipticGridButton(bool)));

	ui->showConstellationBoundariesButtonCheckBox->setChecked(gui->getFlagShowConstellationBoundariesButton());
	connect(ui->showConstellationBoundariesButtonCheckBox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowConstellationBoundariesButton(bool)));

	ui->decimalDegreeCheckBox->setChecked(StelApp::getInstance().getFlagShowDecimalDegrees());
	connect(ui->decimalDegreeCheckBox, SIGNAL(toggled(bool)), gui, SLOT(setFlagShowDecimalDegrees(bool)));
	ui->azimuthFromSouthcheckBox->setChecked(StelApp::getInstance().getFlagSouthAzimuthUsage());
	connect(ui->azimuthFromSouthcheckBox, SIGNAL(toggled(bool)), this, SLOT(updateStartPointForAzimuth(bool)));

	ui->mouseTimeoutCheckbox->setChecked(StelMainView::getInstance().getFlagCursorTimeout());
	ui->mouseTimeoutSpinBox->setValue(StelMainView::getInstance().getCursorTimeout());
	connect(ui->mouseTimeoutCheckbox, SIGNAL(clicked()), this, SLOT(cursorTimeOutChanged()));
	connect(ui->mouseTimeoutCheckbox, SIGNAL(toggled(bool)), this, SLOT(cursorTimeOutChanged()));
	connect(ui->mouseTimeoutSpinBox, SIGNAL(valueChanged(double)), this, SLOT(cursorTimeOutChanged(double)));

	ui->useButtonsBackgroundCheckBox->setChecked(StelMainView::getInstance().getFlagUseButtonsBackground());
	connect(ui->useButtonsBackgroundCheckBox, SIGNAL(toggled(bool)), this, SLOT(usageButtonsBackgroundChanged(bool)));

	ui->indicationMountModeCheckBox->setChecked(mvmgr->getFlagIndicationMountMode());
	connect(ui->indicationMountModeCheckBox, SIGNAL(toggled(bool)), mvmgr, SLOT(setFlagIndicationMountMode(bool)));

	// General Option Save
	connect(ui->saveViewDirAsDefaultPushButton, SIGNAL(clicked()), this, SLOT(saveCurrentViewDirSettings()));
	connect(ui->saveSettingsAsDefaultPushButton, SIGNAL(clicked()), this, SLOT(saveAllSettings()));
	connect(ui->restoreDefaultsButton, SIGNAL(clicked()), this, SLOT(setDefaultViewOptions()));

	// Screenshots
	ui->screenshotDirEdit->setText(StelFileMgr::getScreenshotDir());
	connect(ui->screenshotDirEdit, SIGNAL(textChanged(QString)), this, SLOT(selectScreenshotDir(QString)));
	connect(ui->screenshotBrowseButton, SIGNAL(clicked()), this, SLOT(browseForScreenshotDir()));

	ui->invertScreenShotColorsCheckBox->setChecked(StelMainView::getInstance().getFlagInvertScreenShotColors());
	connect(ui->invertScreenShotColorsCheckBox, SIGNAL(toggled(bool)), &StelMainView::getInstance(), SLOT(setFlagInvertScreenShotColors(bool)));

	connectBoolProperty(ui->autoEnableAtmosphereCheckBox, "LandscapeMgr.flagAtmosphereAutoEnabling");
	connectBoolProperty(ui->autoChangeLandscapesCheckBox, "LandscapeMgr.flagLandscapeAutoSelection");

	// script tab controls
	#ifndef DISABLE_SCRIPTING
	StelScriptMgr& scriptMgr = StelApp::getInstance().getScriptMgr();
	connect(ui->scriptListWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(scriptSelectionChanged(const QString&)));
	connect(ui->runScriptButton, SIGNAL(clicked()), this, SLOT(runScriptClicked()));
	connect(ui->stopScriptButton, SIGNAL(clicked()), this, SLOT(stopScriptClicked()));
	if (scriptMgr.scriptIsRunning())
		aScriptIsRunning();
	else
		aScriptHasStopped();
	connect(&scriptMgr, SIGNAL(scriptRunning()), this, SLOT(aScriptIsRunning()));
	connect(&scriptMgr, SIGNAL(scriptStopped()), this, SLOT(aScriptHasStopped()));
	ui->scriptListWidget->setSortingEnabled(true);
	populateScriptsList();
	connect(this, SIGNAL(visibleChanged(bool)), this, SLOT(populateScriptsList()));
	#endif

	// plugins control
	connect(ui->pluginsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(pluginsSelectionChanged(QListWidgetItem*, QListWidgetItem*)));
	connect(ui->pluginLoadAtStartupCheckBox, SIGNAL(stateChanged(int)), this, SLOT(loadAtStartupChanged(int)));
	connect(ui->pluginsListWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(pluginConfigureCurrentSelection()));
	connect(ui->pluginConfigureButton, SIGNAL(clicked()), this, SLOT(pluginConfigureCurrentSelection()));
	populatePluginsList();

	updateConfigLabels();
	updateTabBarListWidgetWidth();
}

void ConfigurationDialog::updateCurrentLanguage()
{
	QComboBox* cb = ui->programLanguageComboBox;
	QString appLang = StelApp::getInstance().getLocaleMgr().getAppLanguage();
	QString l2 = StelTranslator::iso639_1CodeToNativeName(appLang);

	if (cb->currentText() == l2)
		return;

	int lt = cb->findText(l2, Qt::MatchExactly);
	if (lt == -1 && appLang.contains('_'))
	{
		l2 = appLang.left(appLang.indexOf('_'));
		l2=StelTranslator::iso639_1CodeToNativeName(l2);
		lt = cb->findText(l2, Qt::MatchExactly);
	}
	if (lt!=-1)
		cb->setCurrentIndex(lt);
}

void ConfigurationDialog::updateCurrentSkyLanguage()
{
	QComboBox* cb = ui->skycultureLanguageComboBox;
	QString skyLang = StelApp::getInstance().getLocaleMgr().getSkyLanguage();
	QString l2 = StelTranslator::iso639_1CodeToNativeName(skyLang);

	if (cb->currentText() == l2)
		return;

	int lt = cb->findText(l2, Qt::MatchExactly);
	if (lt == -1 && skyLang.contains('_'))
	{
		l2 = skyLang.left(skyLang.indexOf('_'));
		l2=StelTranslator::iso639_1CodeToNativeName(l2);
		lt = cb->findText(l2, Qt::MatchExactly);
	}
	if (lt!=-1)
		cb->setCurrentIndex(lt);
}

void ConfigurationDialog::selectLanguage(const QString& langName)
{
	QString code = StelTranslator::nativeNameToIso639_1Code(langName);
	StelApp::getInstance().getLocaleMgr().setAppLanguage(code);
	StelMainView::getInstance().initTitleI18n();
}

void ConfigurationDialog::selectSkyLanguage(const QString& langName)
{
	QString code = StelTranslator::nativeNameToIso639_1Code(langName);
	StelApp::getInstance().getLocaleMgr().setSkyLanguage(code);
}

void ConfigurationDialog::setStartupTimeMode()
{
	if (ui->systemTimeRadio->isChecked())
		StelApp::getInstance().getCore()->setStartupTimeMode("actual");
	else if (ui->todayRadio->isChecked())
		StelApp::getInstance().getCore()->setStartupTimeMode("today");
	else
		StelApp::getInstance().getCore()->setStartupTimeMode("preset");

	StelApp::getInstance().getCore()->setInitTodayTime(ui->todayTimeSpinBox->time());
	StelApp::getInstance().getCore()->setPresetSkyTime(ui->fixedDateTimeEdit->dateTime());
}

void ConfigurationDialog::setButtonBarDTFormat()
{
	if (ui->jdRadioButton->isChecked())
		gui->getButtonBar()->setFlagTimeJd(true);
	else
		gui->getButtonBar()->setFlagTimeJd(false);
}

void ConfigurationDialog::showShortcutsWindow()
{
	StelAction* action = StelApp::getInstance().getStelActionManager()->findAction("actionShow_Shortcuts_Window_Global");
	if (action)
		action->setChecked(true);
}

void ConfigurationDialog::setDiskViewport(bool b)
{
	if (b)
		StelApp::getInstance().getCore()->setMaskType(StelProjector::MaskDisk);
	else
		StelApp::getInstance().getCore()->setMaskType(StelProjector::MaskNone);
}

void ConfigurationDialog::setSphericMirror(bool b)
{
	StelCore* core = StelApp::getInstance().getCore();
	if (b)
	{
		savedProjectionType = core->getCurrentProjectionType();
		core->setCurrentProjectionType(StelCore::ProjectionFisheye);
		StelApp::getInstance().setViewportEffect("sphericMirrorDistorter");
	}
	else
	{
		core->setCurrentProjectionType((StelCore::ProjectionType)savedProjectionType);
		StelApp::getInstance().setViewportEffect("none");
	}
}

void ConfigurationDialog::setNoSelectedInfo(void)
{
	gui->setInfoTextFilters(StelObject::InfoStringGroup(0));
	updateSelectedInfoCheckBoxes();
}

void ConfigurationDialog::setAllSelectedInfo(void)
{
	gui->setInfoTextFilters(StelObject::InfoStringGroup(StelObject::AllInfo));
	updateSelectedInfoCheckBoxes();
}

void ConfigurationDialog::setBriefSelectedInfo(void)
{
	gui->setInfoTextFilters(StelObject::InfoStringGroup(StelObject::ShortInfo));
	updateSelectedInfoCheckBoxes();
}

void ConfigurationDialog::setSelectedInfoFromCheckBoxes()
{
	// As this signal will be called when a checkbox is toggled,
	// change the general mode to Custom.
	if (!ui->customSelectedInfoRadio->isChecked())
		ui->customSelectedInfoRadio->setChecked(true);
	
	StelObject::InfoStringGroup flags(0);

	if (ui->checkBoxName->isChecked())
		flags |= StelObject::Name;
	if (ui->checkBoxCatalogNumbers->isChecked())
		flags |= StelObject::CatalogNumber;
	if (ui->checkBoxVisualMag->isChecked())
		flags |= StelObject::Magnitude;
	if (ui->checkBoxAbsoluteMag->isChecked())
		flags |= StelObject::AbsoluteMagnitude;
	if (ui->checkBoxRaDecJ2000->isChecked())
		flags |= StelObject::RaDecJ2000;
	if (ui->checkBoxRaDecOfDate->isChecked())
		flags |= StelObject::RaDecOfDate;
	if (ui->checkBoxHourAngle->isChecked())
		flags |= StelObject::HourAngle;
	if (ui->checkBoxAltAz->isChecked())
		flags |= StelObject::AltAzi;
	if (ui->checkBoxDistance->isChecked())
		flags |= StelObject::Distance;
	if (ui->checkBoxSize->isChecked())
		flags |= StelObject::Size;
	if (ui->checkBoxExtra->isChecked())
		flags |= StelObject::Extra;
	if (ui->checkBoxGalacticCoordinates->isChecked())
		flags |= StelObject::GalacticCoord;
	if (ui->checkBoxSupergalacticCoordinates->isChecked())
		flags |= StelObject::SupergalacticCoord;
	if (ui->checkBoxType->isChecked())
		flags |= StelObject::ObjectType;
	if (ui->checkBoxEclipticCoordsJ2000->isChecked())
		flags |= StelObject::EclipticCoordJ2000;
	if (ui->checkBoxEclipticCoordsOfDate->isChecked())
		flags |= StelObject::EclipticCoordOfDate;
	if (ui->checkBoxConstellation->isChecked())
		flags |= StelObject::IAUConstellation;
	if (ui->checkBoxSiderealTime->isChecked())
		flags |= StelObject::SiderealTime;

	gui->setInfoTextFilters(flags);
}


void ConfigurationDialog::updateStartPointForAzimuth(bool b)
{
	StelApp::getInstance().setFlagSouthAzimuthUsage(b);
}

void ConfigurationDialog::cursorTimeOutChanged()
{
	StelMainView::getInstance().setFlagCursorTimeout(ui->mouseTimeoutCheckbox->isChecked());
	StelMainView::getInstance().setCursorTimeout(ui->mouseTimeoutSpinBox->value());
}

void ConfigurationDialog::usageButtonsBackgroundChanged(bool b)
{
	StelMainView::getInstance().setFlagUseButtonsBackground(b);
	emit StelMainView::getInstance().updateIconsRequested();
}

void ConfigurationDialog::browseForScreenshotDir()
{
	QString oldScreenshorDir = StelFileMgr::getScreenshotDir();
	QString newScreenshotDir = QFileDialog::getExistingDirectory(Q_NULLPTR, q_("Select screenshot directory"), oldScreenshorDir, QFileDialog::ShowDirsOnly);

	if (!newScreenshotDir.isEmpty()) {
		// remove trailing slash
		if (newScreenshotDir.right(1) == "/")
			newScreenshotDir = newScreenshotDir.left(newScreenshotDir.length()-1);

		ui->screenshotDirEdit->setText(newScreenshotDir);
	}
}

void ConfigurationDialog::selectScreenshotDir(const QString& dir)
{
	try
	{
		StelFileMgr::setScreenshotDir(dir);
	}
	catch (std::runtime_error& e)
	{
		Q_UNUSED(e);
		// nop
		// this will happen when people are only half way through typing dirs
	}
}

// Store FOV and viewing dir.
void ConfigurationDialog::saveCurrentViewDirSettings()
{
	StelMovementMgr* mvmgr = GETSTELMODULE(StelMovementMgr);
	Q_ASSERT(mvmgr);

	mvmgr->setInitFov(mvmgr->getCurrentFov());
	mvmgr->setInitViewDirectionToCurrent();
}


// Save the current viewing option including landscape, location and sky culture
// This doesn't include the current viewing direction, time and FOV since those have specific controls
void ConfigurationDialog::saveAllSettings()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);

	// TBD: store more properties directly, avoid to query all modules.
	StelPropertyMgr* propMgr=StelApp::getInstance().getStelPropertyManager();
	Q_ASSERT(propMgr);

	NebulaMgr* nmgr = GETSTELMODULE(NebulaMgr);
	Q_ASSERT(nmgr);
	StelMovementMgr* mvmgr = GETSTELMODULE(StelMovementMgr);
	Q_ASSERT(mvmgr);

	StelCore* core = StelApp::getInstance().getCore();
	const StelProjectorP proj = core->getProjection(StelCore::FrameJ2000);
	Q_ASSERT(proj);

	// view dialog / sky tab settings
	conf->setValue("stars/absolute_scale",			propMgr->getStelPropertyValue("StelSkyDrawer.absoluteStarScale").toFloat());
	conf->setValue("stars/relative_scale",			propMgr->getStelPropertyValue("StelSkyDrawer.relativeStarScale").toFloat());
	conf->setValue("stars/flag_star_twinkle",		propMgr->getStelPropertyValue("StelSkyDrawer.flagTwinkle").toFloat());
	conf->setValue("stars/star_twinkle_amount",		propMgr->getStelPropertyValue("StelSkyDrawer.twinkleAmount").toFloat());
	conf->setValue("astro/flag_star_magnitude_limit",	propMgr->getStelPropertyValue("StelSkyDrawer.flagStarMagnitudeLimit").toBool());
	conf->setValue("astro/star_magnitude_limit",		propMgr->getStelPropertyValue("StelSkyDrawer.customStarMagLimit").toFloat());
	conf->setValue("astro/flag_planet_magnitude_limit",	propMgr->getStelPropertyValue("StelSkyDrawer.flagPlanetMagnitudeLimit").toBool());
	conf->setValue("astro/planet_magnitude_limit",		propMgr->getStelPropertyValue("StelSkyDrawer.customPlanetMagLimit").toFloat());
	conf->setValue("astro/flag_nebula_magnitude_limit",	propMgr->getStelPropertyValue("StelSkyDrawer.flagNebulaMagnitudeLimit").toBool());
	conf->setValue("astro/nebula_magnitude_limit",		propMgr->getStelPropertyValue("StelSkyDrawer.customNebulaMagLimit").toFloat());
	conf->setValue("viewing/use_luminance_adaptation",	propMgr->getStelPropertyValue("StelSkyDrawer.flagLuminanceAdaptation").toBool());
	conf->setValue("astro/flag_planets",			propMgr->getStelPropertyValue("SolarSystem.planetsDisplayed").toBool());
	conf->setValue("astro/flag_planets_hints",		propMgr->getStelPropertyValue("SolarSystem.flagHints").toBool());
	conf->setValue("astro/flag_planets_orbits",		propMgr->getStelPropertyValue("SolarSystem.flagOrbits").toBool());
	conf->setValue("viewing/flag_isolated_trails",		propMgr->getStelPropertyValue("SolarSystem.flagIsolatedTrails").toBool());
	conf->setValue("viewing/flag_isolated_orbits",		propMgr->getStelPropertyValue("SolarSystem.flagIsolatedOrbits").toBool());
	conf->setValue("astro/flag_light_travel_time",		propMgr->getStelPropertyValue("SolarSystem.flagLightTravelTime").toBool());
	conf->setValue("viewing/flag_moon_scaled",		propMgr->getStelPropertyValue("SolarSystem.flagMoonScale").toBool());
	conf->setValue("viewing/moon_scale",			propMgr->getStelPropertyValue("SolarSystem.moonScale").toFloat());
	conf->setValue("viewing/flag_minorbodies_scaled",	propMgr->getStelPropertyValue("SolarSystem.flagMinorBodyScale").toBool());
	conf->setValue("viewing/minorbodies_scale",		propMgr->getStelPropertyValue("SolarSystem.minorBodyScale").toFloat());
	conf->setValue("astro/meteor_zhr",			propMgr->getStelPropertyValue("SporadicMeteorMgr.zhr").toInt());
	conf->setValue("astro/flag_milky_way",			propMgr->getStelPropertyValue("MilkyWay.flagMilkyWayDisplayed").toBool());
	conf->setValue("astro/milky_way_intensity",		propMgr->getStelPropertyValue("MilkyWay.intensity").toFloat());
	conf->setValue("astro/flag_zodiacal_light",		propMgr->getStelPropertyValue("ZodiacalLight.flagZodiacalLightDisplayed").toBool());
	conf->setValue("astro/zodiacal_light_intensity",	propMgr->getStelPropertyValue("ZodiacalLight.intensity").toFloat());
	conf->setValue("astro/flag_grs_custom",			propMgr->getStelPropertyValue("SolarSystem.flagCustomGrsSettings").toBool());
	conf->setValue("astro/grs_longitude",			propMgr->getStelPropertyValue("SolarSystem.customGrsLongitude").toInt());
	conf->setValue("astro/grs_drift",			propMgr->getStelPropertyValue("SolarSystem.customGrsDrift").toDouble());
	conf->setValue("astro/grs_jd",				propMgr->getStelPropertyValue("SolarSystem.customGrsJD").toDouble());
	conf->setValue("astro/flag_planets_labels",		propMgr->getStelPropertyValue("SolarSystem.labelsDisplayed").toBool());
	conf->setValue("astro/labels_amount",			propMgr->getStelPropertyValue("SolarSystem.labelsAmount").toDouble());
	conf->setValue("viewing/flag_planets_native_names",	propMgr->getStelPropertyValue("SolarSystem.flagNativePlanetNames").toBool());
	conf->setValue("astro/flag_use_obj_models",		propMgr->getStelPropertyValue("SolarSystem.flagUseObjModels").toBool());
	conf->setValue("astro/flag_show_obj_self_shadows",	propMgr->getStelPropertyValue("SolarSystem.flagShowObjSelfShadows").toBool());
	conf->setValue("astro/apparent_magnitude_algorithm",	Planet::getApparentMagnitudeAlgorithmString());

	// view dialog / markings tab settings
	conf->setValue("viewing/flag_gridlines",		propMgr->getStelPropertyValue("GridLinesMgr.gridlinesDisplayed").toBool());
	conf->setValue("viewing/flag_azimuthal_grid",		propMgr->getStelPropertyValue("GridLinesMgr.azimuthalGridDisplayed").toBool());
	conf->setValue("viewing/flag_equatorial_grid",		propMgr->getStelPropertyValue("GridLinesMgr.equatorGridDisplayed").toBool());
	conf->setValue("viewing/flag_equatorial_J2000_grid",	propMgr->getStelPropertyValue("GridLinesMgr.equatorJ2000GridDisplayed").toBool());
	conf->setValue("viewing/flag_equator_line",		propMgr->getStelPropertyValue("GridLinesMgr.equatorLineDisplayed").toBool());
	conf->setValue("viewing/flag_equator_J2000_line",	propMgr->getStelPropertyValue("GridLinesMgr.equatorJ2000LineDisplayed").toBool());
	conf->setValue("viewing/flag_ecliptic_line",		propMgr->getStelPropertyValue("GridLinesMgr.eclipticLineDisplayed").toBool());
	conf->setValue("viewing/flag_ecliptic_J2000_line",	propMgr->getStelPropertyValue("GridLinesMgr.eclipticJ2000LineDisplayed").toBool());
	conf->setValue("viewing/flag_ecliptic_grid",		propMgr->getStelPropertyValue("GridLinesMgr.eclipticGridDisplayed").toBool());
	conf->setValue("viewing/flag_ecliptic_J2000_grid",	propMgr->getStelPropertyValue("GridLinesMgr.eclipticJ2000GridDisplayed").toBool());
	conf->setValue("viewing/flag_meridian_line",		propMgr->getStelPropertyValue("GridLinesMgr.meridianLineDisplayed").toBool());
	conf->setValue("viewing/flag_longitude_line",		propMgr->getStelPropertyValue("GridLinesMgr.longitudeLineDisplayed").toBool());
	conf->setValue("viewing/flag_horizon_line",		propMgr->getStelPropertyValue("GridLinesMgr.horizonLineDisplayed").toBool());
	conf->setValue("viewing/flag_galactic_grid",		propMgr->getStelPropertyValue("GridLinesMgr.galacticGridDisplayed").toBool());
	conf->setValue("viewing/flag_galactic_equator_line",	propMgr->getStelPropertyValue("GridLinesMgr.galacticEquatorLineDisplayed").toBool());
	conf->setValue("viewing/flag_cardinal_points",		propMgr->getStelPropertyValue("LandscapeMgr.cardinalsPointsDisplayed").toBool());
	conf->setValue("viewing/flag_prime_vertical_line",	propMgr->getStelPropertyValue("GridLinesMgr.primeVerticalLineDisplayed").toBool());
	conf->setValue("viewing/flag_colure_lines",		propMgr->getStelPropertyValue("GridLinesMgr.colureLinesDisplayed").toBool());
	conf->setValue("viewing/flag_precession_circles",	propMgr->getStelPropertyValue("GridLinesMgr.precessionCirclesDisplayed").toBool());
	conf->setValue("viewing/flag_circumpolar_circles",	propMgr->getStelPropertyValue("GridLinesMgr.circumpolarCirclesDisplayed").toBool());
	conf->setValue("viewing/flag_supergalactic_grid",	propMgr->getStelPropertyValue("GridLinesMgr.supergalacticGridDisplayed").toBool());
	conf->setValue("viewing/flag_supergalactic_equator_line", propMgr->getStelPropertyValue("GridLinesMgr.supergalacticEquatorLineDisplayed").toBool());
	conf->setValue("viewing/flag_celestial_J2000_poles",	propMgr->getStelPropertyValue("GridLinesMgr.celestialJ2000PolesDisplayed").toBool());
	conf->setValue("viewing/flag_celestial_poles",		propMgr->getStelPropertyValue("GridLinesMgr.celestialPolesDisplayed").toBool());
	conf->setValue("viewing/flag_zenith_nadir",		propMgr->getStelPropertyValue("GridLinesMgr.zenithNadirDisplayed").toBool());
	conf->setValue("viewing/flag_ecliptic_J2000_poles",	propMgr->getStelPropertyValue("GridLinesMgr.eclipticJ2000PolesDisplayed").toBool());
	conf->setValue("viewing/flag_ecliptic_poles",		propMgr->getStelPropertyValue("GridLinesMgr.eclipticPolesDisplayed").toBool());
	conf->setValue("viewing/flag_galactic_poles",		propMgr->getStelPropertyValue("GridLinesMgr.galacticPolesDisplayed").toBool());
	conf->setValue("viewing/flag_supergalactic_poles",	propMgr->getStelPropertyValue("GridLinesMgr.supergalacticPolesDisplayed").toBool());
	conf->setValue("viewing/flag_equinox_J2000_points",	propMgr->getStelPropertyValue("GridLinesMgr.equinoxJ2000PointsDisplayed").toBool());
	conf->setValue("viewing/flag_equinox_points",		propMgr->getStelPropertyValue("GridLinesMgr.equinoxPointsDisplayed").toBool());
	conf->setValue("viewing/flag_solstice_J2000_points",	propMgr->getStelPropertyValue("GridLinesMgr.solsticeJ2000PointsDisplayed").toBool());
	conf->setValue("viewing/flag_solstice_points",		propMgr->getStelPropertyValue("GridLinesMgr.solsticePointsDisplayed").toBool());

	conf->setValue("viewing/flag_constellation_drawing",	propMgr->getStelPropertyValue("ConstellationMgr.linesDisplayed").toBool());
	conf->setValue("viewing/flag_constellation_name",	propMgr->getStelPropertyValue("ConstellationMgr.namesDisplayed").toBool());
	conf->setValue("viewing/flag_constellation_boundaries",	propMgr->getStelPropertyValue("ConstellationMgr.boundariesDisplayed").toBool());
	conf->setValue("viewing/flag_constellation_art",	propMgr->getStelPropertyValue("ConstellationMgr.artDisplayed").toBool());
	conf->setValue("viewing/flag_constellation_isolate_selected", propMgr->getStelPropertyValue("ConstellationMgr.isolateSelected").toBool());
	conf->setValue("viewing/flag_landscape_autoselection",	propMgr->getStelPropertyValue("LandscapeMgr.flagLandscapeAutoSelection").toBool());
	conf->setValue("viewing/flag_light_pollution_database",	propMgr->getStelPropertyValue("LandscapeMgr.flagUseLightPollutionFromDatabase").toBool());
	conf->setValue("viewing/flag_atmosphere_auto_enable",	propMgr->getStelPropertyValue("LandscapeMgr.flagAtmosphereAutoEnabling").toBool());
	conf->setValue("viewing/constellation_art_intensity",	propMgr->getStelPropertyValue("ConstellationMgr.artIntensity").toFloat());
	conf->setValue("viewing/constellation_name_style",	ConstellationMgr::getConstellationDisplayStyleString(static_cast<ConstellationMgr::ConstellationDisplayStyle> (propMgr->getStelPropertyValue("ConstellationMgr.constellationDisplayStyle").toInt())  ));
	conf->setValue("viewing/constellation_line_thickness",	propMgr->getStelPropertyValue("ConstellationMgr.constellationLineThickness").toFloat());

	conf->setValue("viewing/flag_asterism_drawing",		propMgr->getStelPropertyValue("AsterismMgr.linesDisplayed").toBool());
	conf->setValue("viewing/flag_asterism_name",		propMgr->getStelPropertyValue("AsterismMgr.namesDisplayed").toBool());
	conf->setValue("viewing/asterism_line_thickness",	propMgr->getStelPropertyValue("AsterismMgr.asterismLineThickness").toFloat());

	conf->setValue("viewing/flag_night",			StelApp::getInstance().getVisionModeNight());
	conf->setValue("astro/flag_stars",			propMgr->getStelPropertyValue("StarMgr.flagStarsDisplayed").toBool());
	conf->setValue("astro/flag_star_name",			propMgr->getStelPropertyValue("StarMgr.flagLabelsDisplayed").toBool());
	conf->setValue("stars/labels_amount",			propMgr->getStelPropertyValue("StarMgr.labelsAmount").toDouble());
	conf->setValue("astro/nebula_hints_amount",		propMgr->getStelPropertyValue("NebulaMgr.hintsAmount").toDouble());
	conf->setValue("astro/nebula_labels_amount",		propMgr->getStelPropertyValue("NebulaMgr.labelsAmount").toDouble());
	conf->setValue("astro/flag_nebula_hints_proportional",	propMgr->getStelPropertyValue("NebulaMgr.hintsProportional").toBool());
	conf->setValue("astro/flag_surface_brightness_usage",	propMgr->getStelPropertyValue("NebulaMgr.flagSurfaceBrightnessUsage").toBool());
	conf->setValue("astro/flag_surface_brightness_arcsec",	propMgr->getStelPropertyValue("NebulaMgr.flagSurfaceBrightnessArcsecUsage").toBool());
	conf->setValue("gui/flag_dso_designation_usage",	propMgr->getStelPropertyValue("NebulaMgr.flagDesignationLabels").toBool());
	conf->setValue("astro/flag_nebula_name",		propMgr->getStelPropertyValue("NebulaMgr.flagHintDisplayed").toBool());
	conf->setValue("astro/flag_use_type_filter",		propMgr->getStelPropertyValue("NebulaMgr.flagTypeFiltersUsage").toBool());
	conf->setValue("astro/flag_nebula_display_no_texture",	!propMgr->getStelPropertyValue("StelSkyLayerMgr.flagShow").toBool() );

	conf->setValue("projection/type",			core->getCurrentProjectionTypeKey());
	conf->setValue("astro/flag_nutation",			core->getUseNutation());
	conf->setValue("astro/flag_topocentric_coordinates",	core->getUseTopocentricCoordinates());

	// view dialog / DSO tag settings
	const Nebula::CatalogGroup& cflags = nmgr->getCatalogFilters();

	conf->beginGroup("dso_catalog_filters");
	conf->setValue("flag_show_ngc",	(bool) (cflags & Nebula::CatNGC));
	conf->setValue("flag_show_ic",	(bool) (cflags & Nebula::CatIC));
	conf->setValue("flag_show_m",	(bool) (cflags & Nebula::CatM));
	conf->setValue("flag_show_c",	(bool) (cflags & Nebula::CatC));
	conf->setValue("flag_show_b",	(bool) (cflags & Nebula::CatB));
	conf->setValue("flag_show_vdb",	(bool) (cflags & Nebula::CatVdB));
	conf->setValue("flag_show_sh2",	(bool) (cflags & Nebula::CatSh2));
	conf->setValue("flag_show_rcw",	(bool) (cflags & Nebula::CatRCW));
	conf->setValue("flag_show_lbn",	(bool) (cflags & Nebula::CatLBN));
	conf->setValue("flag_show_ldn",	(bool) (cflags & Nebula::CatLDN));
	conf->setValue("flag_show_cr",	(bool) (cflags & Nebula::CatCr));
	conf->setValue("flag_show_mel",	(bool) (cflags & Nebula::CatMel));
	conf->setValue("flag_show_ced",	(bool) (cflags & Nebula::CatCed));
	conf->setValue("flag_show_pgc",	(bool) (cflags & Nebula::CatPGC));
	conf->setValue("flag_show_ugc",	(bool) (cflags & Nebula::CatUGC));
	conf->setValue("flag_show_arp",	(bool) (cflags & Nebula::CatArp));
	conf->setValue("flag_show_vv",	(bool) (cflags & Nebula::CatVV));
	conf->setValue("flag_show_pk",	(bool) (cflags & Nebula::CatPK));
	conf->endGroup();

	const Nebula::TypeGroup& tflags = nmgr->getTypeFilters();
	conf->beginGroup("dso_type_filters");
	conf->setValue("flag_show_galaxies", 		 (bool) (tflags & Nebula::TypeGalaxies));
	conf->setValue("flag_show_active_galaxies",	 (bool) (tflags & Nebula::TypeActiveGalaxies));
	conf->setValue("flag_show_interacting_galaxies", (bool) (tflags & Nebula::TypeInteractingGalaxies));
	conf->setValue("flag_show_clusters",		 (bool) (tflags & Nebula::TypeStarClusters));
	conf->setValue("flag_show_bright_nebulae",	 (bool) (tflags & Nebula::TypeBrightNebulae));
	conf->setValue("flag_show_dark_nebulae",	 (bool) (tflags & Nebula::TypeDarkNebulae));
	conf->setValue("flag_show_planetary_nebulae",	 (bool) (tflags & Nebula::TypePlanetaryNebulae));
	conf->setValue("flag_show_hydrogen_regions",	 (bool) (tflags & Nebula::TypeHydrogenRegions));
	conf->setValue("flag_show_supernova_remnants",	 (bool) (tflags & Nebula::TypeSupernovaRemnants));
	conf->setValue("flag_show_other",		 (bool) (tflags & Nebula::TypeOther));
	conf->endGroup();

	// view dialog / landscape tab settings
	conf->setValue("init_location/landscape_name",                     propMgr->getStelPropertyValue("LandscapeMgr.currentLandscapeID").toString());
	conf->setValue("landscape/flag_landscape_sets_location",           propMgr->getStelPropertyValue("LandscapeMgr.flagLandscapeSetsLocation").toBool());
	conf->setValue("landscape/flag_landscape",                         propMgr->getStelPropertyValue("LandscapeMgr.landscapeDisplayed").toBool());
	conf->setValue("landscape/flag_atmosphere",                        propMgr->getStelPropertyValue("LandscapeMgr.atmosphereDisplayed").toBool());
	conf->setValue("landscape/flag_fog",                               propMgr->getStelPropertyValue("LandscapeMgr.fogDisplayed").toBool());
	conf->setValue("landscape/flag_enable_illumination_layer",         propMgr->getStelPropertyValue("LandscapeMgr.illuminationDisplayed").toBool());
	conf->setValue("landscape/flag_enable_labels",                     propMgr->getStelPropertyValue("LandscapeMgr.labelsDisplayed").toBool());
	conf->setValue("landscape/flag_minimal_brightness",                propMgr->getStelPropertyValue("LandscapeMgr.flagLandscapeUseMinimalBrightness").toBool());
	conf->setValue("landscape/flag_landscape_sets_minimal_brightness", propMgr->getStelPropertyValue("LandscapeMgr.flagLandscapeSetsMinimalBrightness").toBool());
	conf->setValue("landscape/minimal_brightness",                     propMgr->getStelPropertyValue("LandscapeMgr.defaultMinimalBrightness").toFloat());
	conf->setValue("stars/init_bortle_scale",                          propMgr->getStelPropertyValue("StelSkyDrawer.bortleScaleIndex").toInt());
	conf->setValue("landscape/atmospheric_extinction_coefficient",     propMgr->getStelPropertyValue("StelSkyDrawer.extinctionCoefficient").toFloat());
	conf->setValue("landscape/pressure_mbar",                          propMgr->getStelPropertyValue("StelSkyDrawer.atmospherePressure").toFloat());
	conf->setValue("landscape/temperature_C",                          propMgr->getStelPropertyValue("StelSkyDrawer.atmosphereTemperature").toFloat());

	// view dialog / starlore tab
	StelApp::getInstance().getSkyCultureMgr().setDefaultSkyCultureID(StelApp::getInstance().getSkyCultureMgr().getCurrentSkyCultureID());

	// Save default location
	StelApp::getInstance().getCore()->setDefaultLocationID(core->getCurrentLocation().getID());

	// configuration dialog / main tab
	QString langName = StelApp::getInstance().getLocaleMgr().getAppLanguage();
	conf->setValue("localization/app_locale", StelTranslator::nativeNameToIso639_1Code(langName));
	langName = StelApp::getInstance().getLocaleMgr().getSkyLanguage();
	conf->setValue("localization/sky_locale", StelTranslator::nativeNameToIso639_1Code(langName));

	// configuration dialog / selected object info tab
	const StelObject::InfoStringGroup& flags = gui->getInfoTextFilters();
	if (flags == StelObject::InfoStringGroup(0))
		conf->setValue("gui/selected_object_info", "none");
	else if (flags == StelObject::InfoStringGroup(StelObject::ShortInfo))
		conf->setValue("gui/selected_object_info", "short");
	else if (flags == StelObject::InfoStringGroup(StelObject::AllInfo))
		conf->setValue("gui/selected_object_info", "all");
	else
	{
		conf->setValue("gui/selected_object_info", "custom");
		
		conf->beginGroup("custom_selected_info");
		conf->setValue("flag_show_name", (bool) (flags & StelObject::Name));
		conf->setValue("flag_show_catalognumber",
		               (bool) (flags & StelObject::CatalogNumber));
		conf->setValue("flag_show_magnitude",
		               (bool) (flags & StelObject::Magnitude));
		conf->setValue("flag_show_absolutemagnitude",
			       (bool) (flags & StelObject::AbsoluteMagnitude));
		conf->setValue("flag_show_radecj2000",
		               (bool) (flags & StelObject::RaDecJ2000));
		conf->setValue("flag_show_radecofdate",
			       (bool) (flags & StelObject::RaDecOfDate));
		conf->setValue("flag_show_hourangle",
		               (bool) (flags & StelObject::HourAngle));
		conf->setValue("flag_show_altaz",
		               (bool) (flags &  StelObject::AltAzi));
		conf->setValue("flag_show_distance",
		               (bool) (flags & StelObject::Distance));
		conf->setValue("flag_show_size",
		               (bool) (flags & StelObject::Size));
		conf->setValue("flag_show_extra",
			       (bool) (flags & StelObject::Extra));
		conf->setValue("flag_show_galcoord",
			       (bool) (flags & StelObject::GalacticCoord));
		conf->setValue("flag_show_supergalcoord",
			       (bool) (flags & StelObject::SupergalacticCoord));
		conf->setValue("flag_show_type",
			       (bool) (flags & StelObject::ObjectType));
		conf->setValue("flag_show_eclcoordofdate",
			       (bool) (flags & StelObject::EclipticCoordOfDate));
		conf->setValue("flag_show_eclcoordj2000",
			       (bool) (flags & StelObject::EclipticCoordJ2000));
		conf->setValue("flag_show_constellation",
			       (bool) (flags & StelObject::IAUConstellation));
		conf->setValue("flag_show_sidereal_time",
			       (bool) (flags & StelObject::SiderealTime));
		conf->endGroup();
	}

	// toolbar auto-hide status
	conf->setValue("gui/auto_hide_horizontal_toolbar", gui->getAutoHideHorizontalButtonBar());
	conf->setValue("gui/auto_hide_vertical_toolbar", gui->getAutoHideVerticalButtonBar());
	conf->setValue("gui/flag_show_nebulae_background_button", gui->getFlagShowNebulaBackgroundButton());
	conf->setValue("gui/flag_show_toast_survey_button", gui->getFlagShowToastSurveyButton());
	conf->setValue("gui/flag_show_bookmarks_button", gui->getFlagShowBookmarksButton());
	conf->setValue("gui/flag_show_icrs_grid_button", gui->getFlagShowICRSGridButton());
	conf->setValue("gui/flag_show_galactic_grid_button", gui->getFlagShowGalacticGridButton());
	conf->setValue("gui/flag_show_ecliptic_grid_button", gui->getFlagShowEclipticGridButton());
	conf->setValue("gui/flag_show_boundaries_button", gui->getFlagShowConstellationBoundariesButton());
	conf->setValue("gui/flag_show_decimal_degrees", StelApp::getInstance().getFlagShowDecimalDegrees());
	conf->setValue("gui/flag_use_azimuth_from_south", StelApp::getInstance().getFlagSouthAzimuthUsage());
	conf->setValue("gui/flag_time_jd", gui->getButtonBar()->getFlagTimeJd());
	conf->setValue("gui/flag_show_buttons_background", StelMainView::getInstance().getFlagUseButtonsBackground());
	conf->setValue("gui/flag_indication_mount_mode", mvmgr->getFlagIndicationMountMode());

	// configuration dialog / navigation tab
	conf->setValue("navigation/flag_enable_zoom_keys", mvmgr->getFlagEnableZoomKeys());
	conf->setValue("navigation/flag_enable_mouse_navigation", mvmgr->getFlagEnableMouseNavigation());
	conf->setValue("navigation/flag_enable_move_keys", mvmgr->getFlagEnableMoveKeys());
	conf->setValue("navigation/startup_time_mode", core->getStartupTimeMode());
	conf->setValue("navigation/today_time", core->getInitTodayTime());
	conf->setValue("navigation/preset_sky_time", core->getPresetSkyTime());
	conf->setValue("navigation/time_correction_algorithm", core->getCurrentDeltaTAlgorithmKey());
	conf->setValue("navigation/init_fov", mvmgr->getInitFov());
	if (mvmgr->getMountMode() == StelMovementMgr::MountAltAzimuthal)
		conf->setValue("navigation/viewing_mode", "horizon");
	else
		conf->setValue("navigation/viewing_mode", "equator");

	StelLocaleMgr & localeManager = StelApp::getInstance().getLocaleMgr();
	conf->setValue("localization/time_display_format", localeManager.getTimeFormatStr());
	conf->setValue("localization/date_display_format", localeManager.getDateFormatStr());

	// configuration dialog / tools tab
	conf->setValue("gui/flag_show_flip_buttons", gui->getFlagShowFlipButtons());
	conf->setValue("video/viewport_effect", StelApp::getInstance().getViewportEffect());
	conf->setValue("projection/viewport", StelProjector::maskTypeToString(proj->getMaskType()));
	conf->setValue("projection/viewport_center_offset_x", core->getCurrentStelProjectorParams().viewportCenterOffset[0]);
	conf->setValue("projection/viewport_center_offset_y", core->getCurrentStelProjectorParams().viewportCenterOffset[1]);
	conf->setValue("projection/flip_horz", core->getCurrentStelProjectorParams().flipHorz);
	conf->setValue("projection/flip_vert", core->getCurrentStelProjectorParams().flipVert);
	conf->setValue("viewing/flag_gravity_labels", proj->getFlagGravityLabels());
	conf->setValue("navigation/auto_zoom_out_resets_direction", mvmgr->getFlagAutoZoomOutResetsDirection());
	conf->setValue("gui/flag_mouse_cursor_timeout", StelMainView::getInstance().getFlagCursorTimeout());
	conf->setValue("gui/mouse_cursor_timeout", StelMainView::getInstance().getCursorTimeout());

	conf->setValue("main/screenshot_dir", StelFileMgr::getScreenshotDir());
	conf->setValue("main/invert_screenshots_colors", StelMainView::getInstance().getFlagInvertScreenShotColors());

	int screenNum = qApp->desktop()->screenNumber(&StelMainView::getInstance());
	conf->setValue("video/screen_number", screenNum);

	// full screen and window size
	conf->setValue("video/fullscreen", StelMainView::getInstance().isFullScreen());
	if (!StelMainView::getInstance().isFullScreen())
	{
		QRect screenGeom = QApplication::desktop()->screenGeometry(screenNum);
		QWidget& mainWindow = StelMainView::getInstance();
		conf->setValue("video/screen_w", mainWindow.size().width());
		conf->setValue("video/screen_h", mainWindow.size().height());
		conf->setValue("video/screen_x", mainWindow.x() - screenGeom.x());
		conf->setValue("video/screen_y", mainWindow.y() - screenGeom.y());
	}

	// clear the restore defaults flag if it is set.
	conf->setValue("main/restore_defaults", false);

	updateConfigLabels();
}

void ConfigurationDialog::updateConfigLabels()
{
	ui->startupFOVLabel->setText(q_("Startup FOV: %1%2").arg(StelApp::getInstance().getCore()->getMovementMgr()->getCurrentFov()).arg(QChar(0x00B0)));

	double az, alt;
	const Vec3d& v = GETSTELMODULE(StelMovementMgr)->getInitViewingDirection();
	StelUtils::rectToSphe(&az, &alt, v);
	az = 3.*M_PI - az;  // N is zero, E is 90 degrees
	if (az > M_PI*2)
		az -= M_PI*2;
	ui->startupDirectionOfViewlabel->setText(q_("Startup direction of view Az/Alt: %1/%2").arg(StelUtils::radToDmsStr(az), StelUtils::radToDmsStr(alt)));
}

void ConfigurationDialog::setDefaultViewOptions()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);

	conf->setValue("main/restore_defaults", true);
	// reset all stored panel locations
	conf->beginGroup("DialogPositions");
	conf->remove("");
	conf->endGroup();

}

void ConfigurationDialog::populatePluginsList()
{
	QListWidget *plugins = ui->pluginsListWidget;
	plugins->blockSignals(true);
	int currentRow = plugins->currentRow();
	QString selectedPluginId = "";
	if (currentRow>0)
		 selectedPluginId = plugins->currentItem()->data(Qt::UserRole).toString();

	plugins->clear();
	QString selectedPluginName = "";
	const QList<StelModuleMgr::PluginDescriptor> pluginsList = StelApp::getInstance().getModuleMgr().getPluginsList();	
	foreach (const StelModuleMgr::PluginDescriptor& desc, pluginsList)
	{
		QString label = q_(desc.info.displayedName);
		QListWidgetItem* item = new QListWidgetItem(label);
		item->setData(Qt::UserRole, desc.info.id);
		plugins->addItem(item);
		if (currentRow>0 && item->data(Qt::UserRole).toString()==selectedPluginId)
			selectedPluginName = label;
	}
	plugins->sortItems(Qt::AscendingOrder);
	plugins->blockSignals(false);
	// If we had a valid previous selection (i.e. not first time we populate), restore it

	if (!selectedPluginName.isEmpty())
		plugins->setCurrentItem(plugins->findItems(selectedPluginName, Qt::MatchExactly).at(0));
	else
		plugins->setCurrentRow(0);


}

void ConfigurationDialog::pluginsSelectionChanged(QListWidgetItem* item, QListWidgetItem* previousItem)
{
	Q_UNUSED(previousItem);
	const QList<StelModuleMgr::PluginDescriptor> pluginsList = StelApp::getInstance().getModuleMgr().getPluginsList();
	foreach (const StelModuleMgr::PluginDescriptor& desc, pluginsList)
	{
		if (item->data(Qt::UserRole).toString()==desc.info.id)
		{
			QString html = "<html><head></head><body>";
			html += "<h2>" + q_(desc.info.displayedName) + "</h2>";			
			QString d = desc.info.description;
			d.replace("\n", "<br />");
			html += "<p>" + q_(d) + "</p>";
			html += "<p><strong>" + q_("Authors") + "</strong>: " + desc.info.authors;
			html += "<br /><strong>" + q_("Contact") + "</strong>: " + desc.info.contact;
			if (!desc.info.version.isEmpty())
				html += "<br /><strong>" + q_("Version") + "</strong>: " + desc.info.version;
			html += "</p></body></html>";
			ui->pluginsInfoBrowser->document()->setDefaultStyleSheet(QString(gui->getStelStyle().htmlStyleSheet));
			ui->pluginsInfoBrowser->setHtml(html);
			ui->pluginLoadAtStartupCheckBox->setChecked(desc.loadAtStartup);
			StelModule* pmod = StelApp::getInstance().getModuleMgr().getModule(desc.info.id, true);
			if (pmod != Q_NULLPTR)
				ui->pluginConfigureButton->setEnabled(pmod->configureGui(false));
			else
				ui->pluginConfigureButton->setEnabled(false);
			return;
		}
	}
}

void ConfigurationDialog::pluginConfigureCurrentSelection()
{
	QString id = ui->pluginsListWidget->currentItem()->data(Qt::UserRole).toString();
	if (id.isEmpty())
		return;

	StelModuleMgr& moduleMgr = StelApp::getInstance().getModuleMgr();
	const QList<StelModuleMgr::PluginDescriptor> pluginsList = moduleMgr.getPluginsList();
	foreach (const StelModuleMgr::PluginDescriptor& desc, pluginsList)
	{
		if (id == desc.info.id)
		{
			StelModule* pmod = moduleMgr.getModule(desc.info.id, QObject::sender()->objectName()=="pluginsListWidget");
			if (pmod != Q_NULLPTR)
			{
				pmod->configureGui(true);
			}
			return;
		}
	}
}

void ConfigurationDialog::loadAtStartupChanged(int state)
{
	if (ui->pluginsListWidget->count() <= 0)
		return;

	QString id = ui->pluginsListWidget->currentItem()->data(Qt::UserRole).toString();
	StelModuleMgr& moduleMgr = StelApp::getInstance().getModuleMgr();
	const QList<StelModuleMgr::PluginDescriptor> pluginsList = moduleMgr.getPluginsList();
	foreach (const StelModuleMgr::PluginDescriptor& desc, pluginsList)
	{
		if (id == desc.info.id)
		{
			moduleMgr.setPluginLoadAtStartup(id, state == Qt::Checked);
			break;
		}
	}
}

#ifndef DISABLE_SCRIPTING
void ConfigurationDialog::populateScriptsList(void)
{
	int prevSel = ui->scriptListWidget->currentRow();	
	StelScriptMgr& scriptMgr = StelApp::getInstance().getScriptMgr();	
	ui->scriptListWidget->clear();	
	ui->scriptListWidget->addItems(scriptMgr.getScriptList());	
	// If we had a valid previous selection (i.e. not first time we populate), restore it
	if (prevSel >= 0 && prevSel < ui->scriptListWidget->count())
		ui->scriptListWidget->setCurrentRow(prevSel);
	else
		ui->scriptListWidget->setCurrentRow(0);
}

void ConfigurationDialog::scriptSelectionChanged(const QString& s)
{
	if (s.isEmpty())
		return;	
	StelScriptMgr& scriptMgr = StelApp::getInstance().getScriptMgr();	
	//ui->scriptInfoBrowser->document()->setDefaultStyleSheet(QString(StelApp::getInstance().getCurrentStelStyle()->htmlStyleSheet));
	QString html = scriptMgr.getHtmlDescription(s);
	ui->scriptInfoBrowser->setHtml(html);	
}

void ConfigurationDialog::runScriptClicked(void)
{
	if (ui->closeWindowAtScriptRunCheckbox->isChecked())
		this->close();	
	StelScriptMgr& scriptMgr = StelApp::getInstance().getScriptMgr();
	if (ui->scriptListWidget->currentItem())
	{
		scriptMgr.runScript(ui->scriptListWidget->currentItem()->text());
	}	
}

void ConfigurationDialog::stopScriptClicked(void)
{
	StelApp::getInstance().getScriptMgr().stopScript();
}

void ConfigurationDialog::aScriptIsRunning(void)
{	
	ui->scriptStatusLabel->setText(q_("Running script: ") + StelApp::getInstance().getScriptMgr().runningScriptId());
	ui->runScriptButton->setEnabled(false);
	ui->stopScriptButton->setEnabled(true);	
}

void ConfigurationDialog::aScriptHasStopped(void)
{
	ui->scriptStatusLabel->setText(q_("Running script: [none]"));
	ui->runScriptButton->setEnabled(true);
	ui->stopScriptButton->setEnabled(false);
}
#endif


void ConfigurationDialog::setFixedDateTimeToCurrent(void)
{
	StelCore* core = StelApp::getInstance().getCore();
	double JD = core->getJD();
	ui->fixedDateTimeEdit->setDateTime(StelUtils::jdToQDateTime(JD+core->getUTCOffset(JD)/24));
	ui->fixedTimeRadio->setChecked(true);
	setStartupTimeMode();
}


void ConfigurationDialog::resetStarCatalogControls()
{
	const QVariantList& catalogConfig = GETSTELMODULE(StarMgr)->getCatalogsDescription();
	nextStarCatalogToDownload.clear();
	int idx=0;
	foreach (const QVariant& catV, catalogConfig)
	{
		++idx;
		const QVariantMap& m = catV.toMap();
		const bool checked = m.value("checked").toBool();
		if (checked)
			continue;
		nextStarCatalogToDownload=m;
		break;
	}

	ui->downloadCancelButton->setVisible(false);
	ui->downloadRetryButton->setVisible(false);

	if (idx > catalogConfig.size() && !hasDownloadedStarCatalog)
	{
		ui->getStarsButton->setVisible(false);
		updateStarCatalogControlsText();
		return;
	}

	ui->getStarsButton->setEnabled(true);
	if (!nextStarCatalogToDownload.isEmpty())
	{
		nextStarCatalogToDownloadIndex = idx;
		starCatalogsCount = catalogConfig.size();
		updateStarCatalogControlsText();
		ui->getStarsButton->setVisible(true);
	}
	else
	{
		updateStarCatalogControlsText();
		ui->getStarsButton->setVisible(false);
	}
}

void ConfigurationDialog::updateStarCatalogControlsText()
{
	if (nextStarCatalogToDownload.isEmpty())
	{
		//There are no more catalogs left?
		if (hasDownloadedStarCatalog)
		{
			ui->downloadLabel->setText(q_("Finished downloading new star catalogs!\nRestart Stellarium to display them."));
		}
		else
		{
			ui->downloadLabel->setText(q_("All available star catalogs have been installed."));
		}
	}
	else
	{
		QString text = QString(q_("Get catalog %1 of %2"))
		               .arg(nextStarCatalogToDownloadIndex)
		               .arg(starCatalogsCount);
		ui->getStarsButton->setText(text);
		
		if (isDownloadingStarCatalog)
		{
			QString text = QString(q_("Downloading %1...\n(You can close this window.)"))
			                 .arg(nextStarCatalogToDownload.value("id").toString());
			ui->downloadLabel->setText(text);
		}
		else
		{
			const QVariantList& magRange = nextStarCatalogToDownload.value("magRange").toList();
			ui->downloadLabel->setText(q_("Download size: %1MB\nStar count: %2 Million\nMagnitude range: %3 - %4")
				.arg(nextStarCatalogToDownload.value("sizeMb").toString())
				.arg(nextStarCatalogToDownload.value("count").toString())
				.arg(magRange.first().toString())
				.arg(magRange.last().toString()));
		}
	}
}

void ConfigurationDialog::cancelDownload(void)
{
	Q_ASSERT(currentDownloadFile);
	Q_ASSERT(starCatalogDownloadReply);
	qWarning() << "Aborting download";
	starCatalogDownloadReply->abort();
}

void ConfigurationDialog::newStarCatalogData()
{
	Q_ASSERT(currentDownloadFile);
	Q_ASSERT(starCatalogDownloadReply);
	Q_ASSERT(progressBar);

	int size = starCatalogDownloadReply->bytesAvailable();
	progressBar->setValue((float)progressBar->getValue()+(float)size/1024);
	currentDownloadFile->write(starCatalogDownloadReply->read(size));
}

void ConfigurationDialog::downloadStars()
{
	Q_ASSERT(!nextStarCatalogToDownload.isEmpty());
	Q_ASSERT(!isDownloadingStarCatalog);
	Q_ASSERT(starCatalogDownloadReply==Q_NULLPTR);
	Q_ASSERT(currentDownloadFile==Q_NULLPTR);
	Q_ASSERT(progressBar==Q_NULLPTR);

	QString path = StelFileMgr::getUserDir()+QString("/stars/default/")+nextStarCatalogToDownload.value("fileName").toString();
	currentDownloadFile = new QFile(path);
	if (!currentDownloadFile->open(QIODevice::WriteOnly))
	{
		qWarning() << "Can't open a writable file for storing new star catalog: " << QDir::toNativeSeparators(path);
		currentDownloadFile->deleteLater();
		currentDownloadFile = Q_NULLPTR;
		ui->downloadLabel->setText(q_("Error downloading %1:\n%2").arg(nextStarCatalogToDownload.value("id").toString()).arg(QString("Can't open a writable file for storing new star catalog: %1").arg(path)));
		ui->downloadRetryButton->setVisible(true);
		return;
	}

	isDownloadingStarCatalog = true;
	updateStarCatalogControlsText();
	ui->downloadCancelButton->setVisible(true);
	ui->downloadRetryButton->setVisible(false);
	ui->getStarsButton->setVisible(true);
	ui->getStarsButton->setEnabled(false);

	QNetworkRequest req(nextStarCatalogToDownload.value("url").toString());
	req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	req.setAttribute(QNetworkRequest::RedirectionTargetAttribute, false);
	req.setRawHeader("User-Agent", StelUtils::getUserAgentString().toLatin1());
	starCatalogDownloadReply = StelApp::getInstance().getNetworkAccessManager()->get(req);
	starCatalogDownloadReply->setReadBufferSize(1024*1024*2);	
	connect(starCatalogDownloadReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
	connect(starCatalogDownloadReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));

	progressBar = StelApp::getInstance().addProgressBar();
	progressBar->setValue(0);
	progressBar->setRange(0, nextStarCatalogToDownload.value("sizeMb").toDouble()*1024);
	progressBar->setFormat(QString("%1: %p%").arg(nextStarCatalogToDownload.value("id").toString()));

	qDebug() << "Downloading file" << nextStarCatalogToDownload.value("url").toString();
}

void ConfigurationDialog::downloadError(QNetworkReply::NetworkError)
{
	Q_ASSERT(currentDownloadFile);
	Q_ASSERT(starCatalogDownloadReply);

	isDownloadingStarCatalog = false;
	qWarning() << "Error downloading file" << starCatalogDownloadReply->url() << ": " << starCatalogDownloadReply->errorString();
	ui->downloadLabel->setText(q_("Error downloading %1:\n%2").arg(nextStarCatalogToDownload.value("id").toString()).arg(starCatalogDownloadReply->errorString()));
	ui->downloadCancelButton->setVisible(false);
	ui->downloadRetryButton->setVisible(true);
	ui->getStarsButton->setVisible(false);
	ui->getStarsButton->setEnabled(true);
}

void ConfigurationDialog::downloadFinished()
{
	Q_ASSERT(currentDownloadFile);
	Q_ASSERT(starCatalogDownloadReply);
	Q_ASSERT(progressBar);

	if (starCatalogDownloadReply->error()!=QNetworkReply::NoError)
	{
		starCatalogDownloadReply->deleteLater();
		starCatalogDownloadReply = Q_NULLPTR;
		currentDownloadFile->close();
		currentDownloadFile->deleteLater();
		currentDownloadFile = Q_NULLPTR;
		StelApp::getInstance().removeProgressBar(progressBar);
		progressBar=Q_NULLPTR;
		return;
	}

	const QVariant& redirect = starCatalogDownloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (!redirect.isNull())
	{
		// We got a redirection, we need to follow
		starCatalogDownloadReply->deleteLater();
		QNetworkRequest req(redirect.toUrl());
		req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
		req.setAttribute(QNetworkRequest::RedirectionTargetAttribute, false);
		req.setRawHeader("User-Agent", StelUtils::getUserAgentString().toLatin1());
		starCatalogDownloadReply = StelApp::getInstance().getNetworkAccessManager()->get(req);
		starCatalogDownloadReply->setReadBufferSize(1024*1024*2);
		connect(starCatalogDownloadReply, SIGNAL(readyRead()), this, SLOT(newStarCatalogData()));
		connect(starCatalogDownloadReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
		connect(starCatalogDownloadReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
		return;
	}

	Q_ASSERT(starCatalogDownloadReply->bytesAvailable()==0);

	isDownloadingStarCatalog = false;
	currentDownloadFile->close();
	currentDownloadFile->deleteLater();
	currentDownloadFile = Q_NULLPTR;
	starCatalogDownloadReply->deleteLater();
	starCatalogDownloadReply = Q_NULLPTR;
	StelApp::getInstance().removeProgressBar(progressBar);
	progressBar=Q_NULLPTR;

	ui->downloadLabel->setText(q_("Verifying file integrity..."));
	if (GETSTELMODULE(StarMgr)->checkAndLoadCatalog(nextStarCatalogToDownload)==false)
	{
		ui->getStarsButton->setVisible(false);
		ui->downloadLabel->setText(q_("Error downloading %1:\nFile is corrupted.").arg(nextStarCatalogToDownload.value("id").toString()));
		ui->downloadCancelButton->setVisible(false);
		ui->downloadRetryButton->setVisible(true);
	}
	else
	{
		hasDownloadedStarCatalog = true;
	}

	resetStarCatalogControls();
}

void ConfigurationDialog::de430ButtonClicked()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);

	StelApp::getInstance().getCore()->setDe430Active(!StelApp::getInstance().getCore()->de430IsActive());
	conf->setValue("astro/flag_use_de430", StelApp::getInstance().getCore()->de430IsActive());

	resetEphemControls(); //refresh labels
}

void ConfigurationDialog::de431ButtonClicked()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);

	StelApp::getInstance().getCore()->setDe431Active(!StelApp::getInstance().getCore()->de431IsActive());
	conf->setValue("astro/flag_use_de431", StelApp::getInstance().getCore()->de431IsActive());

	resetEphemControls(); //refresh labels
}

void ConfigurationDialog::resetEphemControls()
{
	ui->de430checkBox->setEnabled(StelApp::getInstance().getCore()->de430IsAvailable());
	ui->de431checkBox->setEnabled(StelApp::getInstance().getCore()->de431IsAvailable());
	ui->de430checkBox->setChecked(StelApp::getInstance().getCore()->de430IsActive());
	ui->de431checkBox->setChecked(StelApp::getInstance().getCore()->de431IsActive());

	if(StelApp::getInstance().getCore()->de430IsActive())
		ui->de430label->setText("1550..2650");
	else
	{
		if (StelApp::getInstance().getCore()->de430IsAvailable())
			ui->de430label->setText(q_("Available"));
		else
			ui->de430label->setText(q_("Not Available"));
	}
	if(StelApp::getInstance().getCore()->de431IsActive())
		ui->de431label->setText("-13000..17000");
	else
	{
		if (StelApp::getInstance().getCore()->de431IsAvailable())
			ui->de431label->setText(q_("Available"));
		else
			ui->de431label->setText(q_("Not Available"));
	}
}

void ConfigurationDialog::updateSelectedInfoCheckBoxes()
{
	const StelObject::InfoStringGroup& flags = gui->getInfoTextFilters();
	
	ui->checkBoxName->setChecked(flags & StelObject::Name);
	ui->checkBoxCatalogNumbers->setChecked(flags & StelObject::CatalogNumber);
	ui->checkBoxVisualMag->setChecked(flags & StelObject::Magnitude);
	ui->checkBoxAbsoluteMag->setChecked(flags & StelObject::AbsoluteMagnitude);
	ui->checkBoxRaDecJ2000->setChecked(flags & StelObject::RaDecJ2000);
	ui->checkBoxRaDecOfDate->setChecked(flags & StelObject::RaDecOfDate);
	ui->checkBoxHourAngle->setChecked(flags & StelObject::HourAngle);
	ui->checkBoxAltAz->setChecked(flags & StelObject::AltAzi);
	ui->checkBoxDistance->setChecked(flags & StelObject::Distance);
	ui->checkBoxSize->setChecked(flags & StelObject::Size);
	ui->checkBoxExtra->setChecked(flags & StelObject::Extra);
	ui->checkBoxGalacticCoordinates->setChecked(flags & StelObject::GalacticCoord);
	ui->checkBoxSupergalacticCoordinates->setChecked(flags & StelObject::SupergalacticCoord);
	ui->checkBoxType->setChecked(flags & StelObject::ObjectType);
	ui->checkBoxEclipticCoordsJ2000->setChecked(flags & StelObject::EclipticCoordJ2000);
	ui->checkBoxEclipticCoordsOfDate->setChecked(flags & StelObject::EclipticCoordOfDate);
	ui->checkBoxConstellation->setChecked(flags & StelObject::IAUConstellation);
	ui->checkBoxSiderealTime->setChecked(flags & StelObject::SiderealTime);
}

void ConfigurationDialog::updateTabBarListWidgetWidth()
{
	ui->stackListWidget->setWrapping(false);

	// Update list item sizes after translation
	ui->stackListWidget->adjustSize();

	QAbstractItemModel* model = ui->stackListWidget->model();
	if (!model)
	{
		return;
	}

	// stackListWidget->font() does not work properly!
	// It has a incorrect fontSize in the first loading, which produces the bug#995107.
	QFont font;
	font.setPixelSize(14);
	font.setWeight(75);
	QFontMetrics fontMetrics(font);

	int iconSize = ui->stackListWidget->iconSize().width();

	int width = 0;
	for (int row = 0; row < model->rowCount(); row++)
	{
		int textWidth = fontMetrics.width(ui->stackListWidget->item(row)->text());
		width += iconSize > textWidth ? iconSize : textWidth; // use the wider one
		width += 24; // margin - 12px left and 12px right
	}

	// Hack to force the window to be resized...
	ui->stackListWidget->setMinimumWidth(width);
}

void ConfigurationDialog::populateDeltaTAlgorithmsList()
{
	Q_ASSERT(ui->deltaTAlgorithmComboBox);

	// TRANSLATORS: Full phrase is "Algorithm of DeltaT"
	ui->deltaTLabel->setText(QString("%1 %2T:").arg(q_("Algorithm of")).arg(QChar(0x0394)));

	ui->pushButtonCustomDeltaTEquationDialog->setFixedHeight(ui->deltaTAlgorithmComboBox->height());

	QComboBox* algorithms = ui->deltaTAlgorithmComboBox;

	//Save the current selection to be restored later
	algorithms->blockSignals(true);
	int index = algorithms->currentIndex();
	QVariant selectedAlgorithmId = algorithms->itemData(index);
	algorithms->clear();
	//For each algorithm, display the localized name and store the key as user
	//data. Unfortunately, there's no other way to do this than with a cycle.
	algorithms->addItem(q_("Without correction"), "WithoutCorrection");
	algorithms->addItem(q_("Schoch (1931)"), "Schoch");
	algorithms->addItem(q_("Clemence (1948)"), "Clemence");
	algorithms->addItem(q_("IAU (1952)"), "IAU");
	algorithms->addItem(q_("Astronomical Ephemeris (1960)"), "AstronomicalEphemeris");
	algorithms->addItem(q_("Tuckerman (1962, 1964) & Goldstine (1973)"), "TuckermanGoldstine");
	algorithms->addItem(q_("Muller & Stephenson (1975)"), "MullerStephenson");
	algorithms->addItem(q_("Stephenson (1978)"), "Stephenson1978");
	algorithms->addItem(q_("Schmadel & Zech (1979)"), "SchmadelZech1979");
	algorithms->addItem(q_("Schmadel & Zech (1988)"), "SchmadelZech1988");
	algorithms->addItem(q_("Morrison & Stephenson (1982)"), "MorrisonStephenson1982");
	algorithms->addItem(q_("Stephenson & Morrison (1984)"), "StephensonMorrison1984");
	algorithms->addItem(q_("Stephenson & Houlden (1986)"), "StephensonHoulden");
	algorithms->addItem(q_("Espenak (1987, 1989)"), "Espenak");
	algorithms->addItem(q_("Borkowski (1988)"), "Borkowski");
	algorithms->addItem(q_("Chapront-Touze & Chapront (1991)"), "ChaprontTouze");	
	algorithms->addItem(q_("Stephenson & Morrison (1995)"), "StephensonMorrison1995");
	algorithms->addItem(q_("Stephenson (1997)"), "Stephenson1997");
	// The dropdown label is too long for the string, and Meeus 1998 is very popular, this should be in the beginning of the tag.
	algorithms->addItem(q_("Meeus (1998) (with Chapront, Chapront-Touze & Francou (1997))"), "ChaprontMeeus");
	algorithms->addItem(q_("JPL Horizons"), "JPLHorizons");	
	algorithms->addItem(q_("Meeus & Simons (2000)"), "MeeusSimons");
	algorithms->addItem(q_("Morrison & Stephenson (2004, 2005)"), "MorrisonStephenson2004");
	algorithms->addItem(q_("Stephenson, Morrison & Hohenkerk (2016)"), "StephensonMorrisonHohenkerk2016");
	// Espenak & Meeus (2006) used by default
	algorithms->addItem(q_("Espenak & Meeus (2006)").append(" *"), "EspenakMeeus");
	// GZ: I want to try out some things. Something is still wrong with eclipses, see lp:1275092.
	#ifndef NDEBUG
	algorithms->addItem(q_("Espenak & Meeus (2006) no extra moon acceleration"), "EspenakMeeusZeroMoonAccel");
	#endif
	algorithms->addItem(q_("Reijs (2006)"), "Reijs");
	algorithms->addItem(q_("Banjevic (2006)"), "Banjevic");
	algorithms->addItem(q_("Montenbruck & Pfleger (2000)"), "MontenbruckPfleger");
	algorithms->addItem(q_("Reingold & Dershowitz (2002, 2007)"), "ReingoldDershowitz");
	algorithms->addItem(q_("Islam, Sadiq & Qureshi (2008, 2013)"), "IslamSadiqQureshi");
	algorithms->addItem(q_("Khalid, Sultana & Zaidi (2014)"), "KhalidSultanaZaidi");
	algorithms->addItem(q_("Custom equation of %1T").arg(QChar(0x0394)), "Custom");

	//Restore the selection
	index = algorithms->findData(selectedAlgorithmId, Qt::UserRole, Qt::MatchCaseSensitive);
	algorithms->setCurrentIndex(index);
	//algorithms->model()->sort(0);
	algorithms->blockSignals(false);
	setDeltaTAlgorithmDescription();
}

void ConfigurationDialog::setDeltaTAlgorithm(int algorithmID)
{
	StelCore* core = StelApp::getInstance().getCore();
	QString currentAlgorithm = ui->deltaTAlgorithmComboBox->itemData(algorithmID).toString();
	core->setCurrentDeltaTAlgorithmKey(currentAlgorithm);
	setDeltaTAlgorithmDescription();
	if (currentAlgorithm.contains("Custom"))
		ui->pushButtonCustomDeltaTEquationDialog->setEnabled(true);
	else
		ui->pushButtonCustomDeltaTEquationDialog->setEnabled(false);
}

void ConfigurationDialog::setDeltaTAlgorithmDescription()
{
	ui->deltaTAlgorithmDescription->document()->setDefaultStyleSheet(QString(gui->getStelStyle().htmlStyleSheet));
	ui->deltaTAlgorithmDescription->setHtml(StelApp::getInstance().getCore()->getCurrentDeltaTAlgorithmDescription());
}

void ConfigurationDialog::showCustomDeltaTEquationDialog()
{
	if (customDeltaTEquationDialog == Q_NULLPTR)
		customDeltaTEquationDialog = new CustomDeltaTEquationDialog();

	customDeltaTEquationDialog->setVisible(true);
}

void ConfigurationDialog::populateDateFormatsList()
{
	Q_ASSERT(ui->dateFormatsComboBox);

	QComboBox* dfmts = ui->dateFormatsComboBox;

	//Save the current selection to be restored later
	dfmts->blockSignals(true);
	int index = dfmts->currentIndex();
	QVariant selectedDateFormat = dfmts->itemData(index);
	dfmts->clear();
	//For each format, display the localized name and store the key as user
	//data. Unfortunately, there's no other way to do this than with a cycle.
	dfmts->addItem(q_("System default"), "system_default");
	dfmts->addItem(q_("yyyy-mm-dd (ISO 8601)"), "yyyymmdd");
	dfmts->addItem(q_("dd-mm-yyyy"), "ddmmyyyy");
	dfmts->addItem(q_("mm-dd-yyyy"), "mmddyyyy");

	//Restore the selection
	index = dfmts->findData(selectedDateFormat, Qt::UserRole, Qt::MatchCaseSensitive);
	dfmts->setCurrentIndex(index);
	dfmts->blockSignals(false);
}

void ConfigurationDialog::setDateFormat()
{
	QString selectedFormat = ui->dateFormatsComboBox->itemData(ui->dateFormatsComboBox->currentIndex()).toString();

	StelLocaleMgr & localeManager = StelApp::getInstance().getLocaleMgr();
	if (selectedFormat == localeManager.getDateFormatStr())
		return;

	localeManager.setDateFormatStr(selectedFormat);	
}

void ConfigurationDialog::populateTimeFormatsList()
{
	Q_ASSERT(ui->timeFormatsComboBox);

	QComboBox* tfmts = ui->timeFormatsComboBox;

	//Save the current selection to be restored later
	tfmts->blockSignals(true);
	int index = tfmts->currentIndex();
	QVariant selectedTimeFormat = tfmts->itemData(index);
	tfmts->clear();
	//For each format, display the localized name and store the key as user
	//data. Unfortunately, there's no other way to do this than with a cycle.
	tfmts->addItem(q_("System default"), "system_default");
	tfmts->addItem(q_("12-hour format"), "12h");
	tfmts->addItem(q_("24-hour format"), "24h");

	//Restore the selection
	index = tfmts->findData(selectedTimeFormat, Qt::UserRole, Qt::MatchCaseSensitive);
	tfmts->setCurrentIndex(index);
	tfmts->blockSignals(false);
}

void ConfigurationDialog::setTimeFormat()
{
	QString selectedFormat = ui->timeFormatsComboBox->itemData(ui->timeFormatsComboBox->currentIndex()).toString();

	StelLocaleMgr & localeManager = StelApp::getInstance().getLocaleMgr();
	if (selectedFormat == localeManager.getTimeFormatStr())
		return;

	localeManager.setTimeFormatStr(selectedFormat);	
}