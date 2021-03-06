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


#ifndef _PLANETC_DATETIMEDIALOG_HPP_
#define _PLANETC_DATETIMEDIALOG_HPP_

#include <QObject>
#include "StelDialog.hpp"

class Ui_planetc_DateTimeDialogForm;

class PlanetC_DateTimeDialog : public StelDialog
{
	Q_OBJECT
public:
	PlanetC_DateTimeDialog(QString s, QObject* parent, QLayout* layout);
	~PlanetC_DateTimeDialog();
	double newJd();
	bool valid(int y, int m, int d, int h, int min, int s);
	bool validJd(double jday);	
	//! Notify that the application style changed
	void styleChanged();
public slots:
	void retranslate();
	//! update the editing display with new JD.
	void setDateTime(double newJd);

	void close();
	QDateTime current();


protected:
	//! Initialize the dialog widgets and connect the signals/slots
	virtual void createDialogContent();
	void connectSpinnerEvents() const;
	void disconnectSpinnerEvents()const;

private slots:
	//! year slider or dial changed
	void yearChanged(int ny);
	//! year slider or dial changed
	void monthChanged(int nm);
	//! year slider or dial changed
	void dayChanged(int nd);
	//! year slider or dial changed
	void hourChanged(int nh);
	//! year slider or dial changed
	void minuteChanged(int nm);
	//! year slider or dial changed
	void secondChanged(int ns);
	//! JD slider or dial changed
	void jdChanged(double njd);
	//! MJD slider or dial changed
	void mjdChanged(double nmjd);

private:
	Ui_planetc_DateTimeDialogForm* ui;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	double jd;
	void pushToWidgets();
	void setMjd(double mjd) { jd = 2400000.5 + mjd; }
	double getMjd() { return jd - 2400000.5 ; }
};

#endif // _PLANETC_DATETIMEDIALOG_HPP_
