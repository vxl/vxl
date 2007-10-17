// This is basic/bgui3d/bgui3d_tableau.cxx
  
//:
// \file

#include <vcl_iostream.h>
#include "bgui3d_tableau.h"
#include "bgui3d_translate_event.h"
#include <vgui/vgui_gl.h>
#include <vgui/vgui_macro.h>

#include <Inventor/SbColor.h>
#include <Inventor/SoDB.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/actions/SoGLRenderAction.h>


//: The callback called by the scene manager to initiate a redraw
static void
bgui3d_render_callback(void *tableau, SoSceneManager *mgr)
{
  bgui3d_tableau * tab = (bgui3d_tableau *) tableau;
  tab->request_render();
}

//: The callback called by the overlay scene manager to initiate a redraw
static void
bgui3d_render_overlay_callback(void *tableau, SoSceneManager *mgr)
{
  bgui3d_tableau * tab = (bgui3d_tableau *) tableau;
  tab->request_render_overlay();
}

//------------------------------------------------------------------

//: Constructor - don't use this, use bgui3d_tableau_new.
bgui3d_tableau::bgui3d_tableau(SoNode * scene_root)
 : scene_root_(NULL), overlay_scene_root_(NULL),
   scene_manager_(NULL), overlay_scene_manager_(NULL),
   idle_enabled_(false), interaction_type_(SCENEGRAPH)
{
  this->set_scene_root(scene_root);
}


//: Destructor
bgui3d_tableau::~bgui3d_tableau()
{
  this->disable_idle();

  if(scene_root_)
    scene_root_->unref();
  if(overlay_scene_root_)
    overlay_scene_root_->unref();

  //: Ming: no need to release memory of the SoSceneManager* scene_manager_
  ///if(scene_manager_)
  ///  delete scene_manager_;

  //: Ming: no need to release memory of the SoSceneManager* overlay_scene_manager_
  ///if(overlay_scene_manager_)
  ///  delete overlay_scene_manager_;
}


vcl_string bgui3d_tableau::type_name() const {return "bgui3d_tableau";}


//: Set up OpenGL for rendering
void
bgui3d_tableau::setup_opengl() const
{
  // This call is still needed on the linux side
  glEnable(GL_DEPTH_TEST);

  glLineWidth( 1.0 );
  glPointSize( 1.0 );

}


//: Render the scene graph (called on draw events)
bool
bgui3d_tableau::render()
{
  if(!scene_manager_)
    return false;
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  
  // Set up OpenGL for Rendering
  this->setup_opengl();

  // Update the scene manager if the viewport has changed
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  SbViewportRegion vguiViewport;
  vguiViewport.setViewportPixels(vp[0], vp[1], vp[2], vp[3]);

  if( !(scene_manager_->getViewportRegion() == vguiViewport) ){
    scene_manager_->setViewportRegion(vguiViewport);
  }
  
  // Reinitialize to account for changes in the OpenGL context
  scene_manager_->reinitialize();

  // Do the rendering
  scene_manager_->render(scene_manager_->getGLRenderAction(), false, false);

  glPopAttrib();
  
  return true;
}


//: Render the overlay scene graph (called on draw overlay events)
bool
bgui3d_tableau::render_overlay()
{
  if(!overlay_scene_manager_)
    return false;
    
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // Set up OpenGL for Rendering
  this->setup_opengl();
  
  // Update the scene manager if the viewport has changed
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  SbViewportRegion vguiViewport;
  vguiViewport.setViewportPixels(vp[0], vp[1], vp[2], vp[3]);

  if( !(overlay_scene_manager_->getViewportRegion() == vguiViewport) ){
    overlay_scene_manager_->setViewportRegion(vguiViewport);
  }

  // Reinitialize to account for changes in the OpenGL context
  overlay_scene_manager_->reinitialize();

  // Do the rendering
  overlay_scene_manager_->render(overlay_scene_manager_->getGLRenderAction(),false,false);

  glPopAttrib();

  return true;
}


//: Handle vgui events
bool bgui3d_tableau::handle(const vgui_event& e)
{
  
  // Handle draw events
  if ( e.type == vgui_DRAW )
    return this->render();

  if ( e.type == vgui_DRAW_OVERLAY )
    return this->render_overlay();

  if(!scene_manager_)
    return false;
  
  // Attempt to convert all unhandled events to SoEvents
  // and pass the SoEvents to the scene graph for handling
  SoDB::getSensorManager()->processDelayQueue(TRUE);
  SoDB::getSensorManager()->processTimerQueue();
  if( interaction_type_ == SCENEGRAPH )
  {
    bool handled = false;
    SoEvent* event = bgui3d_translate_event(e);
    if (event) {
      handled = scene_manager_->processEvent(event) > 0;
      if(handled)
        return true;
    }
  }
  request_render();  
  
  return vgui_tableau::handle(e);
}


bool
bgui3d_tableau::idle()
{  
  SoDB::getSensorManager()->processTimerQueue();
  SoDB::getSensorManager()->processDelayQueue(TRUE);
  return idle_enabled_;
}


//: Enable handling of idle events
void 
bgui3d_tableau::enable_idle()
{
  idle_enabled_ = true;
  this->post_idle_request();
}


//: Disable handling of idle events
void 
bgui3d_tableau::disable_idle()
{
  idle_enabled_ = false;
}


//: Returns true if idle event handling is enabled
bool 
bgui3d_tableau::is_idle_enabled()
{
  return idle_enabled_;
}


//: Called when the scene manager requests a render action
void
bgui3d_tableau::request_render()
{
  this->post_redraw();
}


//: Called when the overlay scene manager requests a render action
void
bgui3d_tableau::request_render_overlay()
{
  this->post_overlay_redraw();
}


//: Set scene root node
void
bgui3d_tableau::set_scene_root(SoNode* scene_root)
{
  if(scene_root_)
    scene_root_->unref();
  scene_root_ = scene_root;
  if(scene_root_)
    scene_root_->ref();

  if(scene_root_){
    if(!scene_manager_)
    {
      scene_manager_ = new SoSceneManager();
      scene_manager_->getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
      //DELAYED_BLEND
    }
    else
      scene_manager_->deactivate();

    scene_manager_->setSceneGraph(scene_root_);
    scene_manager_->setRenderCallback(bgui3d_render_callback, this);
    scene_manager_->activate();
  }
}


//: Set overlay scene root node
void
bgui3d_tableau::set_overlay_scene_root(SoNode* scene_root)
{
  if(overlay_scene_root_)
    overlay_scene_root_->unref();
  overlay_scene_root_ = scene_root;
  if(overlay_scene_root_)
    overlay_scene_root_->ref();

  if(overlay_scene_root_){
    if(!overlay_scene_manager_)
      overlay_scene_manager_ = new SoSceneManager();
    else
      overlay_scene_manager_->deactivate();

    overlay_scene_manager_->setSceneGraph(scene_root_);
    overlay_scene_manager_->setRenderCallback(bgui3d_render_overlay_callback, this);
    overlay_scene_manager_->activate();
  }
}


//: Set the scene camera
// creates a graphics camera from a vpgl camera (either perspective or affine)
bool
bgui3d_tableau::set_camera(const vpgl_proj_camera<double>& camera)
{
  return false;
}


//: Get the scene camera
// creates a vpgl camera (either perspective or affine) from the graphics camera
vcl_auto_ptr<vpgl_proj_camera<double> >
bgui3d_tableau::camera() const
{
  return vcl_auto_ptr<vpgl_proj_camera<double> >(NULL);
}


void
bgui3d_tableau::set_viewport_region(const SbViewportRegion& region)
{
  if (region.getWindowSize()[0] == -1) return;

  scene_manager_->setViewportRegion(region);
  overlay_scene_manager_->setViewportRegion(region);
  
}

const SbViewportRegion &
bgui3d_tableau::get_viewport_region() const
{
  return scene_manager_->getViewportRegion();
}

//: Set the interaction type
void
bgui3d_tableau::set_interaction_type(interaction_type_enum type)
{
  interaction_type_ = type;
}

//: Get the interaction type
bgui3d_tableau::interaction_type_enum
bgui3d_tableau::interaction_type() const
{
  return interaction_type_;
}


SoSceneManager*
bgui3d_tableau::scene_manager()
{
  if(!scene_manager_)
    {
      scene_manager_ = new SoSceneManager();
      scene_manager_->getGLRenderAction()->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
      //BLEND
    }
    else
      scene_manager_->deactivate();
  return scene_manager_;
}
