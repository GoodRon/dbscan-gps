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
#include <jsoncpp/json/json.h>

#include "nmea.h"
#include "gps.h"
#include "dbscan-gps.hxx"
#include "FastFilter.h"
#include "filtersFunctions.h"
#include "osm-gps-map.h"

using namespace std;
using namespace DbscanGps;
using namespace nmea;

const string gps_log = "data/gps_log_mt284";
const string rules_file = "data/rules.conf";

void visualization(int argc, char** argv, const GpsClusters& clusters, double breakDistance = 0.0) {
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
        vector<OsmGpsMapTrack*> track;
    };

    vector<DrawableTrack> drawableTracks;

    for (auto& cluster: clusters) {
        if (cluster.points.size() <= 0) {
            continue;
        }

        DrawableTrack drawableTrack;
        GdkRGBA color = {(rand()%100)/100.0, (rand()%100)/100.0, (rand()%100)/100.0, 1.0};
        drawableTrack.track.push_back(osm_gps_map_track_new());
        GpsPoint prewPoint = *cluster.points.begin();

        for (auto& clusterPoint: cluster.points) {
            auto lat = convertDegreesFromNmeaToNormal(clusterPoint.latitude);
            auto lon = convertDegreesFromNmeaToNormal(clusterPoint.longitude);
            OsmGpsMapPoint* point = osm_gps_map_point_new_degrees(lat, lon);
            drawableTrack.points.push_back(point);

            // Разрываем треки в одном кластере для лучшего отображения
            if (breakDistance > 0.0) {
                auto prewLat = convertDegreesFromNmeaToNormal(prewPoint.latitude);
                auto prewLon = convertDegreesFromNmeaToNormal(prewPoint.longitude);

                double distance = calculateDistance(prewLat, prewLon, lat, lon);
                if (distance >= breakDistance) {
                    g_object_set(drawableTrack.track.back(), "editable", TRUE, NULL);
                    osm_gps_map_track_set_color(drawableTrack.track.back(), &color);
                    osm_gps_map_track_add(map, drawableTrack.track.back());
                    drawableTrack.track.push_back(osm_gps_map_track_new());
                }
            }
            osm_gps_map_track_add_point(drawableTrack.track.back(), point);
            prewPoint = clusterPoint;
        }

        g_object_set(drawableTrack.track.back(), "editable", TRUE, NULL);
        osm_gps_map_track_set_color(drawableTrack.track.back(), &color);
        osm_gps_map_track_add(map, drawableTrack.track.back());
        drawableTracks.push_back(drawableTrack);
    }

    gtk_widget_show (GTK_WIDGET(map));
    gtk_widget_show (window);

    gtk_main();
}

bool loadRules(const std::string& json, SelectionRules& rules, FastFilter& filter,
               double& breakDistance) {
    ifstream jsonFile;
    jsonFile.open(json);
    if (!jsonFile.good()) {
        cerr << "Can't open json file " << json << endl;
        return false;
    }

    string line;
    string jsonContent;
    while (getline(jsonFile, line)) {
        jsonContent += line;
    }

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(jsonContent, root)) {
        cerr << "Can't parse json from " << json << endl;
        return false;
    }

    // Загружаем параметры DBSCAN
    Json::Value rulesSection = root["rules"];
    rules.epsMeters = rulesSection.get("epsMeters", "0.0").asDouble();
    rules.epsAngle = rulesSection.get("epsAngle", "0.0").asDouble();
    rules.epsSpeed = rulesSection.get("epsSpeed", "0.0").asDouble();
    rules.epsHdop = rulesSection.get("epsHdop", "0.0").asDouble();
    rules.epsVdop = rulesSection.get("epsVdop", "0.0").asDouble();
    rules.epsTimestamp = rulesSection.get("epsTimestamp", "0").asInt64();
    rules.minPts = rulesSection.get("minPts", "0").asUInt();

    // Загружаем параметры фильтрации
    Json::Value filterSection = root["filter"];
    filter.setSpeedLimit(filterSection.get("speedLimit", "60.0").asDouble());
    filter.setCutOffRadius(filterSection.get("cutOffRadius", "10.0").asDouble());
    filter.setMaxAngleDelta(normalizeAngle(filterSection.get("maxAngleDelta", "15.0").asDouble()));
    filter.setMaxSilenceTimeout(filterSection.get("maxSilenceTimeout", "20").asInt64());
    filter.setMaxSpeedDelta(filterSection.get("maxSpeedDelta", "10.0").asDouble());
    filter.setMaxHdop(filterSection.get("maxHDOP", "2.0").asDouble());
    filter.setMaxVdop(filterSection.get("maxVDOP", "2.0").asDouble());
    filter.setHacc(filterSection.get("HACC", "30.0").asDouble());
    filter.setVacc(filterSection.get("VACC", "3.3").asDouble());
    filter.setMinSattelites(filterSection.get("minSatellites", "7").asUInt());
    filter.setMaxSpeed(filterSection.get("maxSpeed", "160.0").asDouble());
    filter.setMinRealiableSpeed(filterSection.get("minReliableSpeed", "8.0").asDouble());
    filter.setMaxAcceleration(filterSection.get("maxAcceleration", "6.0").asDouble());

    // Загружаем параметры визуализации
    Json::Value visualisationSection = root["visualisation"];
    breakDistance = visualisationSection.get("breakDistance", "500.0").asDouble();
    return true;
}

int main(int argc, char** argv) {
    // Загружаем параметры
    FastFilter filter;
    SelectionRules rules;
    double breakDistance = 500.0;
    if (!loadRules(rules_file, rules, filter, breakDistance)) {
        cerr << "Can't read " << rules_file << ", will use default parameters" << endl;
    }

    // Загружаем трек
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

    // Фильтрация
    vector<GpsData> filteredPoints;
    filter.setMaxSilenceTimeout(10);
    for (auto& point: points) {
        if (filter.process(point)) {
            filteredPoints.push_back(point);
        }
    }

    cout << "There is " << points.size() << " points" << endl;
    cout << "There is " << filteredPoints.size() << " filtered points" << endl;

    // Кластеризация
    auto clusters = scan(filteredPoints, rules);

    cout << "Found " << clusters.size() << " clusters" << endl;

    // Сортируем точки по времени для наглядной визуализации
    for (auto& cluster: clusters) {
        sort(cluster.points.begin(), cluster.points.end(), [](const GpsPoint& lhs,
                                                              const GpsPoint& rhs)->bool{
            return lhs.timestamp < rhs.timestamp;
        });
    }

    visualization(argc, argv, clusters, breakDistance);
    return 0;
}
