// This is brl/bbas/bgui/bgui_range_adjuster_tableau.cxx
#include <bgui/bgui_range_adjuster_tableau.h>
//:
// \file
// \author  J. L. Mundy after Matt Leotta original

#include <vcl_cassert.h>
#include <vcl_cmath.h> //for fabs()
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_histogram.h>
#include <vil/vil_pixel_traits.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_projection_inspector.h>

void bgui_range_adjuster_tableau::draw_box()
{
  // Draw a square around the histogram
  vcl_vector<float> x_corners, y_corners;
  x_corners.push_back(left_offset_);
  x_corners.push_back(left_offset_+graph_width_);
  x_corners.push_back(left_offset_+graph_width_);
  x_corners.push_back(left_offset_);
  y_corners.push_back(top_offset_);
  y_corners.push_back(top_offset_);
  y_corners.push_back(top_offset_+graph_height_);
  y_corners.push_back(top_offset_+graph_height_);
  vgui_soview2D_polygon* sov = 
    this->add_polygon(4, &x_corners[0], &y_corners[0]);
  sov->set_selectable(false);
}

void bgui_range_adjuster_tableau::init()
{

  hardware_ = false;
  this->set_foreground(0.8f, 1.0f, 0.0);
  this->set_line_width(5.0f);
  this->draw_box();
}
//========================================================================
//: Constructors
bgui_range_adjuster_tableau::bgui_range_adjuster_tableau(const char* n) :
  vgui_easy2D_tableau(n),left_offset_(10), top_offset_(10),
  graph_width_(256), graph_height_(200), plot_(0), hist_(0,0),
  min_bar_(0),max_bar_(0)
{   this->init(); }
bgui_range_adjuster_tableau::bgui_range_adjuster_tableau(vgui_tableau_sptr const& t,
                                               const char* n) :
  vgui_easy2D_tableau(t, n),left_offset_(10), top_offset_(10),
  graph_width_(256), graph_height_(200), plot_(0), hist_(0,0),
  min_bar_(0),max_bar_(0)
{   this->init(); }
bgui_range_adjuster_tableau::bgui_range_adjuster_tableau(vgui_image_tableau_sptr const& t,
                                               const char* n) :
  vgui_easy2D_tableau(t, n),left_offset_(10), top_offset_(10),
  graph_width_(256), graph_height_(200), plot_(0), hist_(0,0),
  min_bar_(0),max_bar_(0)
{   this->init(); }

//========================================================================
//: Destructor.
bgui_range_adjuster_tableau::~bgui_range_adjuster_tableau()
{
}

//:map the data value of histogrammed intensity to display coordinates
int bgui_range_adjuster_tableau::map_val_to_display(const double val)
{
  //compute display scale
  double scale = 1.0;
  if(vcl_fabs(max_-min_)>0)
    scale = graph_width_/(max_-min_);
  int display_x = (int)((val-min_)*scale + left_offset_);
  return display_x;
}

//:map the data value of histogrammed intensity to display coordinates
double bgui_range_adjuster_tableau::map_display_to_val(const int display_x)
{
  //compute display scale
  double scale = 1.0;
  if(vcl_fabs(max_-min_)>0)
    scale = graph_width_/(max_-min_);
  double temp = display_x - left_offset_;
  temp/=scale;
  return temp + min_;
}

//:draw stretch bars
void bgui_range_adjuster_tableau::draw_stretch_bars()
{
  double temp = hist_.value_with_area_below(0.1);
  int min_pos = this->map_val_to_display(temp);
  temp = hist_.value_with_area_above(0.1);
  int max_pos = this->map_val_to_display(temp);
  this->set_foreground(0.0, 1.0, 0);
  min_bar_ =  
    this->add_line(min_pos, top_offset_, min_pos, graph_height_+top_offset_);
  max_bar_ = 
    this->add_line(max_pos, top_offset_, max_pos, graph_height_+top_offset_);
  this->set_foreground(1.0, 1.0, 0);
  min_bar_->set_selectable(false);
  max_bar_->set_selectable(false);
}
//: Update the histogram from image resource
//  currently handle a few useful cases
bool bgui_range_adjuster_tableau::update(vil_image_resource_sptr const& r)
{
  data_.clear();
  unsigned np = r->nplanes();
  vil_pixel_format f = r->pixel_format();
  vil_pixel_format type = vil_pixel_format_component_format(f);
  switch (type )
    {
    case VIL_PIXEL_FORMAT_BYTE:
      {
        vil_image_view<vxl_byte> v = r->get_view();
        assert(v);
        min_ = vil_pixel_traits<vxl_byte>::minval();
        max_ = vil_pixel_traits<vxl_byte>::maxval();
        vil_histogram_byte(v , data_);
        break;
      }
    case VIL_PIXEL_FORMAT_SBYTE:
      {
        vil_image_view<vxl_sbyte> v = r->get_view();
        assert(v);
        min_ = vil_pixel_traits<vxl_sbyte>::minval();
        max_ = vil_pixel_traits<vxl_sbyte>::maxval();
        vil_histogram<vxl_sbyte>(v, data_, min_, max_, graph_width_);
		hardware_ = false;
        break;
      }
    case VIL_PIXEL_FORMAT_UINT_16:
      {
        vil_image_view<vxl_uint_16> v = r->get_view();
        assert(v);
        min_ = vil_pixel_traits<vxl_uint_16>::minval();
        max_ = vil_pixel_traits<vxl_uint_16>::maxval();
        vil_histogram<vxl_uint_16>(v, data_, min_, max_, graph_width_);
		hardware_ = np==1;
		break;
      }
    case VIL_PIXEL_FORMAT_INT_16:
      {
        vil_image_view<vxl_int_16> v = r->get_view();
        assert(v);
        min_ = vil_pixel_traits<vxl_int_16>::minval();
        max_ = vil_pixel_traits<vxl_int_16>::maxval();
        vil_histogram<vxl_int_16>(v, data_, min_, max_, graph_width_);
		hardware_ = false;
		break;
      }
    case VIL_PIXEL_FORMAT_FLOAT:
      {
        vil_image_view<float> v = r->get_view();
        assert(v);
        min_ = vil_pixel_traits<float>::minval();
        max_ = vil_pixel_traits<float>::maxval();
        vil_histogram<float>(v, data_, min_, max_, graph_width_);
			hardware_ = false;
		break;
      }
    case VIL_PIXEL_FORMAT_DOUBLE:
      {
        vil_image_view<double> v = r->get_view();
        assert(v);
        min_ = vil_pixel_traits<double>::minval();
        max_ = vil_pixel_traits<double>::maxval();
        vil_histogram<double>(v, data_, min_, max_, graph_width_);
		hardware_ = false;
		break;
      }
    default:
      vcl_cout << "In bgui_range_adjuster_tableau - image type not supported\n";
      return false;
    }
  hist_ = bsta_histogram<double>(min_, max_, data_);
  this->draw_histogram();
  this->draw_stretch_bars();
  return true;
}

//: update from child
bool bgui_range_adjuster_tableau::update()
{
  vgui_image_tableau_sptr itab = this->get_child_image_tableau();  
  if(!itab)
    return false;
  vil_image_resource_sptr image = itab->get_image_resource();
  return this->update(image);
}

bool bgui_range_adjuster_tableau::update(const double min, const double max,
                                    vcl_vector<double> const& hist)
{
  if(!hist.size())
	  return false;
  min_ = min;
  max_ = max;
	data_ = hist;
  hist_ = bsta_histogram<double>(min, max, data_);
  this->draw_histogram();
  this->draw_stretch_bars();
  return true;
}

bool bgui_range_adjuster_tableau::update(vil1_memory_image_of< vil1_rgb<unsigned char> >& img)
{
  vil_image_resource_sptr image = vil1_to_vil_image_resource(img);
  return this->update();
}

void bgui_range_adjuster_tableau::draw_histogram()
{
  double max = data_[0];
  for (unsigned int i=1; i<data_.size(); ++i)
    if (max < data_[i]) max = data_[i];

  // scale and shift the data points
  vcl_vector<float> xscaled, yscaled;
  for (unsigned int i=0; i<data_.size(); ++i) {
    xscaled.push_back(left_offset_ + i);
    yscaled.push_back(top_offset_ + graph_height_ - data_[i]/max*graph_height_);
  }

  if (plot_)
    {
      // Update the plot points
      // This is a bit more efficient that deleting and reconstructing
      //   but not as "clean" 
      for (unsigned int i=0; i<xscaled.size(); ++i) {
        plot_->x[i] = xscaled[i];
        plot_->y[i] = yscaled[i];
      }
    }
  else
    {
      plot_ = this->add_linestrip(xscaled.size(), &xscaled[0], &yscaled[0]);
      plot_->set_selectable(false);
    }
  this->post_redraw();
}

vgui_image_tableau_sptr bgui_range_adjuster_tableau::get_child_image_tableau()
{
  vgui_tableau_sptr ch = this->get_child(0);
  if (ch)
    {
      vgui_image_tableau_sptr itab;
      itab.vertical_cast(vgui_find_below_by_type_name(ch,
                                                      vcl_string("vgui_image_tableau")));
      return itab;
    }
  else
    return 0;
}

float bgui_range_adjuster_tableau::screen_to_bar(const float sx)
{
  return  sx + left_offset_;
}

float bgui_range_adjuster_tableau::bar_to_screen(const float bx)
{
  return bx - left_offset_;
}


void bgui_range_adjuster_tableau::adjust_min_bar(const float x)
{
  if(!(min_bar_&&max_bar_))
    return;
  float xb = screen_to_bar(x);
  float xmax = max_bar_->x0;
  if(xb<0)
    {
      min_bar_->x0=0;
      min_bar_->x1=0;
      return;
    }
  if(x>xmax)
    {
      min_bar_->x0=xmax;
      min_bar_->x1=xmax;
      return;
    }
  min_bar_->x0 = xb;
  min_bar_->x1 = xb;
}
void bgui_range_adjuster_tableau::adjust_max_bar(const float x)
{
  if(!(min_bar_&&max_bar_))
    return;
  float xb = screen_to_bar(x);
  float xl = screen_to_bar(graph_width_);
  float xmin = min_bar_->x0;
  if(xb>xl)
    {
      max_bar_->x0=xl;
      max_bar_->x1=xl;
      return;
    }
  if(x<xmin)
    {
      max_bar_->x0=xmin;
      max_bar_->x1=xmin;
      return;
    }
  max_bar_->x0 = xb;
  max_bar_->x1 = xb;
}
//========================================================================
//: Handles all events for this tableau.
bool bgui_range_adjuster_tableau::handle(const vgui_event& event)
{
  if (event.type == vgui_BUTTON_DOWN)
    {
      //Adjust the stretch bars
      float pointx, pointy;
      vgui_projection_inspector p_insp;
      p_insp.window_to_image_coordinates(event.wx, event.wy, pointx, pointy);
      float min_pos = 0, max_pos = 0, y = 0;
      if(event.modifier == vgui_SHIFT)
        this->adjust_max_bar(pointx);
      else
        this->adjust_min_bar(pointx);

      //Use the bar positions to set the range map max min values.
      if(min_bar_)
        min_bar_->get_centroid(&min_pos, &y);
      if(max_bar_)
        max_bar_->get_centroid(&max_pos, &y);

      double min_val = map_display_to_val((int)min_pos);
      double max_val = map_display_to_val((int)max_pos);
      if(min_val>max_val)
        return vgui_easy2D_tableau::handle(event);

      vgui_image_tableau_sptr child = this->get_child_image_tableau();
      if (!child)
        return vgui_easy2D_tableau::handle(event);

      vil_image_resource_sptr r = child->get_image_resource();
      if(!r)
        return vgui_easy2D_tableau::handle(event);
      //If the image can be hardware mapped, it uses glPixelMap* operations
      vgui_range_map_params_sptr rmp;  
      if(r->nplanes()==1)
        rmp = new vgui_range_map_params(min_val,max_val, 1.0f,
                                  false, hardware_, hardware_);
      else if(r->nplanes()==3)
        rmp = new vgui_range_map_params(min_val,max_val, min_val, max_val,
                                        min_val, max_val,1.0f, 1.0f, 1.0f,
                                        false, hardware_, hardware_);
      else
        return vgui_easy2D_tableau::handle(event);

        child->set_mapping(rmp);
    }
  return vgui_easy2D_tableau::handle(event);
}


void bgui_range_adjuster_tableau::clear()
{
  this->remove(plot_);
  delete plot_;
  plot_ = 0;
  this->post_redraw();
}
