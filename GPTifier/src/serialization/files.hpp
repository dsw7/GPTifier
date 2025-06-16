#pragma once

#include <string>
#include <vector>

namespace serialization {

struct File {
    int created_at;
    std::string filename;
    std::string id;
    std::string purpose;
};

struct Files {
    std::string raw_response;
    std::vector<File> files;
};

Files get_files();
bool delete_file(const std::string &file_id);
std::string upload_file(const std::string &filename);

} // namespace serialization
