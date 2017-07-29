
#include <iostream>
#include "syb.hpp"

int main(int argc, char*argv[]) {

  Company company(
    append(Dept("Research", Employee(Person("Ralf", "Amsterdam"), Salary(8000)),
    append(PersonUnit(Employee(Person("Joost", "Amsterdam"), Salary(1000))),
    append(PersonUnit(Employee(Person("Marlow", "Cambridge"), Salary(2000))),
    std::list<SubUnit>()))),
    append(Dept("Strategy", Employee(Person("Blair", "London"), Salary(100000)),
    std::list<SubUnit>()),
    std::list<Dept>())));

  increase(0.20f, company);
  std::cout << at<1>(at<2>(at<2>(at<1>(company).front()))) << std::endl;
  //std::cout << at<1>(at<1>(company).front()) << std::endl;

  return 0;
}
