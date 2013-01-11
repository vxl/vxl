#include "bgui_image_utils.h"

#include <vcl_cstdlib.h> // for rand()
#include <vcl_cmath.h> // for ceil()

#include <vil/vil_new.h>
#include <vil/vil_property.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vgui/vgui_range_map_params.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

bgui_image_utils::bgui_image_utils():
  hist_valid_(false), percent_limit_(0.0002), bin_limit_(1000),
  n_skip_upper_bins_(0), n_skip_lower_bins_(1), min_blocks_(50),
  scan_fraction_(0.005), image_(0), poly_(0)
{
}

bgui_image_utils::bgui_image_utils(vil_image_resource_sptr const& image,
                                   vsol_polygon_2d_sptr const& poly):
  hist_valid_(false), percent_limit_(0.0002),
  bin_limit_(1000), n_skip_upper_bins_(0),
  n_skip_lower_bins_(1), min_blocks_(50),
  scan_fraction_(0.005), image_(image), poly_(poly)
{
  if (!image)
    return;
  unsigned np = image->nplanes();
  hist_.resize(np,bsta_histogram<double>(0.0,0.0,1));
  data_.resize(np);
}

void bgui_image_utils::set_image(vil_image_resource_sptr const& image)
{
  if (!image)
  {
    image_ = 0;
    return;
  }
  image_ = image;
  unsigned np = image->nplanes();
  hist_.resize(np,bsta_histogram<double>(0.0,0.0,1));
  data_.resize(np);
  hist_valid_ = false;
}

void bgui_image_utils::set_poly(vsol_polygon_2d_sptr const& poly)
{
  poly_ = poly;
}

bool bgui_image_utils::range(double& min_value, double& max_value,
                             unsigned plane)
{
  if (!hist_valid_)
    if (!this->construct_histogram())
      return false;

  min_value = this->compute_lower_bound(plane);
  max_value = this->compute_upper_bound(plane);
  return min_value < max_value;
}

// fill the histogram by randomly sampling pixels from the image
bool bgui_image_utils::init_histogram_from_data()
{
  hist_valid_ = false;
  if (!image_)
    return false;

  vil_image_resource_sptr image;
  bool pyr = image_->get_property(vil_property_pyramid, 0);
  if (pyr)
  {
    // cast to pyramid resource
    vil_pyramid_image_resource_sptr pir =
      (vil_pyramid_image_resource*)((vil_image_resource*)image_.ptr());
    // highest resolution resource
    image = pir->get_resource(0);
  }
  else
    image = image_;
  // create a blocked image resource from image
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (!bir)
    bir = vil_new_blocked_image_facade(image);

  if (!poly_) {
    if (!this->set_data_by_random_blocks(min_blocks_, bir, scan_fraction_))
      return false;
  }
  else {
    if (!this->set_data_inside_polygon(bir))
      return false;
  }
  unsigned np = image_->nplanes();

  vil_pixel_format type = image_->pixel_format();

  if (np!=1&&np!=3&&np!=4)
  {
    vcl_cout << "Format not supported by bgui_image_utils\n";
    return false;
  }
  double min_val=0.0, max_val = 255.0;
  switch (type )
  {
   case  VIL_PIXEL_FORMAT_BYTE:
    for (unsigned p = 0; p<np; ++p) {
      hist_[p] = bsta_histogram<double>(min_val, max_val, 255);
      for (vcl_vector<double>::iterator dit = data_[p].begin();
           dit != data_[p].end(); dit++)
        hist_[p].upcount(*dit, 1.0);
    }
    return true;
   case  VIL_PIXEL_FORMAT_UINT_16:
   {
    max_val = 65535.0;

    // determine the min and max range of image values
    vcl_vector<double> minr(np, max_val), maxr(np,min_val);
    for (unsigned p = 0; p<np; ++p) {
      for (vcl_vector<double>::iterator dit = data_[p].begin();
           dit != data_[p].end(); dit++)
      {
        if ((*dit)<minr[p]) minr[p] = *dit;
        if ((*dit)>maxr[p]) maxr[p] = *dit;
      }
      // determine if the number of bins exceeds the limit
      unsigned short smin = static_cast<unsigned short>(minr[p]);
      unsigned short smax = static_cast<unsigned short>(maxr[p]);
      unsigned short nbins = static_cast<unsigned short>(smax-smin);
      if (nbins>bin_limit_) {
        nbins = static_cast<unsigned short>(bin_limit_);
        // increase max value to make bin delta an integer
        double range = smax-smin;
        unsigned short del = static_cast<unsigned short>(vcl_ceil(range/nbins));
        smax = static_cast<unsigned short>(smin + nbins*del);
      }
      hist_[p] = bsta_histogram<double>(static_cast<double>(smin),
                                        static_cast<double>(smax), nbins);
      for (vcl_vector<double>::iterator dit = data_[p].begin();
           dit != data_[p].end(); ++dit)
        hist_[p].upcount(*dit, 1.0);
    }
    return true;
   }
   default:
    vcl_cout << "Format not supported by bgui_image_utils\n";
    return false;
  }
}

// Determine the pixel format of the view and cast to appropriate type
// Upcount the histogram with values from the view.
bool bgui_image_utils::set_data_from_view(vil_image_view_base_sptr const& view,
                                          double fraction)
{
  if (!view)
  {
    vcl_cout << "set histogram failed in bgui_image_utils\n";
    return false;
  }
  vil_pixel_format type = view->pixel_format();
  unsigned ni = view->ni(), nj = view->nj();
  float area_frac = static_cast<float>(ni*nj*fraction);
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
        unsigned i = static_cast<unsigned>((ni-1)*(vcl_rand()/(RAND_MAX+1.0)));
        unsigned j = static_cast<unsigned>((nj-1)*(vcl_rand()/(RAND_MAX+1.0)));
        double val = static_cast<double>(v(i,j,p));
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
        unsigned i = static_cast<unsigned>((ni-1)*(vcl_rand()/(RAND_MAX+1.0)));
        unsigned j = static_cast<unsigned>((nj-1)*(vcl_rand()/(RAND_MAX+1.0)));
        double val = static_cast<double>(v(i,j,p));
        data_[p].push_back(val);
      }
    }
    return true;
   }
   default:
     vcl_cout << "Format not supported for histogram construction by bgui_image_utils\n";
  }
  return false;
}

bool bgui_image_utils::
set_data_by_random_blocks(const unsigned total_num_blocks,
                          vil_blocked_image_resource_sptr const& bir,
                          double fraction)
{
  unsigned nbi = bir->n_block_i(), nbj = bir->n_block_j();
  for (unsigned ib = 0; ib<total_num_blocks; ++ib)
  {
    unsigned bi = static_cast<unsigned>((nbi-1)*(vcl_rand()/(RAND_MAX+1.0)));
    unsigned bj = static_cast<unsigned>((nbj-1)*(vcl_rand()/(RAND_MAX+1.0)));
    if (!this->set_data_from_view(bir->get_block(bi, bj), fraction))
      return false;
  }
  return true;
}

bool bgui_image_utils::
set_data_inside_polygon(vil_blocked_image_resource_sptr const& bir)
{
  vil_blocked_image_resource_sptr cbir =
    vil_new_cached_image_resource(bir);
  if (!cbir) {
    vcl_cout << "Null cached image resource to construct histogram\n";
    return false;
  }
  // convert to vgl_polygon
  vgl_polygon<double> vpoly; vpoly.new_sheet();
  unsigned nverts = poly_->size();
  for (unsigned i = 0; i<nverts; ++i) {
    vsol_point_2d_sptr v = poly_->vertex(i);
    vpoly.push_back(v->x(), v->y());
  }
  vgl_polygon_scan_iterator<double> psi(vpoly);
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

bool bgui_image_utils::construct_histogram()
{
  vul_timer t;
  if (!this->init_histogram_from_data())
  {
    vcl_cout << "Unable to construct histogram in bgui_image_utils\n";
    return false;
  }
  unsigned  np = image_->nplanes();
  // zero out the bins of the histogram according to the skip parameters
  for (unsigned p = 0; p<np; ++p)
    for (unsigned s = 0; s<n_skip_lower_bins_; ++s)
      hist_[p].set_count(s, 0.0);

  unsigned last_bin = hist_[0].nbins()-1;
  for (unsigned p = 0; p<np; ++p)
    for (unsigned s =last_bin; s>last_bin-n_skip_upper_bins_; s--)
      hist_[p].set_count(s, 0.0);

  vcl_cout << "computed histogram on " << np*data_[0].size()
           << " pixels in " << t.real() << " milliseconds\n";
  hist_valid_ = true;
  return true;
}

double bgui_image_utils::compute_lower_bound( unsigned plane )
{
  if (plane >= hist_.size())
    return 0;
  return hist_[plane].value_with_area_below(percent_limit_);
}

double bgui_image_utils::compute_upper_bound( unsigned plane )
{
  if (plane >= hist_.size())
    return 0;
  return hist_[plane].value_with_area_above(percent_limit_);
}

// generate a graph tableau from the histogram
bgui_graph_tableau_sptr bgui_image_utils::hist_graph()
{
  unsigned n_planes = image_->nplanes();

  if (!image_ || n_planes>4)
    return 0;

  if (!hist_valid_)
    if (!this->construct_histogram())
      return 0;

  bgui_graph_tableau_sptr g = bgui_graph_tableau_new(512, 512);

  if (n_planes ==1)
  {
    unsigned lowbin =hist_[0].low_bin(), highbin = hist_[0].high_bin();
    if (lowbin>highbin) return 0; // shouldn't happen
    vcl_vector<double> pos = hist_[0].value_array();
    vcl_vector<double> counts = hist_[0].count_array();
    vcl_vector<double> trim_pos, trim_counts;
    // make sure the lowest sample starts at a multiple of 10
    double p0 = pos[lowbin];
    double pten = 10.0*static_cast<int>(p0/10);
    trim_pos.push_back(pten);
    trim_counts.push_back(0.0);
    for (unsigned b = lowbin; b<=highbin; ++b)
    {
      trim_pos.push_back(static_cast<unsigned>(pos[b]));
      trim_counts.push_back(counts[b]);
    }
    g->update(trim_pos, trim_counts);
    return g;
  }

  // In order to create a multiplot for a color histogram
  // all the plots have to have the same integral bin locations
  // get the max min range of bin values
  double minpos = vnl_numeric_traits<double>::maxval, maxpos = 0;
  double min_delta = vnl_numeric_traits<double>::maxval;
  for (unsigned p = 0; p<n_planes; ++p)
  {
    unsigned lbin = hist_[p].low_bin(), hbin = hist_[p].high_bin();
    vcl_vector<double> pos = hist_[p].value_array();
    if (pos[lbin]<minpos) minpos = pos[lbin];
    if (pos[hbin]>maxpos) maxpos = pos[hbin];
    double delta = hist_[p].delta();
    if (delta<min_delta) min_delta = delta;
  }

  // start at a multiple of 10
  double min_ten = 10.0*static_cast<int>(minpos/10);
  double max_ten = 10.0*(static_cast<int>(maxpos/10)+1);
  double range = max_ten-min_ten;

  // Insure at least a bin width of 1.0
  double delta_i = static_cast<int>(min_delta);
  if (delta_i==0) delta_i += 1.0;
  unsigned npos = static_cast<unsigned>(range/delta_i);
  // set  up the integral positions
  vcl_vector<double> dpos(npos);
  for (unsigned i = 0; i<npos; ++i)
    dpos[i] = i*delta_i;

  vcl_vector<vcl_vector<double> > mpos(n_planes, dpos);

  vcl_vector<vcl_vector<double> > mcounts(n_planes);
  for (unsigned p = 0; p<n_planes; ++p)
  {
    double area = hist_[p].area();
    for (unsigned i = 0; i<npos; ++i) {
      double v = area*hist_[p].p(dpos[i]);
      mcounts[p].push_back(v);
    }
  }
  g->update(mpos, mcounts);

  return g;
}

bool bgui_image_utils::default_range_map(vgui_range_map_params_sptr& rmp,
                                         double gamma, bool invert,
                                         bool gl_map, bool cache)
{
  if (!image_) return false;
  // Allow only grey scale for now
  unsigned nc = image_->nplanes();
  if (!(nc == 1 || nc == 3 || nc == 4))
    return false; // all available formats
  // default values
  static double minv = 0.0, maxv = 1500.0; // typical for uint_16
  if (image_->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
    maxv = 255.0;
  if (image_->pixel_format()==VIL_PIXEL_FORMAT_FLOAT)
    maxv = 1.0;
  if (image_->pixel_format()==VIL_PIXEL_FORMAT_DOUBLE)
    maxv = 1.0;
  switch (nc)
  {
    case 1:
      rmp=new vgui_range_map_params(minv, maxv, float(gamma), invert, gl_map, cache);
      return true;
    case 3:
      rmp = new vgui_range_map_params(minv, maxv, minv, maxv, minv, maxv,
                                      float(gamma), float(gamma), float(gamma), invert,
                                      gl_map, cache);
      return true;
    case 4:
    {
      int band_map = 1; // map RGB-InfraRed -> RGB
      rmp = new vgui_range_map_params(minv, maxv, minv, maxv, minv, maxv,
                                      minv, maxv, float(gamma), float(gamma), float(gamma), float(gamma),
                                      band_map, invert, gl_map, cache);
      return true;
    }
    default:
      return false;
  }
  return true; // never reached
}

bool bgui_image_utils::range_map_from_hist(float gamma, bool invert,
                                           bool gl_map, bool cache,
                                           vgui_range_map_params_sptr& rmp)
{
  rmp = 0;
  if (!image_)
    return false;

  unsigned np = image_->nplanes();
  vcl_vector<double> minr(np, 0.0), maxr(np, 0.0);

  for (unsigned p = 0; p<np; ++p)
    if (!this->range(minr[p], maxr[p], p))
      return false;

  if (np == 1)
  {
    rmp= new vgui_range_map_params(minr[0], maxr[0], gamma, invert,
                                   gl_map, cache);
    return true;
  }
  else if (np == 3)
  {
    rmp = new vgui_range_map_params(minr[0], maxr[0], minr[1], maxr[1],
                                    minr[2], maxr[2],
                                    gamma, gamma, gamma, invert,
                                    gl_map, cache);
    return true;
  }
  else if (np == 4)
  {
    int bm = vgui_range_map_params::RGB_m;
    rmp = new vgui_range_map_params(minr[0], maxr[0], minr[1], maxr[1],
                                    minr[2], maxr[2], minr[3], maxr[3],
                                    gamma, gamma, gamma, gamma, bm, invert,
                                    gl_map, cache);
    return true;
  }
  else
    return false;
}
