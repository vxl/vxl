// This is brl/bbas/bgui3d/bgui3d_project2d_tableau.h
#ifndef bgui3d_project2d_tableau_h_
#define bgui3d_project2d_tableau_h_
//:
// \file
// \brief  A tableau that projects a 3D scene into a 2D image
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   August 6, 2004
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bgui3d_project2d_tableau_sptr.h"
#include "bgui3d_tableau.h"
#include <vnl/vnl_double_3x4.h>


//: Tableau that projects a Coin3D scene graph into an image plane
//  The scenegraph should not contain a camera node.
//  The image plane is specified by 3x4 camera matrix
class bgui3d_project2d_tableau : public bgui3d_tableau
{
 public:
  //: Default Constructor - don't use this, use bgui3d_project2d_tableau_new.
  bgui3d_project2d_tableau();
  //: Constructor - don't use this, use bgui3d_project2d_tableau_new.
  bgui3d_project2d_tableau(const vpgl_proj_camera<double>& camera,
                           SoNode* scene_root = NULL);

  //: Destructor
  virtual ~bgui3d_project2d_tableau();

  //: Return the type name of this tableau
  virtual std::string type_name() const;

  //: Handle vgui events
  virtual bool handle(const vgui_event& event);

  //: Set the scene camera
  // creates a graphics camera from a vpgl camera (either perspective or affine)
  virtual bool set_camera(const vpgl_proj_camera<double>& camera);

  //: Get the scene camera
  // creates a vpgl camera (either perspective or affine) from the graphics camera
  virtual std::unique_ptr<vpgl_proj_camera<double> > camera() const;

  //: Activate a headlight
  void set_headlight(bool enable) { draw_headlight_ = enable; this->post_redraw(); }

  //: Is the headlight active
  bool is_headlight() const { return draw_headlight_; }

  //: Builds a popup menu
  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

 protected:
  //: setup the OpenGL projection matrix
  virtual bool setup_projection();

 private:
  //: The canonical camera looking down the -z axis
  vnl_double_3x4 camera_z_;

  //: The GL model matrix to load for the camera position
  double model_matrix_[16];

  //: Flag for drawing a headlight
  bool draw_headlight_;
};


//: Create a smart pointer to a bgui3d_project2d_tableau tableau.
struct bgui3d_project2d_tableau_new : public bgui3d_project2d_tableau_sptr
{
  typedef bgui3d_project2d_tableau_sptr base;

  bgui3d_project2d_tableau_new() : base(new bgui3d_project2d_tableau()) { }

  bgui3d_project2d_tableau_new(const vpgl_proj_camera<double>& camera, SoNode* scene_root = NULL)
  : base(new bgui3d_project2d_tableau(camera, scene_root)) { }
};

#endif // bgui3d_project2d_tableau_h_
