// This is basic/bgui3d/bgui3d_fullviewer_tableau.cxx

//:
// \file
#include <vcl_iostream.h>

#include "bgui3d_fullviewer_tableau.h"
#include "bgui3d_file_io.h"
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_popup_params.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SoDB.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/misc/SoChildList.h>

//: Constructor 
bgui3d_fullviewer_tableau::bgui3d_fullviewer_tableau(SoNode * scene_root)
 : bgui3d_viewer_tableau(scene_root)
{
}


//: Destructor
bgui3d_fullviewer_tableau::~bgui3d_fullviewer_tableau()
{
}


bool bgui3d_fullviewer_tableau::handle(const vgui_event& e)
{
  if( e.type == vgui_KEY_PRESS && e.key == 'm')
    {
    if( interaction_type_ == CAMERA )
      set_interaction_type( SCENEGRAPH );
    else
      set_interaction_type( CAMERA );
    }
  return bgui3d_tableau::handle(e);
}



//----------------------------------------------------------------------------
//: A vgui command used to write a scene graph to disk
class bgui3d_export_command : public vgui_command
{
  public:
  bgui3d_export_command(bgui3d_fullviewer_tableau* tab) : bgui3d_fullviewer_tab(tab) {}
  void execute()
  {
    vgui_dialog export_dlg("Export Scene Graph");
    static vcl_string file_name = "";
    static vcl_string ext = "";
    export_dlg.file("File:", ext, file_name);
    static int type = 0;
    export_dlg.choice("Type:","IV","VRML","VRML 2.0", type);
    static bool full_scene = false;
    export_dlg.checkbox("Include camera and headlight", full_scene);
    if( !export_dlg.ask())
      return;

    SoNode* root = NULL;
    if(full_scene)
      root = bgui3d_fullviewer_tab->scene_root();
    else
      root = bgui3d_fullviewer_tab->user_scene_root();
      
    switch(type){
    case 0:
      bgui3d_export_iv(root, file_name);
      break;
    case 1:
      bgui3d_export_vrml(root, file_name);
      break;
    case 2:
      bgui3d_export_vrml2(root, file_name);
      break;
    }
  }

  bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
};


//----------------------------------------------------------------------------
//: A vgui command used to import a scene graph from a file
class bgui3d_import_command : public vgui_command
{
  public:
  bgui3d_import_command(bgui3d_fullviewer_tableau* tab) : bgui3d_fullviewer_tab(tab) {}
  void execute()
  {
    vgui_dialog import_dlg("Import Scene Graph");
    static vcl_string file_name = "";
    static vcl_string ext = "iv";
    import_dlg.file("File:", ext, file_name);
    static bool overwrite = false;
    import_dlg.checkbox("Overwrite existing scene", overwrite);
    if( !import_dlg.ask())
      return;

    // read the file
    SoNode* new_scene = bgui3d_import_file(file_name);
    new_scene->ref();

    if(overwrite){
      //bgui3d_fullviewer_tab->set_scene_root(new_scene);

      // get the old scene_root
      SoNode* old_scene = bgui3d_fullviewer_tab->user_scene_root();
      SoSeparator* scene_root = NULL;

      if(old_scene && SoSeparator::getClassTypeId() == old_scene->getTypeId()){
        scene_root = (SoSeparator*)old_scene;
        scene_root->removeAllChildren();
        scene_root->addChild(new_scene);
      }

    }
    // add the new scene to the existing one
    else{
      // get the old scene_root
      SoNode* old_scene = bgui3d_fullviewer_tab->user_scene_root();
      SoSeparator* scene_root = NULL;

      // if the old scene_root was not a separator make it one
      if(old_scene && SoSeparator::getClassTypeId() == old_scene->getTypeId())
        scene_root = (SoSeparator*)old_scene;
      else{
        scene_root = new SoSeparator;
        scene_root->addChild(old_scene);
        bgui3d_fullviewer_tab->set_scene_root(scene_root);
      }
      // add the new scene
      scene_root->addChild(new_scene);
    }
    new_scene->unref();
  }

  bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
};


//----------------------------------------------------------------------------
//: A vgui command used to toggle animation
class bgui3d_animate_command : public vgui_command
{
  public:
  bgui3d_animate_command(bgui3d_fullviewer_tableau* tab) : bgui3d_fullviewer_tab(tab) {}
  void execute()
  {
    if( bgui3d_fullviewer_tab->is_idle_enabled() )
      bgui3d_fullviewer_tab->disable_idle();
    else
      bgui3d_fullviewer_tab->enable_idle();
  }

  bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
};


//----------------------------------------------------------------------------
//: A vgui command used to toggle the headlight
class bgui3d_headlight_command : public vgui_command
{
  public:
  bgui3d_headlight_command(bgui3d_fullviewer_tableau* tab) : bgui3d_fullviewer_tab(tab) {}
  void execute()
  {
    bool headlight = bgui3d_fullviewer_tab->is_headlight();
    bgui3d_fullviewer_tab->set_headlight(!headlight);
  }

  bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
};


//----------------------------------------------------------------------------
//: A vgui command used to toggle camera type
class bgui3d_camera_type_command : public vgui_command
{
  public:
  bgui3d_camera_type_command(bgui3d_fullviewer_tableau* tab,
                             bgui3d_fullviewer_tableau::camera_type_enum type) 
   : bgui3d_fullviewer_tab(tab), cam_type(type) {}
  void execute()
  {
    bgui3d_fullviewer_tab->set_camera_type(cam_type);
  }

  bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
  bgui3d_fullviewer_tableau::camera_type_enum cam_type;
};


//----------------------------------------------------------------------------
//: A vgui command used to toggle interaction type
class bgui3d_interaction_type_command : public vgui_command
{
  public:
  bgui3d_interaction_type_command(bgui3d_fullviewer_tableau* tab,
                             bgui3d_tableau::interaction_type_enum type) 
   : bgui3d_fullviewer_tab(tab), interaction_type(type) {}
  void execute()
  {
    bgui3d_fullviewer_tab->set_interaction_type(interaction_type);
  }

  bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
  bgui3d_fullviewer_tableau::interaction_type_enum interaction_type;
};


//----------------------------------------------------------------------------
//: A vgui command used to select a camera
class bgui3d_select_camera_command : public vgui_command
{
  public:
    bgui3d_select_camera_command(bgui3d_fullviewer_tableau* tab,int idx)
  : bgui3d_fullviewer_tab(tab), index(idx) {}
    void execute()
    {
      bgui3d_fullviewer_tab->select_camera(index);
    }

    bgui3d_fullviewer_tableau *bgui3d_fullviewer_tab;
    int index;
};



//----------------------------------------------------------------------------
//: Builds a popup menu
void bgui3d_fullviewer_tableau::get_popup(const vgui_popup_params& params,
                                          vgui_menu &menu)
{
  vcl_string animation_item;
  if( this->is_idle_enabled() )
    animation_item = "Disable Animation";
  else
    animation_item = "Enable Animation";
  
  vcl_string headlight_item;
  if( this->is_headlight() )
    headlight_item = "Disable Headlight";
  else
    headlight_item = "Enable Headlight";


  vcl_string check_on = "[x]";
  vcl_string check_off = "[ ]";

  if( camera_group_ ){
    int active_cam_idx = camera_group_->whichChild.getValue();
    SoChildList* list = camera_group_->getChildren();
    vgui_menu camera_list_menu;
    if(!this->find_cameras(this->user_scene_root()).empty()){
      vcl_string name = ((active_cam_idx<0)?check_on:check_off) +" Scene Camera";
      camera_list_menu.add(name,new bgui3d_select_camera_command(this,-1));
    }
    for(int i=0; i<list->getLength(); ++i){
      vcl_string name((*list)[i]->getName().getString());
      name = ((i==active_cam_idx)?check_on:check_off) +" "+ name;
      camera_list_menu.add(name,new bgui3d_select_camera_command(this,i));
    }
    if(camera_list_menu.size() > 1)
      menu.add("Choose Camera",camera_list_menu);
  }


  vgui_menu camera_menu;
  camera_type_enum cam_type = this->camera_type();

  camera_menu.add(((cam_type==ORTHOGONAL)?check_on:check_off) + " Orthogonal",
                  new bgui3d_camera_type_command(this,ORTHOGONAL));
  camera_menu.add(((cam_type==PERSPECTIVE)?check_on:check_off) + " Perspective",
                  new bgui3d_camera_type_command(this,PERSPECTIVE));

  vgui_menu interaction_menu;
  interaction_type_enum interaction_type = this->interaction_type();

  interaction_menu.add(((interaction_type==bgui3d_tableau::CAMERA)?check_on:check_off) + " Camera",
                       new bgui3d_interaction_type_command(this,bgui3d_tableau::CAMERA));
  interaction_menu.add(((interaction_type==bgui3d_tableau::SCENEGRAPH)?check_on:check_off) + " SceneGraph",
                       new bgui3d_interaction_type_command(this,bgui3d_tableau::SCENEGRAPH));



  menu.add("Export Scene", new bgui3d_export_command(this));
  menu.add("Import Scene", new bgui3d_import_command(this));
  menu.add("Interaction", interaction_menu);
  menu.add("Camera", camera_menu);
  menu.add(animation_item, new bgui3d_animate_command(this));
  menu.add(headlight_item, new bgui3d_headlight_command(this));

}

