/*
 * Created by Roman Meyta 2014 <meyta@incom.tomsk.ru>
 * Incom inc Tomsk Russia http://incom.tomsk.ru/
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>
#include <ctime>

namespace filesystem {

/**
 * @brief Вернуть список имен файлов в директории
 *
 * @param path путь к директории
 * @return std::vector<std::string>
 */
std::vector<std::string> getFileNames(const std::string& path);

/**
 * @brief Вернуть время последней модификации файла
 *
 * @param file путь к файлу
 * @return time_t
 */
time_t getTimestamp(const std::string& file);

/**
 * @brief Вернуть размер файла в байтах
 * @param file путь к файлу
 * @return long int
 */
long int getFileSizeInBytes(const std::string& file);

}

#endif // FILESYSTEM_H
