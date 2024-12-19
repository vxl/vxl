// This is core/vrel/vrel_estimation_problem.cxx
#include <iostream>
#include <vector>
#include "vrel_estimation_problem.h"

#include <vrel/vrel_wls_obj.h>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

vrel_estimation_problem::vrel_estimation_problem(unsigned int dof, unsigned int num_samples_for_fit)
  : dof_(dof)
  , num_samples_for_fit_(num_samples_for_fit)
  , scale_type_(NONE)
  , single_scale_(0)
  , multiple_scales_(nullptr)
{}

vrel_estimation_problem::vrel_estimation_problem()

  = default;

vrel_estimation_problem::~vrel_estimation_problem() { delete multiple_scales_; }


void
vrel_estimation_problem::compute_weights(const std::vector<double> & residuals,
                                         const vrel_wls_obj * obj,
                                         double scale,
                                         std::vector<double> & weights) const
{
  if (residuals.size() != weights.size())
    weights.resize(residuals.size());

  switch (scale_type_)
  {
    case NONE:
      obj->wgt(residuals.begin(), residuals.end(), scale, weights.begin());
      break;
    case SINGLE:
      obj->wgt(residuals.begin(), residuals.end(), single_scale_, weights.begin());
      break;
    case MULTIPLE:
      obj->wgt(residuals.begin(), residuals.end(), multiple_scales_->begin(), weights.begin());
      break;
    default:
      assert(!"Invalid scale_type");
      break;
  }
}


const std::vector<double> &
vrel_estimation_problem::prior_multiple_scales() const
{
  assert(scale_type_ == MULTIPLE);
  return *multiple_scales_;
}

double
vrel_estimation_problem::prior_scale() const
{
  assert(scale_type_ == SINGLE);
  return single_scale_;
}

void
vrel_estimation_problem::set_prior_multiple_scales(const std::vector<double> & scales)
{
  if (!multiple_scales_)
  {
    multiple_scales_ = new std::vector<double>;
  }
  *multiple_scales_ = scales;
  scale_type_ = MULTIPLE;
}

void
vrel_estimation_problem::set_prior_scale(double scale)
{
  single_scale_ = scale;
  scale_type_ = SINGLE;
}

void
vrel_estimation_problem::set_no_prior_scale()
{
  scale_type_ = NONE;
}
