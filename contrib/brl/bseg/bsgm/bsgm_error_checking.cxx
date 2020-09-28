// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

//#include <iomanip>
//#include <algorithm>
#include <cmath>

//#include "vil/vil_save.h"
//#include "vil/vil_convert.h"
//#include <vil/algo/vil_structuring_element.h>
//#include <vil/algo/vil_sobel_3x3.h>
//#include <vil/algo/vil_median.h>
//#include <vil/algo/vil_binary_erode.h>
//#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_error_checking.h"

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------
void bsgm_check_leftright(
  const vil_image_view<float>& disp1,
  const vil_image_view<float>& disp2,
  const vil_image_view<bool>& invalid1,
  const vil_image_view<bool>& invalid2,
  vil_image_view<bool>& error1)
{
  float dist_thresh = 1.0f;

  // Initialize error map
  error1.set_size(disp1.ni(), disp1.nj());
  error1.fill(true);

  // Iterate through first image
  for (int y = 0; y < static_cast<int>(disp1.nj()); y++) {
    for (int x = 0; x < static_cast<int>(disp1.ni()); x++) {
      if (invalid1(x, y)) continue;

      // Project each pixel into the second image
      int x2 = x + static_cast<int>(std::round(disp1(x, y) ));
      if (x2 < 0 || x2 >= static_cast<int>(disp2.ni())) continue;
      if (invalid2(x2, y)) continue;

      // Project back into first image and compute distance
      float x1 = x2 + disp2(x2, y);
      float dist = x1 - (float)x;
      if (fabs(dist) <= dist_thresh) error1(x, y) = false;
    }
  }
}


//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
void
bsgm_invert_disparities(
  vil_image_view<float>& disp_img,
  int old_invalid,
  int new_invalid )
{
  for( int y = 0; y < static_cast<int>(disp_img.nj()); y++ )
    for( int x = 0; x < static_cast<int>(disp_img.ni()); x++ )
      disp_img(x,y) = disp_img(x,y) == old_invalid ? new_invalid : -disp_img(x,y);
}
