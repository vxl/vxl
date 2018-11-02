#include <iostream>
#include <string>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vpgl/vpgl_perspective_camera.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <bwm/video/bwm_video_cam_istream.h>
#include <bwm/video/bwm_video_cam_ostream.h>

static void test_cam_iostream()
{
      vnl_double_3x3 m;
    m[0][0]=2000;m[0][1]=0;m[0][2]=512;
    m[1][0]=0;m[1][1]=2000;m[1][2]=384;
    m[2][0]=0;m[2][1]=0;m[2][2]=1;

    vpgl_calibration_matrix<double> K(m);
    vgl_homg_point_3d<double>center(0,0,-10.0);

    // rotation angle in radians
    double theta = vnl_math::pi_over_4; // 45 degrees
    // y axis is the rotation axis
    vnl_double_3 axis(0.0, 1.0, 0.0);
    vgl_h_matrix_3d<double> R;
    R.set_identity();
    R.set_rotation_about_axis(axis, theta);
    std::cout <<"Rotation Matrix\n" << R << '\n';
    vpgl_perspective_camera<double> P(K, center, vgl_rotation_3d<double>(R));

    std::cout << "Camera " << P;

    std::string dir = "cam_dir";
    std::cout << "Made camera stream directory "<< dir << '\n';
    vul_file::make_directory(dir.c_str());
    bwm_video_cam_ostream cam_ostr(dir);
    bool open = cam_ostr.is_open();
    TEST("open output camera stream", open, true);
    bool write = true;
    for (unsigned i = 0; i<11; ++i)
      write = write && cam_ostr.write_camera(&P);
    TEST("write to camera stream", write, true);
    cam_ostr.close();
    //test for reading
    std::string glob = dir + "/*";
    bwm_video_cam_istream cam_istr(glob);
    open = cam_istr.is_open();
    TEST("open input camera stream", open, true);
    bool read = false;
    vpgl_perspective_camera<double>* c = nullptr;
    c = cam_istr.read_camera();
    if(c){
      std::cout << "Camera from stream \n" << *c << '\n';
      read = (*c == P);
      if(!read){//may not not exactly be equal due to precision issues
        // Test principal axis vector instead
        vgl_vector_3d<double> pp = P.principal_axis();
        vgl_vector_3d<double> pc = c->principal_axis();
        vgl_vector_3d<double> er = pp-pc;
        read = er.length() < 1.0e-4;
      }
    }
    TEST("read input camera stream", read, true);
    vpl_unlink(dir.c_str());
}

TESTMAIN(test_cam_iostream);
