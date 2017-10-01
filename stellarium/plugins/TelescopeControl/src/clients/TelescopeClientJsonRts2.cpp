/*
 * RTS2 JSON stellarium plugin
 * 
 * Copyright (C) 2014-2017 Petr Kubanek
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

#include "StelUtils.hpp"
#include "StelModuleMgr.hpp"
#include "StelTranslator.hpp"

#include "QByteArray"
#include "QJsonArray"
#include "QJsonDocument"
#include "QJsonParseError"
#include "QJsonObject"
#include "QUrlQuery"

#include "TelescopeClientJsonRts2.hpp"

TelescopeClientJsonRts2::TelescopeClientJsonRts2(const QString &name, const QString &params, Equinox eq)
	: TelescopeClient(name)
	, networkManager(new QNetworkAccessManager)
	, equinox(eq)
	, baseurl("http://localhost:8889/")
	, telName("")
	, telReadonly(false)
	, telLatitude(NAN)
	, telLongitude(NAN)
	, telAltitude(NAN)
	, telTargetDist(NAN)
	, time_delay(500)
{
	telescopeManager = GETSTELMODULE(TelescopeControl);

	// Example params:
	// petr:test@localhost:8889/tel

	qDebug() << "TelescopeRTS2(" << name << ") paramaters: " << params;

	baseurl.setUrl(params);
	if (!baseurl.isValid())
	{
		qWarning() << "TelescopeRTS2(" << name << ") invalid URL";
		return;
	}

	QUrl rurl(baseurl);

	rurl.setPath(baseurl.path() + "/api/devbytype");

	QUrlQuery query;
	query.addQueryItem("t", "2");
	rurl.setQuery(query);

	QNetworkRequest cfgRequest;

	cfgRequest.setUrl(rurl);

	qDebug() << "TelescopeRTS2(" << name << ")::TelescopeRTS2: request url:" << rurl.toString();

	connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

	networkManager.get(cfgRequest);
}

TelescopeClientJsonRts2::~TelescopeClientJsonRts2(void)
{
}

void TelescopeClientJsonRts2::replyFinished(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError)
	{
		if (reply->url().path().endsWith("/api/cmd") && reply->error() == QNetworkReply::ProtocolInvalidOperationError)
		{
			setReadOnly(true);
			return;
		}
		qDebug() << "TelescopeRTS2(" << name << ")::replyFinished: error " << reply->error() << " url: " << reply->url().toString();
		telName = "";
		return;
	}

	QByteArray data = reply->readAll();
	//qDebug() << "TelescopeRTS2(" << name << ")::replyFinished: data " << (QString) data;

	QJsonDocument doc;
	QJsonParseError jsonError;
	doc = QJsonDocument::fromJson(data, &jsonError);

	if (reply->url().path().endsWith("/api/devbytype"))
	{
		QJsonArray arr = doc.array();
		telName = arr[0].toString();

		QUrl diurl(baseurl);

		diurl.setPath(baseurl.path() + "/api/deviceinfo");

		QUrlQuery query;
		query.addQueryItem("d", telName);
		diurl.setQuery(query);

		request.setUrl(diurl);

		qDebug() << "TelescopeRTS2(" << name << ")::replyFinished: request url:" << diurl.toString();

		networkManager.get(request);
	}
	else if (reply->url().path().endsWith("/api/deviceinfo"))
	{
		QJsonObject docObject = doc.object();
		setReadOnly(docObject["readonly"].toBool());

		QUrl rurl(baseurl);

		rurl.setPath(baseurl.path() + "/api/get");

		QUrlQuery query;
		query.addQueryItem("d", telName);
		rurl.setQuery(query);

		request.setUrl(rurl);

		qDebug() << "TelescopeRTS2(" << name << ")::replyFinished: request url:" << rurl.toString();

		networkManager.get(request);
	}
	else if (reply->url().path().endsWith("/api/get"))
	{
		QJsonObject docObject = doc.object();
		QJsonObject dObject = docObject["d"].toObject();
		QJsonObject telObject = dObject["TEL"].toObject();

		telLongitude = dObject["LONGITUD"].toDouble() * M_PI / 180.0;
		telLatitude = dObject["LATITUDE"].toDouble() * M_PI / 180.0;
		telAltitude = dObject["ALTITUDE"].toDouble();
		telTargetDist = dObject["target_distance"].toDouble() * M_PI / 180.0;

		const double ra = telObject["ra"].toDouble() * M_PI / 180.0;
		const double dec = telObject["dec"].toDouble() * M_PI / 180.0;
		const double cdec = cos(dec);
	
		qDebug() << "TelescopeRTS2(" << name << ")::replyFinished: RADEC" << ra << dec;

		Vec3d pos(cos(ra)*cdec, sin(ra)*cdec, sin(dec));
		interpolatedPosition.add(pos, getNow(), getNow(), 0);

		qDebug() << "TelescopeRTS2(" << name << ")::replyFinished: request url:" << request.url().toString();
	
		networkManager.get(request);
	}
	else
	{
		qWarning() << "TelescopeRTS2(" << name << ")::replyFinished: unhandled reply: " << reply->url().toString();
	}
	reply->deleteLater();
}

bool TelescopeClientJsonRts2::isConnected(void) const
{
	return telName.isEmpty() == false;
}

Vec3d TelescopeClientJsonRts2::getJ2000EquatorialPos(const StelCore* core) const
{
	Q_UNUSED(core);
	const qint64 now = getNow() - time_delay;
	return interpolatedPosition.get(now);
}

void TelescopeClientJsonRts2::telescopeGoto(const Vec3d &j2000Pos, StelObjectP selectObject)
{
	if (!isConnected())
		return;

	QUrl set(baseurl);
	set.setPath(baseurl.path() + "/api/cmd");

	QUrlQuery query;
	query.addQueryItem("d", telName);

	bool commanded = false;

	// if it's satellite, use move_tle
	if (selectObject)
	{
		QVariantMap objectMap = selectObject->getInfoMap(StelApp::getInstance().getCore());
		if (objectMap.contains("tle1") && objectMap.contains("tle2"))
		{
			query.addQueryItem("c", QString("move_tle+%22") + objectMap["tle1"].toString() + QString("%22+%22") + objectMap["tle2"].toString() + QString("%22"));
			commanded = true;
		}
	}

	if (commanded == false)
	{
		double ra, dec;
		StelUtils::rectToSphe(&ra, &dec, j2000Pos);

		query.addQueryItem("c", QString("move+%1+%2").arg(ra * 180 / M_PI).arg(dec * 180 / M_PI));
	}
	set.setQuery(query);

	QNetworkRequest setR;
	setR.setUrl(set);

	qDebug() << "TelescopeRTS2(" << name << ")::telescopeGoto: request: " << set.toString();

	networkManager.get(setR);
}

bool TelescopeClientJsonRts2::hasKnownPosition(void) const
{
	return interpolatedPosition.isKnown();
}

QString TelescopeClientJsonRts2::getTelescopeInfoString(const StelCore* core, const InfoStringGroup& flags) const
{
	Q_UNUSED(core);
	Q_UNUSED(flags);
	QString str;
	QTextStream oss(&str);
	
	if (telReadonly)
	{
		oss << q_("Read-only telescope") << "<br /";
	}

	oss << q_("Telescope position: ") << QString("%1 %2 %3m").arg(StelUtils::radToDmsStr(telLongitude,true), StelUtils::radToDmsStr(telLatitude,true)).arg(telAltitude) << "<br />";

	oss << q_("Distance to target position: ") << StelUtils::radToDmsStr(telTargetDist,true) << "<br />";

	return str;
}

void TelescopeClientJsonRts2::setReadOnly(bool readonly)
{
	telReadonly = readonly;
	QSettings* settings = StelApp::getInstance().getSettings();
	Q_ASSERT(settings);

	if (telescopeManager)
	{
		if (telReadonly)
		{
			telescopeManager->setReticleColor(StelUtils::strToVec3f(settings->value("color_telescope_readonly", "1,0,0").toString()));
		}
		else
		{
			telescopeManager->setReticleColor(StelUtils::strToVec3f(settings->value("color_telescope_reticles", "0.6,0.4,0").toString()));
		}
	}
}
