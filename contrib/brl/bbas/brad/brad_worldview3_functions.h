//-----------------------------------------------------------------------------
//:
// \file
// \brief Functions related to the WorldView 3 satellite
// \author Thomas Pollard
// \date April 13, 2017
//
// Functions and variables specific to the WorldView 3 satellite.  If the need
// arises, they can be generalized to more satellites at a later date.
//
//----------------------------------------------------------------------------
#ifndef __BRAD_WV3_FUNCTIONS_H
#define __BRAD_WV3_FUNCTIONS_H

#include <vil/vil_image_view.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <brad/brad_image_metadata.h>


//: Construct a 16-band composite image from a 8-band MUL image and an 8-band
// SWIR image.  In our test data the MUL and SWIR images are not pre-aligned 
// and are in fact very different, which we expect will be the case in general.
// This function creates an image in the MUL image's coordinate system and 
// warps the SWIR bands into it.  The composite image may be scaled or a 
// sub-region extracted from it by setting the relevant optional variables.
// 
// Setting the scale=0.335 will yield a composite image of approximate 
// resolution as the lower resolution SWIR band, which may be desirable.
//
// This function also performs the radiometric calibration using metadata as
// done in brad_nitf_abs_radiometric_calibration_process if the flag is set.
//
// WARNING: as all images including the 16-band float image are stored in 
// memory, this function may use many gigabytes of memory.
bool brad_compose_16band_wv3_img(
  const vil_image_view<vxl_uint_16>& mul_img,
  const vpgl_rational_camera<double>& mul_rpc,
  const brad_image_metadata& mul_meta,
  const vil_image_view<vxl_uint_16>& swir_img,
  const vpgl_rational_camera<double>& swir_rpc,
  const brad_image_metadata& swir_meta,
  vil_image_view<float>& comp_img,
  float scale = -1.0f,
  vgl_box_2d<int> mul_region = vgl_box_2d<int>(),
  bool calibrate_radiometrically = true);


//: A convenience wrapper function for dealing directly with filenames
bool brad_compose_16band_wv3_img(
  const std::string& mul_file,
  const std::string& swir_file,
  vil_image_view<float>& comp_img,
  float scale = -1.0f,
  vgl_box_2d<int> mul_region = vgl_box_2d<int>(),
  bool calibrate_radiometrically = true );


//: Apply the fixed gain/offset from the WorldView3 technical document:
// Radiometric_Use_of_WorldView-3_v2.pdf
void brad_apply_wv3_fixed_calibration(
  vil_image_view<float>& wv3_img);


//: The spectral bands present in a composite WorldView3 image.  An MUL image
// covers the first 8 bands, and SWIR covers the last 8 bands.
void brad_wv3_bands(
  std::vector<float>& bands_min,
  std::vector<float>& bands_max);


#endif
