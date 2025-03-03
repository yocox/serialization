
#include "visit_member.h"

#include "serialize.h"

#include <gtest/gtest.h>

struct Point
{
  int x = 3;
  int y = 5;
};

TEST(VisitMember, MemberCount) { EXPECT_EQ(member_count<Point>(), 2); }

TEST(VisitMember, VisitMembers)
{
  Point p;
  int sum = 0;
  visit_members(p, [&](auto i) { sum += i; });
  EXPECT_EQ(sum, 8);
}

TEST(VisitMember, Point)
{
  std::ostringstream oss;
  OutputArchive oar(oss);

  Point p;
  oar.serialize(p);

  std::cout << "s = " << oss.str() << std::endl;
}