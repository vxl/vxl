#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/algo/vil_distance_transform.h>

static void test_algo_distance_transform()
{
  vcl_cout << "********************************\n"
           << " Testing vil_distance_transform\n"
           << "********************************\n";

  vil_image_view<float> src_im(21,21,1);

  src_im.fill(99.0);
  src_im(10,10)=0;
  src_im(0,5)=0;
  src_im(5,20)=0;
  vil_distance_transform(src_im);

  TEST_NEAR("(10,10)",src_im(10,10),0,1e-6);
  TEST_NEAR("(10,11)",src_im(10,11),1,1e-6);
  TEST_NEAR("(11,10)",src_im(11,10),1,1e-6);
  TEST_NEAR("(10,9)" ,src_im(10,9),1,1e-6);
  TEST_NEAR("(9,10)" ,src_im(9,10),1,1e-6);
  TEST_NEAR("(9,9)"  ,src_im(9,9),1.414214,1e-6);
  TEST_NEAR("(11,9)" ,src_im(11,9),1.4142141,1e-6);
  TEST_NEAR("(9,11)" ,src_im(9,11),1.414214,1e-6);
  TEST_NEAR("(11,11)",src_im(11,11),1.414214,1e-6);
  TEST_NEAR("(11,12)",src_im(11,12),2.414214,1e-6);

  vcl_cout<<"Test edge\n";
  TEST_NEAR("(0,5)",src_im(0,5),0,1e-6);
  TEST_NEAR("(0,3)",src_im(0,3),2,1e-6);
  TEST_NEAR("(0,4)",src_im(0,4),1,1e-6);
  TEST_NEAR("(0,6)",src_im(0,6),1,1e-6);
  TEST_NEAR("(0,7)",src_im(0,7),2,1e-6);

  vcl_cout<<"Test edge\n";
  TEST_NEAR("(5,20)",src_im(5,20),0,1e-6);
  TEST_NEAR("(4,20)",src_im(4,20),1,1e-6);
  TEST_NEAR("(6,20)",src_im(6,20),1,1e-6);
  TEST_NEAR("(7,20)",src_im(7,20),2,1e-6);
}

TESTMAIN(test_algo_distance_transform);
