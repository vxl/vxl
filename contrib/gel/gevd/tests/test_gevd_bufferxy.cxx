//*****************************************************************************
// File name: test_gevd_bufferxy.cxx
// Description: Test gevd_bufferxy class
//-----------------------------------------------------------------------------
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2002/04/05| Luis E. Galup            |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cstring.h> // memcpy
#include <vpl/vpl.h>
#include <testlib/testlib_test.h>
#include <gevd/gevd_bufferxy.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_image.h>

void
test_gevd_bufferxy()
{
  //test first type of constructor
  gevd_bufferxy * gbxy1 = new gevd_bufferxy(3,5,8);

  TEST("GetBitsPixel",gbxy1->GetBitsPixel(),8);
  TEST("GetBytesPixel",gbxy1->GetBytesPixel(),1);
  TEST("GetSizeX",gbxy1->GetSizeX(),3);
  TEST("GetSizeY",gbxy1->GetSizeY(),5);
  TEST("GetArea",gbxy1->GetArea(),15);
  TEST("GetSize",gbxy1->GetSize(),15);

  //GetBuffer tested in base class test

  for (int i=0;i<3;i++)
  {
    char * dataddr = (char *) gbxy1->GetElementAddr(i,1);
    vcl_memcpy(dataddr,"n",2);
  }

  char * dat = (char*) gbxy1->GetElementAddr(2,1);
  vcl_string strdat = dat;
  TEST("GetElementAddr",strdat,"n");

  //test second type of constructor
  gevd_bufferxy * gbxy2 = new gevd_bufferxy(3,5,8,gbxy1->GetBuffer());

  TEST("GetBitsPixel2",gbxy2->GetBitsPixel(),8);
  TEST("GetBytesPixel2",gbxy2->GetBytesPixel(),1);
  TEST("GetSizeX2",gbxy2->GetSizeX(),3);
  TEST("GetSizeY2",gbxy2->GetSizeY(),5);
  TEST("GetArea2",gbxy2->GetArea(),15);
  TEST("GetSize2",gbxy2->GetSize(),15);

  dat = (char*) gbxy2->GetElementAddr(2,1);
  strdat = dat;
  TEST("GetElementAddr2",strdat,"n");

  //test third type of constructor
  gevd_bufferxy * gbxy3 = new gevd_bufferxy(*gbxy2);

  TEST("GetBitsPixel3",gbxy3->GetBitsPixel(),8);
  TEST("GetBytesPixel3",gbxy3->GetBytesPixel(),1);
  TEST("GetSizeX3",gbxy3->GetSizeX(),3);
  TEST("GetSizeY3",gbxy3->GetSizeY(),5);
  TEST("GetArea3",gbxy3->GetArea(),15);
  TEST("GetSize3",gbxy3->GetSize(),15);

  dat = (char*) gbxy3->GetElementAddr(2,1);
  strdat = dat;
  TEST("GetElementAddr3",strdat,"n");

  const char * fn = "gevd_bufferxy_dump_file.tmp";
  gbxy1->dump(fn);
  vpl_unlink (fn);
  vcl_cout << "the following is a dump of a bufferxy. " << (*gbxy1) << vcl_endl;

  delete gbxy1;
  delete gbxy2;
  delete gbxy3;
  vcl_cout << "Test vil buffer constructor\n";

  //Test vil buffer constructor
  //Test byte constructor
  vil_image_resource_sptr rsb = vil_new_image_resource(3,4,1,VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<unsigned char> bview = rsb->get_view(0,3,0,4);
  for(unsigned r  = 0; r<4; ++r)
    for(unsigned c = 0; c<3; ++c)
      bview(c,r)= c*r;
  gevd_bufferxy bbuf(*rsb);
  
  bool good_buf = true;

  for(unsigned r  = 0; r<4; ++r)
    for(unsigned c = 0; c<3; ++c)
      good_buf = 
        good_buf&&(*((unsigned char*)bbuf.GetElementAddr(c,r)))==c*r;
  TEST("Unsigned char vil bufferxy constructor ",good_buf,true);  

  //Test unsigned short constructor
  vil_image_resource_sptr rs = vil_new_image_resource(3,4,1,VIL_PIXEL_FORMAT_UINT_16);
  vil_image_view<unsigned short> usview = rs->get_view(0,3,0,4);
  for(unsigned r  = 0; r<4; ++r)
    for(unsigned c = 0; c<3; ++c)
      usview(c,r)= 1000+c*r;
  gevd_bufferxy buf(*rs);
  
  good_buf = true;

  for(unsigned r  = 0; r<4; ++r)
    for(unsigned c = 0; c<3; ++c)
      good_buf = 
        good_buf&&(*((unsigned short*)buf.GetElementAddr(c,r)))==1000+c*r;
  TEST("Unsigned short vil bufferxy constructor ",good_buf,true);  
  vil_image_resource_sptr vm = vil_new_memory_image(3,4,1,VIL_PIXEL_FORMAT_UINT_16);
  
}

TESTMAIN(test_gevd_bufferxy);
