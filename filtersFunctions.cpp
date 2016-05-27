/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#include <limits>
#include <math.h>
#include <syslog.h>

#include "gps.h"

// TODO проверить математику вычислений, могут появляться ситуации деления на 0!
double calculateDistance(double latFrom, double longFrom, double latTo, double longTo) {
	double fPhimean = 0.0;	// Средняя широта
	double fdLambda = 0.0;	// Разница между двумя значениями долготы
	double fdPhi = 0.0;		// Разница между двумя значениями широты
	double fAlpha = 0.0;	// Смещение
	double fRho = 0.0;		// Меридианский радиус кривизны
	double fNu = 0.0;		// Поперечный радиус кривизны
	double fR = 0.0;		// Радиус сферы Земли
	double fz = 0.0;		// Угловое расстояние от центра сфероида
	double fTemp = 0.0;		// Временная переменная, использующаяся в вычислениях
	double distance = 0.0;  // Вычисленное расстояния в метрах

	// Константы, используемые для вычисления смещения и расстояния
	const double D2R = 0.017453;	// Константа для преобразования градусов в радианы
	const double a = 6378137.0;	// Основные полуоси
	const double e2 = 0.006739496742337;

	if ((longFrom == longTo) && (latFrom == latTo)) {
		return distance;
	}

	fdLambda = (longFrom - longTo) * D2R;
	fdPhi = (latFrom - latTo) * D2R;
	fPhimean = ((latFrom + latTo) / 2.0) * D2R;

	// Вычисляем меридианные и поперечные радиусы кривизны средней широты
	fTemp = 1.0 - e2 * (pow(sin(fPhimean), 2.0));
	fRho = (a * (1.0 - e2)) / pow(fTemp, 1.5);
	fNu = a / (sqrt(1.0 - e2 * (sin(fPhimean) * sin(fPhimean))));

	// Вычисляем угловое расстояние
	fz = sqrt(pow(sin(fdPhi / 2.0), 2.0) + cos(latTo * D2R) * cos(latFrom * D2R) *
			  pow(sin(fdLambda / 2.0), 2.0));
	fz = 2.0 * asin(fz);

	// Вычисляем смещение
	fAlpha = cos(latTo * D2R) * sin(fdLambda) * 1.0 / sin(fz);
	fAlpha = asin(fAlpha);

	// Вычисляем радиус Земли
	fR = (fRho * fNu) / ((fRho * pow(sin(fAlpha), 2.0)) + (fNu * pow(cos(fAlpha), 2.0)));

	// Получаем смещение и расстояние
	distance = (fz * fR);

	// Есди возникли проблемы при вычислении
	if (isnan(distance) || isinf(distance)) {
		return std::numeric_limits<double>::max();
	}
	return distance;
}

double calculateDistanceByAngle(double latFrom, double longFrom, double latTo, double longTo) {
	const double radian = (M_PI / 180.0);

	// Преобразуем градусы в радианы
	double lon1 = radian * -longFrom;
	double lat1 = radian * latFrom;
	double lon2 = radian * -longTo;
	double lat2 = radian * latTo;

	// Вычисляем угловое расстояние по дуге большого радиуса
	double d = 2.0 * asin(sqrt(pow(sin((lat1 - lat2) / 2.0), 2.0) + cos(lat1) * cos(lat2) *
							   pow(sin((lon1 - lon2) / 2.0), 2.0)));

	// Вычисляем расстояние в метрах
	return static_cast<double>(metersInNauticalMile * 60.0 * d / radian);
}

double normalizeAngle(double angle) {
	if (angle < 0.0) {
		angle = fmod(angle, 360.0) + 360.0;
	} else {
		angle = fmod(angle, 360.0);
	}
	return angle;
}

double calculateClockwiseRotation(double previous, double current) {
	return normalizeAngle(normalizeAngle(current) - normalizeAngle(previous));
}

double calculateAngleDelta(double lhs, double rhs) {
	double delta = calculateClockwiseRotation(lhs, rhs);
	if (delta > 180.0) {
		delta = calculateClockwiseRotation(rhs, lhs);
	}
	return delta;
}
