#include <curl/curl.h>
#include <string>

class QueryHandler
{
    private:
        void init_easy();

        std::string api_key;
        bool is_global_init;
        ::CURL* curl; 

    public:
        QueryHandler(const std::string &api_key);
        ~QueryHandler();
};

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);
void run_query(const std::string &api_key, const std::string &prompt);
