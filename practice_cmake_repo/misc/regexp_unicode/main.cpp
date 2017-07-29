
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <boost/regex/icu.hpp>
#include <boost/regex.hpp>

#include <iostream>

void enumerate_numbers(/*const std::string& text*/const icu::UnicodeString  &ustr)
{
  const char* re = "(\\d)+";

  boost::u32regex r = boost::make_u32regex(re, boost::regex_constants::grep);
  //boost::u32regex_token_iterator<std::string::const_iterator>
  boost::u32regex_token_iterator<const UChar  *>
    i(boost::make_u32regex_token_iterator(ustr, r, 1)), j;
  while (i != j)
  {
    //icu::UnicodeString cur(i->str());
    std::wstring wstr(*i);
    std::wcout << *i << std::endl;
    ++i;
  }
}


int main() {

  UnicodeString ustr = UNICODE_STRING_SIMPLE("1$$%1%%%%%20");
  //std::string str = "1$$%1%%%%%20";
  enumerate_numbers(ustr);

  return 0;
}