#include "serialize.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(StdType, Arithmetics)
{
  std::ostringstream oss;
  OutputArchive oar(oss);
  int v = 7;
  oar.serialize(v);
  float f = 3.14;
  oar.serialize(f);

  std::istringstream iss(oss.str());
  InputArchive iar(iss);
  int v2;
  float f2;
  iar.deserialize(v2);
  iar.deserialize(f2);

  EXPECT_EQ(v2, v);
  EXPECT_FLOAT_EQ(f2, f);
}
