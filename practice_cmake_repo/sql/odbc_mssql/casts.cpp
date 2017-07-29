#include "common.h"

#include <regex>
#include "boost/multiprecision/cpp_int.hpp"
#include <sstream>

SQL_NUMERIC_STRUCT numberFromString(const std::string  &str) {
  SQL_NUMERIC_STRUCT ret;
  std::memset(&ret, 0, sizeof(SQL_NUMERIC_STRUCT));

  boost::multiprecision::int128_t accum;
  int precision = 0;
  int scale = 0;
  std::for_each(str.begin(), str.end(), [&](const std::string::value_type  &c)
  {
    if (isdigit(c)) {
      accum *= 10;
      accum += c - '0';
      precision++;
    }
    if (c == '.' || c == ',') {
      scale = str.length() - precision - 1;
    }
  });

  int offset = 0;
  boost::multiprecision::limb_type  *limbs = accum.backend().limbs();
  int limbs_amount = accum.backend().size();

  const int limb_bytes_size = sizeof(boost::multiprecision::limb_type);

  for (int i = 0; i < limbs_amount; ++i) {
    std::memcpy(ret.val + offset, &limbs[i], sizeof(boost::multiprecision::limb_type));
    offset += limb_bytes_size;
  }

  ret.precision = precision;
  ret.scale = scale;
  ret.sign = 1;

  return ret;
}

SQL_NUMERIC_STRUCT numberFromString(const std::string  &str, int precision, int scale) {
  SQL_NUMERIC_STRUCT ret = numberFromString(str);
  ret.precision = precision;
  ret.scale = scale;
  return ret;
}

std::string numberToString(const SQL_NUMERIC_STRUCT  &num) {
  boost::multiprecision::int128_t value = 0;
  boost::multiprecision::int128_t last = 1;
  int idx = 0;
  while (num.val[idx] != 0) {
    unsigned char current_value = num.val[idx];
    unsigned char high = current_value >> 4;
    unsigned char low = current_value & 0x0F;
    if (low) {
      value += last * low;
    }
    last  *= 16;
    if (high) {
      value += last * high;
    }
    last *= 16;
    ++idx;
  }
  std::string ret = value.str();
  std::size_t len = ret.length();

  if (num.scale > 0 && num.scale < len) {
    ret.insert(len - num.scale, 1, '.');
  }
  return ret;
}


SQL_DATE_STRUCT  dateFromString(const std::string  &str) {
  SQL_DATE_STRUCT ret;
  std::memset(&ret, 0, sizeof(SQL_DATE_STRUCT));

  try {
    //order year(4) -> month(2) -> date(2)
    std::regex date_regexp("^(\\d{4})(-|.|/)(\\d{2})(-|.|/)(\\d{2})");
    std::match_results<std::string::const_iterator> results;
    if (std::regex_match(str, results, date_regexp)) {
      std::string year_str = results[1].str();
      std::string month = results[3].str();
      std::string day = results[5].str();

      ret.year = std::stoi(year_str);
      ret.month = std::stoi(month);
      ret.day = std::stoi(day);
    }
  }
  catch (const std::exception  &ex) {
    ex.what();
  }
  return ret;
}

std::string dateToString(const SQL_DATE_STRUCT  &date) {
  std::string ret = std::to_string(date.year) + "-" + std::to_string(date.month) + "-" + std::to_string(date.day);
  return ret;
}