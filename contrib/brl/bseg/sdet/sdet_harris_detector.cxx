// This is brl/bseg/sdet/sdet_harris_detector.cxx
#include "sdet_harris_detector.h"
//:
// \file
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_qsort()
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <vsol/vsol_point_2d.h>

//: A container to support sorting of corners
//  Will result in descending order according to strength
struct sdet_harris_point
{
  sdet_harris_point () {};

  void set_point(vsol_point_2d_sptr const& p) {p_ = p;}
  void set_strength(const float s) {strength_ = s;}
  vsol_point_2d_sptr point() {return p_;}
  double strength() {return strength_;}

 private:
  float strength_;
  vsol_point_2d_sptr p_;
};

//The sort compare function
static int compare(sdet_harris_point*  pa,
                   sdet_harris_point*  pb)
{
  if (pa->strength() < pb->strength())
    return +1;
  return -1;
}

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
sdet_harris_detector::sdet_harris_detector(sdet_harris_detector_params& rpp)
  : sdet_harris_detector_params(rpp)
{
}

//:Default Destructor
sdet_harris_detector::~sdet_harris_detector()
{
}

//-------------------------------------------------------------------------
//: Set the image to be processed
//
void sdet_harris_detector::set_image(vil1_image& image)
{
  if (!image)
    {
      vcl_cout <<"In sdet_harris_detector::set_image(.) - null input\n";
      return;
    }
  points_valid_ = false;
  image_ = image;
}

//--------------------------------------------------------------------------
//: extract a set of vsol_point_2d(s)
void sdet_harris_detector::extract_corners()
{
  if (points_valid_)
    return;

  // Check the image
  if (!image_)
    {
      vcl_cout << "In sdet_harris_detector::extract_corners() - no image\n";
      return;
    }
  int w = image_.width(), h = image_.height();
  vcl_cout << "sdet_harris_detector::extract_corners(): width = "
           << w << " height = " << h << vcl_endl;

  //Process the image to extract the Harris corners
  points_.clear();
  vil1_memory_image_of<float> inputf = brip_vil1_float_ops::convert_to_float(image_);
  vil1_memory_image_of<float> smooth = brip_vil1_float_ops::gaussian(inputf, sigma_);
  vil1_memory_image_of<float> IxIx, IxIy, IyIy, c;
  IxIx.resize(w,h);  IxIy.resize(w,h);   IyIy.resize(w,h);
  brip_vil1_float_ops::grad_matrix_NxN(smooth, n_, IxIx, IxIy, IyIy);
  c = brip_vil1_float_ops::harris(IxIx, IxIy, IyIy, scale_factor_);
  vcl_vector<float> x_pos, y_pos, val;
  brip_vil1_float_ops::non_maximum_suppression(c, n_, thresh_, x_pos, y_pos, val);
  int n_corners = x_pos.size();
  vcl_cout << "Found " << n_corners << " above the threshold\n";
  if (!n_corners)
    {
      vcl_cout << "sdet_harris_detector::extract_corners()- "
               << "No Corners Found \n";
      return;
    }
  //Sort the corners according to strength
  sdet_harris_point* point_array = new sdet_harris_point[n_corners];
  for (int i = 0; i<n_corners; i++)
    {
      vsol_point_2d_sptr p = new vsol_point_2d(x_pos[i], y_pos[i]);
      point_array[i].set_point(p);
      point_array[i].set_strength(val[i]);
    }
  vcl_qsort(point_array, n_corners, sizeof(sdet_harris_point),
            (int (*)(const void *, const void *))&compare);
  //output the corners (limit by maximum number of corners)
  int num = (int)(percent_corners_*n_corners/100.0);
  if (num>n_corners)
    num = n_corners;
  for (int i=0; i<num; i++)
    {
      points_.push_back(point_array[i].point());
      // vcl_cout <<"s[" << i << "]=" << point_array[i].strength() << "\n";
    }
  delete [] point_array;
  points_valid_ = true;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_harris_detector::clear()
{
  points_.clear();
  points_valid_ = false;
}

