#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <icam/icam_depth_transform.h>
#include <icam/icam_sample.h>


static void test_icam_view_sphere()
{
  
//  icam_view_sphere sphere(bb, 100.0);
}

TESTMAIN( test_icam_view_sphere );
