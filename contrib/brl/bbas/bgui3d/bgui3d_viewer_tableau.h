// This is brl/bbas/bgui3d/bgui3d_viewer_tableau.h
#ifndef bgui3d_viewer_tableau_h_
#define bgui3d_viewer_tableau_h_
//:
// \file
// \brief  Abstract base tableau for 3D viewers
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   5/25/04
//
// \verbatim
//  Modifications
// \endverbatim

#include "bgui3d_tableau.h"

// forward declarations
class SoCamera;
class SoDirectionalLight;
class SoGroup;
class SoOrthographicCamera;
class SoPerspectiveCamera;
class SoSwitch;
class SoText2;


//:  Abstract base tableau for 3D viewers
class bgui3d_viewer_tableau : public bgui3d_tableau
{
 protected:
  //: Constructor
  bgui3d_viewer_tableau(SoNode * scene_root = NULL);

  //: Destructor
  virtual ~bgui3d_viewer_tableau();

 public:
  //: Handle vgui events
  virtual bool handle(const vgui_event& event) = 0;

  //: Return the type name of this tableau
  virtual vcl_string type_name() const = 0;

  //: Set the scene root
  virtual void set_scene_root(SoNode* scene_root);

  //: Return the root node in the users portion of the scene graph
  SoNode* user_scene_root() const { return user_scene_root_; }

  //--------------Camera Methods--------------------
  enum camera_type_enum {PERSPECTIVE, ORTHOGONAL};

  //: Set the scene camera
  // Generate an SoCamera from a camera matrix and use it
  virtual bool set_camera(const vpgl_proj_camera<double>& camera);

  //: Get the scene camera
  // Creates a vpgl camera (either perspective or affine) from the active SoCamera
  virtual vcl_auto_ptr<vpgl_proj_camera<double> > camera() const;

  //: Set the camera viewing the scene
  virtual void set_camera(SoCamera *camera);

  //: Select the active camera by index
  // A negative index selects the first user scene camera
  void select_camera(int camera_index);

  //: Return the camera viewing the scene
  SoCamera* camera_node() const;

  //: Set the camera type (Perspective or Orthogonal)
  virtual void set_camera_type(camera_type_enum type);

  //: Return the camera type (Perspective or Orthogonal)
  camera_type_enum camera_type() const;

  //: Toggle the camera type between Perspective and Orthogonal
  virtual void toggle_camera_type();

  //: Adjust the camera to view the entire scene
  virtual void view_all();

  //: Save the current camera as the home position
  virtual void save_home_position();

  //: Restore the saved home position of the camera
  virtual void reset_to_home_position();

  virtual void set_clipping_planes();
  //-------------------------------------------------

  //-------------Headlight Methods-------------------
  //: Activate a headlight
  virtual void set_headlight(bool enable);

  //: Is the headlight active
  bool is_headlight() const;

  //: Return the headlight
  SoDirectionalLight* headlight() const;
  //-------------------------------------------------

  //-------------Text Methods-------------------
  //: set the text
  void setText( const vcl_string& string );

 protected:
  //: Find the parent nodes in the scene graph
  vcl_vector<SoGroup*> get_parents_of_node(SoNode * node);

  //: Convert to perspective
  void convertOrtho2Perspective(const SoOrthographicCamera * in,
                                         SoPerspectiveCamera * out);
  //: Convert to orthographic
  void convertPerspective2Ortho(const SoPerspectiveCamera * in,
                     SoOrthographicCamera * out);

  //: Find the camera nodes in the scenegraph
  vcl_vector<SoCamera*> find_cameras(SoNode* root) const;

  //: Find the VRML viewpoint nodes in the scenegraph and make camera
  // The cameras are added to the camera group (outside the user scene)
  void collect_vrml_cameras(SoNode* root) const;

 protected:

  //: The subgraph provided by the user
  SoNode* user_scene_root_;

  SoSwitch* camera_group_;

  SoCamera* scene_camera_;
  SoNode* stored_camera_;

  camera_type_enum camera_type_;

  SoDirectionalLight * headlight_;

  SoText2* _text;
};

#endif // bgui3d_viewer_tableau_h_
