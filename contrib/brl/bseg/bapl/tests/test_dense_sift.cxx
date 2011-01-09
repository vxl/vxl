#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_dense_sift_sptr.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>

#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>

static void test_dense_sift()
{
  vcl_string fname = testlib_root_dir();
  fname += "/contrib/brl/bseg/bapl/tests/kermit000.jpg";

  vil_image_view<vxl_byte> grey_img;
  vil_convert_planes_to_grey<vxl_byte,vxl_byte> (vil_load(fname.c_str()),grey_img);

  vcl_vector<bapl_keypoint_sptr> sift_keypoints;
  vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(grey_img);

  bapl_keypoint_extractor(image_sptr, sift_keypoints);

  bapl_dense_sift_sptr dense_sift_sptr = new bapl_dense_sift(image_sptr,3,6); //same parameters as default sift

  vcl_vector<bapl_lowe_keypoint_sptr> dense_sift_keypoints;

  vcl_vector<bapl_keypoint_sptr>::iterator keypoint_itr, keypoint_end;
  keypoint_end = sift_keypoints.end();

  unsigned indx;
  for (indx = 0, keypoint_itr = sift_keypoints.begin(); keypoint_itr != keypoint_end; ++keypoint_itr, ++indx)
  {
    //matt returns the keypoints as bapl_keypoint_sptr, dynamically cast so we can compare apples with apples.
    bapl_lowe_keypoint_sptr sift_lowe_keypoint = dynamic_cast<bapl_lowe_keypoint*>((*keypoint_itr).as_pointer());

    bapl_lowe_keypoint_sptr dense_lowe_keypoint;

    dense_sift_sptr->make_keypoint(dense_lowe_keypoint, sift_lowe_keypoint->location_i(), sift_lowe_keypoint->location_j());

    dense_sift_keypoints.push_back(dense_lowe_keypoint);

    //Because dense sift uses only the first orientation for the keypoint, we need to test keypoints with the same locations and
    //similar orientations. We can then check if the similar maximal scale was found.

    //As an aside, its hard to compare dense sift with results of regular sift because the normal algorithm has a refinement step
    //which is impossible to replicate when forcing a keypoint location. So there will be keypoints that don't pass this test.
    //Hopefully most will.
    if ( sift_lowe_keypoint->location_i() == dense_lowe_keypoint->location_i() &&
         sift_lowe_keypoint->location_j() == dense_lowe_keypoint->location_j() &&
         vcl_abs( sift_lowe_keypoint->orientation() - dense_lowe_keypoint->orientation() ) < 0.001 )
    {
      TEST_NEAR("Scale Test: ",dense_lowe_keypoint->scale(), sift_lowe_keypoint->scale(),0.5);
    }
  }//end keypoint iteration
}

TESTMAIN(test_dense_sift);
