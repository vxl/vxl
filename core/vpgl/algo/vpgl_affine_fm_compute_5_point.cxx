// This is core/vpgl/algo/vpgl_affine_fm_compute_5_point.cxx
#ifndef vpgl_affine_fm_compute_5_point_cxx_
#define vpgl_affine_fm_compute_5_point_cxx_
//:
// \file

#include <iostream>
#include "vpgl_affine_fm_compute_5_point.h"
#include "vpgl_affine_fm_robust_est.h"
#include <vrel/vrel_ran_sam_search.h>
#include <vrel/vrel_muset_obj.h>
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


//-------------------------------------------
bool
vpgl_affine_fm_compute_5_point::compute(const std::vector<vgl_point_2d<double>> & pr,
                                        const std::vector<vgl_point_2d<double>> & pl,
                                        vpgl_fundamental_matrix<double> & fm) const
{
  // Check that there are at least 5 points.
  if (pr.size() < 5 || pl.size() < 5)
  {
    std::cerr << "vpgl_affine_fm_compute_5_point: Need at least 5 point pairs.\n"
              << "Number in each set: " << pr.size() << ", " << pl.size() << std::endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if (pr.size() != pl.size())
  {
    std::cerr << "vpgl_affine_fm_compute_5_point: Need correspondence lists of same size.\n";
    return false;
  }

  // Condition if requested.
  std::vector<vgl_point_2d<double>> pr_norm, pl_norm;
  vgl_norm_trans_2d<double> prnt, plnt;
  if (precondition_)
  {
    // use standard homg normalization code
    std::vector<vgl_homg_point_2d<double>> pr_h, pl_h;
    for (auto p : pr)
      pr_h.emplace_back(p.x(), p.y(), 1.0);
    for (auto p : pl)
      pl_h.emplace_back(p.x(), p.y(), 1.0);

    prnt.compute_from_points(pr_h);
    plnt.compute_from_points(pl_h);
    for (unsigned int i = 0; i < pl.size(); i++)
    {
      vgl_homg_point_2d<double> pr_hn = prnt * pr_h[i];
      vgl_homg_point_2d<double> pl_hn = plnt * pl_h[i];
      pr_norm.emplace_back(pr_hn.x(), pr_hn.y());
      pl_norm.emplace_back(pl_hn.x(), pl_hn.y());
    }
  }
  else
  {
    for (unsigned int i = 0; i < pl.size(); i++)
    {
      pr_norm.push_back(pr[i]);
      pl_norm.push_back(pl[i]);
    }
  }
  vpgl_affine_fm_robust_est fg(pr_norm, pl_norm);
  // check if minimum number of points then skip ransac
  int min_n = fg.min_num_points();
  vnl_vector<double> params;
  if (pr_norm.size() == min_n)
  {
    std::vector<int> point_indices(min_n);
    for (int i = 0; i < min_n; ++i)
      point_indices[i] = i;
    fg.fit_from_minimal_set(point_indices, params);
  }
  else
  {
    // execute robust ransac (MUSE)
    double max_outlier_frac = 0.5;
    double desired_prob_good = 0.99;
    int max_pops = 1;
    int trace_level = 0;
    fg.set_no_prior_scale();

    vrel_muset_obj muset(pr_norm.size() + 1);
    vrel_ran_sam_search ransam;
    ransam.set_trace_level(trace_level);
    ransam.set_sampling_params(1 - muset.min_inlier_fraction(), desired_prob_good, max_pops);

    if (!ransam.estimate(&fg, &muset))
    { // access to base classes
      std::cout << "MUSE Fa estimator failed!!\n";
      return false;
    }
    if (verbose_)
      std::cout << "===MUSE Fa succeeded===.\n"
                << "estimate = " << ransam.params() << std::endl
                << "scale = " << ransam.scale() << std::endl;

    params = ransam.params();
  }
  fg.fill_Fa_from_params(params);
  vnl_matrix_fixed<double, 3, 3> Fn = fg.Fa().get_matrix(), Fu;
  if (precondition_)
    Fu = plnt.get_matrix().transpose() * Fn * prnt.get_matrix();
  else
    Fu = Fn;
  // zero out upper 2x2
  Fu[0][0] = 0.0;
  Fu[0][1] = 0.0;
  Fu[1][0] = 0.0;
  Fu[1][1] = 0.0;
  // normalize line coef
  double a = Fu[0][2], b = Fu[1][2];
  double s = sqrt(a * a + b * b);
  Fu /= s;
  fm.set_matrix(Fu);
  return true;
};
#endif // vpgl_affine_fm_compute_5_point_cxx_
