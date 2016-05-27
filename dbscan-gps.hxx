//
// Created by roman on 25.05.16.
//

#ifndef DBSCAN_GPS_HXX
#define DBSCAN_GPS_HXX

#include <functional>
#include <vector>

#include "gps.h"

namespace DbscanGps {

typedef GpsData GpsPoint;
typedef std::vector<GpsPoint> GpsPoints;

struct SelectionRules {
    double epsMeters;
    double epsAngle;
    double epsSpeed;
    double epsHdop;
    double epsVdop;
    time_t epsTimestamp;
    unsigned minPts;
};

struct GpsCluster {
//	std::string name;
    GpsPoints points;
};

typedef std::vector<GpsCluster> GpsClusters;

GpsClusters scan(const GpsPoints& points, const SelectionRules& rules);

}

#endif // DBSCAN_GPS_HXX

