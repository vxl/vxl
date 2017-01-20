#include <testlib/testlib_register.h>


DECLARE( test_eye );
DECLARE( test_orbit );
DECLARE( test_fit_orbit );
DECLARE( test_fit_margin );
DECLARE( test_pc_viewer );
DECLARE( test_mandible);
DECLARE( test_cranium);
DECLARE( test_skin);
DECLARE( test_composite_face);
DECLARE( test_mouth);
DECLARE( test_middle_fat_pocket);
DECLARE( test_fat_pocket_scene);
DECLARE( test_fit_face );
DECLARE( test_fit_fat_pocket );
DECLARE( test_orbicularis_oris);
DECLARE( test_shuttle);

void register_tests()
{

  REGISTER( test_eye );
  REGISTER( test_orbit );
  REGISTER( test_fit_orbit );
  REGISTER( test_fit_margin );
  REGISTER( test_pc_viewer );
  REGISTER( test_mandible );
  REGISTER( test_cranium );
  REGISTER( test_skin );
  REGISTER( test_composite_face );
  REGISTER( test_mouth );
  REGISTER( test_middle_fat_pocket );
  REGISTER( test_fat_pocket_scene );
  REGISTER( test_fit_face );
  REGISTER( test_fit_fat_pocket );
  REGISTER( test_orbicularis_oris );
  REGISTER( test_shuttle );
}

DEFINE_MAIN;
