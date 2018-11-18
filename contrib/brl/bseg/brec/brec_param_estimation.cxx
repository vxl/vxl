//:
// \file
// \brief brec recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include "brec_param_estimation.h"
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_change_obj.h>
#include <bsta/bsta_joint_histogram.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: estimate the initial value as the real variation in the data
double
brec_param_estimation::estimate_fg_pair_density_initial_sigma(std::vector<std::pair<float, float> >& pairs)
{
  //: first find the mean dif
  double mean = 0.0;
  for (auto & pair : pairs) {
    mean += std::abs(pair.first-pair.second);
  }
  mean /= pairs.size();
  std::cout << " mean: " << mean << std::endl;

  double var = 0.0;
  for (auto & pair : pairs) {
    var += std::pow(double(std::abs(pair.first-pair.second)-mean), 2.0);
  }
  var /= pairs.size();

  std::cout << " variance: " << var << std::endl;

  return std::sqrt(var);
}


//: we always assume that the intensities are scaled to [0,1] range, so we get a vector of float pairs
//  Uses Levenberg-Marquardt
double
brec_param_estimation::estimate_fg_pair_density_sigma(std::vector<std::pair<float, float> >& pairs, double initial_sigma)
{
  fg_pair_density_est f(pairs);
  vnl_levenberg_marquardt lm(f);
  vnl_vector<double> x(1);   // init can be far off
  x[0] = initial_sigma;

  //lm.set_x_tolerance(0.00000001);
  //lm.set_f_tolerance(0.00000001);
  //lm.set_g_tolerance(0.00000001);

  //lm.minimize_without_gradient(x);
  lm.minimize_using_gradient(x);

  lm.diagnose_outcome(std::cout);

  std::cout << "x = " << x << std::endl;

  return x[0];
}

double
brec_param_estimation::estimate_fg_pair_density_sigma_amoeba(std::vector<std::pair<float, float> >& pairs, double initial_sigma)
{
  fg_pair_density_est_amoeba f(pairs);

  // Set up the initial guess
  vnl_vector<double> x(1);
  x[0] = initial_sigma;

  vnl_amoeba::minimize(f, x);

  // Summarize the results
  std::cout << "min at " << x << '\n';
  return x[0];
}

bool
brec_param_estimation::create_fg_pairs(const vil_image_resource_sptr& img, const bvgl_changes_sptr& c,
                                       std::vector<std::pair<float, float> >& pairs,
                                       bool print_histogram, const std::string& out_name)
{
  unsigned ni = img->ni();
  unsigned nj = img->nj();

  std::cout << "number of changes: " << c->size() << std::endl;
  if (!c->size()) {
    std::cout << "In brec_param_estimation::create_fg_pairs() -- zero changes\n";
    return false;
  }

  vil_image_view<float> inp_img(ni, nj, 1);

  if (img->nplanes() != 1) {
    std::cout << "In brec_param_estimation::create_fg_pairs() -- input view is not grey scale!\n";
    return false;
  }

  vil_image_view<vxl_byte> inp_img_view(img->get_view());
  vil_convert_stretch_range_limited(inp_img_view, inp_img, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);

  bsta_joint_histogram<float> jh(1.0, 255);
  pairs.clear();
  for (unsigned i = 0; i < c->size(); i++) {
    if (c->obj(i)->type() != "dont_care") {

      bvgl_changes_sptr dummy_c = new bvgl_changes();
      dummy_c->add_obj(c->obj(i));
      vil_image_view<vxl_byte> mask(dummy_c->create_mask_from_objs_all_types(ni, nj));

      //std::stringstream ss; ss << i;
      //vil_save(mask, ("./mask_saved_" + ss.str() + ".png").c_str());

      //: collect pairs from the region and the image
      vgl_polygon_scan_iterator<double> psi(c->obj(i)->poly(), true);
      for (psi.reset(); psi.next();)
      {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x)
        {
          if (x+1 < (int)ni && mask(x+1, y) == 255) {
            jh.upcount(inp_img(x, y), 1.0f, inp_img(x+1, y), 1.0f);
            pairs.emplace_back(inp_img(x, y), inp_img(x+1, y));
          }
        }
      }
    }
  }

  if (print_histogram) {
    std::ofstream of(out_name.c_str());
    jh.print_to_vrml(of);
    of.close();
  }

  return true;
}
