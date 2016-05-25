/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#ifndef FILTERSFUNCTIONS_H
#define FILTERSFUNCTIONS_H

/**
 * @brief Расчитывает расстояние между двумя точками на поврехности Земли
 *
 * @param latFrom Широта первой точки
 * @param longFrom Долгота первой точки
 * @param latTo Широта второй точки
 * @param longTo Долгота второй точки
 *
 * @return double Расстояние в метрах
 */
double calculateDistance(double latFrom, double longFrom, double latTo, double longTo);

/**
 * @brief Расчитывает расстояние между двумя точками на поврехности Земли
 * Точки задаются координатами в градусах со знаком
 *
 * @param latFrom Широта первой точки
 * @param longFrom Долгота первой точки
 * @param latTo Широта второй точки
 * @param longTo Долгота второй точки
 *
 * @return double Расстояние в метрах
 */
double calculateDistanceByAngle(double latFrom, double longFrom, double latTo, double longTo);

/**
 * @brief Нормализовать угол
 * Привести угол в градусах к соответсвию диапазону [0.0, 360.0)
 *
 * @param angle
 * @return double
 */
double normalizeAngle(double angle);

/**
 * @brief Посчитать значение угла поворота по часовой стрелке от previous до current
 *
 * @param previous предыдущий угол
 * @param current текущий угол
 * @return double
 */
double calculateClockwiseRotation(double previous, double current);

/**
 * @brief Расчитать дельту между углами
 * При расчете учитывается минимальный поворот
 *
 * @param lhs первый операнд
 * @param rhs второй операнд
 * @return double
 */
double calculateAngleDelta(double lhs, double rhs);

#endif // FILTERSFUNCTIONS_H
