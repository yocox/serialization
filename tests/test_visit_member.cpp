
#include "serialize.h"
#include "visit_member.h"

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
  oar(p);

  std::cout << "s = " << oss.str() << std::endl;

  std::istringstream iss(oss.str());
  InputArchive iar(iss);

  Point p2{7, 9};
  iar(p2);

  EXPECT_EQ(p.x, p2.x);
  EXPECT_EQ(p.y, p2.y);
}