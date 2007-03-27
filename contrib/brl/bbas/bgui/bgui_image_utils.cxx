#include <vcl_cstdlib.h> // for rand()
#include <vil/vil_new.h>
#include <vil/vil_property.h>
#include <vil/vil_pixel_traits.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <bgui/bgui_image_utils.h>

void bgui_image_utils::range(double& min_value, double& max_value)
{
  if(!hist_valid_)
    this->construct_histogram();
  min_value = this->compute_lower_bound();
  max_value = this->compute_upper_bound();
}
// fill the histogram by randomly sampling pixels from the image
bool bgui_image_utils::init_histogram()
{
  hist_valid_ = false;
  if(!image_)
    return false;
  unsigned bin_limit = 1000;//max number of bins.
  vil_pixel_format type = image_->pixel_format();
  unsigned n_p = image_->nplanes();
  if(n_p!=1)
    {
      vcl_cout << "Format not supported by bgui_image_utils\n";
      return false;
    }
  double min_val=0, max_val = 0;
  switch (type )
    {
    case  VIL_PIXEL_FORMAT_BYTE: 
      {
      min_val = 
        static_cast<double>(vil_pixel_traits<unsigned char>::minval()); 
      max_val = 
        static_cast<double>(vil_pixel_traits<unsigned char>::maxval()); 
      hist_ = bsta_histogram<double>(min_val, max_val, 255);
      return true;
      }
    case  VIL_PIXEL_FORMAT_UINT_16: 
      {
      min_val = 
        static_cast<double>(vil_pixel_traits<unsigned short>::minval()); 
      max_val = 
        static_cast<double>(vil_pixel_traits<unsigned short>::maxval()); 
      hist_ = bsta_histogram<double>(min_val, max_val, bin_limit);
      return true;
      }
   case  VIL_PIXEL_FORMAT_UINT_32: 
     {
      min_val = 
        static_cast<double>(vil_pixel_traits<unsigned int>::minval()); 
      max_val = 
        static_cast<double>(vil_pixel_traits<unsigned int>::maxval()); 
      hist_ = bsta_histogram<double>(min_val, max_val, bin_limit);
      return true;
     }
    default:
      vcl_cout << "Format not supported by bgui_image_utils\n";
    }
  return false;
}
//Determine the pixel format of the view and cast to appropriate type
//Upcount the histogram with values from the view.
void bgui_image_utils::set_hist_from_view(vil_image_view_base_sptr const& view)
{
  if(!image_||!view)
    {
      vcl_cout << "set histogram failed in bgui_image_utils\n";
      return;
    }
  vil_pixel_format type = image_->pixel_format();
  switch (type )
    {
    case  VIL_PIXEL_FORMAT_BYTE: 
      {
        vil_image_view<unsigned char> v = view;
        for(unsigned j = 0; j<v.nj(); ++j)
          for(unsigned i = 0; i<v.ni(); ++i)
            {
              double val = static_cast<double>(v(i,j));
              hist_.upcount(val, 1.0);
            }
        return;
      }
    case  VIL_PIXEL_FORMAT_UINT_16: 
      {
        vil_image_view<unsigned short> v = view;
        for(unsigned j = 0; j<v.nj(); ++j)
          for(unsigned i = 0; i<v.ni(); ++i)
            {
              double val = static_cast<double>(v(i,j));
              hist_.upcount(val, 1.0);
            }
        return;
      }
   case  VIL_PIXEL_FORMAT_UINT_32: 
     {
       vil_image_view<unsigned int> v = view;
        for(unsigned j = 0; j<v.nj(); ++j)
          for(unsigned i = 0; i<v.ni(); ++i)
            {
              double val = static_cast<double>(v(i,j));
              hist_.upcount(val, 1.0);
            }
        return;
     }
    default:
      vcl_cout << "Format not supported by bgui_image_utils\n";
      hist_ = bsta_histogram<double>(0.0, 0.0, 1);//invalid hist
    }
}
void bgui_image_utils::
set_hist_by_random_blocks(const unsigned total_num_blocks,
                          vil_blocked_image_resource_sptr const& bir)
{
  unsigned nbi = bir->n_block_i(), nbj = bir->n_block_j();
  for(unsigned ib = 0; ib<total_num_blocks; ++ib)
    {
      unsigned bi = static_cast<unsigned>((nbi-1)*(vcl_rand()/(RAND_MAX+1.0)));
      unsigned bj = static_cast<unsigned>((nbj-1)*(vcl_rand()/(RAND_MAX+1.0)));
      this->set_hist_from_view(bir->get_block(bi, bj));
    }
}
void bgui_image_utils::construct_histogram()
{
  unsigned sample_factor = 1000;
  if(!this->init_histogram())
    {
      vcl_cout << "Unable to construct histogram in bgui_image_itils\n";
      hist_ = bsta_histogram<double>(0.0, 0.0, 1);//invalid hist
      return;
    }

  // The number of pixels we are willing to wait for.
  unsigned max_pixels = sample_factor*(hist_.nbins());
  // first check if the image is a pyramid
  bool pyr = image_->get_property(vil_property_pyramid, 0);
  //if it is a pyramid then get the lowest resolution level
  vil_image_resource_sptr image;
  if(pyr)
    {
      //cast to pyramid resource
      vil_pyramid_image_resource_sptr pir = 
        (vil_pyramid_image_resource*)((vil_image_resource*)image_.ptr());
      //highest resolution resource
      image = pir->get_resource(0);
    }
  else
    image = image_;
  unsigned ni = image->ni(), nj = image->nj();
  unsigned npix = ni*nj;
  //If there are insufficient pixels use all pixels to create the histogram
  if(npix<max_pixels)
    {
      this->set_hist_from_view(image->get_view());
      return;
    }
  // create a blocked image resource from image
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if(!bir)
    bir = vil_new_blocked_image_facade(image);
  unsigned sbi = bir->size_block_i(), sbj = bir->size_block_j();
  // The number of blocks required to fill the histogram
  unsigned total_num_blocks = static_cast<unsigned>((float)max_pixels/(sbi*sbj));

  /// We need at least one
  if(total_num_blocks ==0) total_num_blocks = 1;

  this->set_hist_by_random_blocks(total_num_blocks, bir);
                                  

  //zero out the bins of the histogram according to the skip parameters
  for(unsigned s = 0; s<n_skip_lower_bins_; ++s)
    hist_.set_count(s, 0.0);

  unsigned last_bin = hist_.nbins()-1;
  for(unsigned s =last_bin; s>last_bin-n_skip_upper_bins_; s--)
    hist_.set_count(s, 0.0);
  hist_valid_ = true;
}

double bgui_image_utils::compute_lower_bound()
{
  return hist_.value_with_area_below(percent_limit_);
}

double bgui_image_utils::compute_upper_bound()
{
  return hist_.value_with_area_above(percent_limit_);
}

//generate a graph tableau from the histogram
bgui_graph_tableau_sptr bgui_image_utils::hist_graph()
{
  if(!hist_valid_)
    this->construct_histogram();
  bgui_graph_tableau_sptr g = bgui_graph_tableau_new(512, 512);
  vcl_vector<double> pos = hist_.value_array();
  vcl_vector<double> counts = hist_.count_array();
  vcl_vector<double> trim_pos, trim_counts;
  unsigned lowbin=0, highbin = pos.size()-1;
  //find first non-zero bin from below
  for(; lowbin<pos.size()&&counts[lowbin]==0; ++lowbin);
  //find first non-zero bin from above
  for(; highbin>0&&counts[highbin]==0; --highbin);
  if(lowbin>=highbin)
    return 0;
  for(unsigned b = lowbin; b<=highbin; ++b)
    {
      trim_pos.push_back(static_cast<unsigned>(pos[b]));
      trim_counts.push_back(counts[b]);
    }
  g->update(trim_pos, trim_counts);
  return g;
}
