// This is mul/vil3d/tests/test_image_resource.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_print.h>

template <class T>
inline void test_image_resource(vcl_string type, vil_pixel_format format, T /*dummy*/)
{
  vcl_cout << "******************************************************************\n"
           << " Testing vil3d_image_resource objects with pixel type = " << type << '\n'
           << "******************************************************************\n";

  vil3d_image_resource_sptr mem = vil3d_new_image_resource(10,5,3,1,format);

  TEST("vil3d_new_image_resource", mem?true:false, true);

  vil3d_image_view<T> view1 = mem->get_view(0, mem->ni(), 0, mem->nj(), 0, mem->nk());
  TEST("vil3d_memory_image::get_view()",
       view1 && view1.ni()==10 && view1.nj()==5 &&
       view1.nk()==3 && view1.nplanes()==1, true );

  view1.fill(0);

  TEST("vil3d_memory_image::put_view()", mem->put_view(view1,0,0,0), true);

  vil3d_image_view<T> view2 = mem->get_copy_view(0, mem->ni(), 0, mem->nj(), 0, mem->nk());
  TEST("vil3d_memory_image::get_copy_view()",
       view2 && view2.ni()==10 && view2.nj()==5 &&
       view2.nk()==3 && view2.nplanes()==1, true );

  view2.set_size(5,2,2);
  view2.fill(10);

  TEST("vil3d_memory_image::put_view(copy)", mem->put_view(view2,5,3,1), true);

  view1 = mem->get_view(0, mem->ni(), 0, mem->nj(), 0, mem->nk());
  vil3d_print_all(vcl_cout, view1);


  T v1, v2;

  vil3d_math_value_range(view1, v1, v2);
  TEST("Value range is 0,10", v1 == 0 && v2 == 10, true);
}

static void test_image_resource()
{
  test_image_resource("float", VIL_PIXEL_FORMAT_FLOAT, float());
  test_image_resource("double", VIL_PIXEL_FORMAT_DOUBLE, double());
  test_image_resource("vxl_byte", VIL_PIXEL_FORMAT_BYTE, vxl_byte());
  test_image_resource("vxl_sbyte", VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte());
  test_image_resource("vxl_int_16", VIL_PIXEL_FORMAT_INT_16, vxl_int_16());
  test_image_resource("vxl_uint_16", VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16());
  test_image_resource("vxl_int_32", VIL_PIXEL_FORMAT_INT_32, vxl_int_32());
  test_image_resource("vxl_uint_32", VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32());
}

TESTMAIN(test_image_resource);
