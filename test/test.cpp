#include "Color.h"
#include "Image.h"

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
