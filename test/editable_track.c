#include <gtk/gtk.h>
#include "osm-gps-map.h"

int
main (int   argc,
      char *argv[])
{
	OsmGpsMap *map;
    GtkWidget *window;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);


	map = g_object_new (OSM_TYPE_GPS_MAP, NULL);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(map));

	OsmGpsMapTrack* track = osm_gps_map_track_new();

	OsmGpsMapPoint* p1, *p2, *p3;
	p1 = osm_gps_map_point_new_radians(1.25663706, -0.488692191);
	p2 = osm_gps_map_point_new_radians(1.06465084, -0.750491578);
	p3 = osm_gps_map_point_new_degrees(56.4833, 84.9545);

	osm_gps_map_track_add_point(track, p1);
	osm_gps_map_track_add_point(track, p2);
	osm_gps_map_track_add_point(track, p3);

	g_object_set(track, "editable", TRUE, NULL);

	GdkRGBA color = {0.0, 1.0, 1.0, 0.0};
	osm_gps_map_track_set_color(track, &color);

	osm_gps_map_track_add(map, track);
	osm_gps_map_set_center_and_zoom(map, 56.4833, 84.9545, 13);
	osm_gps_map_gps_add(map, 56.4833, 84.9545, 50.0);

	gtk_widget_show (GTK_WIDGET(map));
    gtk_widget_show (window);

    gtk_main ();

    return 0;
}


