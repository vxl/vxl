// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_visualize_change_process.cxx
//:
// \file
// \brief  A process for visualizing change detection on original image
//
// \author Andrew Miller
// \date Oct 10, 2011

#include <bprb/bprb_func_process.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>

//brdb stuff
#include <brdb/brdb_value.h>

namespace boxm2_ocl_visualize_change_process_globals
{
  const unsigned n_inputs_     = 4;
  const unsigned n_outputs_    = 1;
}

bool boxm2_ocl_visualize_change_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_visualize_change_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";   //change image
  input_types_[1] = "vil_image_view_base_sptr";   //original image
  input_types_[2] = "float";                      //threshold value
  input_types_[3] = "bool";                       // "true" if low value indicates change (default false)

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  //prob of change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default is .5 thresh val
  brdb_value_sptr thresh  = new brdb_value_t<float>(.5);
  pro.set_input(2, thresh);
  brdb_value_sptr low_is_change_default = new brdb_value_t<bool>(false);
  pro.set_input(3, low_is_change_default);
  return good;
}

bool boxm2_ocl_visualize_change_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_visualize_change_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr  change_sptr    = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr  in_sptr        = pro.get_input<vil_image_view_base_sptr>(i++);
  float                     thresh         = pro.get_input<float>(i++);                 //nxn
  bool                      low_is_change  = pro.get_input<bool>(i++);

  //prep in image, cast to grey float
  vil_image_view_base_sptr  f_in       = boxm2_util::prepare_input_image(in_sptr, true); //true for force gray scale
  vil_image_view<float>*    in_img     = dynamic_cast<vil_image_view<float>* >(f_in.ptr());
  unsigned ni=in_img->ni();
  unsigned nj=in_img->nj();


  //build a mask image
  vil_image_view<bool> mask(ni,nj);
  mask.fill(false);

  //try byte image (passed in mask)
  vil_image_view<vxl_byte>* change_b = dynamic_cast<vil_image_view<vxl_byte>* >(change_sptr.ptr());
  if (change_b)
  {
    if (low_is_change) {
      for (unsigned int i=0; i<ni; ++i)
        for (unsigned int j=0; j<nj; ++j)
          mask(i,j) = (*change_b)(i,j)==0 ? true : false;
    }
    else {
      for (unsigned int i=0; i<ni; ++i)
        for (unsigned int j=0; j<nj; ++j)
          mask(i,j) = (*change_b)(i,j)==0 ? false : true;
    }
  }
  else if (vil_image_view<float>* change_f = dynamic_cast<vil_image_view<float>* >(change_sptr.ptr()))
  {
    if (low_is_change) {
      for (unsigned int i=0; i<ni; ++i)
        for (unsigned int j=0; j<nj; ++j)
          mask(i,j) = ( (*change_f)(i,j) < thresh );
    }
    else {
      for (unsigned int i=0; i<ni; ++i)
        for (unsigned int j=0; j<nj; ++j)
          mask(i,j) =  ( (*change_f)(i,j) > thresh );
    }
  }

  //prepare output RGB image
  vil_image_view<vxl_byte>* out_img = new vil_image_view<vxl_byte>(ni,nj,3);
  for (unsigned int i=0; i<ni; ++i) {
    for (unsigned int j=0; j<nj; ++j) {
      //if it's change, mark it as change
      if ( mask(i,j) )  {
        (*out_img)(i,j,0) = (vxl_byte) 255;
        (*out_img)(i,j,1) = (vxl_byte) 0;
        (*out_img)(i,j,2) = (vxl_byte) 0;
      }
      else {
        //intensity at IJ
        float pixVal = (*in_img)(i,j);
        (*out_img)(i,j,0) = (vxl_byte) ( 255.0f * pixVal );
        (*out_img)(i,j,1) = (vxl_byte) ( 255.0f * pixVal );
        (*out_img)(i,j,2) = (vxl_byte) ( 255.0f * pixVal );
      }
    }
  }

  // store scene smaprt pointer
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, out_img);
  return true;
}
