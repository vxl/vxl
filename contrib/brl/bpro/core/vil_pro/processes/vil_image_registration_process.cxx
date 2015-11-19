// This is brl/bpro/core/vil_pro/processes/vil_image_registration_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//      Process to register a source image to a target image by minimizing the root-mean-square-error (RMSE) of pixel value difference, in a given range of offset adjustment
//      RMSE is computed as sqrt( sum(src(xi-dx, yi-dy) - tar(xi,yi))^2 / N ) where summation is applied on valid pixels used for registration and N is the number of valid pixels
//      The output will be the RMSE along image column, image row and the minimized RMSE of pixel values.  Note that the registration is obtained by explicitly computing the 
//      RMSE given all possible shift, instead of optimization
//
// \verbatim
//  Modifications
// \endverbatim
//
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


namespace vil_image_registration_process_globals
{
  unsigned n_inputs_  = 6;
  unsigned n_outputs_ = 3;
}

//:Constructor
bool vil_image_registration_process_cons(bprb_func_process& pro)
{
  using namespace vil_image_registration_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // source image that require registration
  input_types_[1] = "vil_image_view_base_sptr";  // target image
  input_types_[2] = "unsigned";                  // search space along image column
  input_types_[3] = "unsigned";                  // search space along image row
  input_types_[4] = "double";                    // pixel resolution in meter unit
  input_types_[5] = "float";                     // invalid pixel value
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "double";                   // output offset along column (RMSE_x)
  output_types_[1] = "double";                   // output offset along row (RMSE_y)
  output_types_[2] = "double";                   // RMES of pixel value difference (RMSE_z)
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: Execute the process
bool vil_image_registration_process(bprb_func_process& pro)
{
  // sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << pro.name() << ": Wrong inputs!!!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr src_img_ptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr tgr_img_ptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  unsigned sx = pro.get_input<unsigned>(in_i++);
  unsigned sy = pro.get_input<unsigned>(in_i++);
  double pixel_res = pro.get_input<double>(in_i++);
  float invalid_pixel = pro.get_input<float>(in_i++);

  // convert the image to float type
  unsigned s_ni = src_img_ptr->ni(), s_nj = src_img_ptr->nj();
  unsigned t_ni = tgr_img_ptr->ni(), t_nj = tgr_img_ptr->nj();
  if (s_ni != t_ni || s_nj != t_nj) {
    vcl_cerr << pro.name() << ": Image size mismatch, source image is (" << s_ni << "," << s_nj << ") but target image is (" << t_ni << "," << t_nj << ")!!!\n";
    return false;
  }
  
  vil_image_view<float> src_img = *(vil_convert_cast(float(), src_img_ptr));
  vil_image_view<float> tgr_img = *(vil_convert_cast(float(), tgr_img_ptr));

  // compute the RMSE on the overlapped region
  int min_dx = -1*(int)(sx);
  int max_dx = (int)(sx);
  int min_dy = -1*(int)(sy);
  int max_dy = (int)(sy);
  vcl_map<double, vcl_pair<int, int> > rmse_map;
  for (int dx = min_dx; dx <= max_dx; dx++)
  {
    for (int dy = min_dy; dy <= max_dy; dy++)
    {
      double sum = 0.0;
      unsigned num_p = 0;
      for (int i = sx; i < (s_ni-sx); i++)
      {
        for (int j = sy; j < (s_nj-sy); j++)
        {
          int si = i+dx, sj = j+dy;
          if ( vcl_abs(tgr_img(i, j) - invalid_pixel) < 1E-5)
            continue;
          if ( vcl_abs(src_img(si, sj)-invalid_pixel) < 1E-5)
            continue;
          double diff = (src_img(si, sj)-tgr_img(i,j))*(src_img(si, sj)-tgr_img(i,j));
          sum += diff;
          num_p++;
        }
      }
      // compute rmse
      double rmse = vcl_sqrt(sum/(double)num_p);
      vcl_pair<int, int> key(dx, dy);
      rmse_map.insert(vcl_pair<double, vcl_pair<int, int> >(rmse,key));
    }
  }

  vcl_map<double, vcl_pair<int, int> >::iterator mit = rmse_map.begin();
  double rmse_z = pixel_res * mit->first;
  double rmse_x = pixel_res * vcl_abs(mit->second.first);
  double rmse_y = pixel_res * vcl_abs(mit->second.second);
#if 0
  for (; mit != rmse_map.end(); ++mit) {
    vcl_cout << "RMES: " << mit->first << " -- dx: " << mit->second.first << ", dy: " << mit->second.second << vcl_endl;
  }
#endif

  // output
  pro.set_output_val<double>(0, rmse_x);
  pro.set_output_val<double>(1, rmse_y);
  pro.set_output_val<double>(2, rmse_z);

  return true;
}