#include "msm_add_all_loaders.h"
//:
// \file
#include <msm/msm_translation_aligner.h>
#include <msm/msm_zoom_aligner.h>
#include <msm/msm_rigid_aligner.h>
#include <msm/msm_similarity_aligner.h>
#include <msm/msm_affine_aligner.h>
#include <msm/msm_subset_aligner.h>

#include <msm/msm_no_limiter.h>
#include <msm/msm_box_limiter.h>
#include <msm/msm_ellipsoid_limiter.h>

#include <mbl/mbl_cloneables_factory.h>

//: Add all binary loaders and factory objects for msm library
void msm_add_all_loaders()
{
  // msm_aligner classes:
  mbl_cloneables_factory<msm_aligner>::add(msm_translation_aligner());
  mbl_cloneables_factory<msm_aligner>::add(msm_zoom_aligner());
  mbl_cloneables_factory<msm_aligner>::add(msm_rigid_aligner());
  mbl_cloneables_factory<msm_aligner>::add(msm_similarity_aligner());
  mbl_cloneables_factory<msm_aligner>::add(msm_affine_aligner());
  mbl_cloneables_factory<msm_aligner>::add(msm_subset_aligner());

  vsl_add_to_binary_loader(msm_translation_aligner());
  vsl_add_to_binary_loader(msm_zoom_aligner());
  vsl_add_to_binary_loader(msm_rigid_aligner());
  vsl_add_to_binary_loader(msm_similarity_aligner());
  vsl_add_to_binary_loader(msm_affine_aligner());
  vsl_add_to_binary_loader(msm_subset_aligner());

  // msm_param_limiter classes:
  mbl_cloneables_factory<msm_param_limiter>::add(msm_no_limiter());
  mbl_cloneables_factory<msm_param_limiter>::add(msm_box_limiter());
  mbl_cloneables_factory<msm_param_limiter>::add(msm_ellipsoid_limiter());

  vsl_add_to_binary_loader(msm_no_limiter());
  vsl_add_to_binary_loader(msm_box_limiter());
  vsl_add_to_binary_loader(msm_ellipsoid_limiter());
}
