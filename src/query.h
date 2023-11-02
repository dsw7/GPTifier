#include <string>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);
void run_query(const std::string &api_key, const std::string &prompt);
