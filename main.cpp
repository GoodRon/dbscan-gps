//
// Created by roman on 25.05.16.
//

#include <iostream>
#include <fstream>
#include <vector>

#include <gtk/gtk.h>

#include "nmea.h"
#include "gps.h"
#include "dbscan-gps.hxx"

#include "osm-gps-map.h"

using namespace std;
using namespace DbscanGps;

const string gps_log = "data/gps_log_mt284";

void visualization(int argc, char** argv, const GpsClusters& cluster) {
    OsmGpsMap *map;
    GtkWidget *window;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    map = static_cast<OsmGpsMap*>(g_object_new (OSM_TYPE_GPS_MAP, NULL));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(map));
    osm_gps_map_set_center_and_zoom(map, 56.4833, 84.9545, 13);

    gtk_widget_show (GTK_WIDGET(map));
    gtk_widget_show (window);

    gtk_main();
}

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
//            cout << "Split nmea: " << nmeaLine << endl;
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

    SelectionRules rules = {30.0, 30.0, 0.0, 0.0, 0.0, 0, 4};
    auto clusters = scan(points, rules);

    cout << "Found " << clusters.size() << " clusters" << endl;

    visualization(argc, argv, clusters);
    return 0;
}
