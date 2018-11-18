#include <testlib/testlib_register.h>


DECLARE( bmsh3d_test_face_geom );
DECLARE( test_mesh_break_face );

void register_tests()
{
  REGISTER( bmsh3d_test_face_geom );
  REGISTER( test_mesh_break_face );
}

DEFINE_MAIN;
