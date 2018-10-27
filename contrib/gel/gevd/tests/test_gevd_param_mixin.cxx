//*****************************************************************************
// File name: test_gevd_param_mixin.cxx
// Description: Test gevd_param_mixin class
//-----------------------------------------------------------------------------
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2002/04/04| Luis E. Galup            |Creation
//*****************************************************************************

#include <gevd/gevd_param_mixin.h>
#include <testlib/testlib_test.h>

void
test_gevd_param_mixin()
{
  auto * pgm = new gevd_param_mixin();

  TEST("SanityCheck",pgm->SanityCheck(),true);

  pgm->SetErrorMsg("yo dude");
  TEST("SetErrorMsg",pgm->Valid(),false);

  delete pgm;
}

TESTMAIN(test_gevd_param_mixin);
