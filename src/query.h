#include <curl/curl.h>
#include <string>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);

class QueryHandler
{
    private:
        std::string api_key;
        ::CURL* curl = NULL;

    public:
        QueryHandler(const std::string &api_key);
        ~QueryHandler();
        void run_query(const std::string &prompt, std::string &reply);
};
