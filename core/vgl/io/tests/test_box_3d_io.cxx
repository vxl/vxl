#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vgl/vgl_test.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/io/vgl_io_box_3d.h>

void test_box_3d_double_io()
{
  vcl_cout << "***********************\n"
           << "Testing vgl_box_3d<double> io\n"
           << "***********************\n";

  //// test constructors, accessors
  vgl_box_3d<double> p_out(1.2,3.4,5.6,2.3,4.5,6.7), p_in;

  vsl_b_ofstream bfs_out("vgl_box_3d_test_double_io.bvl.tmp");
  TEST ("Created vgl_box_3d_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_box_3d_test_double_io.bvl.tmp");
  TEST ("Opened vgl_box_3d_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  bfs_in.close();

  TEST ("p_out==p_in",p_out.get_min_x() == p_in.get_min_x()
                   && p_out.get_min_y() == p_in.get_min_y()
                   && p_out.get_min_z() == p_in.get_min_z()
                   && p_out.get_max_x() == p_in.get_max_x()
                   && p_out.get_max_y() == p_in.get_max_y()
                   && p_out.get_max_z() == p_in.get_max_z(), true);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
}

void test_box_3d_prime()
{
  test_box_3d_double_io();
}

TESTMAIN(test_box_3d_prime);
