//
// Created by roman on 25.05.16.
//

#include <set>

#include "dbscan-gps.hxx"
#include "filtersFunctions.h"
#include "nmea.h"

using namespace std;
using namespace nmea;

namespace DbscanGps {

set<size_t> findNeighbours(size_t ipoint, const GpsPoints& points, const SelectionRules& rules) {
    if (ipoint >= points.size()) {
        return set<size_t>();
    }

    set<size_t> neighbours;
    const GpsPoint& point = points[ipoint];

    for (size_t ineighbour = 0; ineighbour < points.size(); ++ineighbour) {
        if (ineighbour == ipoint) {
            continue;
        }
        const GpsPoint& neighbour = points[ineighbour];

        if (rules.epsMeters > 0.0) {
            double distance = calculateDistance(convertDegreesFromNmeaToNormal(point.latitude),
                                                convertDegreesFromNmeaToNormal(point.longitude),
                                                convertDegreesFromNmeaToNormal(neighbour.latitude),
                                                convertDegreesFromNmeaToNormal(neighbour.longitude));

            if (distance > rules.epsMeters) {
                continue;
            }
        }

        if (rules.epsAngle > 0.0) {
            double delta = calculateAngleDelta(point.direction, neighbour.direction);
            if (delta > rules.epsAngle) {
                continue;
            }
        }

        if (rules.epsTimestamp > 0) {
            if (labs(point.timestamp - neighbour.timestamp) > rules.epsTimestamp) {
                continue;
            }
        }

        if (rules.epsSpeed > 0.0) {
            if (fabs(point.speed - neighbour.speed) > rules.epsSpeed) {
                continue;
            }
        }

        if (rules.epsHdop > 0.0) {
            if (fabs(point.hdop - neighbour.hdop) > rules.epsHdop) {
                continue;
            }
        }

        if (rules.epsVdop > 0.0) {
            if (fabs(point.vdop - neighbour.vdop) > rules.epsVdop) {
                continue;
            }
        }

        neighbours.insert(ineighbour);
    }
    return neighbours;
}

GpsClusters scan(const GpsPoints& points, const SelectionRules& rules) {
    set<size_t> visited;
    GpsClusters clusters;
    GpsCluster noise = {};

    for (size_t ipoint = 0; ipoint < points.size(); ++ipoint) {
        if (visited.find(ipoint) != visited.end()) {
            continue;
        }
        visited.insert(ipoint);

        auto neighbours = findNeighbours(ipoint, points, rules);
        if (neighbours.size() < rules.minPts) {
            noise.points.push_back(points[ipoint]);
        }

        GpsCluster cluster = {};
        cluster.points.push_back(points[ipoint]);

        while (!neighbours.empty()) {
            set<size_t> moreNeighbours;
            for (auto& ineighbour: neighbours) {
                if (visited.find(ineighbour) != visited.end()) {
                    continue;
                }
                visited.insert(ineighbour);

                auto nextNeighbours = findNeighbours(ineighbour, points, rules);
                if (nextNeighbours.size() >= rules.minPts) {
                    moreNeighbours.insert(nextNeighbours.begin(),
                                          nextNeighbours.end());
                }
                cluster.points.push_back(points[ineighbour]);
            }
            neighbours = move(moreNeighbours);
        }
        clusters.push_back(move(cluster));
    }
//    clusters.push_back(noise);
    return clusters;
}

}
