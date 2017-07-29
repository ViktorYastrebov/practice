#pragma once

#include "connection.hpp"

//numeric operations
SQL_NUMERIC_STRUCT numberFromString(const std::string  &str);
SQL_NUMERIC_STRUCT numberFromString(const std::string  &str, int precision, int scale);

std::string numberToString(const SQL_NUMERIC_STRUCT  &num);

SQL_DATE_STRUCT  dateFromString(const std::string  &str);
std::string dateToString(const SQL_DATE_STRUCT  &date);