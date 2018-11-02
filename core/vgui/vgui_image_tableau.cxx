// This is core/vgui/vgui_image_tableau.cxx
#include <string>
#include "vgui_image_tableau.h"
//:
// \file
// \brief  See vgui_image_tableau.h for a description of this file.
// \author fsm
//
// \verbatim
//  Modifications
//   15-AUG-2000 Marko Bacic,Oxford RRG -- Removed legacy ROI
//   16-FEB-2007 Andrey Khropov -- consistent choice between vil1_image/vil_image_view
// \endverbatim


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view_base.h>
#include <vil1/vil1_load.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>

#include <vgui/vgui_image_renderer.h>
#include <vgui/vgui_vil_image_renderer.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_range_map_params.h>

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau()
  : vgui_tableau(),
    pixels_centered_( true ),
    rmp_( nullptr ),
    renderer_( nullptr ),
    vil_renderer_( nullptr )
{
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau( vil1_image const &I,
                    vgui_range_map_params_sptr const& mp)
  : vgui_tableau(),
    pixels_centered_( true ),
    renderer_( nullptr ),
    vil_renderer_( nullptr )
{
  set_image( I, mp );
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau( vil_image_view_base const& I,
                    vgui_range_map_params_sptr const& mp )
  : vgui_tableau(),
    pixels_centered_( true ),
    renderer_( nullptr ),
    vil_renderer_( nullptr )
{
  set_image_view( I, mp );
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau( vil_image_resource_sptr const& I,
                    vgui_range_map_params_sptr const& mp )
  : vgui_tableau(),
    pixels_centered_( true ),
    renderer_( nullptr ),
    vil_renderer_( nullptr )
{
  set_image_resource( I, mp);
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau(char const *f,
                   vgui_range_map_params_sptr const& mp)
  : vgui_tableau(),
    name_( f ),
    pixels_centered_( true ),
    renderer_( nullptr ),
    vil_renderer_( nullptr )
{
  set_image( f, mp );
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
~vgui_image_tableau()
{
  delete renderer_;
  delete vil_renderer_;
  renderer_ = nullptr;
  vil_renderer_ = nullptr;
}

//-----------------------------------------------------------------------------

std::string
vgui_image_tableau::
type_name() const
{
  return "vgui_image_tableau";
}


//-----------------------------------------------------------------------------

std::string
vgui_image_tableau::
file_name() const
{
  return name_;
}

//-----------------------------------------------------------------------------

std::string
vgui_image_tableau::
pretty_name() const
{
  return type_name() + "[" + name_ + "]";
}

//-----------------------------------------------------------------------------

vil1_image
vgui_image_tableau::
get_image() const
{
  if (renderer_)
    return renderer_->get_image();
  else
    return nullptr;
}

//-----------------------------------------------------------------------------

vil_image_view_base_sptr
vgui_image_tableau::
get_image_view() const
{
  if (vil_renderer_)
    return vil_renderer_->get_image_resource()->get_view();
  else
    return nullptr;
}

//-----------------------------------------------------------------------------

vil_image_resource_sptr
vgui_image_tableau::
get_image_resource() const
{
  if (vil_renderer_)
    return vil_renderer_->get_image_resource();
  else
    return nullptr;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_view( char const* f, vgui_range_map_params_sptr const& mp)
{
  name_ = f;
  vil_image_view_base_sptr img = vil_load( f );
  if ( img )
    set_image_view( *img, mp );
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_view( vil_image_view_base const& I,
                vgui_range_map_params_sptr const& mp)
{
  set_image_view(I);
  rmp_ = mp;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_resource( vil_image_resource_sptr const& I,
                    vgui_range_map_params_sptr const& mp )
{
  set_image_resource(I);
  rmp_ = mp;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image( vil1_image const& I,
           vgui_range_map_params_sptr const& mp )
{
  set_image(I);
  rmp_ = mp;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_view( vil_image_view_base const& I)
{
  set_image_resource( vil_new_image_resource_of_view( I ) );
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_resource( vil_image_resource_sptr const& I)
{
  if ( !vil_renderer_ )
    vil_renderer_ = new vgui_vil_image_renderer;

  // use the name of the image as the name of the tableau :
  vil_renderer_->set_image_resource( I );

  if ( renderer_ )
  {
    delete renderer_;
    renderer_ = nullptr;
  }
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image( vil1_image const& I)
{
  if ( !renderer_ )
    renderer_ = new vgui_image_renderer;

  // use the name of the image as the name of the tableau :
  renderer_->set_image( I );

  if ( vil_renderer_ )
  {
    delete vil_renderer_;
    vil_renderer_ = nullptr;
  }
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image(char const *f,
          vgui_range_map_params_sptr const& mp)
{
  name_ = f;
  vil1_image img = vil1_load( f );
  if ( img )
    set_image( img, mp );
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
reread_image()
{
  if ( renderer_ )           renderer_->reread_image();
  else if ( vil_renderer_ )  vil_renderer_->reread_image();
}

//-----------------------------------------------------------------------------

unsigned
vgui_image_tableau::
width() const
{
  if ( renderer_ )
    return renderer_->get_image().width();
  else if ( vil_renderer_ && vil_renderer_->get_image_resource())
    return vil_renderer_->get_image_resource()->ni();
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned
vgui_image_tableau::
height() const
{
  if ( renderer_ )
    return renderer_->get_image().height();
  else if ( vil_renderer_ && vil_renderer_->get_image_resource())
    return vil_renderer_->get_image_resource()->nj();
  else
    return 0;
}

//-----------------------------------------------------------------------------

bool
vgui_image_tableau::
get_bounding_box(float low[3], float high[3]) const
{
  low[0] = 0; high[0] = width();
  low[1] = 0; high[1] = height();

  low[2] = 0; high[2] = 0;
  return true;
}

//-----------------------------------------------------------------------------

bool
vgui_image_tableau::
handle(vgui_event const &e)
{
  if (e.type == vgui_DRAW)
  {
    // If blending is turned on, there is a severe performance penalty
    // when rendering an image. So, we turn off blending before calling
    // the renderer. In cases where two images are to be blended, a
    // special tableau should be written, eg. vgui_blender_tableau.
    // fsm
    GLboolean blend_on;
    glGetBooleanv(GL_BLEND, &blend_on);
    if (blend_on)
      glDisable(GL_BLEND);

    if (pixels_centered_)
      glTranslated(-0.5, -0.5, 0);
    //If rmp_ is not null then the rendering will be mapped according to the
    //specified mapping parameters
    if ( renderer_ )          renderer_->render(rmp_);
    else if ( vil_renderer_ ) vil_renderer_->render(rmp_);

    if (pixels_centered_)
      glTranslated(+0.5, +0.5, 0);

    if (blend_on)
      glEnable(GL_BLEND);

    return true;
  }
  else
    return false;
}

void vgui_image_tableau::set_mapping(vgui_range_map_params_sptr const& rmp)
{
  rmp_ = rmp;
  this->post_redraw();
}


//: A vgui command to set the rangemap parameters
class vgui_set_rangemap_command : public vgui_command
{
 public:
  vgui_set_rangemap_command(const vgui_image_tableau_sptr& tab,
                            unsigned int nc=1)
   : tab_(tab), nc_(nc) {}

  void execute()
  {
    vgui_range_map_params_sptr old_rmp = tab_->map_params();
    vgui_range_map_params_sptr rmp = nullptr;
    if (!old_rmp || old_rmp->n_components_ != nc_) {
      rmp = new vgui_range_map_params(0.0, 1.0);
      rmp->n_components_ = nc_;
    }
    else
      rmp = new vgui_range_map_params(*old_rmp);

    // use this array because vgui_dialog does not support long double fields
    double ranges[8];
    int choice = rmp->band_map_;
    vgui_dialog rmp_dialog("Set Range Mapping Parameters");
    if (nc_ == 1) {
      ranges[0] = double(rmp->min_L_);
      ranges[1] = double(rmp->max_L_);
      rmp_dialog.field("Luminance Min ",ranges[0]);
      rmp_dialog.field("Luminance Max ",ranges[1]);
      rmp_dialog.field("Luminance Gamma ",rmp->gamma_L_);
    }
    if (nc_ == 3 || nc_ == 4) {
      ranges[0] = double(rmp->min_R_);
      ranges[1] = double(rmp->max_R_);
      rmp_dialog.field("R Min ",ranges[0]);
      rmp_dialog.field("R Max ",ranges[1]);
      rmp_dialog.field("R Gamma ",rmp->gamma_R_);
      ranges[2] = double(rmp->min_G_);
      ranges[3] = double(rmp->max_G_);
      rmp_dialog.field("G Min ",ranges[2]);
      rmp_dialog.field("G Max ",ranges[3]);
      rmp_dialog.field("G Gamma ",rmp->gamma_G_);
      ranges[4] = double(rmp->min_B_);
      ranges[5] = double(rmp->max_B_);
      rmp_dialog.field("B Min ",ranges[4]);
      rmp_dialog.field("B Max ",ranges[5]);
      rmp_dialog.field("B Gamma ",rmp->gamma_B_);
    }
    if (nc_==4) {
      ranges[6] = double(rmp->min_X_);
      ranges[7] = double(rmp->max_X_);
      rmp_dialog.field("X Min ",ranges[6]);
      rmp_dialog.field("X Max ",ranges[7]);
      rmp_dialog.field("X Gamma ",rmp->gamma_X_);
      std::vector<std::string> choices;
      for (unsigned c = 0; c<vgui_range_map_params::END_m; ++c)
        choices.push_back(vgui_range_map_params::bmap[c]);
      rmp_dialog.choice("Band Map", choices, choice);
    }

    rmp_dialog.checkbox("Invert ",rmp->invert_);

    rmp_dialog.checkbox("Use glPixelMap ",rmp->use_glPixelMap_);
    rmp_dialog.checkbox("Cache Map ",rmp->cache_mapped_pix_);

    if (!rmp_dialog.ask())
      return;

    rmp->band_map_ = choice;
    rmp->min_L_ = ranges[0];
    rmp->max_L_ = ranges[1];
    rmp->min_R_ = ranges[0];
    rmp->max_R_ = ranges[1];
    rmp->min_G_ = ranges[2];
    rmp->max_G_ = ranges[3];
    rmp->min_B_ = ranges[4];
    rmp->max_B_ = ranges[5];
    rmp->min_X_ = ranges[6];
    rmp->max_X_ = ranges[7];

    //Setting the map with a new instance forces redraw
    tab_->set_mapping(rmp);
  }

  vgui_image_tableau_sptr tab_;
  unsigned int nc_;
};

//----------------------------------------------------------------------------
//: Builds a popup menu for the user to set range mapping parameters
void vgui_image_tableau::get_popup(const vgui_popup_params& /*params*/,
                                   vgui_menu &menu)
{
  vgui_menu submenu;

  unsigned int nc = 0;
  if ( renderer_ )
    nc = renderer_->get_image().components();
  else if ( vil_renderer_ )
    nc = vil_renderer_->get_image_resource()->nplanes();

  submenu.add("Range Mapping",new vgui_set_rangemap_command(this,nc));

  menu.add(type_name(), submenu);
}
