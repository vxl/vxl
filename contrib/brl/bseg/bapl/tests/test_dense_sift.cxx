#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_dense_sift.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>


#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>

static void test_dense_sift()
{
  vcl_string fname = testlib_root_dir();

#ifdef VCL_WIN32
  fname += "\\contrib\\brl\\bseg\\bapl\\tests\\kermit000.jpg";
#else
  fname += "/contrib/brl/bseg/bapl/tests/kermit000.jpg";
#endif

  vil_image_view<vxl_byte> grey_img;
  vil_convert_planes_to_grey<vxl_byte,vxl_byte> (vil_load(fname.c_str()),grey_img);


  vcl_vector<bapl_keypoint_sptr> sift_keypoints;
  vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(grey_img);
  bapl_keypoint_extractor(image_sptr, sift_keypoints);

  //RUN DENSE SIFT AND COMPARE FEATURE VECTORS
  vcl_vector<bapl_lowe_keypoint_sptr> dense_sift_keypoints;

  vcl_vector<bapl_keypoint_sptr>::iterator keypoint_itr,keypoint_end;
  keypoint_end = sift_keypoints.end();

  unsigned indx;
  for (indx = 0, keypoint_itr = sift_keypoints.begin(); keypoint_itr!=keypoint_end; ++keypoint_itr, ++indx)
  {
    bapl_lowe_keypoint_sptr temp_keypoint = dynamic_cast<bapl_lowe_keypoint*>((*keypoint_itr).as_pointer());
    bapl_keypoint_sptr dense_keypoint_temp;
    bapl_dense_sift(image_sptr, temp_keypoint->location_i(), temp_keypoint->location_j(), dense_keypoint_temp,3,6);
    dense_sift_keypoints.push_back(dynamic_cast<bapl_lowe_keypoint*>(dense_keypoint_temp.as_pointer()));

    //we only take the first orientation so only test the keypoints if the orientations match
    if (dense_sift_keypoints[indx]->orientation() == temp_keypoint->orientation())
    {
      TEST_NEAR("TEST IF SCALES ARE NEAR", dense_sift_keypoints[indx]->scale() - temp_keypoint->scale() ,0,0.1f);
    }
  }//end keypoint iteration
}

TESTMAIN(test_dense_sift);
