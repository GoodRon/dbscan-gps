//
// Created by roman on 25.05.16.
//

#ifndef DBSCAN_GPS_HXX
#define DBSCAN_GPS_HXX

#include <functional>
#include <vector>

struct GpsData;

namespace DbscanGps {

struct SelectionRules {
    double epsMeters;
    double epsAngle;
    double epsSpeed;
    double epsHdop;
    double epsVdop;
    time_t epsTimestamp;
    unsigned minPts;
};

struct Class {
    gpsPoints points;
};

typedef std::vector<GpsData> GpsPoints;
typedef GpsData GpsPoint;
typedef std::vector<Class> Classes;

Classes scan(const GpsPoints& points, const SelectionRules& rules);

}

#endif // DBSCAN_GPS_HXX

