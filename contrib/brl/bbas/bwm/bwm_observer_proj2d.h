#ifndef bwm_observer_proj2d_h_
#define bwm_observer_proj2d_h_

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
#include <Inventor/actions/SoWriteAction.h>

class bwm_observer_proj2d : public bwm_observer, public bgui3d_project2d_tableau
{
public:
  //: constructors
  bwm_observer_proj2d(const vpgl_proj_camera<double>& camera, SoNode* scene_root)
    : bgui3d_project2d_tableau(camera, scene_root) {}; 

  virtual ~bwm_observer_proj2d() {}

  virtual vcl_string type_name() const { return "bwm_observer_proj2d"; }

  void handle_update(vgui_message const& msg, bwm_observable_sptr observable) 
  { 
    SoWriteAction myAction;
    myAction.getOutput()->openFile("C:\\test_images\\mesh\\root_proj2d.iv");
    myAction.getOutput()->setBinary(FALSE);
    myAction.apply(this->scene_root());
    myAction.getOutput()->closeFile();
    setup_projection(); 
  } 

  //void set_cam(const vpgl_perspective_camera<double> &camera)
  //{ camera_ = new vpgl_perspective_camera<double>(camera); }
  
//: connect the the given face as an inner face to the selected face
  void connect_inner_face(vsol_polygon_2d_sptr poly) {}

  //: create the interior of an objects, as a smaller copy of the outer 
  // object
  void create_interior() {}

  //: Select a polygon before you call this method. If it is a multiface 
  // object, it deletes the object where the selected polygon belongs to
  void delete_object() {}

  //: Deletes the whole set of objects created so far
  void delete_all() {}
private:

  //vpgl_perspective_camera<double> *camera_;
  //SoSeparator* root_;
};

#endif
