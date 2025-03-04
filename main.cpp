#include "serialize.h"

#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

struct Person
{
  std::string name;
  int age;
  int gender;

  template <typename Archive>
  Archive& serialize(Archive&& ar)
  {
    ar(name, age, gender);
    return ar;
  }
};

int main()
{
  std::cout << type_id<int>() << std::endl;
  std::cout << type_id<float>() << std::endl;
  std::cout << type_id<Person>() << std::endl;
  std::ofstream fout("data.txt");
  Person p{"yoco", 25, 1};
}