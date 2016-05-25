//
// Created by roman on 25.05.16.
//

#include <iostream>
#include <fstream>
#include <vector>

#include "nmea.h"
#include "gps.h"

using namespace std;

const string gps_log = "../data/gps_log_mt284";

int main(int argc, char** argv) {
    vector<GpsData> points;

    ifstream log(gps_log.c_str());
    if (!log.good()) {
        cout << "Can't open file '" << gps_log << "'" << endl;
        return -1;
    }

    string line;
    vector<string> lines;
    GpsData point;
    while (true) {
        if (!getline(log, line)) {
            break;
        }

        lines = nmea::split(line);
        for (auto &nmeaLine: lines) {
            cout << "Split nmea: " << nmeaLine << endl;
            if (!nmea::checkCrc(nmeaLine)) {
                cout << "Crc check error for NMEA string: " << nmeaLine << endl;
                continue;
            }

            auto nmeaType = nmea::parse(nmeaLine, point);
            if (nmeaType == nmea::ntRMC) {
                points.push_back(point);
                point.reset();
            }
        }
    }

    cout << "There is " << points.size() << " points" << endl;

    return 0;
}