// This is basic/bgui3d/bgui3d_tableau.h
#ifndef bgui3d_tableau_h_
#define bgui3d_tableau_h_
  
//:
// \file
// \brief  Basic tableau that wraps Coin3D into VGUI
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   5/24/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_memory.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_event.h>
#include <vpgl/vpgl_proj_camera.h>
#include "bgui3d_tableau_sptr.h"

// forward declarations
class SbColor;
class SoNode;
class SoSceneManager;
class SbViewportRegion;

//:  Tableau that wraps Coin3D into VGUI.
class bgui3d_tableau : public vgui_tableau
{
public:
  //: Constructor - don't use this, use bgui3d_tableau_new.
  bgui3d_tableau(SoNode* scene_root = NULL);

  //: Destructor
  virtual ~bgui3d_tableau();

  //: Return the type name of this tableau
  virtual vcl_string type_name() const;

  //: file name isn't valid, so return the type_name
  virtual vcl_string file_name() const { return this->type_name(); }

  //: Handle vgui events
  virtual bool handle(const vgui_event& event);

  //: Set up OpenGL for rendering
  virtual void setup_opengl() const;

  //: Render the scene graph (called on draw events)
  virtual bool render();

  //: Render the overlay scene graph (called on draw overlay events)
  virtual bool render_overlay();

  //: Called when the scene manager requests a render action
  virtual void request_render();

  //: Called when the scene manager requests a render overlay action
  virtual void request_render_overlay();

  //: Called when VGUI is idle
  virtual bool idle();

  //: Enable handling of idle events
  void enable_idle();

  //: Disable handling of idle events
  void disable_idle();

  //: Returns true if idle event handling is enabled
  bool is_idle_enabled();

  //---------------------Scene Methods------------------------------
  //: Set scene root node
  virtual void set_scene_root(SoNode* scene_root);

  //: Return the root node in the scene graph
  SoNode* scene_root() const { return scene_root_; }

  //: Set overlay scene root node
  virtual void set_overlay_scene_root(SoNode* scene_root);

  //: Return the root node in the overlay scene graph
  SoNode* overlay_scene_root() const { return overlay_scene_root_; }

  //---------------------Camera Methods-----------------------------
  //: Set the scene camera
  // creates a graphics camera from a vpgl camera (either perspective or affine)
  virtual bool set_camera(const vpgl_proj_camera<double>& camera);

  //: Get the scene camera
  // creates a vpgl camera (either perspective or affine) from the graphics camera
  virtual vcl_auto_ptr<vpgl_proj_camera<double> > camera() const;
  //----------------------------------------------------------------

  //: Set the viewport
  void set_viewport_region(const SbViewportRegion& region);

  //: Get the viewport
  const SbViewportRegion& get_viewport_region() const;


  //--------------Interaction Methods ----------------
  enum interaction_type_enum {CAMERA, SCENEGRAPH};

  //: Set the interaction type
  virtual void set_interaction_type( interaction_type_enum );

  //: Return the type of the interaction
  interaction_type_enum interaction_type() const;


  SoSceneManager* scene_manager();

protected:
  //: The root node of the scene graph
  SoNode* scene_root_;
  SoNode* overlay_scene_root_;

  //: The scene manager
  SoSceneManager* scene_manager_;
  SoSceneManager* overlay_scene_manager_;
  bool idle_enabled_;
  interaction_type_enum interaction_type_;

private:


};


//: Create a smart pointer to a bgui3d_tableau tableau.
struct bgui3d_tableau_new : public bgui3d_tableau_sptr
{
  typedef bgui3d_tableau_sptr base;
  bgui3d_tableau_new(SoNode* scene_root = NULL)
   : base(new bgui3d_tableau(scene_root)) { }
};

#endif // bgui3d_tableau_h_
