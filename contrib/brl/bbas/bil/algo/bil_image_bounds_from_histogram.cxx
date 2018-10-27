#include <cstdlib>
#include "bil_image_bounds_from_histogram.h"
#include <vil/vil_new.h>
#include <vil/vil_property.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_polygon_scan_iterator.h>
void bil_image_bounds_from_histogram::init(){
  if(!imgr_)
    return;
  np_ = imgr_->nplanes();
  hists_.resize(np_, bsta_histogram<double>(0.0,0.0,1));
  data_.resize(np_);
}
bool bil_image_bounds_from_histogram::construct_histogram(){
vul_timer t;
  if (!this->init_histogram_from_data())
  {
    std::cout << "Unable to construct histogram in bgui_image_utils\n";
    return false;
  }
  // zero out the bins of the histogram according to the skip parameters
  for (unsigned p = 0; p<np_; ++p)
    for (unsigned s = 0; s<n_skip_lower_bins_; ++s)
      hists_[p].set_count(s, 0.0);

  unsigned last_bin = hists_[0].nbins()-1;
  for (unsigned p = 0; p<np_; ++p)
    for (unsigned s =last_bin; s>last_bin-n_skip_upper_bins_; s--)
      hists_[p].set_count(s, 0.0);

  std::cout << "computed histogram on " << np_*data_[0].size()
           << " pixels in " << t.real() << " milliseconds\n";
#if 0
  float area = hist_[0].area();
  float low_perc = percent_limit_;
#if 0
    std::cout << "n pix low " << low_perc*area << '\n';
#endif
  if(low_perc*area < 100.0f)
    low_perc = 100.0f/area;
  if(low_perc >0.01f)
    std::cout << "Warning too few pixels in image\n";
  else
    std::cout << "low limit based on " << low_perc*area << " pixels\n";
  float low = hist_[0].value_with_area_below(low_perc);
  float high = hist_[0].value_with_area_above(0.1f);
  float mean = hist_[0].mean_vals(low, high);
#if 0
  std::cout << " Hist stats \n";
  std::cout << "low\thigh\tmean\n";
  std::cout << low << '\t' << high << '\t' << mean << '\n';
#endif
#endif
  return true;
}
bool bil_image_bounds_from_histogram::
set_data_inside_polygon(vil_blocked_image_resource_sptr const& bir)
{
  vil_blocked_image_resource_sptr cbir =
    vil_new_cached_image_resource(bir);
  if (!cbir) {
    std::cout << "Null cached image resource to construct histogram\n";
    return false;
  }
  vgl_polygon_scan_iterator<double> psi(poly_);
  for (psi.reset(); psi.next(); ) {
    int j = psi.scany();
    int is  = psi.startx(), iend = psi.endx();
    vil_image_view_base_sptr const& view =
      cbir->get_view(is,(iend-is), j,1);
    if (!this->set_data_from_view(view))
      return false;
  }
  return true;
}
// Determine the pixel format of the view and cast to appropriate type
// Upcount the histogram with values from the view.
bool bil_image_bounds_from_histogram::set_data_from_view(vil_image_view_base_sptr const& view,
                                                         double fraction)
{
  if (!view)
  {
    std::cout << "null view in set_data_from_view\n";
    return false;
  }
  // interpret RGBA as a four band image
  vil_pixel_format type = vil_pixel_format_component_format(view->pixel_format());
  unsigned ni = view->ni(), nj = view->nj();
  auto area_frac = static_cast<float>(ni*nj*fraction);
  unsigned np = view->nplanes();
  switch (type )
  {
   case  VIL_PIXEL_FORMAT_BYTE:
   {
    vil_image_view<unsigned char> v = view;
    for (unsigned p = 0; p<np; ++p) {
      float cnt = 0.0f;
      while (cnt++<area_frac)
      {
        auto i = static_cast<unsigned>((ni-1)*(rand()/(RAND_MAX+1.0)));
        auto j = static_cast<unsigned>((nj-1)*(rand()/(RAND_MAX+1.0)));
        auto val = static_cast<double>(v(i,j,p));
        data_[p].push_back(val);
      }
    }
    return true;
   }
   case  VIL_PIXEL_FORMAT_UINT_16:
   {
    vil_image_view<unsigned short> v = view;
    for (unsigned p = 0; p<np; ++p) {
      float cnt = 0.0f;
      while (cnt++<area_frac)
      {
        auto i = static_cast<unsigned>((ni-1)*(rand()/(RAND_MAX+1.0)));
        auto j = static_cast<unsigned>((nj-1)*(rand()/(RAND_MAX+1.0)));
        auto val = static_cast<double>(v(i,j,p));
        data_[p].push_back(val);
      }
    }
    return true;
   }
   case VIL_PIXEL_FORMAT_FLOAT:
   {
    vil_image_view<float> v = view;
    for (unsigned p = 0; p<np; ++p) {
      float cnt = 0.0f;
      while (cnt++<area_frac)
      {
        auto i = static_cast<unsigned>((ni-1)*(rand()/(RAND_MAX+1.0)));
        auto j = static_cast<unsigned>((nj-1)*(rand()/(RAND_MAX+1.0)));
        auto val = static_cast<double>(v(i,j,p));
        data_[p].push_back(val);
      }
    }
    return true;
   }
   default:
     std::cout << "Format not supported for histogram construction by set_data_from_view\n";
  }
  return false;
}

bool bil_image_bounds_from_histogram::
set_data_by_random_blocks(const unsigned total_num_blocks,
                          vil_blocked_image_resource_sptr const& bir,
                          double fraction)
{
  unsigned nbi = bir->n_block_i(), nbj = bir->n_block_j();
  for (unsigned ib = 0; ib<total_num_blocks; ++ib)
  {
    auto bi = static_cast<unsigned>((nbi-1)*(rand()/(RAND_MAX+1.0)));
    auto bj = static_cast<unsigned>((nbj-1)*(rand()/(RAND_MAX+1.0)));
    if (!this->set_data_from_view(bir->get_block(bi, bj), fraction))
      return false;
  }
  return true;
}

bool bil_image_bounds_from_histogram::init_histogram_from_data()
{

  if (!imgr_)
    return false;

  vil_image_resource_sptr image;
  bool pyr = imgr_->get_property(vil_property_pyramid, nullptr);
  if (pyr)
  {
    // cast to pyramid resource
    vil_pyramid_image_resource_sptr pir =
      (vil_pyramid_image_resource*)((vil_image_resource*)imgr_.ptr());
    // highest resolution resource
    image = pir->get_resource(0);
  }
  else
    image = imgr_;
  // create a blocked image resource from image
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (!bir)
    bir = vil_new_blocked_image_facade(image);

  if (!poly_.num_sheets()) {
    if (!this->set_data_by_random_blocks(min_blocks_, bir, scan_fraction_))
      return false;
  }
  else {
    if (!this->set_data_inside_polygon(bir))
      return false;
  }

  // force RBGA to be interpreted as a four-band image
  vil_pixel_format type = vil_pixel_format_component_format(imgr_->pixel_format());

  if (np_!=1&&np_!=3&&np_!=4)
  {
    std::cout << "Format not supported by bil_image_bounds_from_histogram\n";
    return false;
  }
  double min_val=0.0, max_val = 255.0;
  switch (type )
  {
   case  VIL_PIXEL_FORMAT_BYTE:
    for (unsigned p = 0; p<np_; ++p) {
      hists_[p] = bsta_histogram<double>(min_val, max_val, 255);
      for (auto dit = data_[p].begin();
           dit != data_[p].end(); dit++)
        hists_[p].upcount(*dit, 1.0);
    }
    return true;
   case  VIL_PIXEL_FORMAT_UINT_16:
   {
    max_val = 65535.0;

    // determine the min and max range of image values
    std::vector<double> minr(np_, max_val), maxr(np_,min_val);
    for (unsigned p = 0; p<np_; ++p) {
      for (auto dit = data_[p].begin();
           dit != data_[p].end(); dit++)
      {
        if ((*dit)<minr[p]) minr[p] = *dit;
        if ((*dit)>maxr[p]) maxr[p] = *dit;
      }
      auto smin = static_cast<unsigned short>(minr[p]);
      auto smax = static_cast<unsigned short>(maxr[p]);
          // handle case where data is all the same
          if(smin == smax){
                  if(smin == 0)
                          smax = static_cast<unsigned short>(10);
                  else if(smax == max_val)
                          smin = static_cast<unsigned short>(max_val);
                  else
                          smin = 0;
          }
      auto nbins = static_cast<unsigned short>(smax-smin);
          if(nbins == 0)
                  nbins = 1;
           // determine if the number of bins exceeds the limit
      if (nbins>bin_limit_) {
        nbins = bin_limit_;
        // increase max value to make bin delta an integer
        double range = smax-smin;
        auto del = static_cast<unsigned short>(std::ceil(range/nbins));
                unsigned idel = del;
                idel*=nbins;
                if(idel>max_val)
                        smax = max_val;
                else
          smax = static_cast<unsigned short>(smin + nbins*del);
      }
      hists_[p] = bsta_histogram<double>(static_cast<double>(smin),
                                        static_cast<double>(smax), nbins);
      for (auto dit = data_[p].begin();
           dit != data_[p].end(); ++dit)
        hists_[p].upcount(*dit, 1.0);
    }
    return true;
   }
   case  VIL_PIXEL_FORMAT_FLOAT:
   {
     return true;
   }
   default:
    std::cout << "Format not supported by bgui_image_utils\n";
    return false;
  }
}
double bil_image_bounds_from_histogram::lower_bound(unsigned color_plane, double percent_limit) const{
   if (color_plane >=np_)
    return 0;
  return hists_[color_plane].value_with_area_below(percent_limit);
}

double bil_image_bounds_from_histogram::upper_bound(unsigned color_plane, double percent_limit) const{
   if (color_plane >=np_)
    return 0;
  return hists_[color_plane].value_with_area_above(percent_limit);
}
