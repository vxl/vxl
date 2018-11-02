// This is brl/bseg/bbgm/pro/processes/bbgm_local_frame_trans_process.cxx
//:
// \file

#include <iostream>
#include <cmath>
#include <core/vidl_pro/vidl_pro_utils.h>
#include <bprb/bprb_func_process.h>
#include <brip/brip_vil_float_ops.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_mixture.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void eigenvalues(double B, double C, double E, double& la,
                        double& lb)
{
  double temp = std::sqrt((B-E)*(B-E)+4.0*C*C);
  la = 0.5*((B+E) + temp);
  lb = 0.5*((B+E) - temp);
}

bool bbgm_local_frame_trans_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> in_types(8), out_types(1);

  in_types[0]= "bbgm_image_sptr"; //background model
  in_types[1]= "vil_image_view_base_sptr"; //input frame
  in_types[2]= "vil_image_view_base_sptr"; //smoothed input frame
  in_types[3]= "vil_image_view_base_sptr"; //x gradient component
  in_types[4]= "vil_image_view_base_sptr"; //y gradient component
  in_types[5]= "double"; // min_eigenvalue
  in_types[6]= "double"; // max_condition_number
  in_types[7]= "double"; // max_pixel_shift
  pro.set_input_types(in_types);

  out_types[0]="vil_image_view_base_sptr";//transformed frame
  pro.set_output_types(out_types);
  return true;
}

//: Execute the process function
bool bbgm_local_frame_trans_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bbgm_local_frame_trans_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);
  if (!bgm)
  {
    std::cerr << "In bbgm_measure_process::execute() -"
             << " null background distribution image\n";
    return false;
  }
  typedef bsta_gauss_if3 bsta_gauss3_t;
  typedef bsta_gauss3_t::vector_type vector3_;
  typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
  typedef bsta_mixture<gauss_type3> mix_gauss_type3;
  typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;

  auto *model =
    static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());

  //Retrieve input frame
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(1);
  vil_image_view<float> frame = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(frame,1.0/255.0);

  temp = pro.get_input<vil_image_view_base_sptr>(2);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  vil_image_view<float> gauss_smooth = *vil_convert_cast(float(), temp);

  temp = pro.get_input<vil_image_view_base_sptr>(3);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  vil_image_view<float> Ix = *vil_convert_cast(float(), temp);

  temp = pro.get_input<vil_image_view_base_sptr>(4);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  vil_image_view<float> Iy = *vil_convert_cast(float(), temp);

  unsigned ni = frame.ni(), nj = frame.nj(), np = frame.nplanes();
  if (np!=3)
    return false;

  // copy the frame into the output translated frame
  // pixels are modified only if translation computation is valid
  auto* trans_frame = new vil_image_view<float>(frame);

  auto min_eigenvalue =  pro.get_input<double>(5);
  auto max_condition_number =  pro.get_input<double>(6);
  auto max_translation =  pro.get_input<double>(7);

  float total = 0;
  float n_translate = 0;
  double du = 0, dv = 0;
  //compute translation on a 5x5 window
  for (int j = 2; j<static_cast<int>(nj-2); ++j)
  {
    for (int i = 2; i<static_cast<int>(ni-2); ++i)
    {
      total += 1.0f;
      double A = 0, B = 0, C= 0, D = 0, E = 0;
      bool trans = true;
      //run over 5x5 neighborhood about i,j
      for (int jn = -2; jn<=2; ++jn)
        for (int in = -2; in<=2; ++in) {
          //retrieve distribution
          obs_mix_gauss_type3 mog = (*model)(i+in, j+jn);
          bsta_gauss3_t gc = mog.distribution(0);
          vector3_ mean = gc.mean();
          vector3_ var = gc.covar();
          //sum over bands
          for (unsigned p = 0; p<3; ++p)
          {
            double i0 = gauss_smooth(i+in, j+jn, p);
            double ix = Ix(i+in, j+jn, p);
            double iy = Iy(i+in, j+jn, p);
            double rvar = 1.0/var[p];
            A += ix*(i0-mean[p])*rvar;
            B += ix*ix*rvar;
            C += ix*iy*rvar;
            D += iy*(i0-mean[p])*rvar;
            E += iy*iy*rvar;
          }
        }
      //solve for local translation
      // first get eigenvalues to check for singularities
      double la, lb;
      eigenvalues(B, C, E, la, lb);
      if (std::fabs(la)< min_eigenvalue || std::fabs(lb)< min_eigenvalue)
        trans = false;
      if (std::fabs(la/lb)>max_condition_number)
        trans = false;
      if (trans)
      {
        //solve for translation
        double rdet = 1.0/(B*E - C*C);
        double inv00 = E*rdet, inv01 = -C*rdet;
        double inv10 = -C*rdet, inv11 = B*rdet;
        double tu = -(A*inv00 + D*inv01);
        double tv = -(A*inv10 + D*inv11);
        if (std::fabs(tu)<max_translation && std::fabs(tv)<max_translation) {
          n_translate += 1.0f;
          du += std::fabs(tu);
          dv += std::fabs(tv);
          for (unsigned p = 0; p<3; ++p) {
            double itrans = gauss_smooth(i,j,p)+
              tu*Ix(i,j,p)+tv*Iy(i,j,p);
            (*trans_frame)(i,j,p) = static_cast<float>(itrans);
          }
        }
      }
    }
    std::cout << '.';
  }
  std::cout << "\nFraction translated " << n_translate/total
           << " with <tu> = " << du/n_translate
           << "and <tv> = " << dv/n_translate << '\n' << std::flush;
  std::vector<std::string> output_types(1);
  output_types[0]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types);

  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(trans_frame);
  pro.set_output(0, output);

  return true;
}
