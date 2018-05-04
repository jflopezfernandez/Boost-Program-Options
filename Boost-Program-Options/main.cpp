
/** Project: Boost-Program-Options
 *
 *  Author: Jose Fernando Lopez Fernandez
 *
 *  Date: 04-May-2018
 *
 *  Description: For this project, I'm supposing I'm trying to parse options
 *  from the point of view of a project meta-generator. These
 *  will be the targeted use-cases of my experimentation.
 *
 */

#include <boost/chrono.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/ratio.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/** There will be a need to combine the values from the command line and
 *  configuration file. For example, the optimization level specified on the 
 *  command line should override the value from the config file. On the other 
 *  hand, include paths should be combined.
 *
 */

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector) {
	std::copy(vector.begin(), vector.end(), std::ostream_iterator<T>(os, " "));

	return os;
}

const std::string DEFAULT_PROJECT_LANGUAGE = "C";

namespace Options = boost::program_options;

using OptsDescription = boost::program_options::options_description;


int main(int argc, char *argv[])
{
	try {
		std::string configFileName;

		int optimizationLevel = 0;

		boost::optional<std::string> projectLanguage = boost::make_optional<std::string>("");

		// Declare a group of options that will be allowed only on the command line
		OptsDescription generic("Generic options");
		generic.add_options()
			("version,v", "Print program version")
			("help,h", "Print help dialog");

		// Declare a group of options that will be allowed both on the command
		// line and in the configuration file
		OptsDescription config("Configuration");
		config.add_options()
			("config,c", Options::value<std::string>(&configFileName), "Configuration file")
			("language,l", Options::value(&projectLanguage), "Project language")
			("optimization-level,O", Options::value<int>(&optimizationLevel)->default_value(0), "Optimization level")
			("include-path,I", Options::value<std::vector<std::string>>()->composing(), "Include path")
			("classes", Options::value<std::vector<std::string>>(), "Classes");

		// Hidden options which will be allowed both on the command line and
		// in the configuration file, but which will not be shown to the user
		OptsDescription hidden("Hidden options");
		hidden.add_options()
			("input-file", Options::value<std::vector<std::string>>(), "Input file(s)");

		OptsDescription commandLineOptions;
		commandLineOptions.add(generic).add(config).add(hidden);

		OptsDescription configFileOptions;
		configFileOptions.add(config).add(hidden);

		OptsDescription visible("Allowed options");
		visible.add(generic).add(config);

		Options::positional_options_description positionalOptsDesc;
		positionalOptsDesc.add("classes", -1);

		Options::variables_map variablesMap;
		Options::store(Options::command_line_parser(argc, argv).
			options(commandLineOptions).positional(positionalOptsDesc).run(), variablesMap);
		Options::notify(variablesMap);

		if (variablesMap.count("config")) {
			std::ifstream ifs(configFileName.c_str());
			if (!ifs) {
				std::cerr << "[Error]: Could not open config file...\n";

				exit(EXIT_FAILURE);
			} else {
				Options::store(Options::parse_config_file(ifs, configFileOptions), variablesMap);
				Options::notify(variablesMap);
			}
		}

		if (variablesMap.count("help")) {
			std::cout << "Usage: Boost-Program-Options [options]\n"
				<< commandLineOptions
				<< "\n";

			return EXIT_SUCCESS;
		}

		if (variablesMap.count("version")) {
			std::cout << "<Project Name>: Version "
				<< "0.0.1"
				<< ".\n";

			return EXIT_SUCCESS;
		}

		if (variablesMap.count("include-path")) {
			std::cout << "Include path: "
				<< variablesMap["include-path"].as<std::vector<std::string>>()
				<< "\n";
		}

		if (variablesMap.count("input-file")) {
			std::cout << "Input file(s): "
				<< variablesMap["input-file"].as<std::vector<std::string>>()
				<< "\n";
		}

		if (variablesMap.count("classes")) {
			std::cout << "Classes: "
				<< variablesMap["classes"].as<std::vector<std::string>>()
				<< "\n";
		}

		if (!(*projectLanguage).empty()) {
			std::cout << "Project Language: "
				<< *projectLanguage
				<< "\n";
		} else {
			std::cout << "Project language set to default: "
				<< (*projectLanguage = DEFAULT_PROJECT_LANGUAGE)
				<< ".\n";
		}
	} catch (std::exception& e) {
		std::cerr << "[Error]: " << e.what() << "\n";
	}

	return EXIT_SUCCESS;
}