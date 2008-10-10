#include <testlib/testlib_register.h>

DECLARE(test_move_proj_plane);
DECLARE(test_delaunay);
  
void
register_tests()
{
  REGISTER(test_move_proj_plane);
  REGISTER(test_delaunay);
}

DEFINE_MAIN;
