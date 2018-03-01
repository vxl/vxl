// This is brl/bseg/baml/baml_birchfield_tomasi.h
#ifndef baml_birchfield_tomasi_h_
#define baml_birchfield_tomasi_h_

#include <string>
#include <vector>
#include <set>
#include <utility>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vil/vil_image_view.h>


//:
// \file
// \brief
// \author Thomas Pollard
// \date October 10, 2016


//: Compute the difference between two images using the Birchfield-Tomasi
// metric
template <class T>
bool baml_compute_birchfield_tomasi(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  vil_image_view<T>& score,
  int rad = 1 );

#endif // baml_birchfield_tomasi_h_
