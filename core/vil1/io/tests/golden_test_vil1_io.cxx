// This is core/vil1/io/tests/golden_test_vil1_io.cxx

//:
// \file
// \brief Read in a golden data file, and check the values are correct.
//
// If you need to recreate the golden data file build this as a
// standalone program, and run it with the single parameter create:
// \verbatim
// golden_test_vnl_io create
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vsl/vsl_binary_io.h>
#include <vil1/vil1_memory_image_of_format.txx>
#include <vil1/vil1_memory_image_impl.h>
#include <vil1/io/vil1_io_image.h>
#include <vil1/io/vil1_io_image_impl.h>
#include <vil1/io/vil1_io_memory_image.h>
#include <vil1/io/vil1_io_memory_image_format.h>
#include <vil1/io/vil1_io_memory_image_impl.h>
#include <vil1/io/vil1_io_memory_image_of.h>
#include <vil1/io/vil1_io_rgb.h>
#include <vil1/io/vil1_io_rgba.h>

#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

static void golden_test_vil1_io(bool save_file=false)
{
  vcl_cout << "***********************************************************\n"
           << " Testing a golden data file for cross platform consistency\n"
           << "***********************************************************\n";

  //----------------------------------------------------------------------
  // Create objects:
  // If the "create" flag was used on the command line, then the program
  // saves an example of each class
  // Otherwise it just fills them with values for comparison to the values
  // read in.
  //----------------------------------------------------------------------

  // vil1_memory_image_of
  int height = 9;
  int width = 11; // on purpose not equal to height
  int planes = 1;
  vil1_memory_image_of<int> p_out_memory_image_of(width,height),
                           p_in_memory_image_of(1,1);

  int val = 0;
  for (int i=0;i<width;i++)
    for (int j=0;j<height;j++)
      p_out_memory_image_of(i,j) = val++;

    // vil1_memory_image_impl
    vil1_memory_image_of_format<int> memory_image_of_format;
    vil1_memory_image_impl p_out_memory_image_impl(planes,width,height,
                                                   memory_image_of_format.components,
                                                   memory_image_of_format.bits_per_component,
                                                   memory_image_of_format.component_format);
    p_out_memory_image_impl.put_section(p_out_memory_image_of.get_buffer(),
                                        0, 0, width, height);
    vil1_memory_image_of_format<double> format2;
    vil1_memory_image_impl p_in_memory_image_impl(0,0,0,
                                                  format2.components,
                                                  format2.bits_per_component,
                                                  format2.component_format);


    // vil1_memory_image_format
    vil1_memory_image_format p_out_memory_image_format, p_in_memory_image_format;
    p_out_memory_image_format.components=2;
    p_out_memory_image_format.bits_per_component=32;
    p_out_memory_image_format.component_format=VIL1_COMPONENT_FORMAT_IEEE_FLOAT;


    // vil1_rgb
    vil1_rgb<double> p_out_rgb(1.2,3.4,5.6), p_in_rgb;


    // vil1_rgba
    vil1_rgba<double> p_out_rgba(1.2,3.4,5.6,7.8), p_in_rgba;


    vsl_add_to_binary_loader(vil1_io_memory_image_impl());

    // Save if option set
    if (save_file)
    {
      vsl_b_ofstream bfs_out("golden_test_vil1_io.bvl.new");
      if (!bfs_out)
      {
        vcl_cerr<<"Problems opening file for output\n";
        vcl_exit(1);
      }
      vsl_b_write(bfs_out, p_out_memory_image_of);
      vsl_b_write(bfs_out, p_out_memory_image_impl);
      vsl_b_write(bfs_out, p_out_memory_image_format);
      vsl_b_write(bfs_out, p_out_rgb);
      vsl_b_write(bfs_out, p_out_rgba);
      bfs_out.close();
    }

    // Read in file to each class in turn
    vcl_string gold_path=testlib_root_dir()+"/core/vil1/io/tests/golden_test_vil1_io.bvl";
    vsl_b_ifstream bfs_in(gold_path.c_str());
    vsl_b_read(bfs_in, p_in_memory_image_of);
    vsl_b_read(bfs_in, p_in_memory_image_impl);
    vsl_b_read(bfs_in, p_in_memory_image_format);
    vsl_b_read(bfs_in, p_in_rgb);
    vsl_b_read(bfs_in, p_in_rgba);
    bfs_in.close();


    // Test that each object created is the same as read in from the file.
    TEST ("p_out_memory_image_of == p_in_memory_image_of ",
          p_out_memory_image_of.components() == p_in_memory_image_of.components() &&
          p_out_memory_image_of.bits_per_component() ==
          p_in_memory_image_of.bits_per_component() &&
          p_out_memory_image_of.component_format() ==
          p_in_memory_image_of.component_format(), true);


    TEST ("p_out_memory_image_impl == p_in_memory_image_impl (structure)",
          p_out_memory_image_impl.planes() == p_in_memory_image_impl.planes() &&
          p_out_memory_image_impl.height() == p_in_memory_image_impl.height() &&
          p_out_memory_image_impl.width() == p_in_memory_image_impl.width() &&
          p_out_memory_image_impl.components() == p_in_memory_image_impl.components() &&
          p_out_memory_image_impl.bits_per_component() == p_in_memory_image_impl.bits_per_component() &&
          p_out_memory_image_impl.component_format() == p_in_memory_image_impl.component_format(), true);

    // Now get the data and compare them
    vcl_vector<int> buf1(p_in_memory_image_of.size());
    p_out_memory_image_impl.get_section(&buf1[0], 0, 0,
                                        p_out_memory_image_impl.width(),
                                        p_out_memory_image_impl.height());
    vcl_vector<int> buf2(p_in_memory_image_of.size());
    p_in_memory_image_impl.get_section(&buf2[0], 0, 0,
                                       p_out_memory_image_impl.width(),
                                       p_out_memory_image_impl.height());
    bool data_same = false;
    for (unsigned int i=0;i<buf1.size();i++)
      data_same = (buf1[i]==buf2[i]);
    TEST ("p_out_memory_image_impl == p_in_memory_image_impl (data)",
          data_same, true);

    TEST ("p_out_memory_image_format == p_in_memory_image_format",
          p_out_memory_image_format.components ==
          p_in_memory_image_format.components &&
          p_out_memory_image_format.bits_per_component ==
          p_in_memory_image_format.bits_per_component &&
          p_out_memory_image_format.component_format ==
          p_in_memory_image_format.component_format , true);

    TEST ("p_out_rgb == p_in_rgb",
          p_out_rgb.R()==p_in_rgb.R() &&
          p_out_rgb.G()==p_in_rgb.G() &&
          p_out_rgb.B()==p_in_rgb.B() , true);


    TEST ("p_out_rgba == p_in_rgba",
          p_out_rgba.R()==p_in_rgba.R() &&
          p_out_rgba.G()==p_in_rgba.G() &&
          p_out_rgba.B()==p_in_rgba.B() &&
          p_out_rgba.A()==p_in_rgba.A() , true);
}

static void golden_test_vil1_io(int argc, char* argv[])
{
  golden_test_vil1_io(argc==2 && vcl_string(argv[1])==vcl_string("create"));
}

TESTMAIN_ARGS(golden_test_vil1_io);
