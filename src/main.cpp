#include <cstdlib>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void help(const std::string &exec)
{
    std::cerr << "--- GPTifier ---\n";
    std::cerr << "Usage: " + exec + " '<prompt>'" << std::endl;
}

void load_api_key(std::string &api_key)
{
    const char* home_dir = std::getenv("HOME");

    if (not home_dir)
    {
        throw std::runtime_error("Could not locate home directory!");
    }

    std::string path_key = std::string(home_dir) + "/.openai";

    if (not std::filesystem::exists(path_key))
    {
        throw std::runtime_error("Could not locate .openai key file in home directory!");
    }

    std::ifstream s_key(path_key);
    std::getline(s_key, api_key);

    s_key.close();
}

void run_query(const std::string &api_key, const std::string &prompt)
{
    static std::string url = "https://api.openai.com/v1/chat/completions";
    static std::string model_id = "gpt-3.5-turbo";

    std::cout << "--- GPTifier ---\n";
    std::cout << "Using model: " + model_id + "\n";
    std::cout << "Processing query: " + prompt << std::endl;

    std::string header_content_type = "Content-Type: application/json";
    std::string header_auth = "Authorization: Bearer " + api_key;

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, header_content_type.c_str());
    headers = curl_slist_append(headers, header_auth.c_str());
}

int main(int argc, char **argv)
{
    std::string exec = std::string(argv[0]);

    if (argc < 2)
    {
        ::help(exec);
        return EXIT_FAILURE;
    }

    std::string prompt = std::string(argv[1]);

    if ((prompt.compare("-h") == 0) or (prompt.compare("--help") == 0))
    {
        ::help(exec);
        return EXIT_SUCCESS;
    }

    std::string api_key;

    try
    {
        ::load_api_key(api_key);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    ::run_query(api_key, prompt);
    return EXIT_SUCCESS;
}
