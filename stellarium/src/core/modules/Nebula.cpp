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

#include "Nebula.hpp"
#include "NebulaMgr.hpp"
#include "StelTexture.hpp"

#include "StelUtils.hpp"
#include "StelApp.hpp"
#include "StelTextureMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelCore.hpp"
#include "StelPainter.hpp"

#include <QTextStream>
#include <QFile>
#include <QString>

#include <QDebug>
#include <QBuffer>

const QString Nebula::NEBULA_TYPE = QStringLiteral("Nebula");

StelTextureSP Nebula::texCircle;
StelTextureSP Nebula::texGalaxy;
StelTextureSP Nebula::texOpenCluster;
StelTextureSP Nebula::texGlobularCluster;
StelTextureSP Nebula::texPlanetaryNebula;
StelTextureSP Nebula::texDiffuseNebula;
StelTextureSP Nebula::texDarkNebula;
StelTextureSP Nebula::texOpenClusterWithNebulosity;
bool  Nebula::drawHintProportional = false;
bool  Nebula::surfaceBrightnessUsage = false;
bool  Nebula::designationUsage = false;
float Nebula::hintsBrightness = 0.f;
Vec3f Nebula::labelColor = Vec3f(0.4f,0.3f,0.5f);
Vec3f Nebula::circleColor = Vec3f(0.8f,0.8f,0.1f);
Vec3f Nebula::galaxyColor = Vec3f(1.0f,0.2f,0.2f);
Vec3f Nebula::radioGalaxyColor = Vec3f(0.3f,0.3f,0.3f);
Vec3f Nebula::activeGalaxyColor = Vec3f(0.8f,0.8f,0.1f);
Vec3f Nebula::interactingGalaxyColor = Vec3f(0.8f,0.8f,0.1f);
Vec3f Nebula::quasarColor = Vec3f(1.0f,0.2f,0.2f);
Vec3f Nebula::nebulaColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::planetaryNebulaColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::reflectionNebulaColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::bipolarNebulaColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::emissionNebulaColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::darkNebulaColor = Vec3f(0.3f,0.3f,0.3f);
Vec3f Nebula::hydrogenRegionColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::supernovaRemnantColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::interstellarMatterColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::clusterWithNebulosityColor = Vec3f(0.1f,1.0f,0.1f);
Vec3f Nebula::clusterColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::openClusterColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::globularClusterColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::stellarAssociationColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::starCloudColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::emissionObjectColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::blLacObjectColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::blazarColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::molecularCloudColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::youngStellarObjectColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::possibleQuasarColor = Vec3f(1.0f,0.2f,0.2f);
Vec3f Nebula::possiblePlanetaryNebulaColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::protoplanetaryNebulaColor = Vec3f(1.0f,1.0f,0.1f);
Vec3f Nebula::starColor = Vec3f(1.0f,1.0f,0.1f);
bool Nebula::flagUseTypeFilters = false;
Nebula::CatalogGroup Nebula::catalogFilters = Nebula::CatalogGroup(0);
Nebula::TypeGroup Nebula::typeFilters = Nebula::TypeGroup(Nebula::AllTypes);
bool Nebula::flagUseArcsecSurfaceBrightness = false;

Nebula::Nebula()
	: DSO_nb(0)
	, M_nb(0)
	, NGC_nb(0)
	, IC_nb(0)
	, C_nb(0)
	, B_nb(0)
	, Sh2_nb(0)
	, VdB_nb(0)
	, RCW_nb(0)
	, LDN_nb(0)
	, LBN_nb(0)
	, Cr_nb(0)
	, Mel_nb(0)
	, PGC_nb(0)
	, UGC_nb(0)
	, Arp_nb(0)
	, VV_nb(0)
	, Ced_nb("")
	, PK_nb("")
	, withoutID(false)
	, nameI18("")
	, mTypeString()
	, bMag(99.)
	, vMag(99.)
	, majorAxisSize(0.)
	, minorAxisSize(0.)
	, orientationAngle(0)
	, oDistance(0.)
	, oDistanceErr(0.)
	, redshift(99.)
	, redshiftErr(0.)
	, parallax(0.)
	, parallaxErr(0.)
	, nType()
{
}

Nebula::~Nebula()
{
}

QString Nebula::getInfoString(const StelCore *core, const InfoStringGroup& flags) const
{
	QString str;
	QTextStream oss(&str);
	double az_app, alt_app;
	StelUtils::rectToSphe(&az_app,&alt_app,getAltAzPosApparent(core));
	Q_UNUSED(az_app);

	if ((flags&Name) || (flags&CatalogNumber))
		oss << "<h2>";

	if (!nameI18.isEmpty() && flags&Name)
	{
		oss << getNameI18n();
		QString aliases = getI18nAliases();
		if (!aliases.isEmpty())
			oss << " (" << aliases << ")";
	}

	if (flags&CatalogNumber)
	{
		QStringList catIds;
		if (M_nb > 0)
			catIds << QString("M %1").arg(M_nb);
		if (C_nb > 0)
			catIds << QString("C %1").arg(C_nb);
		if (NGC_nb > 0)
			catIds << QString("NGC %1").arg(NGC_nb);
		if (IC_nb > 0)
			catIds << QString("IC %1").arg(IC_nb);		
		if (B_nb > 0)
			catIds << QString("B %1").arg(B_nb);
		if (Sh2_nb > 0)
			catIds << QString("SH 2-%1").arg(Sh2_nb);
		if (VdB_nb > 0)
			catIds << QString("VdB %1").arg(VdB_nb);
		if (RCW_nb > 0)
			catIds << QString("RCW %1").arg(RCW_nb);
		if (LDN_nb > 0)
			catIds << QString("LDN %1").arg(LDN_nb);
		if (LBN_nb > 0)
			catIds << QString("LBN %1").arg(LBN_nb);
		if (Cr_nb > 0)
			catIds << QString("Cr %1").arg(Cr_nb);
		if (Mel_nb > 0)
			catIds << QString("Mel %1").arg(Mel_nb);
		if (PGC_nb > 0)
			catIds << QString("PGC %1").arg(PGC_nb);
		if (UGC_nb > 0)
			catIds << QString("UGC %1").arg(UGC_nb);
		if (!Ced_nb.isEmpty())
			catIds << QString("Ced %1").arg(Ced_nb);
		if (Arp_nb > 0)
			catIds << QString("Arp %1").arg(Arp_nb);
		if (VV_nb > 0)
			catIds << QString("VV %1").arg(VV_nb);
		if (!PK_nb.isEmpty())
			catIds << QString("PK %1").arg(PK_nb);

		if (!nameI18.isEmpty() && !catIds.isEmpty() && flags&Name)
			oss << "<br>";

		oss << catIds.join(" - ");
	}

	if ((flags&Name) || (flags&CatalogNumber))
		oss << "</h2>";

	if (flags&ObjectType)
	{
		QString mt = getMorphologicalTypeString();
		if (mt.isEmpty())
			oss << q_("Type: <b>%1</b>").arg(getTypeString()) << "<br>";
		else
			oss << q_("Type: <b>%1</b> (%2)").arg(getTypeString()).arg(mt) << "<br>";
	}

	if (vMag < 50.f && flags&Magnitude)
	{
		if (nType == NebDn)
		{
			oss << q_("Opacity: <b>%1</b>").arg(getVMagnitude(core), 0, 'f', 2) << "<br>";
		}
		else
		{
			if (core->getSkyDrawer()->getFlagHasAtmosphere() && (alt_app>-3.0*M_PI/180.0)) // Don't show extincted magnitude much below horizon where model is meaningless.
				oss << q_("Magnitude: <b>%1</b> (after extinction: <b>%2</b>)").arg(QString::number(getVMagnitude(core), 'f', 2),
												    QString::number(getVMagnitudeWithExtinction(core), 'f', 2)) << "<br>";
			else
				oss << q_("Magnitude: <b>%1</b>").arg(getVMagnitude(core), 0, 'f', 2) << "<br>";
		}
	}
	if (bMag < 50.f && vMag > 50.f && flags&Magnitude)
	{
		oss << q_("Magnitude: <b>%1</b>").arg(bMag, 0, 'f', 2)
		    << q_(" (Photometric system: B)")
		    << "<br>";
	}
	if (flags&Extra)
	{
		if (vMag < 50 && bMag < 50)
			oss << q_("Color Index (B-V): <b>%1</b>").arg(QString::number(bMag-vMag, 'f', 2)) << "<br>";
	}
	float mmag = qMin(vMag,bMag);
	if (nType != NebDn && mmag < 50 && flags&Extra)
	{
		QString sb = q_("Surface brightness");
		QString ae = q_("after extinction");
		QString mu = QString("<sup>m</sup>/%1'").arg(QChar(0x2B1C));
		if (flagUseArcsecSurfaceBrightness)
			mu = QString("<sup>m</sup>/%1\"").arg(QChar(0x2B1C));

		if (core->getSkyDrawer()->getFlagHasAtmosphere() && (alt_app>-3.0*M_PI/180.0)) // Don't show extincted surface brightness much below horizon where model is meaningless.
		{
			if (getSurfaceBrightness(core)<99)
			{
				if (getSurfaceBrightnessWithExtinction(core)<99)
					oss << QString("%1: <b>%2</b> %5 (%3: <b>%4</b> %5)").arg(sb, QString::number(getSurfaceBrightness(core, flagUseArcsecSurfaceBrightness), 'f', 2),
												  ae, QString::number(getSurfaceBrightnessWithExtinction(core, flagUseArcsecSurfaceBrightness), 'f', 2), mu) << "<br>";
				else
					oss << QString("%1: <b>%2</b> %3").arg(sb, QString::number(getSurfaceBrightness(core, flagUseArcsecSurfaceBrightness), 'f', 2), mu) << "<br>";

				oss << q_("Contrast index: %1").arg(QString::number(getContrastIndex(core), 'f', 2)) << "<br />";
			}
		}
		else
		{
			if (getSurfaceBrightness(core)<99)
			{
				oss << QString("%1: <b>%2</b> %3").arg(sb, QString::number(getSurfaceBrightness(core, flagUseArcsecSurfaceBrightness), 'f', 2), mu) << "<br>";
				oss << q_("Contrast index: %1").arg(QString::number(getContrastIndex(core), 'f', 2)) << "<br />";
			}
		}
	}

	oss << getPositionInfoString(core, flags);

	if (majorAxisSize>0 && flags&Size)
	{
		if (majorAxisSize==minorAxisSize || minorAxisSize==0.f)
			oss << q_("Size: %1").arg(StelUtils::radToDmsStr(majorAxisSize*M_PI/180.)) << "<br>";
		else
		{
			oss << q_("Size: %1 x %2").arg(StelUtils::radToDmsStr(majorAxisSize*M_PI/180.)).arg(StelUtils::radToDmsStr(minorAxisSize*M_PI/180.)) << "<br>";
			if (orientationAngle>0.f)
				oss << q_("Orientation angle: %1%2").arg(orientationAngle).arg(QChar(0x00B0)) << "<br>";
		}
	}

	if (flags&Distance)
	{
		if (parallax!=0.f)
		{
			QString dx;
			// distance in light years from parallax
			float distance = 3.162e-5/(qAbs(parallax)*4.848e-9);
			float distanceErr = 0.f;

			if (parallaxErr>0.f)
				distanceErr = 3.162e-5/(qAbs(parallaxErr)*4.848e-9);

			if (distanceErr>0.f)
				dx = QString("%1%2%3").arg(QString::number(distance, 'f', 3)).arg(QChar(0x00B1)).arg(QString::number(distanceErr, 'f', 3));
			else
				dx = QString("%1").arg(QString::number(distance, 'f', 3));

			if (oDistance==0.f)
			{
				//TRANSLATORS: Unit of measure for distance - Light Years
				oss << q_("Distance: %1 ly").arg(dx) << "<br>";
			}
		}

		if (oDistance>0.f)
		{
			QString dx, dy;
			float dc = 3262.f;
			int ms = 1;
			//TRANSLATORS: Unit of measure for distance - kiloparsecs
			QString dupc = q_("kpc");
			//TRANSLATORS: Unit of measure for distance - Light Years
			QString duly = q_("ly");

			if (nType==NebAGx || nType==NebGx || nType==NebRGx || nType==NebIGx || nType==NebQSO || nType==NebPossQSO)
			{
				dc = 3.262f;
				ms = 3;
				//TRANSLATORS: Unit of measure for distance - Megaparsecs
				dupc = q_("Mpc");
				//TRANSLATORS: Unit of measure for distance - Millions of Light Years
				duly = q_("Mio. ly");
			}

			if (oDistanceErr>0.f)
			{
				dx = QString("%1%2%3").arg(QString::number(oDistance, 'f', 3)).arg(QChar(0x00B1)).arg(QString::number(oDistanceErr, 'f', 3));
				dy = QString("%1%2%3").arg(QString::number(oDistance*dc, 'f', ms)).arg(QChar(0x00B1)).arg(QString::number(oDistanceErr*dc, 'f', ms));
			}
			else
			{
				dx = QString("%1").arg(QString::number(oDistance, 'f', 3));
				dy = QString("%1").arg(QString::number(oDistance*dc, 'f', ms));
			}

			oss << q_("Distance: %1 %2 (%3 %4)").arg(dx).arg(dupc).arg(dy).arg(duly) << "<br>";
		}
	}

	if (flags&Extra)
	{
		if (redshift<99.f)
		{
			QString z;
			if (redshiftErr>0.f)
				z = QString("%1%2%3").arg(QString::number(redshift, 'f', 6)).arg(QChar(0x00B1)).arg(QString::number(redshiftErr, 'f', 6));
			else
				z = QString("%1").arg(QString::number(redshift, 'f', 6));

			oss << q_("Redshift: %1").arg(z) << "<br>";
		}
		if (parallax!=0.f)
		{
			QString px;

			if (parallaxErr>0.f)
				px = QString("%1%2%3").arg(QString::number(qAbs(parallax)*0.001, 'f', 5)).arg(QChar(0x00B1)).arg(QString::number(parallaxErr*0.001, 'f', 5));
			else
				px = QString("%1").arg(QString::number(qAbs(parallax)*0.001, 'f', 5));

			oss << q_("Parallax: %1\"").arg(px) << "<br>";
		}

		if (!getMorphologicalTypeDescription().isEmpty())
			oss << q_("Morphological description: ") << getMorphologicalTypeDescription() << ".<br>";

	}

	postProcessInfoString(str, flags);

	return str;
}

QVariantMap Nebula::getInfoMap(const StelCore *core) const
{
	QVariantMap map = StelObject::getInfoMap(core);

	map["type"]=getTypeString(); // replace "Nebula" type by detail. This is localized. Maybe add argument such as getTypeString(bool translated=true)?
	map.insert("morpho", getMorphologicalTypeString());
	map.insert("surface-brightness", getSurfaceBrightness(core));
	map.insert("bmag", bMag);
	if (vMag < 50 && bMag < 50)
		map.insert("bV", bMag-vMag);
	if (redshift<99.f)
		map.insert("redshift", redshift);

	// TODO: more? Names? Data?
	return map;
}

QString Nebula::getEnglishAliases() const
{
	QString aliases = "";
	if (englishAliases.size()!=0)
		aliases = englishAliases.join(" - ");
	return aliases;
}

QString Nebula::getI18nAliases() const
{
	QString aliases = "";
	if (nameI18Aliases.size()!=0)
		aliases = nameI18Aliases.join(" - ");
	return aliases;
}

float Nebula::getVMagnitude(const StelCore* core) const
{
	Q_UNUSED(core);
	return vMag;
}

double Nebula::getAngularSize(const StelCore *) const
{
	float size = majorAxisSize;
	if (majorAxisSize!=minorAxisSize || minorAxisSize>0)
		size = (majorAxisSize+minorAxisSize)/2.f;
	return size*0.5f;
}

float Nebula::getSelectPriority(const StelCore* core) const
{
	const NebulaMgr* nebMgr = ((NebulaMgr*)StelApp::getInstance().getModuleMgr().getModule("NebulaMgr"));
	// minimize unwanted selection of the deep-sky objects
	if (!nebMgr->getFlagHints())
		return StelObject::getSelectPriority(core)+3.f;

	if (!objectInDisplayedType())
		return StelObject::getSelectPriority(core)+3.f;
	
	const float maxMagHint = nebMgr->computeMaxMagHint(core->getSkyDrawer());
	// make very easy to select if labeled
	float lim, mag;
	lim = mag = getVMagnitude(core);
	if (surfaceBrightnessUsage)
		lim = mag = getSurfaceBrightness(core);

	if (nType==NebDn)
		lim=15.0f - mag - 2.0f*qMin(1.5f, majorAxisSize); // Note that "mag" field is used for opacity in this catalog!
	else if (nType==NebHII) // Sharpless and LBN
		lim=10.0f - 2.0f*qMin(1.5f, majorAxisSize); // Unfortunately, in Sh catalog, we always have mag=99=unknown!

	if (std::min(15.f, lim)<maxMagHint)
		return -10.f;
	else
		return StelObject::getSelectPriority(core)-2.f;

}

Vec3f Nebula::getInfoColor(void) const
{
	return ((NebulaMgr*)StelApp::getInstance().getModuleMgr().getModule("NebulaMgr"))->getLabelsColor();
}

double Nebula::getCloseViewFov(const StelCore*) const
{
	return majorAxisSize>0 ? majorAxisSize * 4 : 1;
}

float Nebula::getSurfaceBrightness(const StelCore* core, bool arcsec) const
{
	float mag = getVMagnitude(core);
	float sq = 3600.f; // arcmin^2
	if (arcsec)
		sq = 12.96e6; // 3600.f*3600.f, i.e. arcsec^2
	if (bMag < 50.f && mag > 50.f)
		mag = bMag;
	if (mag<99.f && majorAxisSize>0 && nType!=NebDn)
		return mag + 2.5*log10(getSurfaceArea()*sq);
	else
		return 99.f;
}

float Nebula::getSurfaceBrightnessWithExtinction(const StelCore* core, bool arcsec) const
{
	float sq = 3600.f; // arcmin^2
	if (arcsec)
		sq = 12.96e6; // 3600.f*3600.f, i.e. arcsec^2
	if (getVMagnitudeWithExtinction(core)<99.f && majorAxisSize>0 && nType!=NebDn)
		return getVMagnitudeWithExtinction(core) + 2.5*log10(getSurfaceArea()*sq);
	else
		return 99.f;
}

float Nebula::getContrastIndex(const StelCore* core) const
{
	// Compute an extended object's contrast index: http://www.unihedron.com/projects/darksky/NELM2BCalc.html

	// Sky brightness
	// Source: Schaefer, B.E. Feb. 1990. Telescopic Limiting Magnitude. PASP 102:212-229
	// URL: http://adsbit.harvard.edu/cgi-bin/nph-iarticle_query?bibcode=1990PASP..102..212S [1990PASP..102..212S]
	float B_mpsas = 21.58 - 5*log10(std::pow(10, 1.586 - core->getSkyDrawer()->getNELMFromBortleScale()/5)-1);
	// Compute an extended object's contrast index
	// Source: Clark, R.N., 1990. Appendix E in Visual Astronomy of the Deep Sky, Cambridge University Press and Sky Publishing.
	// URL: http://www.clarkvision.com/visastro/appendix-e.html
	return -0.4 * (getSurfaceBrightnessWithExtinction(core, true) - B_mpsas);
}

float Nebula::getSurfaceArea(void) const
{
	if (majorAxisSize==minorAxisSize || minorAxisSize==0)
		return M_PI*(majorAxisSize/2.f)*(majorAxisSize/2.f); // S = pi*R^2 = pi*(D/2)^2
	else
		return M_PI*(majorAxisSize/2.f)*(minorAxisSize/2.f); // S = pi*a*b
}

void Nebula::drawHints(StelPainter& sPainter, float maxMagHints) const
{
	StelCore* core = StelApp::getInstance().getCore();

	Vec3d win;
	// Check visibility of DSO hints
	if (!(sPainter.getProjector()->projectCheck(XYZ, win)))
		return;

	float lim = qMin(vMag, bMag);

	if (surfaceBrightnessUsage)
	{
		lim = getSurfaceBrightness(core) - 3.f;
		if (lim > 50) lim = 16.f;
	}
	else
	{
		float mag = getVMagnitude(core);
		if (lim > 50) lim = 15.f;

		// Dark nebulae. Not sure how to assess visibility from opacity? --GZ
		if (nType==NebDn)
		{
			// GZ: ad-hoc visibility formula: assuming good visibility if objects of mag9 are visible, "usual" opacity 5 and size 30', better visibility (discernability) comes with higher opacity and larger size,
			// 9-(opac-5)-2*(angularSize-0.5)
			// GZ Not good for non-Barnards. weak opacity and large surface are antagonists. (some LDN are huge, but opacity 2 is not much to discern).
			// The qMin() maximized the visibility gain for large objects.
			if (majorAxisSize>0 && mag<50)
				lim = 15.0f - mag - 2.0f*qMin(majorAxisSize, 1.5f);
			else if (B_nb>0)
				lim = 9.0f;
			else
				lim= 12.0f; // GZ I assume LDN objects are rather elusive.
		}
		else if (nType==NebHII) // NebHII={Sharpless, LBN, RCW}
		{ // artificially increase visibility of (most) Sharpless objects? No magnitude recorded:-(
			lim=9.0f;
		}
	}

	if (lim>maxMagHints)
		return;

	sPainter.setBlending(true, GL_ONE, GL_ONE);
	float lum = 1.f;//qMin(1,4.f/getOnScreenSize(core))*0.8;

	Vec3f color=circleColor;
	switch (nType)
	{
		case NebGx:
			Nebula::texGalaxy->bind();
			color=galaxyColor;
			break;
		case NebIGx:
			Nebula::texGalaxy->bind();
			color=interactingGalaxyColor;
			break;
		case NebAGx:
			Nebula::texGalaxy->bind();
			color=activeGalaxyColor;
			break;
		case NebQSO:
			Nebula::texGalaxy->bind();
			color=quasarColor;
			break;
		case NebPossQSO:
			Nebula::texGalaxy->bind();
			color=possibleQuasarColor;
			break;
		case NebBLL:
			Nebula::texGalaxy->bind();
			color=blLacObjectColor;
			break;
		case NebBLA:
			Nebula::texGalaxy->bind();
			color=blazarColor;
			break;
		case NebRGx:
			Nebula::texGalaxy->bind();
			color=radioGalaxyColor;
			break;
		case NebOc:
			Nebula::texOpenCluster->bind();
			color=openClusterColor;
			break;
		case NebSA:
			Nebula::texOpenCluster->bind();
			color=stellarAssociationColor;
			break;
		case NebSC:
			Nebula::texOpenCluster->bind();
			color=starCloudColor;
			break;
		case NebCl:
			Nebula::texOpenCluster->bind();
			color=clusterColor;
			break;
		case NebGc:
			Nebula::texGlobularCluster->bind();
			color=globularClusterColor;
			break;
		case NebN:
			Nebula::texDiffuseNebula->bind();
			color=nebulaColor;
			break;
		case NebHII:
			Nebula::texDiffuseNebula->bind();
			color=hydrogenRegionColor;
			break;
		case NebMolCld:
			Nebula::texDiffuseNebula->bind();
			color=molecularCloudColor;
			break;
		case NebYSO:
			Nebula::texDiffuseNebula->bind();
			color=youngStellarObjectColor;
			break;
		case NebRn:		
			Nebula::texDiffuseNebula->bind();
			color=reflectionNebulaColor;
			break;
		case NebSNR:
			Nebula::texDiffuseNebula->bind();
			color=supernovaRemnantColor;
			break;
		case NebBn:
			Nebula::texDiffuseNebula->bind();
			color=bipolarNebulaColor;
			break;
		case NebEn:
			Nebula::texDiffuseNebula->bind();
			color=emissionNebulaColor;
			break;
		case NebPn:
			Nebula::texPlanetaryNebula->bind();
			color=planetaryNebulaColor;
			break;
		case NebPossPN:
			Nebula::texPlanetaryNebula->bind();
			color=possiblePlanetaryNebulaColor;
			break;
		case NebPPN:
			Nebula::texPlanetaryNebula->bind();
			color=protoplanetaryNebulaColor;
			break;
		case NebDn:		
			Nebula::texDarkNebula->bind();
			color=darkNebulaColor;
			break;
		case NebCn:
			Nebula::texOpenClusterWithNebulosity->bind();
			color=clusterWithNebulosityColor;
			break;
		case NebEMO:
			Nebula::texCircle->bind();
			color=emissionObjectColor;
			break;
		default:
			Nebula::texCircle->bind();
	}

	Vec3f col(color[0]*lum*hintsBrightness, color[1]*lum*hintsBrightness, color[2]*lum*hintsBrightness);
	if (!objectInDisplayedType())
		col = Vec3f(0.f,0.f,0.f);
	sPainter.setColor(col[0], col[1], col[2], 1);

	float size = 6.0f;
	float scaledSize = 0.0f;
	if (drawHintProportional)
	{
		if (majorAxisSize>0.)
			scaledSize = majorAxisSize *0.5 *M_PI/180.*sPainter.getProjector()->getPixelPerRadAtCenter();
		else
			scaledSize = minorAxisSize *0.5 *M_PI/180.*sPainter.getProjector()->getPixelPerRadAtCenter();
	}

	// Rotation looks good only for galaxies.
	if ((nType <=NebQSO) || (nType==NebBLA) || (nType==NebBLL) )
	{
		// The rotation angle in drawSprite2dMode() is relative to screen. Make sure to compute correct angle from 90+orientationAngle.
		// Find an on-screen direction vector from a point offset somewhat in declination from our object.
		Vec3d XYZrel(XYZ);
		XYZrel[2]*=0.99;
		Vec3d XYrel;
		sPainter.getProjector()->project(XYZrel, XYrel);
		float screenAngle=atan2(XYrel[1]-XY[1], XYrel[0]-XY[0]);
		sPainter.drawSprite2dMode(XY[0], XY[1], qMax(size, scaledSize), screenAngle*180./M_PI + orientationAngle);
	}
	else	// no galaxy
		sPainter.drawSprite2dMode(XY[0], XY[1], qMax(size, scaledSize));

}

void Nebula::drawLabel(StelPainter& sPainter, float maxMagLabel) const
{
	StelCore* core = StelApp::getInstance().getCore();

	Vec3d win;
	// Check visibility of DSO labels
	if (!(sPainter.getProjector()->projectCheck(XYZ, win)))
		return;

	float lim = qMin(vMag, bMag);

	if (surfaceBrightnessUsage)
	{
		lim = getSurfaceBrightness(core) - 3.f;
		if (lim > 50) lim = 16.f;
	}
	else
	{
		float mag = getVMagnitude(core);

		if (lim > 50) lim = 15.f;

		// Dark nebulae. Not sure how to assess visibility from opacity? --GZ
		if (nType==NebDn)
		{
			// GZ: ad-hoc visibility formula: assuming good visibility if objects of mag9 are visible, "usual" opacity 5 and size 30', better visibility (discernability) comes with higher opacity and larger size,
			// 9-(opac-5)-2*(angularSize-0.5)
			if (majorAxisSize>0 && mag<50)
				lim = 15.0f - mag - 2.0f*qMin(majorAxisSize, 2.5f);
			else if (B_nb>0)
				lim = 9.0f;
			else
				lim= 12.0f; // GZ I assume some LDN objects are rather elusive.
		}
		else if (nType==NebHII)
			lim=9.0f;
	}

	if (lim>maxMagLabel)
		return;

	Vec3f col(labelColor[0], labelColor[1], labelColor[2]);
	if (objectInDisplayedType())
		sPainter.setColor(col[0], col[1], col[2], hintsBrightness);
	else
		sPainter.setColor(col[0], col[1], col[2], 0.f);

	float size = getAngularSize(Q_NULLPTR)*M_PI/180.*sPainter.getProjector()->getPixelPerRadAtCenter();
	float shift = 4.f + (drawHintProportional ? size : size/1.8f);

	QString str = getNameI18n();
	if (str.isEmpty() || designationUsage)
		str = getDSODesignation();

	sPainter.drawText(XY[0]+shift, XY[1]+shift, str, 0, 0, 0, false);
}

QString Nebula::getDSODesignation() const
{
	QString str = "";
	// Get designation for DSO with priority as given here.
	if (catalogFilters&CatM && M_nb>0)
		str = QString("M %1").arg(M_nb);
	else if (catalogFilters&CatC && C_nb>0)
		str = QString("C %1").arg(C_nb);
	else if (catalogFilters&CatNGC && NGC_nb>0)
		str = QString("NGC %1").arg(NGC_nb);
	else if (catalogFilters&CatIC && IC_nb>0)
		str = QString("IC %1").arg(IC_nb);
	else if (catalogFilters&CatB && B_nb>0)
		str = QString("B %1").arg(B_nb);
	else if (catalogFilters&CatSh2 && Sh2_nb>0)
		str = QString("SH 2-%1").arg(Sh2_nb);
	else if (catalogFilters&CatVdB && VdB_nb>0)
		str = QString("VdB %1").arg(VdB_nb);
	else if (catalogFilters&CatRCW && RCW_nb>0)
		str = QString("RCW %1").arg(RCW_nb);
	else if (catalogFilters&CatLDN && LDN_nb>0)
		str = QString("LDN %1").arg(LDN_nb);
	else if (catalogFilters&CatLBN && LBN_nb > 0)
		str = QString("LBN %1").arg(LBN_nb);
	else if (catalogFilters&CatCr && Cr_nb > 0)
		str = QString("Cr %1").arg(Cr_nb);
	else if (catalogFilters&CatMel && Mel_nb > 0)
		str = QString("Mel %1").arg(Mel_nb);
	else if (catalogFilters&CatPGC && PGC_nb > 0)
		str = QString("PGC %1").arg(PGC_nb);
	else if (catalogFilters&CatUGC && UGC_nb > 0)
		str = QString("UGC %1").arg(UGC_nb);
	else if (catalogFilters&CatCed && !Ced_nb.isEmpty())
		str = QString("Ced %1").arg(Ced_nb);
	else if (catalogFilters&CatArp && Arp_nb > 0)
		str = QString("Arp %1").arg(Arp_nb);
	else if (catalogFilters&CatVV && VV_nb > 0)
		str = QString("VV %1").arg(VV_nb);
	else if (catalogFilters&CatPK && !PK_nb.isEmpty())
		str = QString("PK %1").arg(PK_nb);

	return str;
}

void Nebula::readDSO(QDataStream &in)
{
	float	ra, dec;
	unsigned int oType;

	in	>> DSO_nb >> ra >> dec >> bMag >> vMag >> oType >> mTypeString >> majorAxisSize >> minorAxisSize
		>> orientationAngle >> redshift >> redshiftErr >> parallax >> parallaxErr >> oDistance >> oDistanceErr
		>> NGC_nb >> IC_nb >> M_nb >> C_nb >> B_nb >> Sh2_nb >> VdB_nb >> RCW_nb >> LDN_nb >> LBN_nb >> Cr_nb
		>> Mel_nb >> PGC_nb >> UGC_nb >> Ced_nb >> Arp_nb >> VV_nb >> PK_nb;

	int f = NGC_nb + IC_nb + M_nb + C_nb + B_nb + Sh2_nb + VdB_nb + RCW_nb + LDN_nb + LBN_nb + Cr_nb + Mel_nb + PGC_nb + UGC_nb + Arp_nb + VV_nb;
	if (f==0 && Ced_nb.isEmpty() && PK_nb.isEmpty())
		withoutID = true;

	StelUtils::spheToRect(ra,dec,XYZ);
	Q_ASSERT(fabs(XYZ.lengthSquared()-1.)<0.000000001);
	nType = (Nebula::NebulaType)oType;
	pointRegion = SphericalRegionP(new SphericalPoint(getJ2000EquatorialPos(Q_NULLPTR)));
}

bool Nebula::objectInDisplayedType() const
{
	if (!flagUseTypeFilters)
		return true;

	bool r = false;
	int cntype = -1;
	switch (nType)
	{
		case NebGx:
			cntype = 0; // Galaxies
			break;
		case NebAGx:
		case NebRGx:
		case NebQSO:
		case NebPossQSO:
		case NebBLL:
		case NebBLA:
			cntype = 1; // Active Galaxies
			break;
		case NebIGx:
			cntype = 2; // Interacting Galaxies
			break;
		case NebOc:
		case NebGc:
		case NebCl:
		case NebSA:
		case NebSC:
			cntype = 3; // Star Clusters
			break;
		case NebHII:
		case NebISM:
			cntype = 4; // Hydrogen regions (include interstellar matter)
			break;
		case NebN:
		case NebBn:
		case NebEn:
		case NebRn:
			cntype = 5; // Bright Nebulae
			break;
		case NebDn:
		case NebMolCld:
		case NebYSO:
			cntype = 6; // Dark Nebulae
			break;
		case NebPn:
		case NebPossPN:
		case NebPPN:
			cntype = 7; // Planetary Nebulae
			break;
		case NebSNR:
			cntype = 8; // Supernova Remnants
			break;
		case NebCn:
			cntype = 9;
			break;
		default:
			cntype = 10;
			break;
	}
	if (typeFilters&TypeGalaxies && cntype==0)
		r = true;
	else if (typeFilters&TypeActiveGalaxies && cntype==1)
		r = true;
	else if (typeFilters&TypeInteractingGalaxies && cntype==2)
		r = true;
	else if (typeFilters&TypeStarClusters && cntype==3)
		r = true;
	else if (typeFilters&TypeHydrogenRegions && cntype==4)
		r = true;
	else if (typeFilters&TypeBrightNebulae && cntype==5)
		r = true;
	else if (typeFilters&TypeDarkNebulae && cntype==6)
		r = true;
	else if (typeFilters&TypePlanetaryNebulae && cntype==7)
		r = true;
	else if (typeFilters&TypeSupernovaRemnants && cntype==8)
		r = true;
	else if (typeFilters&TypeStarClusters && (typeFilters&TypeBrightNebulae || typeFilters&TypeHydrogenRegions) && cntype==9)
		r = true;
	else if (typeFilters&TypeOther && cntype==10)
		r = true;

	return r;
}

QString Nebula::getMorphologicalTypeString(void) const
{
	return mTypeString;
}

QString Nebula::getMorphologicalTypeDescription(void) const
{
	QString m, r = "";

	// Let's avoid showing a wrong morphological description for galaxies
	// NOTE: Is required the morphological description for galaxies?
	if (nType==NebGx || nType==NebAGx || nType==NebRGx || nType==NebIGx || nType==NebQSO || nType==NebPossQSO || nType==NebBLA || nType==NebBLL)
		return QString();

	QRegExp GlClRx("\\.*(I|II|III|IV|V|VI|VI|VII|VIII|IX|X|XI|XII)\\.*");
	int idx = GlClRx.indexIn(mTypeString);
	if (idx>0)
		m = mTypeString.mid(idx);
	else
		m = mTypeString;

	QStringList glclass;
	glclass << "I" << "II" << "III" << "IV" << "V" << "VI" << "VII" << "VIII" << "IX" << "X" << "XI" << "XII";

	if (GlClRx.exactMatch(m)) // Globular Clusters
	{
		switch(glclass.indexOf(GlClRx.capturedTexts().at(1).trimmed()))
		{
			case 0:
				r = qc_("high concentration of stars toward the center", "Shapley–Sawyer Concentration Class");
				break;
			case 1:
				r = qc_("dense central concentration of stars", "Shapley–Sawyer Concentration Class");
				break;
			case 2:
				r = qc_("strong inner core of stars", "Shapley–Sawyer Concentration Class");
				break;
			case 3:
				r = qc_("intermediate rich concentrations of stars", "Shapley–Sawyer Concentration Class");
				break;
			case 4:
			case 5:
			case 6:
				r = qc_("intermediate concentrations of stars", "Shapley–Sawyer Concentration Class");
				break;
			case 7:
				r = qc_("rather loosely concentration of stars towards the center", "Shapley–Sawyer Concentration Class");
				break;
			case 8:
				r = qc_("loose concentration of stars towards the center", "Shapley–Sawyer Concentration Class");
				break;
			case 9:
				r = qc_("loose concentration of stars", "Shapley–Sawyer Concentration Class");
				break;
			case 10:
				r = qc_("very loose concentration of stars towards the center", "Shapley–Sawyer Concentration Class");
				break;
			case 11:
				r = qc_("almost no concentration towards the center", "Shapley–Sawyer Concentration Class");
				break;
			default:
				r = qc_("undocumented concentration class", "Shapley–Sawyer Concentration Class");
				break;
		}
	}

	QRegExp OClRx("\\.*(I|II|III|IV)(\\d)(p|m|r)(n*)\\.*");
	idx = OClRx.indexIn(mTypeString);
	if (idx>0)
		m = mTypeString.mid(idx);
	else
		m = mTypeString;

	if (OClRx.exactMatch(m)) // Open Clusters
	{
		QStringList occlass, ocrich, rtxt;
		occlass << "I" << "II" << "III" << "IV";
		ocrich << "p" << "m" << "r";
		switch(occlass.indexOf(OClRx.capturedTexts().at(1).trimmed()))
		{
			case 0:
				rtxt << qc_("strong central concentration of stars", "Trumpler's Concentration Class");
				break;
			case 1:
				rtxt << qc_("little central concentration of stars", "Trumpler's Concentration Class");
				break;
			case 2:
				rtxt << qc_("no noticeable concentration of stars", "Trumpler's Concentration Class");
				break;
			case 3:
				rtxt << qc_("a star field condensation", "Trumpler's Concentration Class");
				break;
			default:
				rtxt << qc_("undocumented concentration class", "Trumpler's Concentration Class");
				break;
		}
		switch(OClRx.capturedTexts().at(2).toInt())
		{
			case 1:
				rtxt << qc_("small brightness range of cluster members", "Trumpler's Brightness Class");
				break;
			case 2:
				rtxt << qc_("medium brightness range of cluster members", "Trumpler's Brightness Class");
				break;
			case 3:
				rtxt << qc_("large brightness range of cluster members", "Trumpler's Brightness Class");
				break;
			default:
				rtxt << qc_("undocumented brightness range of cluster members", "Trumpler's Brightness Class");
				break;
		}
		switch(ocrich.indexOf(OClRx.capturedTexts().at(3).trimmed()))
		{
			case 0:
				rtxt << qc_("poor cluster with less than 50 stars", "Trumpler's Number of Members Class");
				break;
			case 1:
				rtxt << qc_("moderately rich cluster with 50-100 stars", "Trumpler's Number of Members Class");
				break;
			case 2:
				rtxt << qc_("rich cluster with more than 100 stars", "Trumpler's Number of Members Class");
				break;
			default:
				rtxt << qc_("undocumented number of members class", "Trumpler's Number of Members Class");
				break;
		}
		if (!OClRx.capturedTexts().at(4).trimmed().isEmpty())
			rtxt << qc_("the cluster lies within nebulosity", "nebulosity factor of open clusters");

		r = rtxt.join(",<br>");
	}

	QRegExp VdBRx("\\.*(I|II|I-II|II P|P),\\s+(VBR|VB|BR|M|F|VF|:)\\.*");
	idx = VdBRx.indexIn(mTypeString);
	if (idx>0)
		m = mTypeString.mid(idx);
	else
		m = mTypeString;

	if (VdBRx.exactMatch(m)) // Reflection Nebulae
	{
		QStringList rnclass, rnbrightness, rtx;
		rnclass << "I" << "II" << "I-II" << "II P" << "P";
		rnbrightness << "VBR" << "VB" << "BR" << "M" << "F" << "VF" << ":";
		switch(rnbrightness.indexOf(VdBRx.capturedTexts().at(2).trimmed()))
		{
			case 0:
			case 1:
				rtx << qc_("very bright", "Reflection Nebulae Brightness");
				break;
			case 2:
				rtx << qc_("bright", "Reflection Nebulae Brightness");
				break;
			case 3:
				rtx << qc_("moderate brightness", "Reflection Nebulae Brightness");
				break;
			case 4:
				rtx << qc_("faint", "Reflection Nebulae Brightness");
				break;
			case 5:
				rtx << qc_("very faint", "Reflection Nebulae Brightness");
				break;
			case 6:
				rtx << qc_("uncertain brightness", "Reflection Nebulae Brightness");
				break;
			default:
				rtx << qc_("undocumented brightness of reflection nebulae", "Reflection Nebulae Brightness");
				break;
		}
		switch(rnclass.indexOf(VdBRx.capturedTexts().at(1).trimmed()))
		{
			case 0:
				rtx << qc_("the illuminating star is embedded in the nebulosity", "Reflection Nebulae Classification");
				break;
			case 1:
				rtx << qc_("star is located outside the illuminated nebulosity", "Reflection Nebulae Classification");
				break;
			case 2:
				rtx << qc_("star is located on the corner of the illuminated nebulosity", "Reflection Nebulae Classification");
				break;
			case 3:
			{
				// TRANSLATORS: peculiar: odd or unusual, cf. peculiar star, peculiar galaxy
				rtx << qc_("star is located outside the illuminated peculiar nebulosity", "Reflection Nebulae Classification");
				break;
			}
			case 4:
			{
				// TRANSLATORS: peculiar: odd or unusual, cf. peculiar star, peculiar galaxy
				rtx << qc_("the illuminated peculiar nebulosity", "Reflection Nebulae Classification");
				break;
			}
			default:
				rtx << qc_("undocumented reflection nebulae", "Reflection Nebulae Classification");
				break;
		}
		r = rtx.join(",<br>");
	}


	QRegExp HIIRx("\\.*(\\d+),\\s+(\\d+),\\s+(\\d+)\\.*");
	idx = HIIRx.indexIn(mTypeString);
	if (idx>0)
		m = mTypeString.mid(idx);
	else
		m = mTypeString;

	if (HIIRx.exactMatch(m)) // HII regions
	{
		int form	= HIIRx.capturedTexts().at(1).toInt();
		int structure	= HIIRx.capturedTexts().at(2).toInt();
		int brightness	= HIIRx.capturedTexts().at(3).toInt();
		QStringList morph;
		switch(form)
		{
			case 1:
				morph << q_("circular form");
				break;
			case 2:
				morph << q_("elliptical form");
				break;
			case 3:
				morph << q_("irregular form");
				break;
			default:
				morph << q_("undocumented form");
				break;
		}
		switch(structure)
		{
			case 1:
				morph << q_("amorphous structure");
				break;
			case 2:
				morph << q_("conventional structure");
				break;
			case 3:
				morph << q_("filamentary structure");
				break;
			default:
				morph << q_("undocumented structure");
				break;
		}
		switch(brightness)
		{
			case 1:
				morph << qc_("faintest", "HII region brightness");
				break;
			case 2:
				morph << qc_("moderate brightness", "HII region brightness");
				break;
			case 3:
				morph << qc_("brightest", "HII region brightness");
				break;
			default:
				morph << q_("undocumented brightness");
				break;
		}
		r = morph.join(",<br>");
	}

	return r;
}

QString Nebula::getTypeString(void) const
{
	QString wsType;

	switch(nType)
	{
		case NebGx:
			wsType = q_("galaxy");
			break;
		case NebAGx:
			wsType = q_("active galaxy");
			break;
		case NebRGx:
			wsType = q_("radio galaxy");
			break;
		case NebIGx:
			wsType = q_("interacting galaxy");
			break;
		case NebQSO:
			wsType = q_("quasar");
			break;
		case NebCl:
			wsType = q_("star cluster");
			break;
		case NebOc:
			wsType = q_("open star cluster");
			break;
		case NebGc:
			wsType = q_("globular star cluster");
			break;
		case NebN:
			wsType = q_("nebula");
			break;
		case NebPn:
			wsType = q_("planetary nebula");
			break;
		case NebDn:
			wsType = q_("dark nebula");
			break;
		case NebCn:
			wsType = q_("cluster associated with nebulosity");
			break;
		case NebBn:
			wsType = q_("bipolar nebula");
			break;
		case NebEn:
			wsType = q_("emission nebula");
			break;
		case NebHII:
			wsType = q_("HII region");
			break;
		case NebRn:
			wsType = q_("reflection nebula");
			break;		
		case NebSNR:
			wsType = q_("supernova remnant");
			break;
		case NebSA:
			wsType = q_("stellar association");
			break;
		case NebSC:
			wsType = q_("star cloud");
			break;
		case NebISM:
			wsType = q_("interstellar matter");
			break;
		case NebEMO:
			wsType = q_("emission object");
			break;
		case NebBLL:
			wsType = q_("BL Lac object");
			break;
		case NebBLA:
			wsType = q_("blazar");
			break;
		case NebMolCld:
			wsType = q_("molecular cloud");
			break;
		case NebYSO:
			wsType = q_("young stellar object");
			break;
		case NebPossQSO:
			wsType = q_("possible quasar");
			break;
		case NebPossPN:
			wsType = q_("possible planetary nebula");
			break;
		case NebPPN:
			wsType = q_("protoplanetary nebula");
			break;
		case NebStar:
			wsType = q_("star");
			break;
		case NebUnknown:
			wsType = q_("object of unknown nature");
			break;
		default:
			wsType = q_("undocumented type");
			break;
	}
	return wsType;
}

