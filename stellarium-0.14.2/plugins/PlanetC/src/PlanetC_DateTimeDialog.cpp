/*
 * Stellarium
 * Copyright (C) 2008 Nigel Kerr
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

/*
  IMPORTANT NOTE:

  This is very similar to the original one in the stellarium trunk, it
  has been slightly modified to fit in the PlanetC interface
*/


#include "Dialog.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelLocaleMgr.hpp"
#include "StelUtils.hpp"

#include "ui_planetc_DateTimeDialogGui.h"
#include "PlanetC_DateTimeDialog.hpp"

#include <QDebug>
#include <QFrame>
#include <QLineEdit>

PlanetC_DateTimeDialog::PlanetC_DateTimeDialog(QObject* parent, QLayout* layout) :
	StelDialog(parent),
	year(0),
	month(0),
	day(0),
	hour(0),
	minute(0),
	second(0),
	jd(0),
	mjd(0)
{
	ui = new Ui_planetc_DateTimeDialogForm;
	dialog = new QWidget();
	createDialogContent();
	layout->addWidget(dialog);
}

PlanetC_DateTimeDialog::~PlanetC_DateTimeDialog()
{
	delete ui;
	ui=NULL;
}

void PlanetC_DateTimeDialog::createDialogContent()
{
	ui->setupUi(dialog);
	double jd = StelApp::getInstance().getCore()->getJD();
	// UTC -> local tz
	setDateTime(jd + (StelApp::getInstance().getLocaleMgr().getGMTShift(jd)/24.0));

	connect(&StelApp::getInstance(), SIGNAL(languageChanged()), this, SLOT(retranslate()));
	//connect(ui->closeStelWindow, SIGNAL(clicked()), this, SLOT(close()));

	connectSpinnerEvents();
}

void PlanetC_DateTimeDialog::connectSpinnerEvents() const
{
	connect(ui->spinner_year, SIGNAL(valueChanged(int)), this, SLOT(yearChanged(int)));
	connect(ui->spinner_month, SIGNAL(valueChanged(int)), this, SLOT(monthChanged(int)));
	connect(ui->spinner_day, SIGNAL(valueChanged(int)), this, SLOT(dayChanged(int)));
	connect(ui->spinner_hour, SIGNAL(valueChanged(int)), this, SLOT(hourChanged(int)));
	connect(ui->spinner_minute, SIGNAL(valueChanged(int)), this, SLOT(minuteChanged(int)));
	connect(ui->spinner_second, SIGNAL(valueChanged(int)), this, SLOT(secondChanged(int)));
	//connect(ui->spinner_jd, SIGNAL(valueChanged(double)), this, SLOT(jdChanged(double)));
	//connect(ui->spinner_mjd, SIGNAL(valueChanged(double)), this, SLOT(mjdChanged(double)));
}

void PlanetC_DateTimeDialog::disconnectSpinnerEvents()const
{
	disconnect(ui->spinner_year, SIGNAL(valueChanged(int)), this, SLOT(yearChanged(int)));
	disconnect(ui->spinner_month, SIGNAL(valueChanged(int)), this, SLOT(monthChanged(int)));
	disconnect(ui->spinner_day, SIGNAL(valueChanged(int)), this, SLOT(dayChanged(int)));
	disconnect(ui->spinner_hour, SIGNAL(valueChanged(int)), this, SLOT(hourChanged(int)));
	disconnect(ui->spinner_minute, SIGNAL(valueChanged(int)), this, SLOT(minuteChanged(int)));
	disconnect(ui->spinner_second, SIGNAL(valueChanged(int)), this, SLOT(secondChanged(int)));
	//disconnect(ui->spinner_jd, SIGNAL(valueChanged(double)), this, SLOT(jdChanged(double)));
	//disconnect(ui->spinner_mjd, SIGNAL(valueChanged(double)), this, SLOT(mjdChanged(double)));
}


//! take in values, adjust for calendrical correctness if needed, and push to
//! the widgets and signals
bool PlanetC_DateTimeDialog::valid(int y, int m, int d, int h, int min, int s)
{
	int dy, dm, dd, dh, dmin, ds;

	if(!StelUtils::changeDateTimeForRollover(y, m, d, h, min, s, &dy, &dm, &dd, &dh, &dmin, &ds))
	{
		dy = y;
		dm = m;
		dd = d;
		dh = h;
		dmin = min;
		ds = s;
	}

	year = dy;
	month = dm;
	day = dd;
	hour = dh;
	minute = dmin;
	second = ds;
	pushToWidgets();
	StelApp::getInstance().getCore()->setJD(newJd());
	return true;
}

bool PlanetC_DateTimeDialog::validJd(double jday)
{
	pushToWidgets();
	StelApp::getInstance().getCore()->setJD(jday);

	return true;
}

void PlanetC_DateTimeDialog::retranslate()
{
	if(dialog)
	{
		ui->retranslateUi(dialog);
	}
}

void PlanetC_DateTimeDialog::styleChanged()
{
	// Nothing for now
}

void PlanetC_DateTimeDialog::close()
{
	ui->dateTimeBox->setFocus();
	StelDialog::close();
}

/************************************************************************
 year slider or dial changed
************************************************************************/

void PlanetC_DateTimeDialog::yearChanged(int newyear)
{
	if(year != newyear)
	{
		valid(newyear, month, day, hour, minute, second);
	}
}
void PlanetC_DateTimeDialog::monthChanged(int newmonth)
{
	if(month != newmonth)
	{
		valid(year, newmonth, day, hour, minute, second);
	}
}
void PlanetC_DateTimeDialog::dayChanged(int newday)
{
	if(day != newday)
	{
		valid(year, month, newday, hour, minute, second);
	}
}
void PlanetC_DateTimeDialog::hourChanged(int newhour)
{
	if(hour != newhour)
	{
		valid(year, month, day, newhour, minute, second);
	}
}
void PlanetC_DateTimeDialog::minuteChanged(int newminute)
{
	if(minute != newminute)
	{
		valid(year, month, day, hour, newminute, second);
	}
}
void PlanetC_DateTimeDialog::secondChanged(int newsecond)
{
	if(second != newsecond)
	{
		valid(year, month, day, hour, minute, newsecond);
	}
}
void PlanetC_DateTimeDialog::jdChanged(double njd)
{
	if(jd != njd)
	{
		validJd(njd);
	}
}
void PlanetC_DateTimeDialog::mjdChanged(double nmjd)
{
	if(mjd != nmjd)
	{
		validJd(2400000.5 + nmjd);
	}
}


double PlanetC_DateTimeDialog::newJd()
{
	double cjd;
	StelUtils::getJDFromDate(&cjd, year, month, day, hour, minute, second);
	cjd -= (StelApp::getInstance().getLocaleMgr().getGMTShift(cjd)/24.0); // local tz -> UTC

	return cjd;
}

void PlanetC_DateTimeDialog::pushToWidgets()
{
	disconnectSpinnerEvents();
	ui->spinner_year->setValue(year);
	ui->spinner_month->setValue(month);
	ui->spinner_day->setValue(day);
	ui->spinner_hour->setValue(hour);
	if(!ui->spinner_minute->hasFocus() || (ui->spinner_minute->value() == -1) || (ui->spinner_minute->value() == 60))
	{
		ui->spinner_minute->setValue(minute);
	}
	if(!ui->spinner_second->hasFocus() || (ui->spinner_second->value() == -1) || (ui->spinner_second->value() == 60))
	{
		ui->spinner_second->setValue(second);
	}
	//ui->spinner_jd->setValue(jd);
	//ui->spinner_mjd->setValue(mjd);
	if(jd<2299161)  // 1582-10-15
		ui->dateTimeBox->setToolTip(q_("Date and Time in Julian calendar"));
	else
		ui->dateTimeBox->setToolTip(q_("Date and Time in Gregorian calendar"));
	connectSpinnerEvents();
}

/************************************************************************
Prepare date elements from newJd and send to spinner_*
 ************************************************************************/
void PlanetC_DateTimeDialog::setDateTime(double newJd)
{
	if(this->visible())
	{
		// JD and MJD should be at the UTC scale on the window!
		double newJdC = newJd + StelApp::getInstance().getLocaleMgr().getGMTShift(newJd)/24.0; // UTC -> local tz
		StelUtils::getDateFromJulianDay(newJdC, &year, &month, &day);
		StelUtils::getTimeFromJulianDay(newJdC, &hour, &minute, &second);
		jd = newJd;
		mjd = newJd-2400000.5;

		pushToWidgets();
	}
}

QDateTime PlanetC_DateTimeDialog::current()
{
	return QDateTime(QDate(year, month, day),
	                 QTime(hour, minute, second));
}
