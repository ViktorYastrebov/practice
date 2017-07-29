#include <iomanip>
#include <sstream>

#include <iostream>

template<class IntegralType>
std::string to_hex(const IntegralType  &value) {
  std::stringbuf buf;
  std::ostream os(&buf);
  os << "0x" << std::setfill('0') << std::setw(sizeof(value) * 2)  << std::hex << value;
  return buf.str().c_str();

  /*std::stringstream stream;
  stream << "0x"
    << std::setfill('0') << std::setw(sizeof(IntegralType) * 2)
    << std::hex << value;
  return stream.str();*/
}

auto main() -> int {

  int a = 123;
  float b = 256;

  std::cout << "a :" << to_hex(a) << std::endl;
  std::cout << "b :" << to_hex(b) << std::endl;

  return 0;
}

/*
#include <iostream>
#include <boost/program_options.hpp>
#include <string>

namespace po = boost::program_options;

auto main(int argc, char *argv[]) -> int {
  std::string value1;
  std::string value2;
  std::string value3;
  bool isValue;

  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "Show help")
    ("value1,x", po::value(&value1), "Set value1")
    ("value2,y", po::value<std::string>(&value2), "Set value2")
    ("value3,z", po::value<std::string>(&value3), "Set value3")
    ("isValue,y", po::bool_switch(&isValue), "Set isValue")
    ;

  po::variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);

  try {
    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }
    if (vm.count("value1")) {
      value1 = vm["value1"].as<std::string>();
    }
    if (vm.count("value2")) {
      value2 = vm["value2"].as<std::string>();
    }
    if (vm.count("value3")) {
      value3 = vm["value3"].as<std::string>();
    }

    std::cout << "value 1 : " << value1 << std::endl;
    std::cout << "value 2 : " << value2 << std::endl;
    std::cout << "value 3 : " << value3 << std::endl;
    std::cout << "isValue : " << std::boolalpha << isValue << std::endl;

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
*/