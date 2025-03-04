#include "serialize.h"

#include <gtest/gtest.h>

#include <memory>
#include <typeindex>

struct Base
{
  int n = 3;
  virtual ~Base() = default;
  // template <typename Archive>
  virtual void serialize(OutputArchive& ar) const { ar(n); }
  virtual void serialize(InputArchive& ar) { ar(n); }
};
struct Derived1 : Base
{
  int m = 5;
  virtual ~Derived1() = default;
  virtual void serialize(OutputArchive& ar) const override
  {
    Base::serialize(ar);
    ar(m);
  }
  virtual void serialize(InputArchive& ar) override
  {
    Base::serialize(ar);
    ar(m);
  }
};
struct Derived2 : Base
{
  int k = 7;
  virtual ~Derived2() = default;
  virtual void serialize(OutputArchive& ar) const override
  {
    Base::serialize(ar);
    ar(k);
  }
  virtual void serialize(InputArchive& ar) override
  {
    Base::serialize(ar);
    ar(k);
  }
};

// A map from type_index to ctor
std::map<std::type_index, std::function<std::unique_ptr<Base>()>> factory;

template <typename T>
void register_type()
{
  factory[typeid(T)] = []() { return std::make_unique<T>(); };
}

TEST(Polymorphism, Shape)
{
  registerFactory<Base>();
  registerFactory<Derived1>();
  registerFactory<Derived2>();

  Base* b = new Base();
  Base* d1 = new Derived1();
  Base* d2 = new Derived2();

  std::ostringstream oss;
  OutputArchive oar(oss);

  oar(d1, d2, b);

  std::cout << "s = " << oss.str() << std::endl;

  std::istringstream iss(oss.str());
  InputArchive iar(iss);

  Base* b1 = nullptr;
  Base* b2 = nullptr;
  Base* b3 = nullptr;

  iar(b2, b3, b1);

  EXPECT_EQ(b1->n, 3);
  EXPECT_EQ(b2->n, 3);
  EXPECT_EQ(b3->n, 3);
  EXPECT_EQ(dynamic_cast<Derived1*>(b2)->m, 5);
  EXPECT_EQ(dynamic_cast<Derived2*>(b3)->k, 7);
}
