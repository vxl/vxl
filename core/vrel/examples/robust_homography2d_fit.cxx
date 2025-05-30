//:
// \file
// \author Chuck Stewart
// \author Bess Lee
//
// \brief Program demonstrating use of the Robust Estimation library in line fitting
//

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_vector.h"

#include <vrel/vrel_homography2d_est.h>
#include <vrel/vrel_lms_obj.h>
#include <vrel/vrel_lts_obj.h>
#include <vrel/vrel_ran_sam_search.h>
#include <vrel/vrel_ransac_obj.h>
#include <vrel/vrel_trunc_quad_obj.h>
#include <vrel/vrel_mlesac_obj.h>
#include <vrel/vrel_muset_obj.h>

int
main()
{
  //
  // Read in the data.
  //
  // homography_fit_20.dat contains data generated by homography_gen
  // with 20% gross outliers.
  //

  std::vector<vnl_vector<double>> from_pts;
  std::vector<vnl_vector<double>> to_pts;
  vnl_vector<double> p(3);
  double x1, x2, y1, y2, w1, w2;

  while (std::cin >> x1 >> y1 >> w1 >> x2 >> y2 >> w2)
  {
    p[0] = x1;
    p[1] = y1;
    p[2] = w1;
    from_pts.push_back(p);

    p[0] = x2;
    p[1] = y2;
    p[2] = w2;
    to_pts.push_back(p);
  }

  //
  // Construct the problem
  //

  vrel_homography2d_est * hg = new vrel_homography2d_est(from_pts, to_pts);

  double max_outlier_frac = 0.5;
  double desired_prob_good = 0.99;
  int max_pops = 1;
  int trace_level = 0;

  //
  //  Least Median of Squares
  //
  {
    int num_dep_res = hg->num_samples_to_instantiate();
    vrel_objective * lms = new vrel_lms_obj(num_dep_res);

    auto * ransam = new vrel_ran_sam_search;
    ransam->set_trace_level(trace_level);
    ransam->set_sampling_params(max_outlier_frac, desired_prob_good, max_pops);

    if (!ransam->estimate(hg, lms))
      std::cout << "LMS failed!!\n";
    else
    {
      std::cout << "LMS succeeded.\n"
                << "estimate = " << ransam->params() << std::endl
                << "scale = " << ransam->scale() << std::endl;
    }
    std::cout << std::endl;
    delete lms;
    delete ransam;
  }

  //
  //  RANSAC
  //
  {
    auto * ransac = new vrel_ransac_obj();
    hg->set_prior_scale(1.0);

    auto * ransam = new vrel_ran_sam_search;
    ransam->set_trace_level(trace_level);
    ransam->set_sampling_params(max_outlier_frac, desired_prob_good, max_pops);

    if (!ransam->estimate(hg, ransac))
      std::cout << "RANSAC failed!!\n";
    else
    {
      std::cout << "RANSAC succeeded.\n"
                << "estimate = " << ransam->params() << std::endl
                << "scale = " << ransam->scale() << std::endl;
    }
    std::cout << std::endl;
    delete ransac;
    delete ransam;
  }


  //
  //  MSAC
  //
  {
    auto * msac = new vrel_trunc_quad_obj();
    auto * ransam = new vrel_ran_sam_search;
    ransam->set_trace_level(trace_level);
    ransam->set_sampling_params(max_outlier_frac, desired_prob_good, max_pops);

    if (!ransam->estimate(hg, msac))
      std::cout << "MSAC failed!!\n";
    else
    {
      std::cout << "MSAC succeeded.\n"
                << "estimate = " << ransam->params() << std::endl
                << "scale = " << ransam->scale() << std::endl;
    }
    std::cout << std::endl;
    delete msac;
    delete ransam;
  }

  //
  //  MLESAC
  //
  {
    int residual_dof = hg->residual_dof();
    auto * mlesac = new vrel_mlesac_obj(residual_dof);

    hg->set_prior_scale(1.0);

    auto * ransam = new vrel_ran_sam_search;
    ransam->set_trace_level(trace_level);
    ransam->set_sampling_params(max_outlier_frac, desired_prob_good, max_pops);

    if (!ransam->estimate(hg, mlesac))
      std::cout << "MLESAC failed!!\n";
    else
    {
      std::cout << "MLESAC succeeded.\n"
                << "estimate = " << ransam->params() << std::endl
                << "scale = " << ransam->scale() << std::endl;
    }
    std::cout << std::endl;
    delete mlesac;
    delete ransam;
  }

  //
  //  MUSE
  //
  {
    hg->set_no_prior_scale();

    auto * muset = new vrel_muset_obj(from_pts.size() + 1);
    auto * ransam = new vrel_ran_sam_search;
    ransam->set_trace_level(trace_level);
    ransam->set_sampling_params(1 - muset->min_inlier_fraction(), desired_prob_good, max_pops);

    if (!ransam->estimate(hg, muset))
      std::cout << "MUSE failed!!\n";
    else
    {
      std::cout << "MUSE succeeded.\n"
                << "estimate = " << ransam->params() << std::endl
                << "scale = " << ransam->scale() << std::endl;
    }
    std::cout << std::endl;
    delete muset;
    delete ransam;
  }

  delete hg;

  return 0;
}
