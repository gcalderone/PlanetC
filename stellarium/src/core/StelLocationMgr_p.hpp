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

#ifndef _STELLOCATIONMGR_P_HPP_
#define _STELLOCATIONMGR_P_HPP_

//! @file
//! StelLocationMgr private implementation details
//! Just contains GPS lookup helpers for now

#include "StelLocationMgr.hpp"

#ifdef ENABLE_GPS

class GPSLookupHelper : public QObject
{
	Q_OBJECT
protected:
	GPSLookupHelper(QObject* parent) : QObject(parent)
	{
	}
public:
	//! True if the helper is ready to be used (query() can be called)
	virtual bool isReady() = 0;
	//! Performs a GPS query.
	//! Either the queryFinished() or the queryError() signal
	//! will be called after this call (or within it)
	virtual void query() = 0;
signals:
	//! Emitted when the query finished successfully
	//! @param loc The location
	void queryFinished(const StelLocation& loc);
	//! Emitted when the query is aborted with an error message
	void queryError(const QString& msg);
};

#ifdef ENABLE_LIBGPS
#include <libgpsmm.h>

class LibGPSLookupHelper : public GPSLookupHelper
{
	Q_OBJECT
public:
	LibGPSLookupHelper(QObject * parent);
	~LibGPSLookupHelper();

	virtual bool isReady() Q_DECL_OVERRIDE;
	virtual void query() Q_DECL_OVERRIDE;
private:
	bool ready;
	gpsmm* gps_rec;
};
#endif //ENABLE_LIBGPS

#include <QNmeaPositionInfoSource>
#include <QSerialPort>
#include <QSerialPortInfo>
class NMEALookupHelper : public GPSLookupHelper
{
	Q_OBJECT
public:
	NMEALookupHelper(QObject* parent);
	virtual bool isReady() Q_DECL_OVERRIDE
	{
		return nmea;
	}

	virtual void query() Q_DECL_OVERRIDE;
private slots:
	void nmeaError(QGeoPositionInfoSource::Error error);
	void nmeaUpdated(const QGeoPositionInfo &update);
	void nmeaTimeout();
private:
	QSerialPort* serial;
	QNmeaPositionInfoSource* nmea;
};

#endif //ENABLE_GPS

#endif // _STELLOCATIONMGR_P_HPP_
