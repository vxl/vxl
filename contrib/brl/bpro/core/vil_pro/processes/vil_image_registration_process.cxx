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
//  Yi Dong Nov, 2015 -- update to increase search space along z direction
//  Yi Dong Dec, 2015 -- add option that uses mask image to mask out the invalid pixels
// \endverbatim
//
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


namespace vil_image_registration_process_globals
{
  unsigned n_inputs_  = 8;
  unsigned n_outputs_ = 5;
}

//:Constructor
bool vil_image_registration_process_cons(bprb_func_process& pro)
{
  using namespace vil_image_registration_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // source image that require registration
  input_types_[1] = "vil_image_view_base_sptr";  // target image
  input_types_[2] = "unsigned";                  // search space along image column
  input_types_[3] = "unsigned";                  // search space along image row
  input_types_[4] = "double";                    // search space in pixel values
  input_types_[5] = "double";                    // pixel resolution in meter unit
  input_types_[6] = "float";                     // invalid pixel value
  input_types_[7] = "vil_image_view_base_sptr";  // mask image used to mask out the invalid pixels
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";                   // translation along image column (trans_x)
  output_types_[1] = "double";                   // translation along image row (trans_y)
  output_types_[2] = "double";                   // translation along image pixel values (trans_z)
  output_types_[3] = "double";                   // minimized RMSEz given translation (trans_x, trans_y, transs_z)
  output_types_[4] = "double";                   // minimized Variance given translation (trans_x, trans_y, transs_z)

  // set default arguments
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(7, empty_mask);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: Execute the process
bool vil_image_registration_process(bprb_func_process& pro)
{
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs!!!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr src_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr tgr_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto sx = pro.get_input<unsigned>(in_i++);
  auto sy = pro.get_input<unsigned>(in_i++);
  auto   sz = pro.get_input<double>(in_i++);
  auto pixel_res = pro.get_input<double>(in_i++);
  auto invalid_pixel = pro.get_input<float>(in_i++);
  vil_image_view_base_sptr mask_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  // convert the image to float type
  unsigned s_ni = src_img_sptr->ni(), s_nj = src_img_sptr->nj();
  unsigned t_ni = tgr_img_sptr->ni(), t_nj = tgr_img_sptr->nj();
  if (s_ni != t_ni || s_nj != t_nj) {
    std::cerr << pro.name() << ": Image size mismatch, source image is (" << s_ni << "," << s_nj << ") but target image is (" << t_ni << "," << t_nj << ")!!!\n";
    return false;
  }

  vil_image_view<float> src_img = *(vil_convert_cast(float(), src_img_sptr));
  vil_image_view<float> tgr_img = *(vil_convert_cast(float(), tgr_img_sptr));

  // catch a "null" mask (not really null because that throws an error)
  bool use_mask = true;
  if (mask_img_sptr->ni() == 1 && mask_img_sptr->nj() == 1) {
    use_mask = false;
  }
  else {
    std::cout << "Use mask = true" << std::endl;
    if (mask_img_sptr->ni() != s_ni || mask_img_sptr->nj() != s_nj) {
      std::cerr << pro.name() << ": Mask image size mismatch -- source image size: (" << s_ni << "," << s_nj
               << "), mask image size: (" << mask_img_sptr->ni() << "," << mask_img_sptr->nj() << ")!!\n";
      return false;
    }
  }
  auto * mask_img=dynamic_cast<vil_image_view<unsigned char> *>(mask_img_sptr.ptr());
  if (!mask_img)
  {
    std::cerr << pro.name() <<": mask image is not an unsigned char map"<< std::endl;
    return false;
  }

  // compute the RMSE on the overlapped region
  int min_dx = -1*(int)(sx);
  int max_dx = (int)(sx);
  int min_dy = -1*(int)(sy);
  int max_dy = (int)(sy);
  double min_dz = -1*sz;
  double max_dz = sz;
  double step_dz = 0.2;
  std::map<double, std::vector<double> > rmse_map;
  for (int dx = min_dx; dx <= max_dx; dx++)
  {
    for (int dy = min_dy; dy <= max_dy; dy++)
    {
      for (double dz = min_dz; dz <= max_dz; dz+=step_dz)
      {
        double var = 0.0;
        unsigned num_p = 0;
        for (int i = sx; i < (int)(s_ni-sx); i++)
        {
          for (int j = sy; j < (int)(s_nj-sy); j++)
          {
            int si = i+dx, sj = j+dy;
            if ( use_mask ) {
              if ( (*mask_img)(i,j) == 0 || (*mask_img)(si, sj) == 0 )
                continue;
            }
            if ( std::abs(tgr_img(i, j) - invalid_pixel) < 1E-5)
              continue;
            if ( std::abs(src_img(si, sj)-invalid_pixel) < 1E-5)
              continue;
            double diff = (src_img(si, sj)-tgr_img(i,j)+dz)*(src_img(si, sj)-tgr_img(i,j)+dz);
            var += diff;
            num_p++;
          }
        }
        // compute rmse
        double rmse = std::sqrt(var/(double)num_p);
        std::vector<double> values;
        values.push_back(dx);
        values.push_back(dy);
        values.push_back(dz);
        values.push_back(var);
        rmse_map.insert(std::pair<double, std::vector<double> >(rmse,values));
      } // end of loop over z
    } // end of loop over y
  } // end of loop over x

  auto mit = rmse_map.begin();
  double rmse_z  = mit->first;
  double trans_x = pixel_res * mit->second[0];
  double trans_y = pixel_res * mit->second[1];
  double trans_z = mit->second[2];
  double var     = mit->second[3];

#if 0
  for (; mit != rmse_map.end(); ++mit) {
    std::cout << "RMES: " << mit->first << " -- dx: " << mit->second[0] << ", dy: " << mit->second[1] << ", dz: " << mit->second[2]
             << ", var: " << mit->second[3]
             << std::endl;
  }
#endif

  // output
  pro.set_output_val<double>(0, trans_x);
  pro.set_output_val<double>(1, trans_y);
  pro.set_output_val<double>(2, trans_z);
  pro.set_output_val<double>(3, rmse_z);
  pro.set_output_val<double>(4, var);

  return true;
}
