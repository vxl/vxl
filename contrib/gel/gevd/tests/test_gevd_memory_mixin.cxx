//*****************************************************************************
// File name: test_gevd_memory_mixin.cxx
// Description: Test gevd_memory_mixin class
//-----------------------------------------------------------------------------
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2002/04/05| Luis E. Galup            |Creation
//*****************************************************************************

#include <vcl_cstring.h> // memcpy
#include <testlib/testlib_test.h>
#include <gevd/gevd_memory_mixin.h>

void
test_gevd_memory_mixin()
{
  char * ib = 0;
  gevd_memory_mixin * gmm = new gevd_memory_mixin(8,(void*)ib);

  int off = gmm->GetOffset();
  TEST("GetOffset.",off,0);

  gmm->WriteBytes("hickory",8);

  gmm->SkipToStart();
  gmm->SkipBytes(1);
  gmm->SetOffset();
  off = gmm->GetOffset();

  TEST("SkipBytes",off,1);

  ib = new char[2];
  gmm->ReadBytes(ib,2);
  TEST("ReadBytes 1",ib[0]=='i' && ib[1]=='c', true);

  gmm->ReadBytes(ib,2,3);
  TEST("ReadBytes 2",ib[0]=='o' && ib[1]=='r', true);

  //now use the other constructor, and do the same stuff.
  char * buf = new char[8];
  const char * testwrd = "hickory";
  vcl_memcpy(buf,testwrd,8);
  gevd_memory_mixin * gmm2 = new gevd_memory_mixin(8,(void*)buf);

  off = gmm2->GetOffset();
  TEST("GetOffset2.",off,0);

  gmm2->SkipToStart();
  gmm2->SkipBytes(1);
  gmm2->SetOffset();
  off = gmm2->GetOffset();

  TEST("SkipBytes2",off,1);

  char * ib2 = new char[2];
  gmm2->ReadBytes(ib2,2);
  TEST("ReadBytes 3",ib2[0]=='i' && ib2[1]=='c', true);

  gmm2->ReadBytes(ib2,2,3);
  TEST("ReadBytes 4",ib2[0]=='o' && ib2[1]=='r', true);

  delete gmm;
  delete gmm2;
  delete ib;
  delete ib2;
}

TESTMAIN(test_gevd_memory_mixin);
