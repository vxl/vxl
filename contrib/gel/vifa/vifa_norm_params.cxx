// This is gel/vifa/vifa_norm_params.cxx

#undef ROI_SUPPORTED  // No TargetJr-style ROI supported yet

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vifa/vifa_norm_params.h>
#include <vifa/vifa_image_histogram.h>
#ifdef ROI_SUPPORTED
#include <ImageClasses/RectROI.h>
#endif  // ROI_SUPPORTED


vifa_norm_params::
vifa_norm_params(float  IntLow,
                 float  ProbLow,
                 float  IntHigh,
                 float  ProbHigh) :
  _ilow(IntLow),
  _plow(ProbLow),
  _ihigh(IntHigh),
  _phigh(ProbHigh),
  _imin(0.0),
  _imax(0.0),
  _slope(0.0),
  _b(0.0)
{
  calculate_clip_points();
}

vifa_norm_params::
vifa_norm_params(const vifa_norm_params&  old_params)
{
  _ilow = old_params._ilow;
  _plow = old_params._plow;
  _ihigh = old_params._ihigh;
  _phigh = old_params._phigh;
  calculate_clip_points();
}

void vifa_norm_params::
recompute(void)
{
  calculate_clip_points();
}

float vifa_norm_params::
normalize(float raw_intensity)
{
  if (_imin == _imax)
  {
    return raw_intensity;
  }

  if (raw_intensity <= _imin)
  {
    return 0.0;
  }

  if (raw_intensity >= _imax)
  {
    return 1.0;
  }

  return raw_intensity * _slope + _b;
}

bool vifa_norm_params::
get_norm_bounds(vil_image_view_base*  img,
                float                 low_bound_pcent,
                float                 high_bound_pcent,
                float&                normal_low,
                float&                normal_high)
{
  if (img && ((low_bound_pcent != 0.0) || (high_bound_pcent != 0.0)))
  {
#ifdef ROI_SUPPORTED
    RectROI*  roi = img->GetROI();  // save the old ROI
    roi->Protect();

    int xsize = img->width();
    int  ysize = img->height();
    RectROI*  temp_roi;

    // For large images (>4M) bound the ROI to max 1K border around ROI
    if (xsize * ysize > 4000000)
    {
      int  border_size = 1000;
      int startx = vcl_max(roi->GetOrigX() - border_size, 0);
      int starty = vcl_max(roi->GetOrigY() - border_size, 0);
      int roi_sizex = vcl_min(roi->GetSizeX() + 2 * border_size, xsize);
      int roi_sizey = vcl_min(roi->GetSizeY() + 2 * border_size, ysize);
      temp_roi = new RectROI(startx, starty, roi_sizex, roi_sizey);
    }
    else
    {
      temp_roi = new RectROI(0, 0, xsize, ysize);
    }
    img->SetROI(temp_roi);
#endif  // ROI_SUPPORTED

    vifa_image_histogram  hist(img, 0.50);
    normal_low = hist.LowClipVal(0.01 * low_bound_pcent);
    normal_high = hist.HighClipVal(0.01 * high_bound_pcent);

#ifdef ROI_SUPPORTED
    // Restore the original ROI
    img->SetROI(roi);
#endif  // ROI_SUPPORTED

    return true;
  }
  else
  {
    return false;
  }
}

void vifa_norm_params::
print_info(void)
{
  vcl_cout << "vifa_norm_params:\n"
           << "  low % thresh    = " << _plow << vcl_endl
           << "  high % thresh   = " << _phigh << vcl_endl
           << "  low int thresh  = " << _ilow << vcl_endl
           << "  high int thresh = " << _ihigh << vcl_endl
           << "  int min         = " << _imin << vcl_endl
           << "  int max         = " << _imax << vcl_endl;
}

void vifa_norm_params::
calculate_clip_points(void)
{
  _imin = 0.0;
  _imax = 0.0;

  float int_range = _ihigh - _ilow;
  if (int_range < 1.0e-04)
  {
    return;
  }

  float p_range = _phigh - _plow;
  if (p_range < 1.0e-06)
  {
    return;
  }

  // find m and b in y=mx+b

  _slope = p_range / int_range;
  _b = (_plow - (_slope * _ilow));

  // solve for x when y=0, y=1

  _imin = (0.0 - _b) / _slope;
  _imax = (1.0 - _b) / _slope;

  //  vcl_cout << "slope: " << _slope << " b: " << _b << " imin: " << _imin
  //           << " imax " << _imax << vcl_endl;
}
