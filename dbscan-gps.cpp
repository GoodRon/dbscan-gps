//
// Created by roman on 25.05.16.
//

#include <unordered_set>

#include "dbscan-gps.hxx"
#include "filtersFunctions.h"
#include "nmea.h"

using namespace std;
using namespace nmea;

namespace DbscanGps {

unordered_set<GpsPoints::const_iterator> findNeighbours(const GpsPoints::const_iterator& ipoint,
                                                        const GpsPoints& points,
                                                        const SelectionRules& rules) {
    unordered_set<GpsPoints::const_iterator> neighbours;
    const GpsPoint& point = *ipoint;

    for (auto ineighbour = points.begin(); ineighbour != points.end(); ++ineighbour) {
        if (ineighbour == ipoint) {
            continue;
        }
        const GpsPoint& neighbour = *ineighbour;

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

        if (rules.epsTimestamp > 0.0) {
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

GpsCluster construtCluster(const GpsPoints::const_iterator& ipoint,
                           const unordered_set<GpsPoints::const_iterator>& neighbours,
                           const SelectionRules& rules) {
    GpsCluster cluster;
    cluster.points.push_back(*ipoint);

}

GpsClusters scan(const GpsPoints& points, const SelectionRules& rules) {
    unordered_set<GpsPoints::const_iterator> visited;
    GpsClusters clusters;
    GpsCluster noise;

    for (auto ipoint = points.begin(); ipoint != points.end(); ++ipoint) {
        if (visited.find(ipoint) != visited.end()) {
            continue;
        }
        visited.insert(ipoint);

        auto neighbours = findNeighbours(ipoint, points, rules);
        if (neighbours.size() < rules.minPts) {
            noise.points.push_back(*ipoint);
        }

        // New cluster
        GpsCluster cluster;
        cluster.points.push_back(*ipoint);

        while (!neighbours.empty()) {
            unordered_set<GpsPoints::const_iterator> moreNeighbours;
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
                cluster.points.push_back(*ineighbour);
            }
            neighbours = move(moreNeighbours);
        }
        clusters.push_back(move(cluster));
    }
    return clusters;
}

}
