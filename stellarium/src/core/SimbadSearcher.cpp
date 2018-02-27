/*
 * Copyright (C) 2008 Fabien Chereau
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

#include "SimbadSearcher.hpp"

#include "StelUtils.hpp"
#include "StelTranslator.hpp"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QTimer>

SimbadLookupReply::SimbadLookupReply(const QString& aurl, QNetworkAccessManager* anetMgr, int delayMs)
	: url(aurl)
	, reply(Q_NULLPTR)
	, netMgr(anetMgr)
	, currentStatus(SimbadLookupQuerying)
{
	if(delayMs <= 0)
		delayTimerCompleted();
	else
	{
		// First wait before starting query. This avoids sending a query for each autocompletion letter.
		QTimer::singleShot(delayMs, this, SLOT(delayTimerCompleted()));
	}
}

SimbadLookupReply::~SimbadLookupReply()
{
	if (reply)
	{
		disconnect(reply, SIGNAL(finished()), this, SLOT(httpQueryFinished()));
		reply->abort();
		//do not use delete here
		reply->deleteLater();
		reply = Q_NULLPTR;
	}
}

//This is provided for the correct deletion of the reply in the RemoteControl plugin
void SimbadLookupReply::deleteNetworkReply()
{
	if(reply)
	{
		disconnect(reply, SIGNAL(finished()), this, SLOT(httpQueryFinished()));
		reply->abort();
		delete reply;
		reply = Q_NULLPTR;
	}
}

void SimbadLookupReply::delayTimerCompleted()
{
	reply = netMgr->get(QNetworkRequest(url));
	connect(reply, SIGNAL(finished()), this, SLOT(httpQueryFinished()));
}

void SimbadLookupReply::httpQueryFinished()
{
	if (reply->error()!=QNetworkReply::NoError || reply->bytesAvailable()==0)
	{
		currentStatus = SimbadLookupErrorOccured;
		errorString = QString("%1: %2").arg(q_("Network error")).arg(reply->errorString());
		emit statusChanged();
		return;
	}

	// No error, try to parse the Simbad result
	QByteArray line;
	bool found = false;
	//qDebug() << reply->readAll();
	reply->reset();
	while (!reply->atEnd())
	{
		line = reply->readLine();
		if (line.startsWith("::data"))
		{
			found = true;
			line = reply->readLine();	// Discard first header line
			break;
		}
	}
	if (found)
	{
		line = reply->readLine();
		line.chop(1); // Remove a line break at the end
		while (!line.isEmpty())
		{
			if (line=="No Coord.")
			{
				reply->readLine();
				line = reply->readLine();
				line.chop(1); // Remove a line break at the end
				continue;
			}
			QList<QByteArray> l = line.split(' ');
			if (l.size()!=2)
			{
				currentStatus = SimbadLookupErrorOccured;
				errorString = q_("Error parsing position");
				emit statusChanged();
				return;
			}
			else
			{
				bool ok1, ok2;
				const double ra = l[0].toDouble(&ok1)*M_PI/180.;
				const double dec = l[1].toDouble(&ok2)*M_PI/180.;
				if (ok1==false || ok2==false)
				{
					currentStatus = SimbadLookupErrorOccured;
					errorString = q_("Error parsing position");
					emit statusChanged();
					return;
				}
				Vec3d v;
				StelUtils::spheToRect(ra, dec, v);
				line = reply->readLine();
				line.chop(1); // Remove a line break at the end
				line.replace("NAME " ,"");				
				resultPositions[line.simplified()]=v; // Remove an extra spaces
			}
			line = reply->readLine();
			line.chop(1); // Remove a line break at the end
		}
	}

	currentStatus = SimbadLookupFinished;
	emit statusChanged();
}

// Get a I18n string describing the current status.
QString SimbadLookupReply::getCurrentStatusString() const
{
	switch (currentStatus)
	{
		case SimbadLookupQuerying:
			return q_("Querying");
		case SimbadLookupErrorOccured:
			return q_("Error");
		case SimbadLookupFinished:
			return resultPositions.isEmpty() ? q_("Not found") : q_("Found");
	}
	return QString();
}

SimbadSearcher::SimbadSearcher(QObject* parent) : QObject(parent)
{
	networkMgr = new QNetworkAccessManager(this);
}

// Lookup in Simbad for the passed object name.
SimbadLookupReply* SimbadSearcher::lookup(const QString& serverUrl, const QString& objectName, int maxNbResult, int delayMs)
{
	// Create the Simbad query
	QString url(serverUrl);
	QString query = "format object \"%COO(d;A D)\\n%IDLIST(1)\"\n";
	query += QString("set epoch J2000\nset limit %1\n query id ").arg(maxNbResult);
	query += objectName;
	QByteArray ba = QUrl::toPercentEncoding(query, "", "");

	url += "simbad/sim-script?script=";
	url += ba.constData();
	return new SimbadLookupReply(url, networkMgr, delayMs);
}
