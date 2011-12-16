// This is brl/bseg/boxm2/pro/processes/boxm2_blob_change_detection_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Executes blob change detection at specific threshold
//
// \author Andrew Miller
// \date May 26, 2011

#include <boxm2/util/boxm2_detect_change_blobs.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vcl_vector.h>
#include <vcl_string.h>

namespace boxm2_blob_change_detection_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;

  const float EPSILON = .02f;
}

bool boxm2_blob_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_blob_change_detection_process_globals;

  //process takes 4 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";
  //two default args - depth maps from two scenes (will be combined to form mask)
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vil_image_view_base_sptr";

  // default arguments
  brdb_value_sptr empty_img1 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(1,1));
  brdb_value_sptr empty_img2 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(1,1));
  pro.set_input(2, empty_img1);
  pro.set_input(3, empty_img2);

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool boxm2_blob_change_detection_process(bprb_func_process& pro)
{
  using namespace boxm2_blob_change_detection_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr  change_img = pro.get_input<vil_image_view_base_sptr>(i++);
  float                     thresh     = pro.get_input<float>(i++);
  vil_image_view_base_sptr  depth_img1 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr  depth_img2 = pro.get_input<vil_image_view_base_sptr>(i++);

  //cast to float image
  vil_image_view<float>* change = dynamic_cast<vil_image_view<float>*>(change_img.ptr());
  if ( !change ) {
    vcl_cout<<"Detection Map cannot be converted to float image"<<vcl_endl;
    return false;
  }
  vil_image_view<float>* depth1 = dynamic_cast<vil_image_view<float>*>(depth_img1.ptr());
  if ( !depth1 ) {
    vcl_cout<<"Detection Map cannot be converted to float image"<<vcl_endl;
    return false;
  }
  vil_image_view<float>* depth2 = dynamic_cast<vil_image_view<float>*>(depth_img2.ptr());
  if ( !depth2 ) {
    vcl_cout<<"Detection Map cannot be converted to float image"<<vcl_endl;
    return false;
  }

  //use two images if they are non default, zero out change image at delta depth image > .02
  //Two depth maps are a comparison of the expected depth from the images point of
  //view.  If there is a large discrepency in depth, the change detection algorithm
  //will not use these pixels.
  if (depth1->ni() == depth2->ni() && depth1->ni() == change->ni()) {
    vcl_cout<<"boxm2_blob_change_detection_process::using depth maps for mask"<<vcl_endl;
    for (unsigned int i=0; i<change->ni(); ++i)
      for (unsigned int j=0; j<change->nj(); ++j)
        if ( (*depth1)(i,j) - (*depth2)(i,j) > EPSILON ||
             (*depth2)(i,j) - (*depth1)(i,j) > EPSILON )
          (*change)(i,j) = 0.0f;
  }
  vil_save( *change, "test_change.tiff");


  //detect change blobs
  vcl_vector<boxm2_change_blob> blobs;
  boxm2_util_detect_change_blobs( *change,thresh,blobs );

  //create a blob image
  vil_image_view<vxl_byte>* blobImg = new vil_image_view<vxl_byte>(change_img->ni(), change_img->nj());
  blobImg->fill(0);
  vcl_vector<boxm2_change_blob>::iterator iter;
  for (iter=blobs.begin(); iter!=blobs.end(); ++iter)
  {
    //paint each blob pixel white
#if 0
    if (iter->blob_size() < 500) {
#endif
      for (unsigned int p=0; p<iter->blob_size(); ++p) {
        PairType pair = iter->get_pixel(p);
        (*blobImg)( pair.x(), pair.y() ) = (vxl_byte) 255; // was: vnl_random::lrand32(10,255);
      }
#if 0
    }
    else {
      vcl_cout<<"Skipping blob of size "<<iter->blob_size()<<vcl_endl;
    }
#endif
  }

  //set outputs
  i = 0;
  pro.set_output_val<vil_image_view_base_sptr>(i, blobImg);
  return true;
}

