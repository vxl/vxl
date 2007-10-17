#include <testlib/testlib_register.h>


DECLARE( bmsh3d_test_face_geom );
DECLARE( test_mesh_break_face );
DECLARE( bmsh3d_mesh_tri_test );

void register_tests()
{
  REGISTER( bmsh3d_test_face_geom );
  REGISTER( test_mesh_break_face );
  // REGISTER( dbmsh3d_mesh_tri_test );
}

DEFINE_MAIN;


