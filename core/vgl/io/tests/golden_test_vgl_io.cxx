// This is ./vxl/vgl/io/tests/golden_test_vgl_io.cxx

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_root_dir.h>

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
// \verbatim

void golden_test_vgl_io(bool save_file=false);

// This nasty macro stuff is to allow the program to be compiled as a stand
// alone program or as part of a larger test program.
#ifndef TESTMAIN
  #include <vgl/vgl_test.h>
  int main( int argc, char* argv[] )
  {
    vgl_test_start("golden_test_all_vgl_io");
    bool save_file=false;

    if (argc==2)
    {
      vcl_string conf = argv[1];
      vcl_string ref="create";
      if (conf==ref)
      {
        save_file =true;
      }
    }
    golden_test_vgl_io(save_file);
    return vgl_test_summary();
  }
#else
  #include <vgl/vgl_test.h>
#endif

float epsilon(float)
{
return 1e-5f;
}

double epsilon(double)
{
return 1e-14f;
}

template <class T>
bool almost_equal(const T& real1, const T& real2)
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

template bool almost_equal(const double&, const double&);
template bool almost_equal(const float&, const float&);

void golden_test_vgl_io(bool save_file)
{
  //------------------------------------------------------------------------
  // Create objects:
  // If the "create" flag was used on the command line, then the program saves
  // an example of each class. Otherwise it just fills them with values
  // for comparison to the values read in.
  //------------------------------------------------------------------------


  vcl_cout << "***********************\n"
           << "Testing a golden data file for cross platform consistency\n"
           << "***********************\n";


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
    TEST ("Opened golden_vgl_io_test.bvl for writing", (!bfs_out), false);
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
  vcl_string gold_path=vul_root_dir()+"/vxl/vgl/io/tests/golden_vgl_io_test.bvl";
  vsl_b_ifstream bfs_in(gold_path.c_str());
  TEST ("Opened golden_vgl_io_test.bvl for reading", (!bfs_in), false);
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
  TEST ("double_point_2d_out == double_point_2d_in",
        almost_equal(double_point_2d_out.x(), double_point_2d_in.x()) &&
        almost_equal(double_point_2d_out.y(), double_point_2d_in.y()),
        true);
  TEST ("float_point_2d_out == float_point_2d_in",
        almost_equal(float_point_2d_out.x(), float_point_2d_in.x()) &&
        almost_equal(float_point_2d_out.y(), float_point_2d_in.y()),
        true);
  TEST ("double_point_2d_out2 == double_point_2d_in2",
        almost_equal(double_point_2d_out2.x(), double_point_2d_in2.x()) &&
        almost_equal(double_point_2d_out2.y(), double_point_2d_in2.y()),
        true);
  TEST ("float_point_2d_out2 == float_point_2d_in2",
        almost_equal(float_point_2d_out2.x(), float_point_2d_in2.x()) &&
        almost_equal(float_point_2d_out2.y(), float_point_2d_in2.y()),
        true);
  TEST ("double_point_3d_out == double_point_3d_in",
        almost_equal(double_point_3d_out.x(), double_point_3d_in.x()) &&
        almost_equal(double_point_3d_out.y(), double_point_3d_in.y()) &&
        almost_equal(double_point_3d_out.z(), double_point_3d_in.z()),
        true);
  TEST ("float_point_3d_out == float_point_3d_in",
        almost_equal(float_point_3d_out.x(), float_point_3d_in.x()) &&
        almost_equal(float_point_3d_out.y(), float_point_3d_in.y()) &&
        almost_equal(float_point_3d_out.z(), float_point_3d_in.z()),
        true);
  TEST ("double_point_3d_out2 == double_point_3d_in2",
        almost_equal(double_point_3d_out2.x(), double_point_3d_in2.x()) &&
        almost_equal(double_point_3d_out2.y(), double_point_3d_in2.y()) &&
        almost_equal(double_point_3d_out2.z(), double_point_3d_in2.z()),
        true);
  TEST ("float_point_3d_out2 == float_point_3d_in2",
        almost_equal(float_point_3d_out2.x(), float_point_3d_in2.x()) &&
        almost_equal(float_point_3d_out2.y(), float_point_3d_in2.y()) &&
        almost_equal(float_point_3d_out2.z(), float_point_3d_in2.z()),
        true);
  TEST ("double_homg_point_2d_out == double_homg_point_2d_in",
        almost_equal(double_homg_point_2d_out.x(), double_homg_point_2d_in.x()) &&
        almost_equal(double_homg_point_2d_out.y(), double_homg_point_2d_in.y()) &&
        almost_equal(double_homg_point_2d_out.w(), double_homg_point_2d_in.w()),
        true);
  TEST ("float_homg_point_2d_out == float_homg_point_2d_in",
        almost_equal(float_homg_point_2d_out.x(), float_homg_point_2d_in.x()) &&
        almost_equal(float_homg_point_2d_out.y(), float_homg_point_2d_in.y()) &&
        almost_equal(float_homg_point_2d_out.w(), float_homg_point_2d_in.w()),
        true);
  TEST ("double_homg_point_2d_out2 == double_homg_point_2d_in2",
        almost_equal(double_homg_point_2d_out2.x(), double_homg_point_2d_in2.x()) &&
        almost_equal(double_homg_point_2d_out2.y(), double_homg_point_2d_in2.y()) &&
        almost_equal(double_homg_point_2d_out2.w(), double_homg_point_2d_in2.w()),
        true);
  TEST ("float_homg_point_2d_out2 == float_homg_point_2d_in2",
        almost_equal(float_homg_point_2d_out2.x(), float_homg_point_2d_in2.x()) &&
        almost_equal(float_homg_point_2d_out2.y(), float_homg_point_2d_in2.y()) &&
        almost_equal(float_homg_point_2d_out2.w(), float_homg_point_2d_in2.w()),
        true);
  TEST ("double_homg_point_3d_out == double_homg_point_3d_in",
        almost_equal(double_homg_point_3d_out.x(), double_homg_point_3d_in.x()) &&
        almost_equal(double_homg_point_3d_out.y(), double_homg_point_3d_in.y()) &&
        almost_equal(double_homg_point_3d_out.z(), double_homg_point_3d_in.z()) &&
        almost_equal(double_homg_point_3d_out.w(),
        double_homg_point_3d_in.w()),
        true);
  TEST ("float_homg_point_3d_out == float_homg_point_3d_in",
        almost_equal(float_homg_point_3d_out.x(), float_homg_point_3d_in.x()) &&
        almost_equal(float_homg_point_3d_out.y(), float_homg_point_3d_in.y()) &&
        almost_equal(float_homg_point_3d_out.z(), float_homg_point_3d_in.z()) &&
        almost_equal(float_homg_point_3d_out.w(),
        float_homg_point_3d_in.w()),
        true);
  TEST ("double_homg_point_3d_out2 == double_homg_point_3d_in2",
        almost_equal(double_homg_point_3d_out2.x(), double_homg_point_3d_in2.x()) &&
        almost_equal(double_homg_point_3d_out2.y(), double_homg_point_3d_in2.y()) &&
        almost_equal(double_homg_point_3d_out2.z(), double_homg_point_3d_in2.z()) &&
        almost_equal(double_homg_point_3d_out2.w(),
        double_homg_point_3d_in2.w()),
        true);
  TEST ("float_homg_point_3d_out2 == float_homg_point_3d_in2",
        almost_equal(float_homg_point_3d_out2.x(), float_homg_point_3d_in2.x()) &&
        almost_equal(float_homg_point_3d_out2.y(), float_homg_point_3d_in2.y()) &&
        almost_equal(float_homg_point_3d_out2.z(), float_homg_point_3d_in2.z()) &&
        almost_equal(float_homg_point_3d_out2.w(), float_homg_point_3d_in2.w()),
        true);
  TEST ("double_line_2d_out == double_line_2d_in",
        almost_equal(double_line_2d_out.a(), double_line_2d_in.a()) &&
        almost_equal(double_line_2d_out.b(), double_line_2d_in.b()) &&
        almost_equal(double_line_2d_out.c(), double_line_2d_in.c()),
        true);
  TEST ("float_line_2d_out == float_line_2d_in",
        almost_equal(float_line_2d_out.a(), float_line_2d_in.a()) &&
        almost_equal(float_line_2d_out.b(), float_line_2d_in.b()) &&
        almost_equal(float_line_2d_out.c(), float_line_2d_in.c()),
        true);
  TEST ("double_homg_line_2d_out == double_homg_line_2d_in",
        almost_equal(double_homg_line_2d_out.a(), double_homg_line_2d_in.a()) &&
        almost_equal(double_homg_line_2d_out.b(), double_homg_line_2d_in.b()) &&
        almost_equal(double_homg_line_2d_out.c(), double_homg_line_2d_in.c()),
        true);
  TEST ("float_homg_line_2d_out == float_homg_line_2d_in",
        almost_equal(float_homg_line_2d_out.a(), float_homg_line_2d_in.a()) &&
        almost_equal(float_homg_line_2d_out.b(), float_homg_line_2d_in.b()) &&
        almost_equal(float_homg_line_2d_out.c(), float_homg_line_2d_in.c()),
        true);
  TEST ("double_homg_line_3d_2_points_out == double_homg_line_3d_2_points_in",
        almost_equal(
          double_homg_line_3d_2_points_out.point_finite().x()
          *double_homg_line_3d_2_points_in.point_finite().w(),
          double_homg_line_3d_2_points_out.point_finite().w()
          *double_homg_line_3d_2_points_in.point_finite().x()) &&
        almost_equal(
          double_homg_line_3d_2_points_out.point_finite().y()
          *double_homg_line_3d_2_points_in.point_finite().w(),
          double_homg_line_3d_2_points_out.point_finite().w()
          *double_homg_line_3d_2_points_in.point_finite().y()) &&
        almost_equal(
          double_homg_line_3d_2_points_out.point_finite().z()
          *double_homg_line_3d_2_points_in.point_finite().w(),
          double_homg_line_3d_2_points_out.point_finite().w()
          *double_homg_line_3d_2_points_in.point_finite().z()) &&
        almost_equal(
          double_homg_line_3d_2_points_out.point_infinite().x()
          *double_homg_line_3d_2_points_in.point_infinite().z(),
          double_homg_line_3d_2_points_out.point_infinite().z()
          *double_homg_line_3d_2_points_in.point_infinite().x()) &&
        almost_equal(
          double_homg_line_3d_2_points_out.point_infinite().y()
          *double_homg_line_3d_2_points_in.point_infinite().z(),
          double_homg_line_3d_2_points_out.point_infinite().z()
          *double_homg_line_3d_2_points_in.point_infinite().y()),
        true);
  TEST ("float_homg_line_3d_2_points_out == float_homg_line_3d_2_points_in",
        almost_equal(
          float_homg_line_3d_2_points_out.point_finite().x()
          *float_homg_line_3d_2_points_in.point_finite().w(),
          float_homg_line_3d_2_points_out.point_finite().w()
          *float_homg_line_3d_2_points_in.point_finite().x()) &&
        almost_equal(
          float_homg_line_3d_2_points_out.point_finite().y()
          *float_homg_line_3d_2_points_in.point_finite().w(),
          float_homg_line_3d_2_points_out.point_finite().w()
          *float_homg_line_3d_2_points_in.point_finite().y()) &&
        almost_equal(
          float_homg_line_3d_2_points_out.point_finite().z()
          *float_homg_line_3d_2_points_in.point_finite().w(),
          float_homg_line_3d_2_points_out.point_finite().w()
          *float_homg_line_3d_2_points_in.point_finite().z()) &&
        almost_equal(
          float_homg_line_3d_2_points_out.point_infinite().x()
          *float_homg_line_3d_2_points_in.point_infinite().z(),
          float_homg_line_3d_2_points_out.point_infinite().z()
          *float_homg_line_3d_2_points_in.point_infinite().x()) &&
        almost_equal(
          float_homg_line_3d_2_points_out.point_infinite().y()
          *float_homg_line_3d_2_points_in.point_infinite().z(),
          float_homg_line_3d_2_points_out.point_infinite().z()
          *float_homg_line_3d_2_points_in.point_infinite().y()),
        true);
  TEST ("double_line_segment_2d_out == double_line_segment_2d_in",
        almost_equal(double_line_segment_2d_out.point1().x(),
          double_line_segment_2d_in.point1().x()) &&
        almost_equal(double_line_segment_2d_out.point1().y(),
          double_line_segment_2d_in.point1().y()) &&
        almost_equal(double_line_segment_2d_out.point2().x(),
          double_line_segment_2d_in.point2().x()) &&
        almost_equal(double_line_segment_2d_out.point2().y(),
          double_line_segment_2d_in.point2().y()),
        true);
  TEST ("float_line_segment_2d_out == float_line_segment_2d_in",
        almost_equal(float_line_segment_2d_out.point1().x(),
          float_line_segment_2d_in.point1().x()) &&
        almost_equal(float_line_segment_2d_out.point1().y(),
          float_line_segment_2d_in.point1().y()) &&
        almost_equal(float_line_segment_2d_out.point2().x(),
          float_line_segment_2d_in.point2().x()) &&
        almost_equal(float_line_segment_2d_out.point2().y(),
          float_line_segment_2d_in.point2().y()),
        true);
  TEST ("double_line_segment_3d_out==double_line_segment_3d_in",
        almost_equal(double_line_segment_3d_out.point1().x(),
          double_line_segment_3d_in.point1().x()) &&
        almost_equal(double_line_segment_3d_out.point1().y(),
          double_line_segment_3d_in.point1().y()) &&
        almost_equal(double_line_segment_3d_out.point1().z(),
          double_line_segment_3d_in.point1().z()) &&
        almost_equal(double_line_segment_3d_out.point2().x(),
          double_line_segment_3d_in.point2().x()) &&
        almost_equal(double_line_segment_3d_out.point2().y(),
          double_line_segment_3d_in.point2().y()) &&
        almost_equal(double_line_segment_3d_out.point2().z(),
          double_line_segment_3d_in.point2().z()),
        true);
  TEST ("float_line_segment_3d_out == float_line_segment_3d_in",
        almost_equal(float_line_segment_3d_out.point1().x(),
          float_line_segment_3d_in.point1().x()) &&
        almost_equal(float_line_segment_3d_out.point1().y(),
          float_line_segment_3d_in.point1().y()) &&
        almost_equal(float_line_segment_3d_out.point1().z(),
          float_line_segment_3d_in.point1().z()) &&
        almost_equal(float_line_segment_3d_out.point2().x(),
          float_line_segment_3d_in.point2().x()) &&
        almost_equal(float_line_segment_3d_out.point2().y(),
          float_line_segment_3d_in.point2().y()) &&
        almost_equal(float_line_segment_3d_out.point2().z(),
          float_line_segment_3d_in.point2().z()),
        true);
  TEST ("double_box_2d_out == double_box_2d_in",
        almost_equal(double_box_2d_out.min_x(), double_box_2d_in.min_x()) &&
        almost_equal(double_box_2d_out.min_y(), double_box_2d_in.min_y()) &&
        almost_equal(double_box_2d_out.max_x(), double_box_2d_in.max_x()) &&
        almost_equal(double_box_2d_out.max_y(), double_box_2d_in.max_y()),
        true);
  TEST ("float_box_2d_out == float_box_2d_in",
        almost_equal(float_box_2d_out.min_x(), float_box_2d_in.min_x()) &&
        almost_equal(float_box_2d_out.min_y(), float_box_2d_in.min_y()) &&
        almost_equal(float_box_2d_out.max_x(), float_box_2d_in.max_x()) &&
        almost_equal(float_box_2d_out.max_y(), float_box_2d_in.max_y()),
        true);
  TEST ("double_box_3d_out == double_box_3d_in",
        almost_equal(double_box_3d_out.min_x(), double_box_3d_in.min_x()) &&
        almost_equal(double_box_3d_out.max_x(), double_box_3d_in.max_x()) &&
        almost_equal(double_box_3d_out.min_y(), double_box_3d_in.min_y()) &&
        almost_equal(double_box_3d_out.max_y(), double_box_3d_in.max_y()) &&
        almost_equal(double_box_3d_out.min_z(), double_box_3d_in.min_z()) &&
        almost_equal(double_box_3d_out.max_z(), double_box_3d_in.max_z()),
        true);
  TEST ("float_box_3d_out == float_box_3d_in",
        almost_equal(float_box_3d_out.min_x(), float_box_3d_in.min_x()) &&
        almost_equal(float_box_3d_out.max_x(), float_box_3d_in.max_x()) &&
        almost_equal(float_box_3d_out.min_y(), float_box_3d_in.min_y()) &&
        almost_equal(float_box_3d_out.max_y(), float_box_3d_in.max_y()) &&
        almost_equal(float_box_3d_out.min_z(), float_box_3d_in.min_z()) &&
        almost_equal(float_box_3d_out.max_z(), float_box_3d_in.max_z()),
        true);
  TEST ("double_homg_plane_3d_out == double_homg_plane_3d_in",
        almost_equal(double_homg_plane_3d_out.nx()*double_homg_plane_3d_in.d(),
                     double_homg_plane_3d_out.d()*double_homg_plane_3d_in.nx()) &&
        almost_equal(double_homg_plane_3d_out.ny()*double_homg_plane_3d_in.d(),
                     double_homg_plane_3d_out.d()*double_homg_plane_3d_in.ny()) &&
        almost_equal(double_homg_plane_3d_out.nz()*double_homg_plane_3d_in.d(),
                     double_homg_plane_3d_out.d()*double_homg_plane_3d_in.nz()),
        true);
  TEST ("float_homg_plane_3d_out == float_homg_plane_3d_in",
        almost_equal(float_homg_plane_3d_out.nx()*float_homg_plane_3d_in.d(),
                     float_homg_plane_3d_out.d()*float_homg_plane_3d_in.nx()) &&
        almost_equal(float_homg_plane_3d_out.ny()*float_homg_plane_3d_in.d(),
                     float_homg_plane_3d_out.d()*float_homg_plane_3d_in.ny()) &&
        almost_equal(float_homg_plane_3d_out.nz()*float_homg_plane_3d_in.d(),
                     float_homg_plane_3d_out.d()*float_homg_plane_3d_in.nz()),
        true);
  bool poly_is_equal;
  poly_is_equal = (polygon_out.num_sheets() == polygon_in.num_sheets());
  if (poly_is_equal==true)
  {
    for (int i=0;i<3;i++)
    {
      for (int j=0;j<5;j++)
      {
        poly_is_equal = (poly_is_equal && (
          almost_equal(polygon_out[i][j].x(), polygon_in[i][j].x()) &&
          almost_equal(polygon_out[i][j].y(), polygon_in[i][j].y())
          ));
      }
    }
  }
  TEST ("polygon_out == polygon_in",
        poly_is_equal,
        true);
  TEST ("double_point_2d_out == double_point_2d_in_bis",
        almost_equal(double_point_2d_out.x(), double_point_2d_in_bis.x()) &&
        almost_equal(double_point_2d_out.y(), double_point_2d_in_bis.y()),
        true);
}
