// This is core/vgl/io/tests/golden_test_vgl_io.cxx

//:
// \file
// \brief Read in a golden data file, and check the values are correct.
//
// This by default reads a golden test file and checks the values.
// You can make it generate a new golden file by running it with the
// create option.
//
// If you need to recreate the golden data file build this as a
// standalone program, and run it with the single parameter create:
// \verbatim
// golden_test_vnl_io create
// \endverbatim
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - corrected implementation of the "near equality" for homogeneous objects
//   23 Oct.2003 - Peter Vanroose - made the program more readable by adding overloads for almost_equal() for all geom types
// \endverbatim

#include <vcl_string.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <testlib/testlib_root_dir.h>

#include <vgl/io/vgl_io_box_2d.h>
#include <vgl/io/vgl_io_box_3d.h>
#include <vgl/io/vgl_io_homg_line_2d.h>
#include <vgl/io/vgl_io_homg_line_3d_2_points.h>
#include <vgl/io/vgl_io_homg_plane_3d.h>
#include <vgl/io/vgl_io_homg_point_2d.h>
#include <vgl/io/vgl_io_homg_point_3d.h>
#include <vgl/io/vgl_io_line_2d.h>
#include <vgl/io/vgl_io_line_segment_2d.h>
#include <vgl/io/vgl_io_line_segment_3d.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <vgl/io/vgl_io_polygon.h>

void golden_test_vgl_io(bool save_file=false);

// This nasty macro stuff is to allow the program to be compiled either as
// a stand alone program or as part of a larger test program.
#ifndef MAIN
  #include <testlib/testlib_test.h>
  MAIN_ARGS( golden_test_vgl_io )
  {
    START("golden_test_all_vgl_io");
    golden_test_vgl_io(argc==2 && vcl_string(argv[1])==vcl_string("create"));
    SUMMARY();
  }
#else
  #include <testlib/testlib_test.h>
#endif

inline float epsilon(float) { return 1e-5f; }
inline double epsilon(double) { return 1e-14f; }

template <class T>
bool almost_equal_helper(const T& real1, const T& real2)
{
  bool result;
  T max,min,factor;
  if (real1<real2)
  {
    min=real1;
    max=real2;
  }
  else {
    min=real2;
    max=real1;
  }
  factor=epsilon(min);
  result=((max-min)<=factor*(max>0?max:-max));
  if (result==false)
  {
    vcl_cout << factor << " : " << min << " -- " << max << " -- "
             << (max>0?max:-max) << " -- "
             << max-min << " -- " << factor*(max>0?max:-max) << vcl_endl;
  }
  return result;
}

bool almost_equal(const double& a, const double& b)
{
  return almost_equal_helper(a, b);
}
bool almost_equal(const float& a, const float& b)
{
  return almost_equal_helper(a, b);
}

template <class T>
inline bool almost_equal(const vgl_point_2d<T>& p1, const vgl_point_2d<T>& p2)
{
  return almost_equal(p1.x(), p2.x()) &&
         almost_equal(p1.y(), p2.y());
}

template <class T>
inline bool almost_equal(const vgl_point_3d<T>& p1, const vgl_point_3d<T>& p2)
{
  return almost_equal(p1.x(), p2.x()) &&
         almost_equal(p1.y(), p2.y()) &&
         almost_equal(p1.z(), p2.z());
}

template <class T>
inline bool almost_equal(const vgl_homg_point_2d<T>& p1, const vgl_homg_point_2d<T>& p2)
{
  return almost_equal(p1.x()*p2.w(), p2.x()*p1.w()) &&
         almost_equal(p1.y()*p2.w(), p2.y()*p1.w()) &&
         almost_equal(p1.y()*p2.x(), p2.y()*p1.x());
}

template <class T>
inline bool almost_equal(const vgl_homg_point_3d<T>& p1, const vgl_homg_point_3d<T>& p2)
{
  return almost_equal(p1.x()*p2.w(), p2.x()*p1.w()) &&
         almost_equal(p1.y()*p2.w(), p2.y()*p1.w()) &&
         almost_equal(p1.z()*p2.w(), p2.z()*p1.w()) &&
         almost_equal(p1.x()*p2.z(), p2.x()*p1.z()) &&
         almost_equal(p1.y()*p2.z(), p2.y()*p1.z()) &&
         almost_equal(p1.x()*p2.y(), p2.x()*p1.y());
}

template <class T>
inline bool almost_equal(const vgl_line_2d<T>& l1, const vgl_line_2d<T>& l2)
{
  return almost_equal(l1.a()*l2.c(), l2.a()*l1.c()) &&
         almost_equal(l1.b()*l2.c(), l2.b()*l1.c()) &&
         almost_equal(l1.b()*l2.a(), l2.b()*l1.a());
}

template <class T>
inline bool almost_equal(const vgl_homg_line_2d<T>& l1, const vgl_homg_line_2d<T>& l2)
{
  return almost_equal(l1.a()*l2.c(), l2.a()*l1.c()) &&
         almost_equal(l1.b()*l2.c(), l2.b()*l1.c()) &&
         almost_equal(l1.b()*l2.a(), l2.b()*l1.a());
}

template <class T>
inline bool almost_equal(const vgl_homg_plane_3d<T>& p1, const vgl_homg_plane_3d<T>& p2)
{
  return almost_equal(p1.nx()*p2.d(), p2.nx()*p1.d()) &&
         almost_equal(p1.ny()*p2.d(), p2.ny()*p1.d()) &&
         almost_equal(p1.nz()*p2.d(), p2.nz()*p1.d()) &&
         almost_equal(p1.nx()*p2.nz(), p2.nx()*p1.nz()) &&
         almost_equal(p1.ny()*p2.nz(), p2.ny()*p1.nz()) &&
         almost_equal(p1.nx()*p2.ny(), p2.nx()*p1.ny());
}

template <class T>
inline bool almost_equal(const vgl_homg_line_3d_2_points<T>& l1, const vgl_homg_line_3d_2_points<T>& l2)
{
  return almost_equal(l1.point_finite(),  l2.point_finite()) &&
         almost_equal(l1.point_infinite(),l2.point_infinite());
}

template <class T>
inline bool almost_equal(const vgl_line_segment_2d<T>& l1, const vgl_line_segment_2d<T>& l2)
{
  return almost_equal(l1.point1(),l2.point1()) &&
         almost_equal(l1.point2(),l2.point2());
}

template <class T>
inline bool almost_equal(const vgl_line_segment_3d<T>& l1, const vgl_line_segment_3d<T>& l2)
{
  return almost_equal(l1.point1(),l2.point1()) &&
         almost_equal(l1.point2(),l2.point2());
}

template <class T>
inline bool almost_equal(const vgl_box_2d<T>& b1, const vgl_box_2d<T>& b2)
{
  return almost_equal(b1.min_x(), b2.min_x()) &&
         almost_equal(b1.min_y(), b2.min_y()) &&
         almost_equal(b1.max_x(), b2.max_x()) &&
         almost_equal(b1.max_y(), b2.max_y());
}

template <class T>
inline bool almost_equal(const vgl_box_3d<T>& b1, const vgl_box_3d<T>& b2)
{
  return almost_equal(b1.min_x(), b2.min_x()) &&
         almost_equal(b1.min_y(), b2.min_y()) &&
         almost_equal(b1.min_z(), b2.min_z()) &&
         almost_equal(b1.max_x(), b2.max_x()) &&
         almost_equal(b1.max_y(), b2.max_y()) &&
         almost_equal(b1.max_z(), b2.max_z());
}

inline bool almost_equal(vgl_polygon const& p1, vgl_polygon const& p2)
{
  if (p1.num_sheets() != p2.num_sheets())
    return false;
  for (int i=0;i<3;i++)
    for (int j=0;j<5;j++)
      if (!almost_equal(p1[i][j], p2[i][j]))
        return false;
  return true;
}

void golden_test_vgl_io(bool save_file)
{
  //------------------------------------------------------------------------
  // Create objects:
  // If the "create" flag was used on the command line, then the program saves
  // an example of each class. Otherwise it just fills them with values
  // for comparison to the values read in.
  //------------------------------------------------------------------------


  vcl_cout << "***********************************************************\n"
           << " Testing a golden data file for cross platform consistency\n"
           << "***********************************************************\n";


  // vgl_point_2d

  double double_pos[2]={2.3,4.7};
  float float_pos[2]={3.3f,4.9f};

  vgl_point_2d<double> double_point_2d_out(double_pos),double_point_2d_in;
  vgl_point_2d<double> double_point_2d_out2(4.6,7.8),double_point_2d_in2;
  vgl_point_2d<double> double_point_2d_in_bis;
  vgl_point_2d<float> float_point_2d_out(float_pos),float_point_2d_in;
  vgl_point_2d<float> float_point_2d_out2(4.7f,6.8f),float_point_2d_in2;

  // vgl_point_3d

  double double_3d_pos[3]={2.3,4.7,5.6};
  float float_3d_pos[3]={5.3f,6.7f,8.5f};

  vgl_point_3d<double> double_point_3d_out(double_3d_pos),double_point_3d_in;
  vgl_point_3d<double> double_point_3d_out2(4.6,7.8,0.4),double_point_3d_in2;
  vgl_point_3d<float> float_point_3d_out(float_3d_pos),float_point_3d_in;
  vgl_point_3d<float> float_point_3d_out2(5.9f,8.8f,9.2f),float_point_3d_in2;

  // vgl_homg_point_2d

  double double_homg_pos[3]={2.3, 4.7, 6.7};
  float float_homg_pos[3]={2.3f, 4.7f, 5.6f};

  vgl_homg_point_2d<double> double_homg_point_2d_out(double_homg_pos);
  vgl_homg_point_2d<double> double_homg_point_2d_in;
  vgl_homg_point_2d<double> double_homg_point_2d_out2(4.6,7.8,9.1);
  vgl_homg_point_2d<double> double_homg_point_2d_in2;

  vgl_homg_point_2d<float> float_homg_point_2d_out(float_homg_pos);
  vgl_homg_point_2d<float> float_homg_point_2d_in;
  vgl_homg_point_2d<float> float_homg_point_2d_out2(4.6f,7.8f,9.1f);
  vgl_homg_point_2d<float> float_homg_point_2d_in2;

  // vgl_homg_point_3d

  double double_3d_homg_pos[4]={2.3,4.7,6.7,1.0};
  float float_3d_homg_pos[4]={2.3f, 4.7f, 5.6f, 2.4f};

  vgl_homg_point_3d<double> double_homg_point_3d_out(double_3d_homg_pos),
                            double_homg_point_3d_in;
  vgl_homg_point_3d<double> double_homg_point_3d_out2(4.6,7.8,9.1),
                            double_homg_point_3d_in2;
  vgl_homg_point_3d<float> float_homg_point_3d_out(float_3d_homg_pos),
                           float_homg_point_3d_in;
  vgl_homg_point_3d<float> float_homg_point_3d_out2(4.6f,7.8f,9.1f),
                           float_homg_point_3d_in2;

  // vgl_line_2d

  vgl_line_2d<double> double_line_2d_out(double_point_2d_out,
                                         double_point_2d_out2),
                      double_line_2d_in;
  vgl_line_2d<float>  float_line_2d_out(float_point_2d_out,
                                        float_point_2d_out2),
                      float_line_2d_in;

  // vgl_homg_line_2d

  vgl_homg_line_2d<double> double_homg_line_2d_out(double_homg_point_2d_out,
                                                   double_homg_point_2d_out2),
                           double_homg_line_2d_in;
  vgl_homg_line_2d<float>  float_homg_line_2d_out(float_homg_point_2d_out,
                                                  float_homg_point_2d_out2),
                           float_homg_line_2d_in;

  // vgl_homg_line_3d_2_points

  vgl_homg_line_3d_2_points<double>
      double_homg_line_3d_2_points_out(double_homg_point_3d_out,
                                       double_homg_point_3d_out2),
      double_homg_line_3d_2_points_in;
  vgl_homg_line_3d_2_points<float>
      float_homg_line_3d_2_points_out(float_homg_point_3d_out,
                                      float_homg_point_3d_out2),
      float_homg_line_3d_2_points_in;

  // vgl_line_segment_2d

  vgl_line_segment_2d<double>
      double_line_segment_2d_out(double_point_2d_out,double_point_2d_out2),
      double_line_segment_2d_in;
  vgl_line_segment_2d<float>
      float_line_segment_2d_out(float_point_2d_out,float_point_2d_out2),
      float_line_segment_2d_in;

  // vgl_line_segment_3d

  vgl_line_segment_3d<double>
      double_line_segment_3d_out(double_point_3d_out,double_point_3d_out2),
      double_line_segment_3d_in;
  vgl_line_segment_3d<float>
      float_line_segment_3d_out(float_point_3d_out,float_point_3d_out2),
      float_line_segment_3d_in;

  // vgl_box_2d

  double double_min_pos[2]={3.4,5.6};
  double double_max_pos[2]={7.8,9.1};
  float float_min_pos[2]={2.4f,4.6f};
  float float_max_pos[2]={7.7f,9.0f};

  vgl_box_2d<double> double_box_2d_out(double_min_pos,double_max_pos),
                     double_box_2d_in;
  vgl_box_2d<float>  float_box_2d_out(float_min_pos,float_max_pos),
                     float_box_2d_in;

  // vgl_box_3d

  vgl_box_3d<double> double_box_3d_out(-4.6,2.8,0.56,4.3,4.7,2.12),
                     double_box_3d_in;
  vgl_box_3d<float>  float_box_3d_out(-3.6f,1.8f,5.46f,3.9f,6.7f,9.04f),
                     float_box_3d_in;

  // vgl_homg_plane_3d

  vgl_vector_3d<double> double_homg_normal(2.3, 4.7, 6.7);
  vgl_vector_3d<float> float_homg_normal(2.3f, 4.7f, 5.6f);

  vgl_homg_plane_3d<double>
      double_homg_plane_3d_out(double_homg_normal,double_homg_point_3d_out),
      double_homg_plane_3d_in;
  vgl_homg_plane_3d<float>
      float_homg_plane_3d_out(float_homg_normal,float_homg_point_3d_out),
      float_homg_plane_3d_in;

  // vgl_polygon

  vgl_polygon polygon_out(3),polygon_in;
  for (int i=0;i<3;i++)
    for (int j=0;j<5;j++)
      polygon_out[i].push_back(vgl_point_2d<float>((float)i+((float)j)/10.0f,
                               (float)i-((float)j)/10.0f));

  // Save if option set
  if (save_file)
  {
    vsl_b_ofstream bfs_out("golden_vgl_io_test.bvl");
    TEST("Opened golden_vgl_io_test.bvl for writing", (!bfs_out), false);
    if (!bfs_out)
    {
      vcl_cerr<<"Problems opening file for output\n";
      vcl_exit(1);
    }
    vsl_b_write(bfs_out, double_point_2d_out);
    vsl_b_write(bfs_out, double_point_2d_out2);
    vsl_b_write(bfs_out, float_point_2d_out);
    vsl_b_write(bfs_out, float_point_2d_out2);
    vsl_b_write(bfs_out, double_point_3d_out);
    vsl_b_write(bfs_out, double_point_3d_out2);
    vsl_b_write(bfs_out, float_point_3d_out);
    vsl_b_write(bfs_out, float_point_3d_out2);
    vsl_b_write(bfs_out, double_homg_point_2d_out);
    vsl_b_write(bfs_out, double_homg_point_2d_out2);
    vsl_b_write(bfs_out, float_homg_point_2d_out);
    vsl_b_write(bfs_out, float_homg_point_2d_out2);
    vsl_b_write(bfs_out, double_homg_point_3d_out);
    vsl_b_write(bfs_out, double_homg_point_3d_out2);
    vsl_b_write(bfs_out, float_homg_point_3d_out);
    vsl_b_write(bfs_out, float_homg_point_3d_out2);
    vsl_b_write(bfs_out, double_line_2d_out);
    vsl_b_write(bfs_out, float_line_2d_out);
    vsl_b_write(bfs_out, double_homg_line_2d_out);
    vsl_b_write(bfs_out, float_homg_line_2d_out);
    vsl_b_write(bfs_out, double_homg_line_3d_2_points_out);
    vsl_b_write(bfs_out, float_homg_line_3d_2_points_out);
    vsl_b_write(bfs_out, double_line_segment_2d_out);
    vsl_b_write(bfs_out, float_line_segment_2d_out);
    vsl_b_write(bfs_out, double_line_segment_3d_out);
    vsl_b_write(bfs_out, float_line_segment_3d_out);
    vsl_b_write(bfs_out, double_box_2d_out);
    vsl_b_write(bfs_out, float_box_2d_out);
    vsl_b_write(bfs_out, double_box_3d_out);
    vsl_b_write(bfs_out, float_box_3d_out);
    vsl_b_write(bfs_out, double_homg_plane_3d_out);
    vsl_b_write(bfs_out, float_homg_plane_3d_out);
    vsl_b_write(bfs_out, polygon_out);
    vsl_b_write(bfs_out, double_point_2d_out);
    bfs_out.close();
  }

  // Read in file to each class in turn
  vcl_string gold_path=testlib_root_dir()+"/core/vgl/io/tests/golden_vgl_io_test.bvl";
  vsl_b_ifstream bfs_in(gold_path.c_str());
  TEST("Opened golden_vgl_io_test.bvl for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, double_point_2d_in);
  vsl_b_read(bfs_in, double_point_2d_in2);
  vsl_b_read(bfs_in, float_point_2d_in);
  vsl_b_read(bfs_in, float_point_2d_in2);
  vsl_b_read(bfs_in, double_point_3d_in);
  vsl_b_read(bfs_in, double_point_3d_in2);
  vsl_b_read(bfs_in, float_point_3d_in);
  vsl_b_read(bfs_in, float_point_3d_in2);
  vsl_b_read(bfs_in, double_homg_point_2d_in);
  vsl_b_read(bfs_in, double_homg_point_2d_in2);
  vsl_b_read(bfs_in, float_homg_point_2d_in);
  vsl_b_read(bfs_in, float_homg_point_2d_in2);
  vsl_b_read(bfs_in, double_homg_point_3d_in);
  vsl_b_read(bfs_in, double_homg_point_3d_in2);
  vsl_b_read(bfs_in, float_homg_point_3d_in);
  vsl_b_read(bfs_in, float_homg_point_3d_in2);
  vsl_b_read(bfs_in, double_line_2d_in);
  vsl_b_read(bfs_in, float_line_2d_in);
  vsl_b_read(bfs_in, double_homg_line_2d_in);
  vsl_b_read(bfs_in, float_homg_line_2d_in);
  vsl_b_read(bfs_in, double_homg_line_3d_2_points_in);
  vsl_b_read(bfs_in, float_homg_line_3d_2_points_in);
  vsl_b_read(bfs_in, double_line_segment_2d_in);
  vsl_b_read(bfs_in, float_line_segment_2d_in);
  vsl_b_read(bfs_in, double_line_segment_3d_in);
  vsl_b_read(bfs_in, float_line_segment_3d_in);
  vsl_b_read(bfs_in, double_box_2d_in);
  vsl_b_read(bfs_in, float_box_2d_in);
  vsl_b_read(bfs_in, double_box_3d_in);
  vsl_b_read(bfs_in, float_box_3d_in);
  vsl_b_read(bfs_in, double_homg_plane_3d_in);
  vsl_b_read(bfs_in, float_homg_plane_3d_in);
  vsl_b_read(bfs_in, polygon_in);
  vsl_b_read(bfs_in, double_point_2d_in_bis);
  bfs_in.close();

  // Test that each object created is the same as read in from the file.
#define TEST_equal(x) TEST(#x "_out == " #x "_in", almost_equal(x ## _out, x ## _in), true);
#define TEST_equal_2(x) TEST(#x "_out2 == " #x "_in2", almost_equal(x ## _out2, x ## _in2), true);
  TEST_equal(double_point_2d);
  TEST_equal(float_point_2d);
  TEST_equal_2(double_point_2d);
  TEST_equal_2(float_point_2d);
  TEST_equal(double_point_3d);
  TEST_equal(float_point_3d);
  TEST_equal_2(double_point_3d);
  TEST_equal_2(float_point_3d);
  TEST_equal(double_homg_point_2d);
  TEST_equal(float_homg_point_2d);
  TEST_equal_2(double_homg_point_2d);
  TEST_equal_2(float_homg_point_2d);
  TEST_equal(double_homg_point_3d);
  TEST_equal(float_homg_point_3d);
  TEST_equal_2(double_homg_point_3d);
  TEST_equal_2(float_homg_point_3d);
  TEST_equal(double_line_2d);
  TEST_equal(float_line_2d);
  TEST_equal(double_homg_line_2d);
  TEST_equal(float_homg_line_2d);
  TEST_equal(double_homg_line_3d_2_points);
  TEST_equal(float_homg_line_3d_2_points);
  TEST_equal(double_line_segment_2d);
  TEST_equal(float_line_segment_2d);
  TEST_equal(double_line_segment_3d);
  TEST_equal(float_line_segment_3d);
  TEST_equal(double_box_2d);
  TEST_equal(float_box_2d);
  TEST_equal(double_box_3d);
  TEST_equal(float_box_3d);
  TEST_equal(double_homg_plane_3d);
  TEST_equal(float_homg_plane_3d);
  TEST_equal(polygon);
  TEST("double_point_2d_out == double_point_2d_in_bis",
       almost_equal(double_point_2d_out, double_point_2d_in_bis), true);
  vsl_indent_clear_all_data();
}
