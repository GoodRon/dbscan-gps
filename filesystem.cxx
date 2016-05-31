/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "filesystem.h"

using namespace std;

namespace filesystem {

vector<string> getFileNames(const string& path) {
	vector<string> list;
	DIR* directory = opendir(path.c_str());
	if (directory == NULL) {
		return list;
	}
	dirent* record = NULL;

	while ((record = readdir(directory)) != NULL) {
		if ((strcmp(record->d_name, ".") != 0) && (strcmp(record->d_name, "..") != 0)) {
			list.push_back(record->d_name);
		}
	}
	closedir(directory);
	return list;
}

time_t getTimestamp(const string& file) {
	struct stat status = {};
	if (stat(file.c_str(), &status) != 0) {
		return 0;
	}
	return status.st_mtime;
}

long int getFileSizeInBytes(const string& file) {
	struct stat status = {};
	if (stat(file.c_str(), &status) != 0) {
		return 0;
	}
	return status.st_size;
}

}
