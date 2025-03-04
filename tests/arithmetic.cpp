#include "serialize.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(StdType, Arithmetics)
{
  std::ostringstream oss;
  OutputArchive oar(oss);
  int v = 7;
  oar(v);
  float f = 3.14;
  oar(f);

  std::istringstream iss(oss.str());
  InputArchive iar(iss);
  int v2;
  float f2;
  iar(v2);
  iar(f2);

  EXPECT_EQ(v2, v);
  EXPECT_FLOAT_EQ(f2, f);
}
