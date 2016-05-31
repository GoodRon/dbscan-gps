/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#include <ctime>
#include <cmath>
#include <syslog.h>

#include "FastFilter.h"
#include "filtersFunctions.h"
#include "nmea.h"

using namespace nmea;

FastFilter::FastFilter() :
	GpsFilter("FastFilter"),
	m_speedLimit(60.0),
	m_cutOffRadius(10.0),
	m_maxAngleDelta(normalizeAngle(15.0)),
	m_maxSilenceTimeout(20),
	m_maxSpeedDelta(10.0) {
}

FastFilter::~FastFilter() {
}

void FastFilter::setSpeedLimit(double speedLimit) {
	m_speedLimit = speedLimit;
}

void FastFilter::setCutOffRadius(double cutOffRadius) {
	m_cutOffRadius = cutOffRadius;
}

void FastFilter::setMaxAngleDelta(double maxAngleDelta) {
	m_maxAngleDelta = normalizeAngle(maxAngleDelta);
}

void FastFilter::setMaxSilenceTimeout(time_t maxSilenceTimeout) {
	m_maxSilenceTimeout = maxSilenceTimeout;
}

void FastFilter::setMaxSpeedDelta(double maxSpeedDelta) {
	m_maxSpeedDelta = maxSpeedDelta;
}

bool FastFilter::process(GpsData& gpsPoint) {
	// Если это первая точка - все просто
	if (m_isFirstPoint) {
		m_fixedPoint = gpsPoint;
		m_isFirstPoint = false;
		syslog(LOG_NOTICE, "first point with timestamp %s", ctime(&gpsPoint.timestamp));
		return true;
	}

	// Проверка преодоления радиуса отсечения
	if (!checkCutOffRadius(gpsPoint)) {
		// Заменяем координаты и направление
		gpsPoint.latitude = m_fixedPoint.latitude;
		gpsPoint.northLatitude = m_fixedPoint.northLatitude;
		gpsPoint.longitude = m_fixedPoint.longitude;
		gpsPoint.eastLongitude = m_fixedPoint.eastLongitude;
		gpsPoint.altitude = m_fixedPoint.altitude;
		gpsPoint.direction = m_fixedPoint.direction;
	}

	// Если установлен таймаут между точками, проверяем его
	if (checkTimeout(gpsPoint)) {
		m_fixedPoint = gpsPoint;
		syslog(LOG_NOTICE, "send point because timout");
		return true;
	}

	// Проверка дельты угла направления
	if (checkAngle(gpsPoint)) {
		m_fixedPoint = gpsPoint;
		syslog(LOG_NOTICE, "send point because angle delta");
		return true;
	}

	// Проверка изменения скорости
	if (checkSpeed(gpsPoint)) {
		m_fixedPoint = gpsPoint;
		syslog(LOG_NOTICE, "send point because speed");
		return true;
	}
	return false;
}

bool FastFilter::checkAngle(const GpsData& gpsPoint) const {
	if ((m_maxAngleDelta > 0.0) &&
		(calculateAngleDelta(gpsPoint.direction, m_fixedPoint.direction) > m_maxAngleDelta) &&
		(gpsPoint.speed >= m_minReliableSpeed)) {
		return true;
	}
	return false;
}

bool FastFilter::checkSpeed(const GpsData& gpsPoint) const {
	if ((m_speedLimit > 0) && (gpsPoint.speed < m_speedLimit)) {
		return false;
	}

	if (fabs(gpsPoint.speed - m_fixedPoint.speed) > m_maxSpeedDelta) {
		return true;
	}
	return false;
}

bool FastFilter::checkTimeout(const GpsData& gpsPoint) const {
	if ((m_maxSilenceTimeout > 0) && (gpsPoint.timestamp - m_fixedPoint.timestamp >=
									  m_maxSilenceTimeout)) {
		return true;
	}
	return false;
}

bool FastFilter::checkCutOffRadius(const GpsData& gpsPoint) const {
	if (gpsPoint.speed > m_minReliableSpeed) {
		return true;
	}

	double distance = calculateDistance(convertDegreesFromNmeaToNormal(m_fixedPoint.latitude),
										convertDegreesFromNmeaToNormal(m_fixedPoint.longitude),
										convertDegreesFromNmeaToNormal(gpsPoint.latitude),
										convertDegreesFromNmeaToNormal(gpsPoint.longitude));
	if (distance > m_cutOffRadius) {
		return true;
	}
	return false;
}
