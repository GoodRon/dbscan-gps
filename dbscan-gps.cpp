//
// Created by roman on 25.05.16.
//

#include "dbscan-gps.hxx"

struct MarkedPoint {
    gpsPoint point;
    bool isChecked;
};

typedef std::vector<MarkedPoint> MarkedPoints;

DbscanClasses scan(const GpsPoints& points, const SelectionRules& rules) {

}

GpsPoints findNeighbours(const GpsPoint& point, const GpsPoints& points,
                         const SelectionRules& rules) {
    GpsPoints neighbours;

    for (auto& neighbour: points) {
        // TODO optimize it
        if (neighbour == point) {
            continue;
        }

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

        neighbours.push_back(neighbour);
    }
    return neighbours;
}