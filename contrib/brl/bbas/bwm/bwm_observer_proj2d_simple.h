#ifndef bwm_observer_proj2d_simple_h_
#define bwm_observer_proj2d_simple_h_

#include "bwm_observer_proj2d.h"
#include "bwm_observable.h"
#include <vcl_map.h>
#include <vcl_vector.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bgui3d/bgui3d_examiner_tableau_sptr.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoEventCallback.h> 

class bwm_observer_proj2d_simple :  public bwm_observer_proj2d
{
public:
  //: constructors
  bwm_observer_proj2d_simple(vpgl_proj_camera<double>* camera,
    SoNode* scene_root): bwm_observer_proj2d(camera, scene_root){}//bgui3d_project2d_tableau(*camera, scene_root) {};

  virtual ~bwm_observer_proj2d_simple() {}

  void update(vgui_message const& msg){ setup_projection(); } 

  void set_cam(const vpgl_perspective_camera<double> &camera)
  { camera_ = new vpgl_perspective_camera<double>(camera); }
  
private:
  vpgl_perspective_camera<double> *camera_;
  SoSeparator* root_;
};

#endif
