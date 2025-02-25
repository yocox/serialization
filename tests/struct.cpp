#include "serialize.h"

#include <gtest/gtest.h>

#include <memory>
#include <typeindex>

struct person
{
  int id;
  int age{};
};

struct Base
{
  int n = 3;
  virtual ~Base() = default;
};
struct Derived1 : Base
{
  int m = 5;
  virtual ~Derived1() = default;
};
struct Derived2 : Base
{
  int k = 7;
  virtual ~Derived2() = default;
};

// A map from type_index to ctor
std::map<std::type_index, std::function<std::unique_ptr<Base>()>> factory;

template <typename T>
void register_type()
{
  factory[typeid(T)] = []() { return std::make_unique<T>(); };
}

TEST(Polymorphism, Base)
{
  register_type<Base>();
  register_type<Derived1>();
  register_type<Derived2>();

  std::unique_ptr<Base> b = std::make_unique<Base>();
  std::unique_ptr<Base> d1 = std::make_unique<Derived1>();
  std::unique_ptr<Base> d2 = std::make_unique<Derived2>();
}
