/*
 * Angle Measure plug-in for Stellarium
 *
 * Copyright (C) 2014 Alexander Wolf
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

#ifndef _ANGLEMEASUREDIALOG_HPP_
#define _ANGLEMEASUREDIALOG_HPP_

#include "StelDialog.hpp"
#include "StelGui.hpp"
#include "StelTranslator.hpp"

#include <QString>

class Ui_angleMeasureDialog;
class AngleMeasure;

//! Main window of the Angle Measure plug-in.
//! @ingroup angleMeasure
class AngleMeasureDialog : public StelDialog
{
	Q_OBJECT

public:
	AngleMeasureDialog();
	~AngleMeasureDialog();

public slots:
	void retranslate();

protected:
	void createDialogContent();

private:
	Ui_angleMeasureDialog* ui;
	AngleMeasure* am;

	void setAboutHtml();

private slots:
	void saveAngleMeasureSettings();
	void resetAngleMeasureSettings();
};


#endif /* _ANGLEMEASUREDIALOG_HPP_ */