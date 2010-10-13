#include "color.h"
#include "image.h"
#include "2d/cube.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE c10t_tests
#include <boost/test/unit_test.hpp>


#include <iostream>

BOOST_AUTO_TEST_CASE( test_cube_projection_1 )
{
  // x, y, z
  Cube c(10, 10, 20);
  point p1(0, 0, 0), p2(0, 0, 20), p3(10, 0, 20), p4(10, 0, 0);
  int x, y;
  
  {
    c.project_top(p1, x, y);
    BOOST_REQUIRE(x == 0 && y == 0);
    c.project_top(p2, x, y);
    BOOST_REQUIRE(x == 0 && y == 20);
    c.project_top(p3, x, y);
    BOOST_REQUIRE(x == 10 && y == 20);
    c.project_top(p4, x, y);
    BOOST_REQUIRE(x == 10 && y == 0);
  }
  
  {
    c.project_oblique(p1, x, y);
    BOOST_REQUIRE(x == 0 && y == 10);
    c.project_oblique(p2, x, y);
    BOOST_REQUIRE(x == 0 && y == 30);
    c.project_oblique(p3, x, y);
    BOOST_REQUIRE(x == 10 && y == 30);
    c.project_oblique(p4, x, y);
    BOOST_REQUIRE(x == 10 && y == 10);
  }
  
  {
    c.project_obliqueangle(p1, x, y);
    BOOST_REQUIRE(x == 20 && y == 10);
    c.project_obliqueangle(p2, x, y);
    BOOST_REQUIRE(x == 0 && y == 30);
    c.project_obliqueangle(p3, x, y);
    BOOST_REQUIRE(x == 10 && y == 40);
    c.project_obliqueangle(p4, x, y);
    BOOST_REQUIRE(x == 30 && y == 20);
  }
}

BOOST_AUTO_TEST_CASE( test_cube_projection_2 )
{
  // x, y, z
  Cube c(20, 10, 10);
  point p1(0, 0, 0), p2(0, 0, 10), p3(20, 0, 10), p4(20, 0, 0);
  int x, y;
  
  {
    c.project_top(p1, x, y);
    BOOST_REQUIRE(x == 0 && y == 0);
    c.project_top(p2, x, y);
    BOOST_REQUIRE(x == 0 && y == 10);
    c.project_top(p3, x, y);
    BOOST_REQUIRE(x == 20 && y == 10);
    c.project_top(p4, x, y);
    BOOST_REQUIRE(x == 20 && y == 0);
  }
  
  {
    c.project_oblique(p1, x, y);
    BOOST_REQUIRE(x == 0 && y == 10);
    c.project_oblique(p2, x, y);
    BOOST_REQUIRE(x == 0 && y == 20);
    c.project_oblique(p3, x, y);
    BOOST_REQUIRE(x == 20 && y == 20);
    c.project_oblique(p4, x, y);
    BOOST_REQUIRE(x == 20 && y == 10);
  }
  
  {
    c.project_obliqueangle(p1, x, y);
    BOOST_REQUIRE(x == 10 && y == 10);
    c.project_obliqueangle(p2, x, y);
    BOOST_REQUIRE(x == 0 && y == 20);
    c.project_obliqueangle(p3, x, y);
    BOOST_REQUIRE(x == 20 && y == 40);
    c.project_obliqueangle(p4, x, y);
    BOOST_REQUIRE(x == 30 && y == 30);
  }
}
