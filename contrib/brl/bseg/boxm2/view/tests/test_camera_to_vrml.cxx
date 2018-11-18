#include <testlib/testlib_test.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <boxm2/view/boxm2_view_utils.h>

static void test_camera_to_vrml()
{

    vpgl_perspective_camera<double> pcam;
    vnl_matrix_fixed<double, 3, 3> k, Rm;
    vnl_vector_fixed<double, 3> tv;

    k(0,0)=1280;k(0,1)=   0;k(0,2)=512;
    k(1,0)=   0;k(1,1)=1280;k(1,2)=384;
    k(2,0)=   0;k(2,1)=   0;k(2,2)=1;

    vpgl_calibration_matrix<double> K(k);

    Rm(0,0)=-1.63064e-016; Rm(0,1)=   1;          Rm(0,2)=0;
    Rm(1,0)=0.87742;       Rm(1,1)=1.70003e-016;  Rm(1,2)=-0.479723;
    Rm(2,0)=-0.479723;     Rm(2,1)=-8.32667e-017; Rm(2,2)=-0.87742;

    vgl_rotation_3d<double> rot(Rm);
    vgl_vector_3d<double> t(-0.05 ,0.23576, 2.96987);
    pcam.set_calibration(K);
    pcam.set_rotation(rot);
    pcam.set_translation(t);

    vgl_vector_3d<double> rot_axis(0.240313,0.240313,0.940478);
    double rot_angle=1.63212;

    vgl_vector_3d<double> res_axis;
    double res_angle;
    boxm2_view_utils::convert_camera_to_vrml(pcam.principal_axis(),
                                           vgl_vector_3d<double>(0,0,1),
                                           res_axis,res_angle);

    TEST_NEAR("VRML Orientation ",(rot_axis-res_axis).length(),0.0,1e-3);

}

TESTMAIN(test_camera_to_vrml);
