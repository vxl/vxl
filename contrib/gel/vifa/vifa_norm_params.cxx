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
  ilow(IntLow),
  plow(ProbLow),
  ihigh(IntHigh),
  phigh(ProbHigh),
  imin_(0.0f),
  imax_(0.0f),
  slope_(0.0f),
  b_(0.0f)
{
  calculate_clip_points();
}

vifa_norm_params::
vifa_norm_params(const vifa_norm_params&  old_params)
  : gevd_param_mixin(), vul_timestamp(), vbl_ref_count()
{
  ilow = old_params.ilow;
  plow = old_params.plow;
  ihigh = old_params.ihigh;
  phigh = old_params.phigh;
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
  if (imin_ == imax_)
    return raw_intensity;

  if (raw_intensity <= imin_)
    return 0.0f;

  if (raw_intensity >= imax_)
    return 1.0f;

  return raw_intensity * slope_ + b_;
}

bool vifa_norm_params::
get_norm_bounds(vil_image_view_base*  img,
                float                 low_bound_pcent,
                float                 high_bound_pcent,
                float&                normal_low,
                float&                normal_high)
{
  if (img && ((low_bound_pcent != 0.0f) || (high_bound_pcent != 0.0f)))
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
      temp_roi = new RectROI(0, 0, xsize, ysize);

    img->SetROI(temp_roi);
#endif  // ROI_SUPPORTED

    vifa_image_histogram  hist(img, 0.5f);
    normal_low = hist.LowClipVal(0.01f * low_bound_pcent);
    normal_high = hist.HighClipVal(0.01f * high_bound_pcent);

#ifdef ROI_SUPPORTED
    // Restore the original ROI
    img->SetROI(roi);
#endif  // ROI_SUPPORTED

    return true;
  }
  else
    return false;
}

void vifa_norm_params::
print_info(void)
{
  vcl_cout << "vifa_norm_params:\n"
           << "  low % thresh    = " << plow << vcl_endl
           << "  high % thresh   = " << phigh << vcl_endl
           << "  low int thresh  = " << ilow << vcl_endl
           << "  high int thresh = " << ihigh << vcl_endl
           << "  int min         = " << imin_ << vcl_endl
           << "  int max         = " << imax_ << vcl_endl;
}

void vifa_norm_params::
calculate_clip_points(void)
{
  imin_ = 0.0f;
  imax_ = 0.0f;

  float int_range = ihigh - ilow;
  if (int_range < 1e-4)
    return;

  float p_range = phigh - plow;
  if (p_range < 1e-6)
    return;

  // find m and b in y=mx+b

  slope_ = p_range / int_range;
  b_ = (plow - (slope_ * ilow));

  // solve for x when y=0, y=1

  imin_ = (0.0f - b_) / slope_;
  imax_ = (1.0f - b_) / slope_;

  //  vcl_cout << "slope: " << slope_ << " b: " << b_ << " imin: " << imin_
  //           << " imax " << imax_ << vcl_endl;
}
