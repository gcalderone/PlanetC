/*
 * Planetarium Control (PlanetC) plug-in for Stellarium
 *
 * Copyright (C) 2016-2018 Giorgio Calderone
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

#include "StelApp.hpp"
#include "StelModuleMgr.hpp"
#include "StelUtils.hpp"

#include "PlanetC_Settings.hpp"
#include "PlanetC_UI.hpp"



PlanetC_StyleEditor::PlanetC_StyleEditor(PlanetC_UI* ui) :
	PlanetC_Dialog(ui),
	txtCss(this),
	btnApply("Apply", this),
	btnReset("Reset", this)
{
	this->ui = ui;

	setWindowTitle("Stellarium CSS editor");

	QWidget* widget = new QWidget(this);
	setCentralWidget(widget);

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(&btnApply);
	hlayout->addWidget(&btnReset);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(&txtCss);
	layout->addLayout(hlayout);

	widget->setLayout(layout);

	connect(&btnApply, SIGNAL(clicked()), this, SLOT(apply()));
	connect(&btnReset, SIGNAL(clicked()), this, SLOT(reset()));
	txtCss.setStyleSheet("color: rgb(200, 200, 200);  background: rgb(20, 20, 20);");
}


QString PlanetC_StyleEditor::loadCurrentFile()
{
	QString file = PlanetC::filePath("style.css");
	if(file == "")
		file = ":/PlanetC/style.css";

	if(txtCss.toPlainText() == "")
	{
		QFile f(file);
		f.open(QIODevice::ReadOnly);
		txtCss.setPlainText(f.readAll());
		f.close();
	}
	return file;
}

void PlanetC_StyleEditor::reset()
{
	QFile f(PlanetC::filePath("style.css", false));
	if(f.exists())
		f.remove();
	txtCss.setPlainText("");
	ui->setUIStyle(1);
}

void PlanetC_StyleEditor::apply()
{
	QFile w(PlanetC::filePath("style.css", false));
	w.open(QIODevice::WriteOnly);
	w.write(txtCss.toPlainText().toLatin1());
	w.close();
	ui->setUIStyle(1);
}



PlanetC_Settings::PlanetC_Settings(QWidget* parent) :
	PlanetC_Dialog(parent),
	editor((PlanetC_UI*) parent)
{
	//Default parameters
	conf = StelApp::getInstance().getSettings();
	pUIScale       = conf->value("planetc/consolle_scale" , "1.0").toFloat();
	pUIRate        = conf->value("planetc/consolle_refresh_rate" , "10.0").toFloat();
	pClockFontSize = conf->value("planetc/clock_font_size", "22").toInt();
	pShiftClock    = conf->value("planetc/shift_clock"    , "0.15").toFloat();
	pShiftCardPo   = conf->value("planetc/shift_cardinals", "0.05").toFloat();
	pTimeMove      = conf->value("planetc/time_move"      , "6").toFloat();
	pTimeZoom      = conf->value("planetc/time_zoom"      , "10").toFloat();
	pStartDate     = conf->value("planetc/Start_date"     , "2016-01-01T12:00:00").toString();
	pEqMountTrackAppPos = true;

	pExtProjFullscreen = conf->value("planetc/extproj_Fullscreen", "1").toBool();
	pExtProj_X         = conf->value("planetc/extproj_left"      , "0").toInt();
	pExtProj_Y         = conf->value("planetc/extproj_top"       , "0").toInt();
	pExtProj_W         = conf->value("planetc/extproj_width"     , "600").toInt();
	pExtProj_H         = conf->value("planetc/extproj_height"    , "600").toInt();

	ui = new Ui_PlanetC_Settings_Form();
	createDialogContent();
	//dialog = new QWidget(NULL, Qt::FramelessWindowHint);
	//createDialogContent();
	//dialog->adjustSize(); //required on windows?
	//dialog->hide();

	QString newHtml = "<h2>PlanetC (v" + QString(PLANETC_PLUGIN_VERSION) + ")</h2>";
	newHtml += "<h4>A powerful user interface for Stellarium.</h4>";
	newHtml += "<hr>";
	newHtml += "<h4>PlanetC has been developed by <a href=\"mailto:giorgio.calderone@gmail.com\">Giorgio Calderone</a>";
	newHtml += " and the <a href=\"http://www.starfreedom.it/\">Star Freedom</a> association.";
	newHtml += " More info at:<br>";
	newHtml += " -- <a href=\"https://github.com/gcalderone/PlanetC/wiki\">PlanetC Wiki</a><br>";
	newHtml += " -- <a href=\"https://github.com/gcalderone/PlanetC\">Source code</a><br>";
	newHtml += " -- <a href=\"http://www.starfreedom.it/software_planetc/\">Star Freedom</a><br><br>";
	newHtml += "For info write at <a href=\"mailto:planetc.info@gmail.com\">planetc.info@gmail.com</a><br><br>";
	newHtml += "This PlanetC version is based on <a href=\"http://stellarium.org/\">Stellarium</a> v" + StelUtils::getApplicationVersion() + "<br>";
	newHtml += "</h4>";
	newHtml += "<h3>Copyright &copy; 2016-2018 Giorgio Calderone</h3>";
	newHtml += "<p>This program is free software; you can redistribute it and/or ";
	newHtml += "modify it under the terms of the GNU General Public License ";
	newHtml += "as published by the Free Software Foundation; either version 2 ";
	newHtml += "of the License, or (at your option) any later version.</p>";
	newHtml += "<p>This program is distributed in the hope that it will be useful, ";
	newHtml += "but WITHOUT ANY WARRANTY; without even the implied ";
	newHtml += "warranty of MERCHANTABILITY or FITNESS FOR A ";
	newHtml += "PARTICULAR PURPOSE.  See the GNU General Public ";
	newHtml += "License for more details.</p>";
	newHtml += "<p>You should have received a copy of the GNU General Public ";
	newHtml += "License along with this program; if not, write to:</p>";
	newHtml += "<pre>Free Software Foundation, Inc.\n";
	newHtml += "51 Franklin Street, Suite 500\n";
	newHtml += "Boston, MA  02110-1335, USA.\n</pre>";
	newHtml += "<p><a href=\"http://www.fsf.org\">www.fsf.org</a></p>";

	ui->textAbout->clear();
	ui->textAbout->document()->setDefaultStyleSheet(QString("a { color: rgb(162, 140, 66); }"));
	ui->textAbout->setHtml(newHtml);
	ui->textAbout->scrollToAnchor("top");

	//enable/disable spin box
	setExtProjSettings(0);
}


void PlanetC_Settings::createDialogContent()
{
	QWidget* ww = new QWidget(this);
	setCentralWidget(ww);
	ui->setupUi(ww);
	ww->adjustSize();
	adjustSize();
	ui->TitleBar->setVisible(false);

	ui->spinUIScale->setValue(pUIScale);
	ui->spinUIRate->setValue(pUIRate);
	ui->spinClockFontSize->setValue(pClockFontSize);
	ui->spinClockShift->setValue(pShiftClock);
	ui->spinCardShift->setValue(pShiftCardPo);
	ui->spinTimeMove->setValue(pTimeMove);
	ui->spinTimeZoom->setValue(pTimeZoom);
	ui->btnEqMountTrackAppPos->setChecked(pEqMountTrackAppPos);
	ui->chkExtProjFullscreen->setChecked(pExtProjFullscreen);
	ui->spinExtProjX->setValue(pExtProj_X);
	ui->spinExtProjY->setValue(pExtProj_Y);
	ui->spinExtProjW->setValue(pExtProj_W);
	ui->spinExtProjH->setValue(pExtProj_H);

	connect(ui->spinUIScale          , SIGNAL(valueChanged(double)), this, SLOT(setUIScale(double)));
	connect(ui->spinUIRate           , SIGNAL(valueChanged(double)), this, SLOT(setUIRate(double)));
	connect(ui->spinClockFontSize    , SIGNAL(valueChanged(int))   , this, SLOT(setClockFontSize(int)));
	connect(ui->spinClockShift       , SIGNAL(valueChanged(double)), this, SLOT(setShiftClock(double)));
	connect(ui->spinCardShift        , SIGNAL(valueChanged(double)), this, SLOT(setShiftCardPo(double)));
	connect(ui->spinTimeMove         , SIGNAL(valueChanged(double)), this, SLOT(setTimeMove(double)));
	connect(ui->spinTimeZoom         , SIGNAL(valueChanged(double)), this, SLOT(setTimeZoom(double)));
	connect(ui->btnEqMountTrackAppPos, SIGNAL(toggled(bool))       , this, SLOT(setEqMountTrackAppPos(bool)));
	connect(ui->closeStelWindow      , SIGNAL(clicked())           , this, SLOT(close()));
	connect(ui->btnOpenStyleEditor   , SIGNAL(clicked())           , this, SLOT(openStyleEditor()));

	connect(ui->chkExtProjFullscreen , SIGNAL(stateChanged(int))   , this, SLOT(setExtProjSettings(int)));
	connect(ui->spinExtProjX         , SIGNAL(valueChanged(int))   , this, SLOT(setExtProjSettings(int)));
	connect(ui->spinExtProjY         , SIGNAL(valueChanged(int))   , this, SLOT(setExtProjSettings(int)));
	connect(ui->spinExtProjW         , SIGNAL(valueChanged(int))   , this, SLOT(setExtProjSettings(int)));
	connect(ui->spinExtProjH         , SIGNAL(valueChanged(int))   , this, SLOT(setExtProjSettings(int)));
}

void PlanetC_Settings::setUIScale(double v)
{
	pUIScale = v;
	conf->setValue("planetc/consolle_scale", QString::number(v, 'f', 2));
}

void PlanetC_Settings::setUIRate(double v)
{
	pUIRate = v;
	conf->setValue("planetc/consolle_refresh_rate", QString::number(v, 'f', 2));
}

void PlanetC_Settings::setClockFontSize(int v)
{
	pClockFontSize = v;
	conf->setValue("planetc/clock_font_size", QString::number(v, 'f', 0));
}

void PlanetC_Settings::setTimeMove(double v)
{
	pTimeMove = v;
	conf->setValue("planetc/time_move", QString::number(v, 'f', 2));
}

void PlanetC_Settings::setTimeZoom(double v)
{
	pTimeZoom = v;
	conf->setValue("planetc/time_zoom", QString::number(v, 'f', 2));
}

void PlanetC_Settings::setStartDate(QString v)
{
	pStartDate = v;
	conf->setValue("planetc/Start_date", v);
}

void PlanetC_Settings::setShiftClock(double v)
{
	pShiftClock = v;
	conf->setValue("planetc/shift_clock", QString::number(v, 'f', 2));
}

void PlanetC_Settings::setShiftCardPo(double v)
{
	pShiftCardPo = v;
	conf->setValue("planetc/shift_cardinals", QString::number(v, 'f', 2));
}

void PlanetC_Settings::setEqMountTrackAppPos(bool v)
{
	pEqMountTrackAppPos = v;
}

void PlanetC_Settings::openStyleEditor()
{
	editor.show();
}

void PlanetC_Settings::setExtProjSettings(int v)
{
	pExtProjFullscreen = ui->chkExtProjFullscreen->isChecked();
	pExtProj_X = ui->spinExtProjX->value();
	pExtProj_Y = ui->spinExtProjY->value();
	pExtProj_W = ui->spinExtProjW->value();
	pExtProj_H = ui->spinExtProjH->value();

	conf->setValue("planetc/extproj_Fullscreen", QString::number(pExtProjFullscreen));
	conf->setValue("planetc/extproj_left"      , QString::number(pExtProj_X));
	conf->setValue("planetc/extproj_top"       , QString::number(pExtProj_Y));
	conf->setValue("planetc/extproj_width"     , QString::number(pExtProj_W));
	conf->setValue("planetc/extproj_height"    , QString::number(pExtProj_H));

	ui->spinExtProjX->setEnabled(!pExtProjFullscreen);
	ui->spinExtProjY->setEnabled(!pExtProjFullscreen);
	ui->spinExtProjH->setEnabled(!pExtProjFullscreen);
	ui->spinExtProjW->setEnabled(!pExtProjFullscreen);
}
