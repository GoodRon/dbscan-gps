/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#ifndef FASTFILTER_H
#define FASTFILTER_H

#include "GpsFilter.h"

/**
 * @brief Класс фильтра навигационных точек, основанный на алгоритме быстрой фильтрации
 */
class FastFilter : public GpsFilter {
public:
	/**
	 * @brief Конструктор с дефолтными значениями параметров
	 */
	FastFilter();

	/**
	 * @brief Деструктор
	 */
	virtual ~FastFilter();

	/**
	 * @brief Установить ограничение скорости по городу
	 *
	 * @param speedLimit
	 */
	void setSpeedLimit(double speedLimit);

	/**
	 * @brief Установить радиус отсечения
	 *
	 * @param cutOffRadius
	 */
	void setCutOffRadius(double cutOffRadius);

	/**
	 * @brief Установить максимальное изменение угла направления движения
	 *
	 * @param maxAngleDelta
	 */
	void setMaxAngleDelta(double maxAngleDelta);

	/**
	 * @brief Установить максимальный таймаут между точками
	 *
	 * @param maxSilenceTimeout
	 */
	void setMaxSilenceTimeout(time_t maxSilenceTimeout);

	/**
	 * @brief Установить максимальное изменение скорости после выхода за ограничения по скорости
	 *
	 * @param maxSpeedDelta
	 */
	void setMaxSpeedDelta(double maxSpeedDelta);

	/**
	 * @brief Быстрая фильтрация
	 * Фильтр ориентируется на изменение скорости, направления или превышение таймаута
	 *
	 * @param gpsPoint навигационная точка
	 * @return bool
	 */
	virtual bool process(GpsData& gpsPoint);

private:
	/**
	 * @brief Проверить угол поворота
	 *
	 * @param gpsPoint
	 * @return bool
	 */
	bool checkAngle(const GpsData& gpsPoint) const;

	/**
	 * @brief Проверка перехода дискретной границы скоростей
	 *
	 * @param gpsPoint
	 * @return bool
	 */
	bool checkSpeed(const GpsData& gpsPoint) const;

	/**
	 * @brief Проверить таймаут между точками
	 *
	 * @param gpsPoint
	 * @return bool
	 */
	bool checkTimeout(const GpsData& gpsPoint) const;

	/**
	 * @brief Проверить преодоление радиуса отсечения
	 *
	 * @param gpsPoint
	 * @return bool
	 */
	bool checkCutOffRadius(const GpsData& gpsPoint) const;

private:
	/**
	 * @brief Ограничение скорости по городу км/ч
	 */
	double m_speedLimit;

	/**
	 * @brief Радиус отсечения. Используется для фильтрации навигационного шума при остановке
	 */
	double m_cutOffRadius;

	/**
	 * @brief Максимальное изменение угла направления движения, по превышению которого
	 * навигационная точка отправляется в ДЦ
	 */
	double m_maxAngleDelta;

	/**
	 * @brief Максимальный таймаут между точками
	 */
	time_t m_maxSilenceTimeout;

	/**
	 * @brief Максимальное изменение скорости после выхода за ограничения по скорости, по
	 * превышению которого навигационная точка отправляется в ДЦ
	 */
	double m_maxSpeedDelta;
};

#endif // FASTFILTER_H
