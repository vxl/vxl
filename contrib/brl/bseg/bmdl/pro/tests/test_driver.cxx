#include <testlib/testlib_register.h>

DECLARE( test_bmdl_classify_process );
DECLARE( test_bmdl_trace_boundaries_process );
DECLARE( test_bmdl_generate_mesh_process );

void register_tests()
{
  REGISTER( test_bmdl_classify_process );
  REGISTER( test_bmdl_trace_boundaries_process );
  REGISTER( test_bmdl_generate_mesh_process );
}

DEFINE_MAIN;
