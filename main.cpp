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

  OutputArchive& serialize(OutputArchive& ar) const
  {
    ar.serialize(name);
    ar.serialize(age);
    ar.serialize(gender);
    return ar;
  }
  std::istream& deserialize(std::istream& in)
  {
    in >> name >> age >> gender;
    return in;
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