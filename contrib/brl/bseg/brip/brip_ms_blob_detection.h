//-----------------------------------------------------------------------------
//:
// This is: brl/bseg/brip/brip_ms_blob_detection.h
// \file
// \brief Functions for blob detection on multispectral imagery
// \author Selene Chew
// \date June 07, 2017
//
//----------------------------------------------------------------------------
#ifndef __BRIP_BLOB_FUNCTIONS_H
#define __BRIP_BLOB_FUNCTIONS_H

#include <vector>
#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>

// Connected Components Bounding Box
// Computes bounding boxes for blobs detected by performing
// blob detection on the spectral angle map (SAM) computed using
// the masked pixels in mask_material as material samples.
// Thresholds blob image, finds connected components and prunes
// components of inadequate size.
//
// valid_mask contains the pixels in the region of interest.
// If you don't have a specific region on interest, a mask of
// all true will use whole region. Bounding box given by the
// min/max i/j values in the vectors (equivalently returned
// as vector of vgl_polygon)
//
// expects image as pre-calibrated multispectral band image
// if using worldview3 imagery, calibration can be done using
// brad/brad_wv3_functions
void brip_blob_connect_comp_bb(
  const vil_image_view<float>& image,
  const vil_image_view<bool>& material_mask,
  const vil_image_view<bool>& valid_mask,
  const float lambda0,
  const float lambda1,
  std::vector<unsigned>& i_min,
  std::vector<unsigned>& j_min,
  std::vector<unsigned>& i_max,
  std::vector<unsigned>& j_max,
  std::vector<vgl_polygon<double> >& poly
);

// Local Maximum Bounding Box
// Computes bounding boxes for local maximums of blob detection
// on the spectral angle map computed using
// the masked pixels in mask_material as material samples.
//
// conf is a vector of confidences corresponding to the boudning boxes
// valid_mask contains the pixels in the region of interest.
// If you don't have a specific region on interest, a mask of
// all true will use whole region. Bounding box given by the
// min/max i/j values in the vectors (equivalently returned
// as vector of vgl_polygon)
//
// expects image as pre-calibrated multispectral band image
// if using worldview3 imagery, calibration can be done using
// brad/brad_wv3_functions
void brip_blob_local_max_bb(
  const vil_image_view<float>& image,
  const vil_image_view<bool>& material_mask,
  const vil_image_view<bool>& valid_mask,
  const float lambda0,
  const float lambda1,
  const int n_size,
  std::vector<unsigned>& i_min,
  std::vector<unsigned>& j_min,
  std::vector<unsigned>& i_max,
  std::vector<unsigned>& j_max,
  std::vector<vgl_polygon<double> >& poly,
  std::vector<float>& conf
);
#endif
