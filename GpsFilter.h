/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#ifndef GPSFILTER_H
#define GPSFILTER_H

#include <string>

#include "gps.h"

/**
 * @brief Абстрактный класс фильтра навигационных данных
 */
class GpsFilter {
public:
	/**
	 * @brief Конструктор
	 *
	 * @param name название фильтра
	 */
	GpsFilter(const std::string& name);

	/**
	 * @brief Деструктор
	 */
	virtual ~GpsFilter() {}

	/**
	 * @brief Вернуть название фильтра
	 *
	 * @return std::string
	 */
	std::string getName() const;

	/**
	 * @brief Задать максимальное значение HDOP
	 *
	 * @param maxHDOP
	 */
	void setMaxHdop(double maxHDOP);

	/**
	 * @brief Задать максимальное значение VDOP
	 *
	 * @param maxVDOP
	 */
	void setMaxVdop(double maxVDOP);

	/**
	 * @brief Задать весовой коэффициент для HDOP
	 *
	 * @param HACC
	 */
	void setHacc(double HACC);

	/**
	 * @brief Задать весовой коэффициент для VDOP
	 *
	 * @param VACC
	 */
	void setVacc(double VACC);

	/**
	 * @brief Задать минимальное количество видимых спутников
	 *
	 * @param minSatellites
	 */
	void setMinSattelites(int minSatellites);

	/**
	 * @brief Задать максимальную скорость км/ч
	 *
	 * @param maxSpeed
	 */
	void setMaxSpeed(double maxSpeed);

	/**
	 * @brief Задать минимальную достоверную фиксируемую скорость от gps км/ч
	 *
	 * @param minReliableSpeed
	 */
	void setMinRealiableSpeed(double minReliableSpeed);

	/**
	 * @brief Задать максимальное значение ускорения м/с^2
	 *
	 * @param maxAcceleration
	 */
	void setMaxAcceleration(double maxAcceleration);

	/**
	 * @brief Сбросить зафиксированную точку
	 */
	void resetFixedPoint();

	/**
	 * @brief Задать зафиксированную точку
	 *
	 * @param point
	 */
	void refixPoint(const GpsData& point);

	/**
	 * @brief Предварительная фильтрация навигационных данных
	 * Базовая проверка выхода за допустимые границы параметров навигационной точки. При этом
	 * внутреннее состояние самого фильтра не меняется. Проверяются такие параметры, как: признак
	 * актуальности данных, количество видимых спутников, скорость, значения VDOP и HDOP и, если
	 * была зафиксирована базовая точка, ускорение.
	 *
	 * @param gpsPoint навигационная точка
	 * @return bool true - точка прошла фильтр, false - точка была отброшена
	 */
	bool prefiltering(const GpsData& gpsPoint) const;

	/**
	 * @brief Функция фильтрации
	 *
	 * @param gpsPoint навигационная точка
	 * @return bool true - точка прошла фильтр, false - точка была отброшена
	 */
	virtual bool process(GpsData& gpsPoint) = 0;

protected:
	/**
	 * @brief Название фильтра
	 */
	std::string m_name;

	/**
	 * @brief Предыдущая зафиксированная точка
	 */
	GpsData m_fixedPoint;

	/**
	 * @brief Флаг первой точки
	 */
	bool m_isFirstPoint;

	/**
	 * @brief Максимальное значение HDOP
	 */
	double m_maxHDOP;

	/**
	 * @brief Максимальное значение VDOP
	 */
	double m_maxVDOP;

	/**
	 * @brief Весовой коэффициент для HDOP
	 */
	double m_HACC;

	/**
	 * @brief Весовой коэффициент для VDOP
	 */
	double m_VACC;

	/**
	 * @brief Минимальное количество видимых спутников
	 */
	unsigned m_minSatellites;

	/**
	 * @brief Максимальная скорость км/ч
	 */
	double m_maxSpeed;

	/**
	 * @brief Минимальная достоверная фиксируемая скорость от gps км/ч
	 */
	double m_minReliableSpeed;

	/**
	 * @brief Максимальное значение ускорения м/с^2
	 */
	double m_maxAcceleration;
};

#endif // GPSFILTER_H
