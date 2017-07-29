#include <iostream>
#include <iomanip>
#include "mpirxx.h"

#include <sstream>

std::string format_mpz(const std::string  &str) {
  std::string ret;
  std::for_each(str.begin(), str.end(), [&](const std::string::value_type &val)
  {
    if(isdigit(val) && (val != '.' || val != ',')) {
      ret += val;
    }
  });
  return ret;
}


void mpf_example() {
  mpf_class pi("3.141592653589793238", 1024, 10);
  mpf_class pi2("3.141592653589793238", 1024, 10); //3,141592653589793238
  mpf_class result;
  result.set_prec(1024);
  result = pi*pi2;



  std::cout << "pi.precision() = " << pi.get_prec() << std::endl;
  std::cout << "pi.value = " << std::setprecision(30) << pi << std::endl;

  mp_exp_t expr = 1;
  std::cout << "precision = " << result.get_prec() << std::endl;
  std::cout << "value = " << result.get_str(expr) << "exp = " << expr << std::endl;

  result.set_str("12345678901234567890.30", 10);

  std::cout << "precision = " << result.get_prec() << std::endl;
  std::cout << "value = " << std::setprecision(30) << result << std::endl;
}

void mpz_example() {
  mpz_class val1(format_mpz("100'123'000'456'000'789'000'123'000'456'000"), 10);
  std::cout << val1 << std::endl;
}

void mpz_to_hex() {
  //can be the solution for MSSQL server numeric convetion
  mpz_class value(format_mpz("1'234"));

  std::ostringstream oss;
  oss << std::hex << value;
  std::cout << "hex value :" << oss.str() << std::endl;
}

auto main() -> int {

  //mpf_example();
  //mpz_example();
  mpz_to_hex();
  return 0;
}