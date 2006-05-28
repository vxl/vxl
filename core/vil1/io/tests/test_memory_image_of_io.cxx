// This is core/vil1/io/tests/test_memory_image_of_io.cxx
#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/io/vil1_io_memory_image_of.h>
#include <vil1/io/vil1_io_image_impl.h>
#include <vil1/io/vil1_io_memory_image_impl.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_memory_image_of_io()
{
  vcl_cout << "*******************************\n"
           << "Testing vil1_memory_image_of io\n"
           << "*******************************\n";

  // Image set-up and construction
  int height = 10;
  int width = 10;
  vil1_memory_image_of<int> p_out(height,width);
  vil1_memory_image_of<int> p_in(1,1);
  int val = 0;
  for (int i=0;i<width;i++)
    for (int j=0;j<height;j++)
    {
       p_out(i,j) = val;
       val++;
    }

   // Add to the list of loaders...
   vsl_add_to_binary_loader(vil1_io_memory_image_impl());

  // Test the save
  vsl_b_ofstream bfs_out("vil1_memory_image_of_io.bvl.tmp");
  TEST("Created vil1_memory_image_of_io.bvl.tmp for writing", !bfs_out, false);
  vsl_b_write(bfs_out,p_out);
  bfs_out.close();

  // And the load
  vsl_b_ifstream bfs_in("vil1_memory_image_of_io.bvl.tmp");
  TEST("Opened vil1_memory_image_of_io.bvl.tmp for reading", !bfs_in, false);
  vsl_b_read(bfs_in,p_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vil1_memory_image_of_io.bvl.tmp");

  // Compare the images' size and so on
  TEST("p_out == p_in (structure)",
       p_out.size() == p_in.size() &&
       p_out.planes() == p_in.planes() &&
       p_out.height() == p_in.height() &&
       p_out.components() && p_in.components() &&
       p_out.bits_per_component() == p_in.bits_per_component() &&
       p_out.component_format() == p_in.component_format(), true);

   // Now get the data and compare them
  bool data_same = false;
  for (int i=0;i<p_out.cols();i++)
    for (int j=0;j<p_out.rows();j++)
     data_same = (p_out(i,j)==p_in(i,j));
  TEST("p_out == p_in (data)", data_same, true);

  // And have a look at the summary
  vsl_print_summary(vcl_cout, p_in);
  vcl_cout << vcl_endl;
}

TESTMAIN( test_memory_image_of_io );
