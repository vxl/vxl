#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <bmvv/bmvv_recon_manager.h>
#include <bmvv/bmvv_recon_menus.h>

//Static munu callback functions

void bmvv_recon_menus::quit_callback()
{
  bmvv_recon_manager::instance()->quit();
}


void bmvv_recon_menus::load_image_callback()
{
  bmvv_recon_manager::instance()->load_image();
}

void bmvv_recon_menus::clear_display_callback()
{
  bmvv_recon_manager::instance()->clear_display();
}

void bmvv_recon_menus::clear_selected_callback()
{
  bmvv_recon_manager::instance()->clear_selected();
}

void bmvv_recon_menus::read_3d_points_callback()
{
  bmvv_recon_manager::instance()->read_3d_points();
}

void bmvv_recon_menus::initial_model_projection_callback()
{
  bmvv_recon_manager::instance()->initial_model_projection();
}

void bmvv_recon_menus::model_projection_callback()
{
  bmvv_recon_manager::instance()->model_projection();
}

void bmvv_recon_menus::print_selected_corr_callback()
{
  bmvv_recon_manager::instance()->print_selected_corr();
}

void bmvv_recon_menus::pick_corr_callback()
{
  bmvv_recon_manager::instance()->pick_corr();
}

void bmvv_recon_menus::write_corrs_callback()
{
  bmvv_recon_manager::instance()->write_corrs();
}

void bmvv_recon_menus::read_corrs_callback()
{
  bmvv_recon_manager::instance()->read_corrs();
}

void bmvv_recon_menus::compute_homographies_callback()
{
  bmvv_recon_manager::instance()->compute_homographies();
}

void bmvv_recon_menus::write_homographies_callback()
{
  bmvv_recon_manager::instance()->write_homographies();
}

void bmvv_recon_menus::read_homographies_callback()
{
  bmvv_recon_manager::instance()->read_homographies();
}

void bmvv_recon_menus::project_image_callback()
{
  bmvv_recon_manager::instance()->project_image();
}

void bmvv_recon_menus::set_images_callback()
{
  bmvv_recon_manager::instance()->set_images();
}

void bmvv_recon_menus::compute_harris_corners_callback()
{
  bmvv_recon_manager::instance()->compute_harris_corners();
}

void bmvv_recon_menus::overlapping_projections_callback()
{
  bmvv_recon_manager::instance()->overlapping_projections();
}

void bmvv_recon_menus::overlapping_projections_z_callback()
{
  bmvv_recon_manager::instance()->overlapping_projections_z();
}

void bmvv_recon_menus::overlapping_harris_proj_z_callback()
{
  bmvv_recon_manager::instance()->overlapping_harris_proj_z();
}

void bmvv_recon_menus::cross_correlate_plane_callback()
{
  bmvv_recon_manager::instance()->cross_correlate_plane();
}

void bmvv_recon_menus::cross_correlate_z_callback()
{
  bmvv_recon_manager::instance()->cross_correlate_z();
}

void bmvv_recon_menus::cross_correlate_harris_z_callback()
{
  bmvv_recon_manager::instance()->cross_correlate_harris_z();
}

void bmvv_recon_menus::depth_image_callback()
{
  bmvv_recon_manager::instance()->depth_image();
}

void bmvv_recon_menus::harris_depth_match_callback()
{
  bmvv_recon_manager::instance()->harris_depth_match();
}

void bmvv_recon_menus::z_corr_image_callback()
{
  bmvv_recon_manager::instance()->z_corr_image();
}

void bmvv_recon_menus::corr_plot_callback()
{
  bmvv_recon_manager::instance()->corr_plot();
}

void bmvv_recon_menus::map_point_callback()
{
  bmvv_recon_manager::instance()->map_point();
}

void bmvv_recon_menus::map_image_callback()
{
  bmvv_recon_manager::instance()->map_image();
}

void bmvv_recon_menus::map_harris_corners_callback()
{
  bmvv_recon_manager::instance()->map_harris_corners();
}

void bmvv_recon_menus::match_harris_corners_callback()
{
  bmvv_recon_manager::instance()->match_harris_corners();
}

void bmvv_recon_menus::harris_sweep_callback()
{
  bmvv_recon_manager::instance()->harris_sweep();
}

void bmvv_recon_menus::display_matched_corners_callback()
{
  bmvv_recon_manager::instance()->display_matched_corners();
}

void bmvv_recon_menus::display_harris_3d_callback()
{
  bmvv_recon_manager::instance()->display_harris_3d();
}

void bmvv_recon_menus::write_points_vrml_callback()
{
  bmvv_recon_manager::instance()->write_points_vrml();
}

void bmvv_recon_menus::read_points_vrml_callback()
{
  bmvv_recon_manager::instance()->read_points_vrml();
}

void bmvv_recon_menus::read_change_data_callback()
{
  bmvv_recon_manager::instance()->read_change_data();
}

void bmvv_recon_menus::write_volumes_vrml_callback()
{
  bmvv_recon_manager::instance()->write_volumes_vrml();
}

void bmvv_recon_menus::write_change_volumes_vrml_callback()
{
  bmvv_recon_manager::instance()->write_change_volumes_vrml();
}

void bmvv_recon_menus::compute_change_callback()
{
  bmvv_recon_manager::instance()->compute_change();
}


//bmvv_recon_menus definition
vgui_menu bmvv_recon_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menudebug;
  //file menu entries
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Read 3d Points", read_3d_points_callback);
  menufile.add( "Write Correspondences", write_corrs_callback);
  menufile.add( "Read Correspondences", read_corrs_callback);
  menufile.add( "Write Homographies", write_homographies_callback);
  menufile.add( "Read Homographies", read_homographies_callback);
  menufile.add( "Write Points (VRML)", write_points_vrml_callback);
  menufile.add( "Read Points (VRML)", read_points_vrml_callback);
  menufile.add( "Write Volumes (VRML)", write_volumes_vrml_callback);
  menufile.add( "Read Change Data (VRML)", read_change_data_callback);
  menufile.add( "Write Change Volumes (VRML)", write_change_volumes_vrml_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add("Clear Selected", clear_selected_callback);
  menuview.add("Initial Model Projection", initial_model_projection_callback);
  menuview.add("Model Projection", model_projection_callback);


  //edit menu entries
  menuedit.add("Selected Corr", print_selected_corr_callback);
  menuedit.add("Pick Corr", pick_corr_callback, (vgui_key)'p', vgui_CTRL);
  menuedit.add("Compute Homographies", compute_homographies_callback);
  menuedit.add("Set Images", set_images_callback);
  menuedit.add("Compute Harris Corners", compute_harris_corners_callback);
  menuedit.add("Depth Image", depth_image_callback);
  menuedit.add("Harris Depth Match", harris_depth_match_callback);
  menuedit.add("Compute Change", compute_change_callback);

  //debug menus entries
  menudebug.add("Image Projection", project_image_callback);
  menudebug.add("Overlapping Projections", overlapping_projections_callback);
  menudebug.add("Overlapping Projections at Z", 
                overlapping_projections_z_callback);
  menudebug.add("Overlapping Harris Projections at Z", 
                overlapping_harris_proj_z_callback);
  menudebug.add("CrossCorrelate On Plane", cross_correlate_plane_callback);
  menudebug.add("CrossCorrelate At Z", cross_correlate_z_callback);
  menudebug.add("CrossCorrelate Harris Corners At Z",
                cross_correlate_harris_z_callback);
  menudebug.add("Base Correlation Image at Z Index",z_corr_image_callback);
  menudebug.add("Correlation vs z at Picked Point",corr_plot_callback);
  menudebug.add("Map a Picked Point", map_point_callback);
  menudebug.add("Map the Selected Image", map_image_callback);
  menudebug.add("Map Harris Corners", map_harris_corners_callback);
  menudebug.add("Match Harris Corners", match_harris_corners_callback);
  menudebug.add("Harris Sweep", harris_sweep_callback);
  menudebug.add("Matched Corners at Z", display_matched_corners_callback);
  menudebug.add("Display Harris 3d", display_harris_3d_callback);


  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Debug", menudebug);
  return menubar;
}

