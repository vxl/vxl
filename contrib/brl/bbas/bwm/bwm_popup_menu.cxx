#include "bwm_popup_menu.h"
//:
// \file

#include "bwm_tableau_img.h"
#include "bwm_tableau_rat_cam.h"
#include "bwm_tableau_geo_cam.h"
#include "bwm_tableau_generic_cam.h"
#include "bwm_tableau_video.h"
#include "bwm_command_macros.h"
#include "bwm_tableau_mgr.h"

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
  // 2D objects submenu
  vgui_menu submenu;

  bwm_tableau_img* img_tab = static_cast<bwm_tableau_img* > (tab_.as_pointer());
  submenu.add("Polygon..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_polygon),
              vgui_key('g'),  vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("PolyLine..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_polyline),
              vgui_key('l'),  vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("Box..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_box),
              vgui_key('b'), vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("Pointset..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_pointset));
  submenu.separator();

  submenu.add("Point..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_point),
              vgui_key('t'), vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("Copy..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::copy),
              vgui_key('c'), vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  submenu.add("Paste..",
              new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::paste),
              vgui_key('p'), vgui_modifier(vgui_SHIFT) );
  submenu.separator();

  vgui_menu poly_draw_menu;
  poly_draw_menu.add("Polygon...", new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::set_poly_mode));
  poly_draw_menu.add("Vertex...", new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::set_vertex_mode));
  submenu.separator();

  submenu.add("Set Draw Mode", poly_draw_menu);
  submenu.separator();

  submenu.add( "Save 2D Spatial Objects (binary)",
               new vgui_command_simple<bwm_tableau_img>(img_tab, &bwm_tableau_img::save_spatial_objects_2d));
  submenu.add( "Load 2D Spatial Objects (binary)",
               new vgui_command_simple<bwm_tableau_img>(img_tab, &bwm_tableau_img::load_spatial_objects_2d));
  submenu.add( "Save Pointset 2D (ascii)",
               new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                        &bwm_tableau_img::save_pointset_2d_ascii));
  submenu.add( "Load Pointset 2D (ascii)",
               new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                        &bwm_tableau_img::load_pointset_2d_ascii));
  submenu.add( "Load bounding boxes (ascii)",
             new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                      &bwm_tableau_img::load_bounding_boxes_2d_ascii));
  submenu.add( "Load oriented boxes (ascii)",
               new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                        &bwm_tableau_img::load_oriented_boxes_2d_ascii));
  // Delete 2D submenu
  vgui_menu selmenu;
  selmenu.add( "Delete Selected",
               new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_poly),
               vgui_key('d'), vgui_modifier(vgui_SHIFT));
  selmenu.separator();

  selmenu.add( "Delete All",
               new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_all),
               vgui_key('a'), vgui_modifier(vgui_SHIFT));

  selmenu.separator();

  selmenu.add( "Delete objects on all frames",
               new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_all_frames) );

  selmenu.separator();

  submenu.add( "Delete ", selmenu);

  menu.add( "2D Objects", submenu);
  menu.separator();

  // Image Processing submenu
  vgui_menu image_submenu;

  // Add command to run 2D Gauss fit
  MENU_TAB_ADD_PROCESS("Gauss fit - Subpixel", "gauss process", image_submenu, img_tab);

  MENU_TAB_ADD_PROCESS("Step Edges VD", "step_edge", image_submenu, img_tab);
  MENU_TAB_ADD_PROCESS("Detect Lines", "detect_lines", image_submenu, img_tab);

  image_submenu.add( "Redisplay Edges",
                     new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::recover_edges),
                     vgui_key('b'), vgui_modifier(vgui_SHIFT));

  image_submenu.add( "Redisplay Lines",
                     new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::recover_lines));

  image_submenu.add( "Clear Segmentation Display",
                     new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::clear_box));

  image_submenu.add( "Crop Image",
                     new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::crop_image));

  if (bwm_tableau_mgr::is_registered("bwm_tableau_video"))
  {
    vgui_menu mask_menu;
    mask_menu.add( "Init Mask",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::init_mask));

    mask_menu.add( "Set Change Type",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::set_change_type));

    mask_menu.add( "Add Regions to Mask(selected polygons)",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::add_poly_to_mask));

    mask_menu.add( "Remove Regions from Mask(selected polygons)",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::remove_poly_from_mask));

    //mask_menu.add( "Create Mask Image(bool)",
    //               new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::create_mask));

    mask_menu.add( "Save Mask Image",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::save_mask));

    mask_menu.add( "Save Changes(Binary)",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::save_changes_binary));

    mask_menu.add( "Load Changes(Binary)",
                   new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::load_changes_binary));
    image_submenu.add("Mask Ops", mask_menu);
  }
  //image_submenu.separator();

  menu.add("Image Processing", image_submenu);
  menu.separator();

  // Image Display submenu
  vgui_menu img_other;

  MENU_TAB_ADD_PROCESS("Range Map", "range_map", img_other, img_tab);
  MENU_TAB_ADD_PROCESS("Intensity Profile", "intensity_profile", img_other, img_tab);
  MENU_TAB_ADD_PROCESS("Histogram Plot", "histogram", img_other, img_tab);

  MENU_TAB_ADD_PROCESS("Histogram Plot(Poly)", "histogram_poly", img_other, img_tab);

  img_other.add( "Show Path" ,
                 new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                          &bwm_tableau_img::
                                                          toggle_show_image_path));
  img_other.separator();
  img_other.add( "Zoom to Fit" ,
                 new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                          &bwm_tableau_img::
                                                          zoom_to_fit));
  img_other.add( "Scroll to Image Location" ,
                 new vgui_command_simple<bwm_tableau_img>(img_tab,
                                                          &bwm_tableau_img::
                                                          scroll_to_point));
  menu.add("Image Display", img_other);
//  menu.separator();

//  menu.add( "Deselect All Objects", // Not needed with shift middle mouse
//            new vgui_command_simple<bwm_tableau_img>(img_tab,&bwm_tableau_img::deselect_all),
//            vgui_key('-'));

  // add more based on the tableau type
  // all camera tableau children will do the following menu items
  if (tab_->type_name().compare("bwm_tableau_proj_cam")    == 0 ||
      tab_->type_name().compare("bwm_tableau_geo_cam")     == 0 ||
      tab_->type_name().compare("bwm_tableau_rat_cam")     == 0 ||
      tab_->type_name().compare("bwm_tableau_video")       == 0 ||
      tab_->type_name().compare("bwm_tableau_generic_cam") == 0)
  {
    // 3D Objects menu
    bwm_tableau_cam* cam_tab = static_cast<bwm_tableau_cam* > (tab_.as_pointer());
    //might be a generic camera tableau
    bwm_tableau_generic_cam* gen_tab = nullptr;
    if (cam_tab->type_name()=="bwm_tableau_generic_cam")
      gen_tab = static_cast<bwm_tableau_generic_cam* > (tab_.as_pointer());

    vgui_menu mesh_submenu;
    std::string on = "[x] ", off = "[ ] ";
    mesh_submenu.add( ((cam_tab->show_vertices_)?on:off)+"show vertices",
                      new bwm_vertex_toggle_command(cam_tab, &(cam_tab->show_vertices_)));

    vgui_menu draw_menu;
    draw_menu.add("Mesh...", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_mesh_mode));
    draw_menu.add("Face...", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_face_mode));
    draw_menu.add("Edge...", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_edge_mode));
    draw_menu.add("Vertex...", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_vertex_mode));
    mesh_submenu.separator();

    mesh_submenu.add("Set Draw Mode", draw_menu);
    mesh_submenu.separator();

    mesh_submenu.add("Load Mesh",
                     new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::load_mesh),
                     vgui_key('m'), vgui_modifier(vgui_SHIFT) );
    mesh_submenu.separator();

    mesh_submenu.add("Create Terrain",
                     new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::create_terrain),
                     vgui_key('t'), vgui_modifier(vgui_SHIFT) );
    mesh_submenu.separator();

    mesh_submenu.add("Create Mesh Polygon",
                     new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::create_polygon_mesh),
                     vgui_key('p'), vgui_modifier(vgui_SHIFT) );
    mesh_submenu.separator();

    mesh_submenu.add("Create Circular Polygon",
                     new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::create_circular_polygon),
                     vgui_key('c'), vgui_modifier(vgui_SHIFT) );
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

    mesh_submenu.separator();

    mesh_submenu.add("Set Draw Mode", draw_menu);

    mesh_submenu.separator();
#if 0
    vgui_menu boxm_submenu;

    boxm_submenu.add("Create BOXM scene from mesh",
                     new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::create_boxm_scene));
    boxm_submenu.separator();
    boxm_submenu.add("Load BOXM scene",
                     new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::load_boxm_scene));

    mesh_submenu.add("BOXM", boxm_submenu);
#endif
    if (tab_->type_name().compare("bwm_tableau_rat_cam") == 0) {
      bwm_tableau_rat_cam* rat_cam_tab = static_cast<bwm_tableau_rat_cam* > (tab_.as_pointer());
      mesh_submenu.separator();
      mesh_submenu.add( "Center on Geographic Position",
                        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
                        &bwm_tableau_rat_cam::center_pos));

      mesh_submenu.separator();
      mesh_submenu.add( "Save Adjusted Camera",
                        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
                        &bwm_tableau_rat_cam::save_adj_camera));
    }
    mesh_submenu.separator();
    mesh_submenu.add( "Toggle GeoPosition Display",
                      new vgui_command_simple<bwm_tableau_cam>(cam_tab,
                      &bwm_tableau_cam::show_geo_position));

    mesh_submenu.separator();
    mesh_submenu.add( "GeoPosition of Selected Vertex",
                      new vgui_command_simple<bwm_tableau_cam>(cam_tab,
                      &bwm_tableau_cam::geo_position_vertex));

    mesh_submenu.separator();
    mesh_submenu.add( "Local Position of Selected Vertex",
                      new vgui_command_simple<bwm_tableau_cam>(cam_tab,
                      &bwm_tableau_cam::local_position_vertex));

    // 3D Delete submenu
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
    vgui_menu cal_submenu;
    cal_submenu.add("Set Focal Length(pix)", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::set_focal_length));
    cal_submenu.add("Set Camera Height (m)", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::set_cam_height));
    cal_submenu.add("Define Horizon(selected line)", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::set_horizon));
    cal_submenu.add("Calibrate From Horizon", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::calibrate_cam_from_horizon));
    cal_submenu.add("Toggle Horizon", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::toggle_cam_horizon));
    cal_submenu.separator();
    //cal_submenu.add("Set Ground Plane", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::set_ground_plane));
    cal_submenu.add("Add Ground Plane", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::add_ground_plane));
    //cal_submenu.add("Set Sky", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::set_sky));
    cal_submenu.add("Add Sky", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::add_sky));
    //cal_submenu.add("Add Vertical Region", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::add_vertical_depth_region));
    cal_submenu.add("Add Region", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::add_region));
    cal_submenu.add("Edit Region Properties", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::edit_region_props));
    cal_submenu.add("Edit Region Weights", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::edit_region_weights));
    cal_submenu.add("Save Depth Map Scene", new vgui_command_simple<bwm_tableau_cam>(cam_tab, &bwm_tableau_cam::save_depth_map_scene));
    menu.add("Camera Calibration", cal_submenu);
  // Registration menu
    vgui_menu reg_submenu, threed_menu;
    menu.separator();

    menu.add( "Set as Master", new vgui_command_simple<bwm_tableau_cam>(cam_tab,&bwm_tableau_cam::set_master));

    if (gen_tab) {
      menu.separator();
      menu.add( "Ray image", new vgui_command_simple<bwm_tableau_generic_cam>(gen_tab,&bwm_tableau_generic_cam::ray_image));
    }
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

    // Correspondences menu
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
      lvcs_submenu.add( "Adjust Camera to World Point", new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
                        &bwm_tableau_rat_cam::adjust_camera_offset));
      lvcs_submenu.separator();
      lvcs_submenu.add( "Adjust Camera To LVCS",
                        new vgui_command_simple<bwm_tableau_rat_cam>(rat_cam_tab,
                        &bwm_tableau_rat_cam::adjust_camera_to_world_pt));
      menu.add("LVCS", lvcs_submenu);
    }
  }

  // video tableau specific menu items
  if (tab_->type_name().compare("bwm_tableau_video") == 0)
  {
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

    video_submenu.separator();

    video_submenu.add( "Save as Image List",
                       new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::save_as_image_list));

    menu.add("Video", video_submenu);
    menu.separator();

    vgui_menu video_corr_submenu;
    video_corr_submenu.add( "Attach a 3D point",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::set_world_pt));
    video_corr_submenu.separator();
    video_corr_submenu.add( "Add Match",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::add_match));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Add Match at Vertex",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,&bwm_tableau_video::add_match_at_vertex));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Remove Selected Corr Match",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::remove_selected_corr_match));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Remove Selected Corr ",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::remove_selected_corr));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Track Selected Corr ",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::set_selected_corr_for_tracking));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Stop Tracking Corr ",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::unset_selected_corr_for_tracking));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Display Corrs at Frame #",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::display_video_corrs));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Display Corrs on Current Frame",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::display_current_video_corrs));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Clear Corr Display",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::clear_video_corrs_display));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Toggle Correspondence Display (All Frames)",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::toggle_corr_display));
    video_corr_submenu.separator();
    video_corr_submenu.add( "Toggle World Point Display (if corrs displayed)",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::toggle_world_pt_display));
    video_corr_submenu.separator();
    video_corr_submenu.add( "Project Selected World Point",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::display_selected_world_pt));
    video_corr_submenu.separator();

    video_corr_submenu.add( "Extract World Plane",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::extract_world_plane));

    video_corr_submenu.separator();

    video_corr_submenu.add( "Extract Neighborhoods",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::extract_neighborhoods));

    video_corr_submenu.separator();

    video_corr_submenu.add( "Extract Histograms",
                            new vgui_command_simple<bwm_tableau_video>(video_tab,
                            &bwm_tableau_video::extract_histograms));
    menu.add("Video Correspondence", video_corr_submenu);
  }
}
