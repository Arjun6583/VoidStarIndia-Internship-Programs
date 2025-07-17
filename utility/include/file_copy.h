#ifndef FILE_COPY_H
#define FILE_COPY_H

#include <string>

bool copy_file_using_fopen(const std::string &source_path, const std::string &destination_path);

bool copy_file_using_open(const std::string &source_path, const std::string &destination_path);

bool copy_file_using_fopen_s(const std::string &source_path, const std::string &destination_path);

bool check_platform_file_path(const std::string &source_path, const std::string &destination_path);

bool vsi_create_file(const std::string &source_path, const std::string &destination_path);

#endif