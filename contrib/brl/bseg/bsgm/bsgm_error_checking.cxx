// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

//#include <iomanip>
//#include <algorithm>
#include <cmath>
#include <tuple>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
//#include "vil/vil_save.h"
//#include "vil/vil_convert.h"
//#include <vil/algo/vil_structuring_element.h>
//#include <vil/algo/vil_sobel_3x3.h>
//#include <vil/algo/vil_median.h>
//#include <vil/algo/vil_binary_erode.h>
//#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_error_checking.h"


//-----------------------------------------------------------------------------
// implement non static memory for digital line generation
bool bsgm_generate_line(bool& init, float xs, float ys,
                        float xe, float ye,
                        float& x, float& y,
                        // internal memory 
                        float& dx, float& dy, float& mag,
                        float& xinc, float& yinc,
                        int& x1, int& y1)
{
  //static float dx, dy, mag, xinc, yinc;
  //static int x1, y1;
  if (init)
  {
    dx = xe-xs;
    dy = ye-ys;
    mag = (float)std::sqrt(dx*dx + dy*dy);
    if (mag<1)//Can't reach the next pixel under any circumstances
      return false;
    xinc = dx/mag;
    yinc = dy/mag;
    x1 = int(xe);
    y1 = int(ye);
    x = xs;
    y = ys;
    init = false;
//  done = false;
    return true;
  }

  //Previous pixel location
  int xp = int(x);
  int yp = int(y);
  //Increment along the line until the motion is greater than one pixel
  for (int i = 0; i<25; i++)
  {
    x += (float)0.5*xinc;
    y += (float)0.5*yinc;
    //Check to see if we have finished the span
    int xc = int(x), yc = int(y);
    bool passed_xe = ((xinc>=0)&&(xc>x1))||((xinc<0)&&(xc<x1));
    bool passed_ye = ((yinc>=0)&&(yc>y1))||((yinc<0)&&(yc<y1));
    if (passed_xe||passed_ye)
      return false;
    //Check if we have advanced by at least 1 pixel
    if (std::fabs(static_cast<float>(xc-xp))>=1||std::fabs(static_cast<float>(yc-yp))>=1)
      return true;
  }
  std::cout << "In bsgm_generate_line: - shouldn't happen - "
           << "xs="<<xs<<" ys="<<ys<<" xe="<<xe<<" ye="<< ye << std::endl;
  return false;
}


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
