// This is gel/gevd/tests/test_gevd_bufferxy.cxx
// Description: Test gevd_bufferxy class

#include <iostream>
#include <cstring>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vpl/vpl.h"
#include "testlib/testlib_test.h"
#include <gevd/gevd_bufferxy.h>
#include "vil/vil_image_view.h"
#include "vil/vil_new.h"

void
test_gevd_bufferxy()
{
  // test first type of constructor
  auto * gbxy1 = new gevd_bufferxy(3,5,8);

  TEST("GetBitsPixel",  gbxy1->GetBitsPixel(),  8);
  TEST("GetBytesPixel", gbxy1->GetBytesPixel(), 1);
  TEST("GetSizeX",      gbxy1->GetSizeX(),      3);
  TEST("GetSizeY",      gbxy1->GetSizeY(),      5);
  TEST("GetArea",       gbxy1->GetArea(),      15);
  TEST("GetSize",       gbxy1->GetSize(),      15);

  // GetBuffer tested in base class test

  for (int i=0;i<3;i++)
  {
    char* dataddr = (char*) gbxy1->GetElementAddr(i,1);
    std::memcpy(dataddr,"n",2);
  }

  char* dat = (char*) gbxy1->GetElementAddr(2,1);
  std::string strdat = dat;
  TEST("GetElementAddr", strdat, "n");

  // test second type of constructor
  auto * gbxy2 = new gevd_bufferxy(3,5,8,gbxy1->GetBuffer());

  TEST("GetBitsPixel2", gbxy2->GetBitsPixel(), 8);
  TEST("GetBytesPixel2",gbxy2->GetBytesPixel(),1);
  TEST("GetSizeX2",     gbxy2->GetSizeX(),     3);
  TEST("GetSizeY2",     gbxy2->GetSizeY(),     5);
  TEST("GetArea2",      gbxy2->GetArea(),     15);
  TEST("GetSize2",      gbxy2->GetSize(),     15);

  dat = (char*) gbxy2->GetElementAddr(2,1);
  strdat = dat;
  TEST("GetElementAddr2", strdat, "n");

  // test third type of constructor
  auto * gbxy3 = new gevd_bufferxy(*gbxy2);

  TEST("GetBitsPixel3", gbxy3->GetBitsPixel(), 8);
  TEST("GetBytesPixel3",gbxy3->GetBytesPixel(),1);
  TEST("GetSizeX3",     gbxy3->GetSizeX(),     3);
  TEST("GetSizeY3",     gbxy3->GetSizeY(),     5);
  TEST("GetArea3",      gbxy3->GetArea(),     15);
  TEST("GetSize3",      gbxy3->GetSize(),     15);

  dat = (char*) gbxy3->GetElementAddr(2,1);
  strdat = dat;
  TEST("GetElementAddr3", strdat, "n");

  std::cout << "the following is a dump of a bufferxy:\n" << (*gbxy1)<< std::endl;
  const char * fn = "gevd_bufferxy_dump_file.tmp";
  gbxy1->dump(fn);
  // "unlink" should return 0 on success, nonzero on "file not found":
  TEST("dump bufferxy to a file", vpl_unlink(fn), 0);

  delete gbxy1;
  delete gbxy2;
  delete gbxy3;
  std::cout << "Test vil buffer constructor\n";

  // Test vil buffer constructor
  // Test byte constructor
  vil_image_resource_sptr rsb = vil_new_image_resource(3,4,1,VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<unsigned char> bview = rsb->get_view(0,3,0,4);
  for (unsigned r  = 0; r<4; ++r)
    for (unsigned c = 0; c<3; ++c)
      bview(c,r)= (unsigned char)(c*r);
  gevd_bufferxy bbuf(rsb);

  bool good_buf = true;

  for (unsigned r  = 0; r<4; ++r)
    for (unsigned c = 0; c<3; ++c)
      if (*((unsigned char*)bbuf.GetElementAddr(c,r)) != c*r)
        good_buf = false;
  TEST("Unsigned char vil bufferxy constructor ", good_buf, true);

  // Test byte constructor with non-contiguous memory
  vil_image_resource_sptr rsbn = vil_new_image_resource(5,7,1,VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<unsigned char> bviewn = rsbn->get_view();
  for (unsigned i = 0; i < bviewn.ni(); ++i){
    for (unsigned j = 0; j < bviewn.nj(); ++j){
      bviewn(i,j) = (unsigned char)(i*j);
    }
  }

  vil_image_view<unsigned char> bviewn2 = rsbn->get_view(1,3,2,4);
  vil_image_resource_sptr rsbn2 = vil_new_image_resource_of_view(bviewn2);
  gevd_bufferxy bbufn(rsbn2);

  good_buf = true;
  for (unsigned i = 0; i < bviewn2.ni(); ++i){
    for (unsigned j = 0; j < bviewn2.nj(); ++j){
      if (*((unsigned char*)bbufn.GetElementAddr(i,j)) != bviewn2(i,j)){
        good_buf = false;
      }
    }
  }
  TEST("Unsigned char vil non-contiguous bufferxy constructor ", good_buf, true);

  // Test unsigned short constructor
  vil_image_resource_sptr rs = vil_new_image_resource(3,4,1,VIL_PIXEL_FORMAT_UINT_16);
  vil_image_view<unsigned short> usview = rs->get_view(0,3,0,4);
  for (unsigned r  = 0; r<4; ++r)
    for (unsigned c = 0; c<3; ++c)
      usview(c,r)= (unsigned short)(1000+c*r);
  gevd_bufferxy buf(rs);

  good_buf = true;

  for (unsigned r  = 0; r<4; ++r)
    for (unsigned c = 0; c<3; ++c)
#if 0 // gevd_bufferxy returns an ordinary unsigned short buffer (old situation)
      if (*((unsigned short*)buf.GetElementAddr(c,r)) != 1000+c*r)
#else // gevd_bufferxy returns a normalized unsigned char buffer (new situation)
      if (*((unsigned char*)buf.GetElementAddr(c,r)) != c*r*255/6)
#endif
        good_buf = false;
  TEST("Unsigned short vil bufferxy constructor ", good_buf, true);

  // Test unsigned short constructor with non-contiguous memory
  vil_image_resource_sptr rsn = vil_new_image_resource(5,7,1,VIL_PIXEL_FORMAT_UINT_16);
  vil_image_view<unsigned short> usviewn = rsn->get_view();
  for (unsigned i = 0; i<usviewn.ni(); ++i){
    for (unsigned j = 0; j<usviewn.nj(); ++j){
      usviewn(i,j) = (unsigned short)(1000+i*j);
    }
  }

  vil_image_view<unsigned short> usviewn2 = rsn->get_view(1,3,2,4);
  vil_image_resource_sptr rsn2 = vil_new_image_resource_of_view(usviewn2);
  gevd_bufferxy bufn(rsn2);

  good_buf = true;
  for (unsigned i = 0; i < usviewn2.ni(); ++i){
    for (unsigned j = 0; j < usviewn2.nj(); ++j){
      // Minimum value of usviewn2 is 1002, maximum value is 1015, range is thus 13
      if (*((unsigned char*)bufn.GetElementAddr(i,j)) != (usviewn2(i,j)-1002)*255/13){
        good_buf = false;
      }
    }
  }
  TEST("Unsigned short vil non-contiguous bufferxy constructor ", good_buf, true);
}

TESTMAIN(test_gevd_bufferxy);
