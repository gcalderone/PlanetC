/*
 * Stellarium
 * Copyright (C) 2002 Fabien Chereau (some old code from the Planet class)
 * Copyright (C) 2010 Bogdan Marinov
 * Copyright (C) 2013-14 Georg Zotti (accuracy&speedup)
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
 
#include "MinorPlanet.hpp"

#include "StelApp.hpp"
#include "StelCore.hpp"

#include "StelTexture.hpp"
#include "StelTextureMgr.hpp"
#include "StelTranslator.hpp"
#include "StelUtils.hpp"
#include "StelFileMgr.hpp"

#include <QRegExp>
#include <QDebug>

MinorPlanet::MinorPlanet(const QString& englishName,
			 double radius,
			 double oblateness,
			 Vec3f halocolor,
			 float albedo,
			 float roughness,
			 //float outgas_intensity,
			 //float outgas_falloff,
			 const QString& atexMapName,
			 const QString& aobjModelName,
			 posFuncType coordFunc,
			 void* auserDataPtr,
			 OsculatingFunctType *osculatingFunc,
			 bool acloseOrbit,
			 bool hidden,
			 const QString &pTypeStr)
	: Planet (englishName,
		  radius,
		  oblateness,
		  halocolor,
		  albedo,
		  roughness,
		  //0.f, // outgas_intensity,
		  //0.f, // outgas_falloff,
		  atexMapName,
		  "",
		  aobjModelName,
		  coordFunc,
		  auserDataPtr,
		  osculatingFunc,
		  acloseOrbit,
		  hidden,
		  false, //No atmosphere
		  true,  //Halo
		  pTypeStr),
	minorPlanetNumber(0),
	slopeParameter(-1.0f), //== mark as uninitialized: used in getVMagnitude()
	semiMajorAxis(0.),
	nameIsProvisionalDesignation(false),
	properName(englishName)
{
	//TODO: Fix the name
	// - Detect numeric prefix and set number if any
	// - detect provisional designation
	// - create the HTML name
	//Try to detect number
	//TODO: Move this to the minor planet parse code in the plug-in?	
	/*
	QString name = englishName;
	QRegExp bracketedNumberPrefixPattern("^\\((\\d+)\\)\\s");
	QRegExp freeNumberPrefixPattern("^(\\d+)\\s[A-Za-z]{3,}");
	if (bracketedNumberPrefixPattern.indexIn(name) == 0)
	{
		QString numberString = bracketedNumberPrefixPattern.cap(1);
		bool ok = false;
		number = numberString.toInt(&ok);
		if (!ok)
			number = 0;

		//TODO: Handle a name consisting only of a number
		name.remove(0, numberString.length() + 3);
		htmlName = QString("(%1) ").arg(number);
	}
	else if (freeNumberPrefixPattern.indexIn(name) == 0)
	{
		QString numberString =freeNumberPrefixPattern.cap(1);
		bool ok = false;
		number = numberString.toInt(&ok);
		if (!ok)
			number = 0;

		//TODO: Handle a name consisting only of a number
		name.remove(0, numberString.length() + 3);
		htmlName = QString("(%1) ").arg(number);
	}*/

	//Try to detect a naming conflict
	if (englishName.endsWith('*'))
		properName = englishName.left(englishName.count() - 1);

	//Try to detect provisional designation	
	QString provisionalDesignation = renderProvisionalDesignationinHtml(englishName);
	if (!provisionalDesignation.isEmpty())
	{
		nameIsProvisionalDesignation = true;
		provisionalDesignationHtml = provisionalDesignation;
	}
}

MinorPlanet::~MinorPlanet()
{
	//Do nothing for the moment
}

void MinorPlanet::setSemiMajorAxis(double value)
{
	semiMajorAxis = value;
	// GZ: in case we have very many asteroids, this helps improving speed usually without sacrificing accuracy:
	deltaJDE = 2.0*qMax(semiMajorAxis, 0.1)*StelCore::JD_SECOND;
}

void MinorPlanet::setMinorPlanetNumber(int number)
{
	if (minorPlanetNumber)
		return;

	minorPlanetNumber = number;
}

void MinorPlanet::setAbsoluteMagnitudeAndSlope(const float magnitude, const float slope)
{
	if (slope < 0 || slope > 1.0)
	{
		qDebug() << "MinorPlanet::setAbsoluteMagnitudeAndSlope(): Invalid slope parameter value (must be between 0 and 1)";
		return;
	}

	//TODO: More checks?
	//TODO: Make it set-once like the number?

	absoluteMagnitude = magnitude;
	slopeParameter = slope;
}

void MinorPlanet::setProvisionalDesignation(QString designation)
{
	//TODO: This feature has to be implemented better, anyway.
	provisionalDesignationHtml = renderProvisionalDesignationinHtml(designation);
}

QString MinorPlanet::getInfoString(const StelCore *core, const InfoStringGroup &flags) const
{
	//Mostly copied from Planet::getInfoString():

	QString str;
	QTextStream oss(&str);
	double az_app, alt_app;
	StelUtils::rectToSphe(&az_app,&alt_app,getAltAzPosApparent(core));
	bool withDecimalDegree = StelApp::getInstance().getFlagShowDecimalDegrees();
	double distanceAu = getJ2000EquatorialPos(core).length();
	Q_UNUSED(az_app);

	if (flags&Name)
	{
		oss << "<h2>";
		if (minorPlanetNumber)
			oss << QString("(%1) ").arg(minorPlanetNumber);
		if (nameIsProvisionalDesignation)
			oss << provisionalDesignationHtml;
		else
			oss << getNameI18n();  // UI translation can differ from sky translation
		oss.setRealNumberNotation(QTextStream::FixedNotation);
		oss.setRealNumberPrecision(1);
		if (sphereScale != 1.f)
			oss << QString::fromUtf8(" (\xC3\x97") << sphereScale << ")";
		oss << "</h2>";
		if (!nameIsProvisionalDesignation && !provisionalDesignationHtml.isEmpty())
		{
			oss << QString(q_("Provisional designation: %1")).arg(provisionalDesignationHtml);
			oss << "<br>";
		}
	}

	if (flags&ObjectType && getPlanetType()!=isUNDEFINED)
	{
		oss << q_("Type: <b>%1</b>").arg(q_(getPlanetTypeString())) << "<br />";
	}

	if (flags&Magnitude)
	{
	    if (core->getSkyDrawer()->getFlagHasAtmosphere() && (alt_app>-3.0*M_PI/180.0)) // Don't show extincted magnitude much below horizon where model is meaningless.
		oss << q_("Magnitude: <b>%1</b> (after extinction: <b>%2</b>)").arg(QString::number(getVMagnitude(core), 'f', 2),
										QString::number(getVMagnitudeWithExtinction(core), 'f', 2)) << "<br>";
	    else
		oss << q_("Magnitude: <b>%1</b>").arg(getVMagnitude(core), 0, 'f', 2) << "<br>";

	}

	if (flags&AbsoluteMagnitude)
	{
		//TODO: Make sure absolute magnitude is a sane value
		//If the H-G system is not used, use the default radius/albedo mechanism
		if (slopeParameter < 0)
		{
			oss << q_("Absolute Magnitude: %1").arg(getVMagnitude(core) - 5. * (std::log10(distanceAu*AU/PARSEC)-1.), 0, 'f', 2) << "<br>";
		}
		else
		{
			oss << q_("Absolute Magnitude: %1").arg(absoluteMagnitude, 0, 'f', 2) << "<br>";
		}
	}

	oss << getPositionInfoString(core, flags);

	if (flags&Distance)
	{
		double hdistanceAu = getHeliocentricEclipticPos().length();
		double hdistanceKm = AU * hdistanceAu;
		if (englishName!="Sun")
		{
			if (hdistanceAu < 0.1)
			{
				// xgettext:no-c-format
				oss << QString(q_("Distance from Sun: %1AU (%2 km)"))
				       .arg(hdistanceAu, 0, 'f', 6)
				       .arg(hdistanceKm, 0, 'f', 3);
			}
			else
			{
				// xgettext:no-c-format
				oss << QString(q_("Distance from Sun: %1AU (%2 Mio km)"))
				       .arg(hdistanceAu, 0, 'f', 3)
				       .arg(hdistanceKm / 1.0e6, 0, 'f', 3);
			}
			oss << "<br>";
		}
		double distanceKm = AU * distanceAu;
		if (distanceAu < 0.1)
		{
			// xgettext:no-c-format
			oss << QString(q_("Distance: %1AU (%2 km)"))
				   .arg(distanceAu, 0, 'f', 6)
				   .arg(distanceKm, 0, 'f', 3);
		}
		else
		{
			// xgettext:no-c-format
			oss << QString(q_("Distance: %1AU (%2 Mio km)"))
				   .arg(distanceAu, 0, 'f', 3)
				   .arg(distanceKm / 1.0e6, 0, 'f', 3);
		}
		oss << "<br>";
	}

	float aSize = 2.*getAngularSize(core)*M_PI/180.;
	if (flags&Size && aSize>1e-6)
	{
		if (withDecimalDegree)
			oss << q_("Apparent diameter: %1").arg(StelUtils::radToDecDegStr(aSize,5,false,true)) << "<br>";
		else
			oss << q_("Apparent diameter: %1").arg(StelUtils::radToDmsStr(aSize, true)) << "<br>";
	}

	// If semi-major axis not zero then calculate and display orbital period for asteroid in days
	double siderealPeriod = getSiderealPeriod();
	if (flags&Extra)
	{
		if (siderealPeriod>0)
		{
			// TRANSLATORS: Sidereal (orbital) period for solar system bodies in days and in Julian years (symbol: a)
			oss << q_("Sidereal period: %1 days (%2 a)").arg(QString::number(siderealPeriod, 'f', 2)).arg(QString::number(siderealPeriod/365.25, 'f', 3)) << "<br>";
		}

		const Vec3d& observerHelioPos = core->getObserverHeliocentricEclipticPos();
		const double elongation = getElongation(observerHelioPos);

		if (withDecimalDegree)
		{
			oss << QString(q_("Phase Angle: %1")).arg(StelUtils::radToDecDegStr(getPhaseAngle(observerHelioPos),4,false,true)) << "<br>";
			oss << QString(q_("Elongation: %1")).arg(StelUtils::radToDecDegStr(elongation,4,false,true)) << "<br>";
		}
		else
		{
			oss << QString(q_("Phase Angle: %1")).arg(StelUtils::radToDmsStr(getPhaseAngle(observerHelioPos), true)) << "<br>";
			oss << QString(q_("Elongation: %1")).arg(StelUtils::radToDmsStr(elongation, true)) << "<br>";
		}
	}

	postProcessInfoString(str, flags);

	return str;
}

double MinorPlanet::getSiderealPeriod() const
{
	double period;
	if (semiMajorAxis>0)
		period = StelUtils::calculateSiderealPeriod(semiMajorAxis);
	else
		period = 0;

	return period;
}

float MinorPlanet::getVMagnitude(const StelCore* core) const
{
	//If the H-G system is not used, use the default radius/albedo mechanism
	if (slopeParameter < 0)
	{
		return Planet::getVMagnitude(core);
	}

	//Calculate phase angle
	//(Code copied from Planet::getVMagnitude())
	//(this is actually vector subtraction + the cosine theorem :))
	const Vec3d& observerHelioPos = core->getObserverHeliocentricEclipticPos();
	const float observerRq = observerHelioPos.lengthSquared();
	const Vec3d& planetHelioPos = getHeliocentricEclipticPos();
	const float planetRq = planetHelioPos.lengthSquared();
	const float observerPlanetRq = (observerHelioPos - planetHelioPos).lengthSquared();
	const float cos_chi = (observerPlanetRq + planetRq - observerRq)/(2.0*std::sqrt(observerPlanetRq*planetRq));
	const float phaseAngle = std::acos(cos_chi);

	//Calculate reduced magnitude (magnitude without the influence of distance)
	//Source of the formulae: http://www.britastro.org/asteroids/dymock4.pdf
	const float tanPhaseAngleHalf=std::tan(phaseAngle*0.5f);
	const float phi1 = std::exp(-3.33f * std::pow(tanPhaseAngleHalf, 0.63f));
	const float phi2 = std::exp(-1.87f * std::pow(tanPhaseAngleHalf, 1.22f));
	float reducedMagnitude = absoluteMagnitude - 2.5f * std::log10( (1.0f - slopeParameter) * phi1 + slopeParameter * phi2 );

	//Calculate apparent magnitude
	float apparentMagnitude = reducedMagnitude + 5.0f * std::log10(std::sqrt(planetRq * observerPlanetRq));

	return apparentMagnitude;
}

void MinorPlanet::translateName(const StelTranslator &translator)
{
	nameI18 = translator.qtranslate(properName);
	if (englishName.endsWith('*'))
	{
		nameI18.append('*');
	}
}

QString MinorPlanet::renderProvisionalDesignationinHtml(QString plainTextName)
{
	QRegExp provisionalDesignationPattern("^(\\d{4}\\s[A-Z]{2})(\\d*)$");
	if (provisionalDesignationPattern.indexIn(plainTextName) == 0)
	{
		QString main = provisionalDesignationPattern.cap(1);
		QString suffix = provisionalDesignationPattern.cap(2);
		if (!suffix.isEmpty())
		{
			return (QString("%1<sub>%2</sub>").arg(main).arg(suffix));
		}
		else
		{
			return main;
		}
	}
	else
	{
		return QString();
	}
}

