// This is brl/bseg/boxm2/pro/processes/boxm2_blob_change_detection_process.cxx
//:
// \file
// \brief  Executes blob change detection at specific threshold
//
// \author Andrew Miller
// \date May 26, 2011
#include <bprb/bprb_func_process.h>
#include <boxm2/util/boxm2_detect_change_blobs.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

namespace boxm2_blob_change_detection_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm2_blob_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_blob_change_detection_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";

  // process has no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
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

  //cast to float image
  vil_image_view<float>* change = dynamic_cast<vil_image_view<float>*>(change_img.ptr());
  if ( !change ) {
    vcl_cout<<"Detection Map cannot be converted to float image"<<vcl_endl;
    return false;
  }

  //detect change blobs
  vcl_vector<boxm2_change_blob> blobs;
  boxm2_util_detect_change_blobs( *change,thresh,blobs );

  //create a blob image
  vil_image_view<vxl_byte> blobImg(change_img->ni(), change_img->nj());
  blobImg.fill(0);
  vcl_vector<boxm2_change_blob>::iterator iter;
  for (iter=blobs.begin(); iter!=blobs.end(); ++iter)
  {
    //paint each blob pixel white
    for (unsigned int p=0; p<iter->blob_size(); ++p) {
      PairType pair = iter->get_pixel(p);
      blobImg( pair.x(), pair.y() ) = (vxl_byte) 255;
    }
  }
  vil_save( blobImg, "blobImg.png" );

  return true;
}

