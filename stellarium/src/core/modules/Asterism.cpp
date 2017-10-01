/*
 * Stellarium
 * Copyright (C) 2017 Alexander Wolf
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

#include "StelProjector.hpp"
#include "Asterism.hpp"
#include "StarMgr.hpp"

#include "StelPainter.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelModuleMgr.hpp"
#include "StelTranslator.hpp"
#include "AsterismMgr.hpp"

#include <algorithm>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QFontMetrics>

Vec3f Asterism::lineColor = Vec3f(0.4,0.4,0.8);
Vec3f Asterism::labelColor = Vec3f(0.4,0.4,0.8);
const QString Asterism::ASTERISM_TYPE = QStringLiteral("Asterism");

Asterism::Asterism()
	: numberOfSegments(0)
	, asterism(Q_NULLPTR)
{
}

Asterism::~Asterism()
{
	delete[] asterism;
	asterism = Q_NULLPTR;
}

bool Asterism::read(const QString& record, StarMgr *starMgr)
{
	unsigned int HP;

	abbreviation.clear();
	numberOfSegments = 0;

	QString buf(record);
	QTextStream istr(&buf, QIODevice::ReadOnly);
	QString abb;
	istr >> abb >> numberOfSegments;
	if (istr.status()!=QTextStream::Ok)
		return false;

	// It's better to allow mixed-case abbreviations now that they can be displayed on screen. We then need toUpper() in comparisons.
	//abbreviation = abb.toUpper();
	abbreviation=abb;

	asterism = new StelObjectP[numberOfSegments*2];
	for (unsigned int i=0;i<numberOfSegments*2;++i)
	{
		HP = 0;
		istr >> HP;
		if(HP == 0)
		{
			// TODO: why is this delete commented?
			// delete[] asterism;
			return false;
		}

		asterism[i]=starMgr->searchHP(HP);
		if (!asterism[i])
		{
			qWarning() << "Error in Asterism " << abbreviation << ": can't find star HP= " << HP;
			// TODO: why is this delete commented?
			// delete[] asterism;
			return false;
		}
	}

	XYZname.set(0.,0.,0.);
	for(unsigned int ii=0;ii<numberOfSegments*2;++ii)
	{
		XYZname+= asterism[ii]->getJ2000EquatorialPos(StelApp::getInstance().getCore());
	}
	XYZname.normalize();

	return true;
}

void Asterism::drawOptim(StelPainter& sPainter, const StelCore* core, const SphericalCap& viewportHalfspace) const
{
	if (lineFader.getInterstate()<=0.0001f)
		return;

	sPainter.setColor(lineColor[0], lineColor[1], lineColor[2], lineFader.getInterstate());

	Vec3d star1;
	Vec3d star2;
	for (unsigned int i=0;i<numberOfSegments;++i)
	{
		star1=asterism[2*i]->getJ2000EquatorialPos(core);
		star2=asterism[2*i+1]->getJ2000EquatorialPos(core);
		star1.normalize();
		star2.normalize();
		sPainter.drawGreatCircleArc(star1, star2, &viewportHalfspace);
	}
}

void Asterism::drawName(StelPainter& sPainter) const
{
	if (!nameFader.getInterstate())
		return;

	QString name = getNameI18n();
	sPainter.setColor(labelColor[0], labelColor[1], labelColor[2], nameFader.getInterstate());
	sPainter.drawText(XYname[0], XYname[1], name, 0., -sPainter.getFontMetrics().width(name)/2, 0, false);

}

const Asterism* Asterism::isStarIn(const StelObject* s) const
{
	for(unsigned int i=0;i<numberOfSegments*2;++i)
	{
		if (asterism[i]->getEnglishName()==s->getEnglishName())
			return this;
	}
	return Q_NULLPTR;
}

void Asterism::update(int deltaTime)
{
	lineFader.update(deltaTime);
	nameFader.update(deltaTime);
}

QString Asterism::getInfoString(const StelCore *core, const InfoStringGroup &flags) const
{
	Q_UNUSED(core);
	QString str;
	QTextStream oss(&str);

	if (flags&Name)
		oss << "<h2>" << getNameI18n() << "</h2>";

	if (flags&ObjectType)
		oss << q_("Type: <b>%1</b>").arg(q_("asterism")) << "<br />";

	postProcessInfoString(str, flags);

	return str;
}

