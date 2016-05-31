/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#include "GpsFilter.h"

/**
 * @brief Константа для перевода км/ч в м/с
 */
const double kphToMps = 1000.0 / 3600.0;

GpsFilter::GpsFilter(const std::string& name) :
	m_name(name),
	m_fixedPoint(),
	m_isFirstPoint(true),
	m_maxHDOP(4.0),
	m_maxVDOP(4.0),
	m_HACC(30.0),
	m_VACC(3.3),
	m_minSatellites(7),
	m_maxSpeed(160.0),
	m_minReliableSpeed(8.0),
	m_maxAcceleration(6.0) {
}

std::string GpsFilter::getName() const {
	return m_name;
}

void GpsFilter::setMaxHdop(double maxHDOP) {
	m_maxHDOP = maxHDOP;
}

void GpsFilter::setMaxVdop(double maxVDOP) {
	m_maxVDOP = maxVDOP;
}

void GpsFilter::setHacc(double HACC) {
	m_HACC = HACC;
}

void GpsFilter::setVacc(double VACC) {
	m_VACC = VACC;
}

void GpsFilter::setMinSattelites(int minSatellites) {
	m_minSatellites = minSatellites;
}

void GpsFilter::setMaxSpeed(double maxSpeed) {
	m_maxSpeed = maxSpeed;
}

void GpsFilter::setMinRealiableSpeed(double minReliableSpeed) {
	m_minReliableSpeed = minReliableSpeed;
}

void GpsFilter::setMaxAcceleration(double maxAcceleration) {
	m_maxAcceleration = maxAcceleration;
}

void GpsFilter::resetFixedPoint() {
	m_isFirstPoint = false;
}

void GpsFilter::refixPoint(const GpsData& point) {
	m_fixedPoint = point;
}

bool GpsFilter::prefiltering(const GpsData& gpsPoint) const {
	// Базовая проверка выхода за допустимые границы параметров
	if (!gpsPoint.actuality || gpsPoint.satellites < m_minSatellites ||
		gpsPoint.speed > m_maxSpeed || gpsPoint.vdop > m_maxVDOP || gpsPoint.hdop > m_maxHDOP) {
		return false;
	}

	// Если предыдущих точек не было или проблемы с разницей времен, не проверяем ускорение
	if (m_isFirstPoint || (gpsPoint.timestamp <= m_fixedPoint.timestamp)) {
		return true;
	}

	// WARNING подумать, нужно ли вообще здесь проверять ускорение
	// Ускорение в м/с^2
	double acceleration = (gpsPoint.speed - m_fixedPoint.speed) * kphToMps /
						  (gpsPoint.timestamp - m_fixedPoint.timestamp);

	if (fabs(acceleration) > fabs(m_maxAcceleration)) {
		return false;
	}
	return true;
}
