// This is gel/vgel/vgel_kl_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgel_kl_params.h"

vgel_kl_params::vgel_kl_params()
{
  set_defaults();
}

vgel_kl_params::~vgel_kl_params()
{
}

vgel_kl_params::vgel_kl_params(const vgel_kl_params & params)
{
  numpoints = params.numpoints;
  search_range = params.search_range;
  mindist = params.mindist;
  window_width = params.window_width;
  window_height = params.window_height;
  min_eigenvalue = params.min_eigenvalue;
  min_determinant = params.min_determinant;
  min_displacement = params.min_displacement;
  max_iterations = params.max_iterations;
  max_residue = params.max_residue;
  grad_sigma = params.grad_sigma;
  smooth_sigma_fact = params.smooth_sigma_fact;
  pyramid_sigma_fact = params.pyramid_sigma_fact;
  replaceLostPoints = params.replaceLostPoints;
  sequentialMode = params.sequentialMode;
  smoothBeforeSelecting = params.smoothBeforeSelecting;
  writeInternalImages = params.writeInternalImages;
  nSkippedPixels = params.nSkippedPixels;
  verbose = params.verbose;
}


void vgel_kl_params::set_defaults()
{
  numpoints = 100;
  search_range = 15;

  mindist = 10;
  window_width = 7;
  window_height = 7;
  min_eigenvalue = 1;
  min_determinant = (float)0.01;
  min_displacement = (float)0.1;
  max_iterations = 10;
  max_residue = 10.0;
  grad_sigma = 1.0;
  smooth_sigma_fact = (float)0.1;
  pyramid_sigma_fact = (float)0.9;
  replaceLostPoints = true;
  sequentialMode = false;
  smoothBeforeSelecting = true;
  writeInternalImages = false;
  nSkippedPixels = 0;
  verbose = true;
}
