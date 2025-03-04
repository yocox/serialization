#include "serialize.h"

#include <gtest/gtest.h>

#include <sstream>
#include <type_traits>
#include <typeindex>

struct Node
{
  int data;
  Node* next;

  template <typename Archive>
  void serialize(Archive& ar) const
  {
    ar(data);
    ar(next);
  }

  template <typename Archive>
  void deserialize(Archive& ar)
  {
    ar(data);
    ar(next);
  }
};

TEST(StdType, Pointer)
{
  registerFactory<Node>();
  Node n1{19, nullptr};
  Node n2{91, &n1};

  std::ostringstream oss;
  OutputArchive oar(oss);

  oar(&n2);
  std::cout << "s = " << oss.str() << std::endl;

  std::istringstream iss(oss.str());
  InputArchive iar(iss);
  Node* n3 = nullptr;
  iar(n3);

  EXPECT_EQ(n3->data, 91);
  EXPECT_EQ(n3->next->data, 19);
}

TEST(StdType, TypeIndex)
{
  std::type_index ti = typeid(int);

  std::cout << typeid(int).name() << std::endl;
}