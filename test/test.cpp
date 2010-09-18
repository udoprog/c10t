#include "Color.h"
#include "Image.h"
#include "2d/cube.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE c10t_tests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_imagebuffer_base )
{
  ImageBuffer cb(10, 10, 10);
  
  BOOST_REQUIRE(cb.get_width() == 10);
  BOOST_REQUIRE(cb.get_height() == 10);
  BOOST_REQUIRE(cb.get_depth() == 10);
  
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 0);
  BOOST_REQUIRE(cb.get_pixel_depth(9, 9) == 0);
  cb.set_pixel_depth(0, 0, 10);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 10);
  cb.set_pixel_depth(9, 9, 10);
  BOOST_REQUIRE(cb.get_pixel_depth(9, 9) == 10);
}

// test non-accumulating height when adding opaque pixels
BOOST_AUTO_TEST_CASE( test_imagebuffer_opaque_acc ) 
{
  ImageBuffer cb(10, 10, 10);
  
  Color c(0x80, 0x80, 0x80, 0xff);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 0);
  cb.add_pixel(0, 0, c);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 1);
  cb.add_pixel(0, 0, c);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 1);
  BOOST_REQUIRE(cb.get_pixel_depth(9, 9) == 0);
}

// test accumulating height when adding transparent pixels
BOOST_AUTO_TEST_CASE( test_imagebuffer_transparent_acc )
{
  ImageBuffer cb(10, 10, 10);
  
  Color c(0x80, 0x80, 0x80, 0xfe);
  Color co(0x80, 0x80, 0x80, 0xff);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 0);
  cb.add_pixel(0, 0, c);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 1);
  cb.add_pixel(0, 0, c);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 2);
  
  BOOST_REQUIRE(cb.get_pixel_depth(9, 9) == 0);
  cb.add_pixel(9, 9, co);
  BOOST_REQUIRE(cb.get_pixel_depth(9, 9) == 1);
}

// test proper setting and reading
BOOST_AUTO_TEST_CASE( test_imagebuffer_all )
{
  ImageBuffer cb(10, 10, 10);
  
  Color c(0x80, 0x80, 0x80, 0xfe);
  Color c2(0x80, 0x80, 0x80, 0xa0);
  Color co(0x80, 0x80, 0x80, 0xff);
  
  cb.add_pixel(0, 0, c);
  cb.add_pixel(0, 1, c);
  cb.add_pixel(0, 1, c2);
  cb.add_pixel(9, 9, co);
  cb.add_pixel(9, 9, co);

  BOOST_REQUIRE(cb.get_pixel_depth(0, 0) == 1);
  BOOST_REQUIRE(cb.get_pixel_depth(0, 1) == 2);
  BOOST_REQUIRE(cb.get_pixel_depth(9, 9) == 1);

  {
    Color r;
    cb.get_pixel(0, 0, 0, r);
    BOOST_REQUIRE(r.a == c.a);
  }

  {
    Color r;
    cb.get_pixel(0, 1, 0, r);
    BOOST_REQUIRE(r.a == c.a);
    cb.get_pixel(0, 1, 1, r);
    BOOST_REQUIRE(r.a == c2.a);
  }
}

BOOST_AUTO_TEST_CASE( test_imagebuffer_size )
{
  ImageBuffer cb(10, 10, 10);
  Color c(0x80, 0x80, 0x80, 0xfe);
  cb.set_pixel(9, 9, 9, c);
}

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
