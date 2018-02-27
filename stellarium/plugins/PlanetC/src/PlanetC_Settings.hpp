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
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _PLANETC_SETTINGS_HPP_
#define _PLANETC_SETTINGS_HPP_


#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSettings>
#include "StelDialog.hpp"
#include "PlanetC_Widgets.hpp"

#include "ui_planetc_Settings.h"
class Ui_PlanetC_Settings_Form;
class PlanetC_UI;



class PlanetC_StyleEditor : public PlanetC_Dialog
{
	Q_OBJECT

public:
	PlanetC_StyleEditor(PlanetC_UI* ui);
	~PlanetC_StyleEditor() {}
	QString loadCurrentFile();

private:
	PlanetC_UI* ui;
	QPlainTextEdit txtCss;
	QPushButton btnApply;
	QPushButton btnReset;

private slots:
	void apply();
	void reset();
};






class PlanetC_Settings : public PlanetC_Dialog
{
	Q_OBJECT
	friend class PlanetC_UI;

public:
	PlanetC_Settings(QWidget*);

	~PlanetC_Settings()
	{
		delete ui;
	}

	float getUIScale()
	{
		return pUIScale;
	}

	float getUIRate()
	{
		return pUIRate;
	}

	int getClockFontSize()
	{
		return pClockFontSize;
	}

	float getTimeMove()
	{
		return pTimeMove;
	}

	float getTimeZoom()
	{
		return pTimeZoom;
	}

	QString getStartDate()
	{
		return pStartDate;
	}

	float getShiftClock()
	{
		return pShiftClock;
	}

	float getShiftCardPo()
	{
		return pShiftCardPo;
	}

	bool getEqMountTrackAppPos()
	{
		return pEqMountTrackAppPos;
	}

	bool getExtProjFullscreen()
	{
		return pExtProjFullscreen;
	}

	int getExtProj_X()
	{
		return pExtProj_X;
	}

	int getExtProj_Y()
	{
		return pExtProj_Y;
	}

	int getExtProj_W()
	{
		return pExtProj_W;
	}

	int getExtProj_H()
	{
		return pExtProj_H;
	}

private slots:
	void setUIScale(double v);
	void setUIRate(double v);
	void setClockFontSize(int v);
	void setTimeMove(double v);
	void setTimeZoom(double v);
	void setStartDate(QString v);
	void setShiftClock(double v);
	void setShiftCardPo(double v);
	void setEqMountTrackAppPos(bool v);
	void setExtProjSettings(int);
	void openStyleEditor();

protected:
	//! Initialize the dialog widgets and connect the signals/slots
	virtual void createDialogContent();

private:
	Ui_PlanetC_Settings_Form* ui;

	QSettings* conf;
	float   pUIScale; //scale factor to be applied to CSS (see PlanetC::setUIScale)
	float   pUIRate;
	int     pClockFontSize;
	float   pTimeMove;
	float   pTimeZoom;
	QString pStartDate;
	float   pShiftClock;
	float   pShiftCardPo;
	bool    pEqMountTrackAppPos;
	bool    pExtProjFullscreen;
	int     pExtProj_X;
	int     pExtProj_Y;
	int     pExtProj_W;
	int     pExtProj_H;

	PlanetC_StyleEditor editor;
};

#endif //_PLANETC_SETTINGS_HPP_
