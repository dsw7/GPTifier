#include "responses.hpp"
#include "params.hpp"
#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

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
        ::print_separator();

        std::cout << "\033[1mResults:\033[31m " + error + "\033[0m\n";
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        ::print_separator();

        std::cout << "\033[1mResults:\033[32m " + content + "\033[0m\n";
    }

    ::print_separator();
}

void write_message_to_file(const std::string &message)
{
    std::string path_completion = ::get_proj_home_dir() + "/completions.gpt";

    std::cout << "> Writing reply to file " + path_completion + '\n';

    std::ofstream st_filename(path_completion, std::ios::app);
    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + path_completion);
    }

    std::string separator(110, '=');
    st_filename << separator + '\n';
    st_filename << "[GPTifier] Results:\n";
    st_filename << separator + '\n';
    st_filename << message << '\n';

    st_filename.close();
}

void export_chat_completion_response(const ::str_response &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        std::cerr << "Cannot export results as error occurred\n";
        ::print_separator();
        return;
    }

    std::cout << "\033[1mExport:\033[0m\n";
    std::string choice;

    while (true)
    {
        std::cout << "> Write reply to file? [y/n]: ";
        std::cin >> choice;

        if (choice.compare("y") == 0)
        {
            break;
        }
        else if (choice.compare("n") == 0)
        {
            break;
        }
        else
        {
            std::cout << "> Invalid choice. Input either 'y' or 'n'!\n";
        }
    }

    if (choice.compare("n") == 0)
    {
        std::cout << "> Not exporting response.\n";
    }
    else
    {
        write_message_to_file(results["choices"][0]["message"]["content"]);
    }

    ::print_separator();
}

void dump_chat_completion_response(const ::str_response &response)
{
    std::cout << "Dumping results to " + params.dump + '\n';
    std::ofstream st_filename(params.dump);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open '" + params.dump + "'");
    }

    nlohmann::json results = nlohmann::json::parse(response);
    static short int indent_pretty_print = 2;

    st_filename << std::setw(indent_pretty_print) << results;
    st_filename.close();
}

} // namespace responses
