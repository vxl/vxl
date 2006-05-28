// This is core/vil1/io/tests/test_memory_image_impl_io.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_memory_image_impl.h>
#include <vil1/vil1_memory_image_of_format.txx>
#include <vil1/io/vil1_io_memory_image_impl.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_memory_image_impl_io()
{
  vcl_cout << "*********************************\n"
           << "Testing vil1_memory_image_impl io\n"
           << "*********************************\n";

  // Image set-up and construction
  int planes = 1;
  int height = 3;
  int width = 4;
  vil1_memory_image_of_format<int> format;

  // We need an image_of<foo> to get a sensible buffer
  vil1_memory_image_of<int> imageof(height,width);
  int val = 0;
  for (int i=0;i<width;i++)
    for (int j=0;j<height;j++)
    {
       imageof(i,j) = val;
       val++;
    }
  vil1_memory_image_impl p_out(planes,height,width,
                               format.components,
                               format.bits_per_component,
                               format.component_format);
  p_out.put_section(imageof.get_buffer(), 0, 0, width, height);

  vil1_memory_image_of_format<int> format2;
  vil1_memory_image_impl p_in(0,0,0,
                              format2.components,
                              format2.bits_per_component,
                              format2.component_format);

  // Construct the loader object
  vil1_io_memory_image_impl io_impl;
  vsl_add_to_binary_loader(vil1_io_memory_image_impl());


  // Test the save
  vsl_b_ofstream bfs_out("vil1_memory_image_impl_io.bvl.tmp");
  TEST ("Created vil1_memory_image_imple_io.bvl.tmp for writing",
        (!bfs_out), false);
  io_impl.b_write_by_base(bfs_out,p_out);
  bfs_out.close();

  // And the load
  vsl_b_ifstream bfs_in("vil1_memory_image_impl_io.bvl.tmp");
  TEST ("Opened vil1_memory_image_impl_io.bvl.tmp for reading",
        (!bfs_in), false);
  io_impl.b_read_by_base(bfs_in,p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vil1_memory_image_impl_io.bvl.tmp");

  // Compare the images' size and so on
  TEST ("p_out == p_in (structure)",
        p_out.planes() == p_in.planes() &&
        p_out.height() == p_in.height() &&
        p_out.width() == p_in.width() &&
        p_out.components() == p_in.components() &&
        p_out.bits_per_component() == p_in.bits_per_component() &&
        p_out.component_format() == p_in.component_format(), true);

   // Now get the data and compare them
  vcl_vector<int> buf1(imageof.size());
  p_out.get_section(&buf1[0], 0, 0, width, height);
  vcl_vector<int> buf2(imageof.size());
  p_in.get_section(&buf2[0], 0, 0, width, height);
  bool data_same = false;
  for (unsigned int i=0;i<buf1.size();i++)
     data_same = (buf1[i]==buf2[i]);
  TEST ( "p_out == p_in (data)", data_same, true);

  // And have a look at the summary
  vsl_print_summary(vcl_cout, p_in);
  vcl_cout << vcl_endl;
}


TESTMAIN( test_memory_image_impl_io );
