#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <sstream>
#include <vector>

namespace {

std::string read_file_contents(const std::string &filename) {
	std::ifstream file(filename);
	return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

template<typename T>
void write_file_contents(const std::string &filename, const T &contents) {
	std::ofstream file(filename);
	file << contents;
}

} /* namespace <anonymous> */

int main(int argc, char *argv[]) {
	namespace po = boost::program_options;

	const std::string dirname = "/sys/class/backlight/intel_backlight";

	unsigned int max_brightness;
	unsigned int brightness;

	double current_ratio;

	int delta;


	po::options_description desc("Options");
	desc.add_options()
		("inc,i", po::value<std::vector<unsigned int>>()->composing()
		 ->notifier([&delta](const auto &vs) { for(auto v: vs) delta += v; }), "increase brightness by percentage")
		("dec,d", po::value<std::vector<unsigned int>>()->composing()
		 ->notifier([&delta](const auto &vs) { for(auto v: vs) delta -= v; }), "decrease brightness by percentage")
		("print,p", "print the brightness")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	{
		std::istringstream brstr(read_file_contents(dirname + "/brightness"));
		brstr >> brightness;
	}

	{
		std::istringstream mbrstr(read_file_contents(dirname + "/max_brightness"));
		mbrstr >> max_brightness;
	}

	current_ratio = static_cast<double>(brightness) / max_brightness;

	double new_ratio = current_ratio + static_cast<double>(delta) / 100;
	if(new_ratio < 0.0) new_ratio = 0.0;
	else if(new_ratio > 1.0) new_ratio = 1.0;

	brightness = static_cast<int>(new_ratio * max_brightness);

	write_file_contents(dirname + "/brightness", brightness);

	if(vm.find("print") != vm.end()) {
		std::cout << "brightness: " << brightness << '/' << max_brightness << " (" << static_cast<unsigned int>(new_ratio * 100) << "%)\n";
	}
}
