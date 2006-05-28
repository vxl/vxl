// This is core/vil1/io/tests/test_memory_image_format_io.cxx
#include <vcl_iostream.h>
#include <vil1/io/vil1_io_memory_image_format.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_memory_image_format_io()
{
  vcl_cout << "*******************************************\n"
           << "Testing vil1_memory_image_format<double> io\n"
           << "*******************************************\n";
  //// test constructors, accessors
  vil1_memory_image_format p_out, p_in;
  p_out.components=2;
  p_out.bits_per_component=32;
  p_out.component_format=VIL1_COMPONENT_FORMAT_IEEE_FLOAT;

  vsl_b_ofstream bfs_out("vil1_memory_image_format_test_double_io.bvl.tmp");
  TEST ("Created vil1_memory_image_format_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vil1_memory_image_format_test_double_io.bvl.tmp");
  TEST ("Opened vil1_memory_image_format_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vil1_memory_image_format_test_double_io.bvl.tmp");

  TEST ("p_out == p_in", p_out.components==p_in.components &&
        p_out.bits_per_component==p_in.bits_per_component &&
        p_out.component_format==p_in.component_format , true);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
}


TESTMAIN( test_memory_image_format_io );
