#include "bwm_popup_menu.h"
#include "bwm_tableau_img.h"
#include "bwm_tableau_rat_cam.h"
#include "bwm_tableau_video.h"
#include "bwm_command_macros.h"

//: Use in menus to toggle a parameter
class bwm_vertex_toggle_command : public vgui_command
{
 public:
  bwm_vertex_toggle_command(bwm_tableau_cam* t, const void* boolref) : 
       tab(t), bref((bool*) boolref) {}
  void execute() { *bref = !(*bref); tab->observer()->show_vertices(*bref); }

  bwm_tableau_cam *tab;
  bool* bref;
};

void bwm_popup_menu::get_menu(vgui_menu &menu)
{

  vgui_menu submenu;

  bwm_tableau_img* img_tab = static_cast<bwm_tableau_img* > (tab_.as_pointer());
  submenu.add("Polygon..",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_polygon),
    vgui_key('p'),
    vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("PolyLine..",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_polyline),
    vgui_key('l'),
    vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("Box..",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_box),
    vgui_key('b'),
    vgui_modifier(vgui_SHIFT) );

  submenu.separator();

  submenu.add("Pointset..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_pointset));

  submenu.separator();
  submenu.add("Point..",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_point),
    vgui_key('t'),
    vgui_modifier(vgui_SHIFT) );

  submenu.separator();
  vgui_menu selmenu;
  selmenu.add( "Delete Selected",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_poly),
    vgui_key('d'), vgui_modifier(vgui_SHIFT));
  selmenu.separator();
  
  selmenu.add( "Delete All",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_all),
    vgui_key('a'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Delete ", selmenu);
  menu.add( "2D Objects", submenu);
  menu.separator();

  vgui_menu image_submenu;
  MENU_TAB_ADD_PROCESS("Range Map", "range_map", image_submenu, img_tab);
  MENU_TAB_ADD_PROCESS("Intensity Profile", "intensity_profile", image_submenu, img_tab);
  MENU_TAB_ADD_PROCESS("Histogram Plot", "histogram", image_submenu, img_tab);
  MENU_TAB_ADD_PROCESS("Step Edges VD", "step_edge", image_submenu, img_tab);
  MENU_TAB_ADD_PROCESS("Detect Lines", "detect_lines", image_submenu, img_tab);
  image_submenu.add( "Redisplay Edges",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::recover_edges),
    vgui_key('b'), vgui_modifier(vgui_SHIFT));
  image_submenu.add( "Redisplay Lines",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::recover_lines));
  image_submenu.add( "Clear Segmentation Display",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_box));
  //image_submenu.separator();
  menu.add("Image Display", image_submenu);
  menu.separator();

  vgui_menu img_other;
  img_other.add( "Show Path" ,
    new vgui_command_simple<bwm_tableau_img>(img_tab,
                                             &bwm_tableau_img::
                                             toggle_show_image_path));
  img_other.separator();
  img_other.add( "Zoom to Fit" ,
    new vgui_command_simple<bwm_tableau_img>(img_tab,
                                             &bwm_tableau_img::
                                             zoom_to_fit));
  img_other.separator();
  img_other.add( "Scroll to Image Location" ,
    new vgui_command_simple<bwm_tableau_img>(img_tab,
                                             &bwm_tableau_img::
                                             scroll_to_point));
  menu.add("Image Processing", img_other);
  menu.separator();
  menu.add( "Deselect All Objects",
    new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::deselect_all),
    vgui_key('-'));
  
  // add more based on the tableau type
  // all camera tableau children will do the following menu items
  if ((tab_->type_name().compare("bwm_tableau_proj_cam") == 0) || 
    (tab_->type_name().compare("bwm_tableau_rat_cam") == 0)  || 
    (tab_->type_name().compare("bwm_tableau_video") == 0)) {

    bwm_tableau_cam* cam_tab = static_cast<bwm_tableau_cam* > (tab_.as_pointer());
    vgui_menu reg_submenu, threed_menu;
    menu.separator();
    menu.add( "Set as Master", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_master)); 
  
    reg_submenu.add( "Set as EO", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_eo)); 
    reg_submenu.separator();
    reg_submenu.add( "Set as Other Mode", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_other_mode)); 
    reg_submenu.separator();
    
    if (tab_->type_name().compare("bwm_tableau_rat_cam") == 0) {
      bwm_tableau_rat_cam* rat_cam_tab = static_cast<bwm_tableau_rat_cam* > (tab_.as_pointer());
      reg_submenu.add( "Register Image to World Pt", 
        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
        &bwm_tableau_rat_cam::adjust_camera_to_world_pt));
      reg_submenu.separator();
      reg_submenu.add( "Register to Master", 
        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab, 
        &bwm_tableau_rat_cam::register_search_to_master));
      reg_submenu.separator();
      reg_submenu.add( "Transfer Edges from Master", 
        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
        &bwm_tableau_rat_cam::project_edges_from_master));
    }
    menu.separator();
    menu.add("Registration", reg_submenu);

    vgui_menu mesh_submenu;
    vcl_string on = "[x] ", off = "[ ] ";
    mesh_submenu.add( ((cam_tab->show_vertices_)?on:off)+"show vertices", new bwm_vertex_toggle_command(cam_tab, &(cam_tab->show_vertices_)));
    mesh_submenu.separator();

    mesh_submenu.add("Create Mesh Polygon", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::create_polygon_mesh),
      vgui_key('p'), vgui_modifier(vgui_SHIFT) );

    mesh_submenu.separator();
    mesh_submenu.add("Triangulate..", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::triangulate_mesh),
      vgui_key('t'), vgui_modifier(vgui_SHIFT));
    mesh_submenu.separator();

    mesh_submenu.add( "Create Inner Face", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::create_inner_face));
    mesh_submenu.separator();

    mesh_submenu.add( "Move Face with Selected Vertex", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::move_obj_by_vertex),
      vgui_key('m'), vgui_modifier(vgui_SHIFT));
    mesh_submenu.separator();

    mesh_submenu.add( "Extrude Face", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::extrude_face),
      vgui_key('e'), vgui_modifier(vgui_SHIFT));
    mesh_submenu.separator();

    mesh_submenu.add( "Divide Face", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::divide_face));
    mesh_submenu.separator();

    mesh_submenu.add("Scan Regions", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::scan_regions));

    if (tab_->type_name().compare("bwm_tableau_rat_cam") == 0) {
      bwm_tableau_rat_cam* rat_cam_tab = static_cast<bwm_tableau_rat_cam* > (tab_.as_pointer());
      mesh_submenu.separator();
      mesh_submenu.add( "Center on Geographic Position", 
        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
        &bwm_tableau_rat_cam::center_pos));
    }

    vgui_menu del_menu;
    del_menu.add( "Delete Selected", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::delete_object));
    del_menu.separator();

    del_menu.add( "Delete All", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::delete_all));
    mesh_submenu.separator();
    mesh_submenu.add("Delete", del_menu);
    menu.separator();
    menu.add("3D Objects", mesh_submenu);

    menu.separator();
    vgui_menu corr_menu;
    corr_menu.add( "Move (selected)" , 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::move_corr));
    corr_menu.separator();

    corr_menu.add( "Set Corr to Selected Vertex" ,
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_corr_to_vertex));
    corr_menu.separator();

    corr_menu.add( "Save World Point (selected)" , 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::world_pt_corr));
    menu.add( "Image Correspondence", corr_menu);

    menu.separator();
    vgui_menu plane_submenu;
    plane_submenu.add( "Define XY Projection Plane", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::define_xy_proj_plane));
    plane_submenu.separator();

    plane_submenu.add( "Define YZ Projection Plane", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::define_yz_proj_plane));
    plane_submenu.separator();

    plane_submenu.add( "Define XZ Projection Plane", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::define_xz_proj_plane));
    plane_submenu.separator();

    plane_submenu.add( "Selected Face", 
      new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::select_proj_plane));
    menu.add("Projection Plane", plane_submenu);

    if (tab_->type_name().compare("bwm_tableau_rat_cam") == 0) {
      bwm_tableau_rat_cam* rat_cam_tab = static_cast<bwm_tableau_rat_cam* > (tab_.as_pointer());
      
      
      menu.separator();
      vgui_menu lvcs_submenu;
      //lvcs_submenu.add( "Load LVCS", new bwm_load_lvcs_command(this));
      //lvcs_submenu.add( "Save LVCS", new bwm_save_lvcs_command(this));
      lvcs_submenu.add( "Define LVCS", 
        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
        &bwm_tableau_rat_cam::define_lvcs)); 
      //lvcs_submenu.add( "Convert File to LVCS", new bwm_convert_to_lvcs_command(this));
      lvcs_submenu.separator();
      lvcs_submenu.add( "Adjust Camera To LVCS", 
        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
        &bwm_tableau_rat_cam::adjust_camera_to_world_pt)); 
      menu.add("LVCS", lvcs_submenu);
    }
  } 

  // video tableau specific menu items
  if (tab_->type_name().compare("bwm_tableau_video") == 0) {
    bwm_tableau_video* video_tab = static_cast<bwm_tableau_video* > (tab_.as_pointer());
    menu.separator();
    vgui_menu video_submenu;
    video_submenu.add( "Play", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::play)); 
    video_submenu.separator();

    video_submenu.add( "Stop", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::stop)); 
    video_submenu.separator();

    video_submenu.add( "Pause", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::pause)); 
    video_submenu.separator();

    video_submenu.add( "Goto Frame", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::seek)); 
    video_submenu.separator();

    video_submenu.add( "Next Frame", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::next_frame)); 
    video_submenu.separator();

    video_submenu.add( "Previous Frame", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::previous_frame)); 
    menu.add("Video", video_submenu);
    menu.separator();

    vgui_menu video_corr_submenu;
    video_corr_submenu.add( "Add Match", 
      new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::add_match)); 
    video_corr_submenu.separator();

    video_corr_submenu.add( "Remove Selected Corr Match",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,
                       &bwm_tableau_video::remove_selected_corr_match));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Remove Selected Corr ",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,
                       &bwm_tableau_video::remove_selected_corr)); 
    video_corr_submenu.separator();

    video_corr_submenu.add( "Display Corrs",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,
                       &bwm_tableau_video::display_video_corrs)); 
    video_corr_submenu.separator();

    video_corr_submenu.add( "Display Current Corrs",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,
                       &bwm_tableau_video::display_current_video_corrs)); 
    video_corr_submenu.separator();

    video_corr_submenu.add( "Clear Corr Display",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,
                       &bwm_tableau_video::clear_video_corrs_display)); 
    video_corr_submenu.separator();

    video_corr_submenu.add( "Toggle World Point Display",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,
                       &bwm_tableau_video::toggle_world_pt_display)); 
    menu.add("Video Correspondence", video_corr_submenu);  
  }
}