//
// Created by roman on 25.05.16.
//

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <memory>
#include <cstdlib>

#include <gtk/gtk.h>

#include "nmea.h"
#include "gps.h"
#include "dbscan-gps.hxx"
#include "FastFilter.h"
#include "osm-gps-map.h"

using namespace std;
using namespace DbscanGps;

const string gps_log = "data/gps_log_mt284";

void visualization(int argc, char** argv, const GpsClusters& clusters) {
    OsmGpsMap *map;
    GtkWidget *window;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    map = static_cast<OsmGpsMap*>(g_object_new (OSM_TYPE_GPS_MAP, NULL));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(map));
    osm_gps_map_set_center_and_zoom(map, 56.4833, 84.9545, 13);

    srand(time(NULL));

    struct DrawableTrack {
        vector<OsmGpsMapPoint*> points;
        OsmGpsMapTrack* track;
    };

    vector<DrawableTrack> drawableTracks;

    for (auto& cluster: clusters) {
//        cout << "cluster:" << endl;

        DrawableTrack drawableTrack;
        drawableTrack.track = osm_gps_map_track_new();

        for (auto& clusterPoint: cluster.points) {

            auto lat = nmea::convertDegreesFromNmeaToNormal(clusterPoint.latitude);
            auto lon = nmea::convertDegreesFromNmeaToNormal(clusterPoint.longitude);

//            cout << "point lat: " << lat << " lon: "
//                 << lon << endl;

            OsmGpsMapPoint* point = osm_gps_map_point_new_degrees(lat, lon);

            drawableTrack.points.push_back(point);
            osm_gps_map_track_add_point(drawableTrack.track, point);
        }
        g_object_set(drawableTrack.track, "editable", TRUE, NULL);
        GdkRGBA color = {(rand()%100)/100.0, (rand()%100)/100.0, (rand()%100)/100.0, 1.0};
        osm_gps_map_track_set_color(drawableTrack.track, &color);
        osm_gps_map_track_add(map, drawableTrack.track);
        drawableTracks.push_back(drawableTrack);
    }

    gtk_widget_show (GTK_WIDGET(map));
    gtk_widget_show (window);

    gtk_main();
}

int main(int argc, char** argv) {
    ifstream log(gps_log.c_str());
    if (!log.good()) {
        cout << "Can't open file '" << gps_log << "'" << endl;
        return -1;
    }

    string line;
    vector<string> lines;
    GpsData point;
    vector<GpsData> points;
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

    // Фильтрация
    vector<GpsData> filteredPoints;
    FastFilter filter;
    filter.setMaxSilenceTimeout(10);
    for (auto& point: points) {
        if (filter.process(point)) {
            filteredPoints.push_back(point);
        }
    }

    cout << "There is " << filteredPoints.size() << " filtered points" << endl;

    // Кластеризация
    SelectionRules rules = {450.0, 50.0, 10.0, 0.0, 0.0, 120, 3};
    auto clusters = scan(filteredPoints, rules);

    cout << "Found " << clusters.size() << " clusters" << endl;

    // Сортируем точки по времени для наглядной визуализации
    for (auto& cluster: clusters) {
        sort(cluster.points.begin(), cluster.points.end(), [](const GpsPoint& lhs,
                                                              const GpsPoint& rhs)->bool{
            return lhs.timestamp < rhs.timestamp;
        });
    }

    visualization(argc, argv, clusters);
    return 0;
}
