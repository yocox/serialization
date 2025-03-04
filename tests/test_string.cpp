
#include "serialize.h"

#include <gtest/gtest.h>

TEST(StdType, String)
{
  std::ostringstream oss;
  OutputArchive oar(oss);
  std::string v = "abcdef";
  oar(v);
  v = "xyz";

  std::istringstream iss(oss.str());
  InputArchive iar(iss);
  iar(v);

  EXPECT_EQ(v, "abcdef");
}
