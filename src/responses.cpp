#include "responses.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>

namespace responses
{

void print_chat_completion_response(const ::str_response &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        results["error"]["message"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        utils::print_separator();

        std::cout << "\033[1mResults:\033[31m " + error + "\033[0m\n";
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        utils::print_separator();

        std::cout << "\033[1mResults:\033[32m " + content + "\033[0m\n";
    }

    utils::print_separator();
}

void dump_chat_completion_response(const ::str_response &response, const std::string &filename)
{
    std::cout << "Dumping results to " + filename + '\n';
    std::ofstream st_filename(filename);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + filename);
    }

    nlohmann::json results = nlohmann::json::parse(response);
    static short int indent_pretty_print = 2;

    st_filename << std::setw(indent_pretty_print) << results << std::endl;
    st_filename.close();
}

} // namespace responses
