// This is brl/cvg/cvg_hemisphere_tableau.h
#ifndef cvg_hemisphere_tableau_h
#define cvg_hemisphere_tableau_h
//:
// \file
// \brief A tableau to view octree models in real time
// \author Vishal Jain
// \date Aug 11, 2010

#define PYRAMID_MAX_LEVEL 2

// ===vgui includes===
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>

// ===utilities===
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_random.h>

// ===vbl===
#include <vbl/vbl_array_2d.h>

// ===vsph===
#include <vpgl/vsph/vsph_view_sphere.h>
#include <vpgl/vsph/vsph_view_point.h>
#include <vpgl/vsph/vsph_sph_point_3d.h>

class cvg_hemisphere_tableau : public vgui_image_tableau
{
 public:
  cvg_hemisphere_tableau() {}
  cvg_hemisphere_tableau(vil_image_view_base const& img);
  cvg_hemisphere_tableau(vil_image_resource_sptr const& img);
  cvg_hemisphere_tableau(vil_image_resource_sptr const& img,
                         vsph_view_sphere<vsph_view_point<vcl_string> > sphere);

  virtual ~cvg_hemisphere_tableau() {}

  //: i'd imagine this will be initialized with a 2d array of images
  bool init();

  //: virtual function handles mouse and keyboard actions
  virtual bool handle( vgui_event const& e );

 protected:

  //: image filename sphere
  vsph_view_sphere<vsph_view_point<vcl_string> > img_sphere_;

  //: current location
  vsph_sph_point_3d curr_point_;

  //: current image pyramid
  vil_pyramid_image_view<vxl_byte>* curr_pyramid_;
  int curr_level_;

  //: load, set and post redraw helper method
  void set_expected_pyramid();
  void set_expected_image();
  double compress_range(double rad);
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<cvg_hemisphere_tableau> cvg_hemisphere_tableau_sptr;

//: Create a smart-pointer to a cvg_hemisphere_tableau tableau.
struct cvg_hemisphere_tableau_new : public cvg_hemisphere_tableau_sptr
{
  //: Constructor - create an empty cvg_hemisphere_tableau
  typedef cvg_hemisphere_tableau_sptr base;
  cvg_hemisphere_tableau_new() : base( new cvg_hemisphere_tableau ) { }

  //: Constructor - create with view_base
  cvg_hemisphere_tableau_new( vil_image_view_base const& t )
    : base( new cvg_hemisphere_tableau(t) ) { }

  //: Constructor - create with resource
  cvg_hemisphere_tableau_new( vil_image_resource_sptr const& t )
    : base( new cvg_hemisphere_tableau(t) ) { }

  //: constructor with view sphere
  cvg_hemisphere_tableau_new( vil_image_resource_sptr const& t,
                              vsph_view_sphere<vsph_view_point<vcl_string> > sphere)
    : base( new cvg_hemisphere_tableau(t, sphere) ) { }
};


#endif // cvg_hemisphere_tableau_h
