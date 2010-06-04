// First define testmain
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vimt3d/vimt3d_run_length_encoding.h>


void test_run_length_encoding1()
{
  {
    vimt3d_image_3d_of<float> image(9,10,11);
    image.image().fill(0);
    image.image()(1,2,3)=4;
    image.image()(3,6,4)=5;
    image.image()(8,7,4)=6;
    image.image()(8,9,10)=14;

    vsl_b_ofstream out("./test_run_length_encoding1.bfs");
    vimt3d_run_length_encode(out,image);
    out.close();
  }

  vimt3d_image_3d_of<float> image2;
  vsl_b_ifstream in("./test_run_length_encoding1.bfs");
  vimt3d_run_length_decode(in,image2);
  in.close();
 
  TEST("Run length encoded image relaoded correctly: ", image2.image()(1,2,3)==4 , true );
  TEST("Run length encoded image relaoded correctly: ", image2.image()(3,6,4)==5, true );
  TEST("Run length encoded image relaoded correctly: ", image2.image()(8,7,4)==6 , true );
  TEST("Run length encoded image relaoded correctly: ", image2.image()(8,9,10)==14 , true );
  TEST("Run length encoded image relaoded correctly: ", image2.image()(1,2,2)==0 , true );
}

MAIN( test_run_length_encoding )
{
  START( "test_run_length_encoding" );
  test_run_length_encoding1();

  SUMMARY();
}

