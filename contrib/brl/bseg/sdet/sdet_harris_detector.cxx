//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/sdet/sdet_harris_detector.cxx

//:
// \file
#include <vil/vil_memory_image_of.h>
#include <brip/brip_float_ops.h>
#include <vsol/vsol_point_2d.h>
#include <sdet/sdet_harris_detector.h>

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
void sdet_harris_detector::set_image(vil_image& image)
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

  vil_memory_image_of<unsigned char> input(image_);
  vil_memory_image_of<float> inputf = brip_float_ops::convert_to_float(input);
  vil_memory_image_of<float> smooth = brip_float_ops::gaussian(inputf, sigma_);
  vil_memory_image_of<float> IxIx, IxIy, IyIy, c;
  IxIx.resize(w,h);  IxIy.resize(w,h);   IyIy.resize(w,h);
  brip_float_ops::grad_matrix_NxN(smooth, n_, IxIx, IxIy, IyIy);
  c = brip_float_ops::harris(IxIx, IxIy, IyIy, scale_factor_);
  vcl_vector<float> x_pos, y_pos, val;
  brip_float_ops::non_maximum_supression(c, n_, thresh_, x_pos, y_pos, val);
  int n_corners = x_pos.size();
  if(!n_corners)
    {
      vcl_cout << "sdet_harris_detector::extract_corners()- "
               << "No Corners Found \n";
      return;
    }
  for(int i=0; i<n_corners; i++)
    {
      vsol_point_2d_sptr p = new vsol_point_2d(x_pos[i], y_pos[i]);
      points_.push_back(p);
    }
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

