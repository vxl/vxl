#include "segv_vil_menus.h"
//:
// \file
#include "vgui/vgui.h"
#include "vgui/vgui_key.h"
#include "vgui/vgui_menu.h"
#include "segv_vil_segmentation_manager.h"

// Static menu callback functions

void segv_vil_menus::quit_callback()
{
  segv_vil_segmentation_manager::instance()->quit();
}


void segv_vil_menus::load_image_callback()
{
  segv_vil_segmentation_manager::instance()->load_image();
}

void segv_vil_menus::save_image_callback()
{
  segv_vil_segmentation_manager::instance()->save_image();
}

void segv_vil_menus::save_camera_callback()
{
  segv_vil_segmentation_manager::instance()->save_nitf_camera();
}

void segv_vil_menus::remove_image_callback()
{
  segv_vil_segmentation_manager::instance()->remove_image();
}


void segv_vil_menus::convert_to_grey_callback()
{
  segv_vil_segmentation_manager::instance()->convert_to_grey();
}

void segv_vil_menus::set_range_params_callback()
{
  segv_vil_segmentation_manager::instance()->set_range_params();
}

void segv_vil_menus::clear_display_callback()
{
  segv_vil_segmentation_manager::instance()->clear_display();
}

void segv_vil_menus::threshold_image_callback()
{
  segv_vil_segmentation_manager::instance()->threshold_image();
}

void segv_vil_menus::gradient_mag_angle_callback()
{
  segv_vil_segmentation_manager::instance()->gradient_mag_angle();
}

void segv_vil_menus::fft_callback()
{
  segv_vil_segmentation_manager::instance()->fft();
}

void segv_vil_menus::harris_corners_callback()
{
  segv_vil_segmentation_manager::instance()->harris_corners();
}

void segv_vil_menus::nonmaximal_suppression_callback()
{
  segv_vil_segmentation_manager::instance()->nonmaximal_suppression();
}

void segv_vil_menus::vd_edges_callback()
{
  segv_vil_segmentation_manager::instance()->vd_edges();
}

void segv_vil_menus::third_order_edges_callback()
{
  segv_vil_segmentation_manager::instance()->third_order_edges();
}

void segv_vil_menus::fit_lines_callback()
{
  segv_vil_segmentation_manager::instance()->fit_lines();
}

void segv_vil_menus::fit_conics_callback()
{
  segv_vil_segmentation_manager::instance()->fit_conics();
}

void segv_vil_menus::fit_overlay_conics_callback()
{
  segv_vil_segmentation_manager::instance()->fit_overlay_conics();
}

void segv_vil_menus::mser_conics_callback()
{
  segv_vil_segmentation_manager::instance()->mser_conics();
}


void segv_vil_menus::regions_callback()
{
  segv_vil_segmentation_manager::instance()->regions();
}

void segv_vil_menus::rotate_image_callback()
{
  segv_vil_segmentation_manager::instance()->rotate_image();
}

void segv_vil_menus::reduce_image_callback()
{
  segv_vil_segmentation_manager::instance()->reduce_image();
}

void segv_vil_menus::reduce_image_bicubic_callback()
{
  segv_vil_segmentation_manager::instance()->reduce_image_bicubic();
}

void segv_vil_menus::expand_image_callback()
{
  segv_vil_segmentation_manager::instance()->expand_image();
}

void segv_vil_menus::expand_image_bicubic_callback()
{
  segv_vil_segmentation_manager::instance()->expand_image_bicubic();
}

void segv_vil_menus::flip_image_lr_callback()
{
  segv_vil_segmentation_manager::instance()->flip_image_lr();
}

void segv_vil_menus::display_images_as_color_callback()
{
  segv_vil_segmentation_manager::instance()->display_images_as_color();
}

void segv_vil_menus::add_images_callback()
{
  segv_vil_segmentation_manager::instance()->add_images();
}

void segv_vil_menus::subtract_images_callback()
{
  segv_vil_segmentation_manager::instance()->subtract_images();
}

void segv_vil_menus::negate_image_callback()
{
  segv_vil_segmentation_manager::instance()->negate_image();
}

void segv_vil_menus::extrema_callback()
{
  segv_vil_segmentation_manager::instance()->extrema();
}

void segv_vil_menus::rot_extrema_callback()
{
  segv_vil_segmentation_manager::instance()->rot_extrema();
}

void segv_vil_menus::beaudet_callback()
{
  segv_vil_segmentation_manager::instance()->beaudet();
}

void segv_vil_menus::parallel_coverage_callback()
{
  segv_vil_segmentation_manager::instance()->parallel_coverage();
}

void segv_vil_menus::entropy_callback()
{
  segv_vil_segmentation_manager::instance()->entropy();
}

void segv_vil_menus::minfo_callback()
{
  segv_vil_segmentation_manager::instance()->minfo();
}

void segv_vil_menus::max_trace_scale_callback()
{
  segv_vil_segmentation_manager::instance()->max_trace_scale();
}

void segv_vil_menus::color_order_callback()
{
  segv_vil_segmentation_manager::instance()->color_order();
}

void segv_vil_menus::inline_viewer_callback()
{
  segv_vil_segmentation_manager::instance()->inline_viewer();
}

void segv_vil_menus::intensity_profile_callback()
{
  segv_vil_segmentation_manager::instance()->intensity_profile();
}

void segv_vil_menus::intensity_histogram_callback()
{
  segv_vil_segmentation_manager::instance()->intensity_histogram();
}

// JIM PROJECT POINTS
void segv_vil_menus::project_points_callback()
{
  segv_vil_segmentation_manager::instance()->project_points();
}

void segv_vil_menus::image_as_vrml_points_callback()
{
  segv_vil_segmentation_manager::instance()->image_as_vrml_points();
}

void segv_vil_menus::line_image_callback()
{
  segv_vil_segmentation_manager::instance()->draw_line_image();
}
void segv_vil_menus::display_roi_callback()
{
  segv_vil_segmentation_manager::instance()->display_roi();
}
void segv_vil_menus::create_roi_callback()
{
  segv_vil_segmentation_manager::instance()->create_roi();
}
void segv_vil_menus::crop_image_callback()
{
  segv_vil_segmentation_manager::instance()->crop_image();
}
void segv_vil_menus::gaussian_callback()
{
  segv_vil_segmentation_manager::instance()->gaussian();
}

void segv_vil_menus::abs_value_callback()
{
  segv_vil_segmentation_manager::instance()->abs_value();
}

//segv_vil_menus definition
vgui_menu segv_vil_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menuops;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Save Image", save_image_callback);
  menufile.add( "Save NITF Camera", save_camera_callback);

  //view menu entries
  menuview.add("Remove Image", remove_image_callback);
  menuview.add("Clear Spatial Objects", clear_display_callback);
  menuview.add("Display ROI", display_roi_callback);
  menuview.add("Intensity Profile", intensity_profile_callback);
  menuview.add("Set Range Params", set_range_params_callback);
  menuview.add("Inline Histogram Viewer", inline_viewer_callback);
  menuview.add("Histogram Plot ", intensity_histogram_callback);
  menuview.add("RotateImage ", rotate_image_callback);
  menuview.add("ReduceImage(Burt)", reduce_image_callback);
  menuview.add("ReduceImage(bicubic)", reduce_image_bicubic_callback);
  menuview.add("ExpandImage(Burt) ", expand_image_callback);
  menuview.add("ExpandImage(bicubic) ", expand_image_bicubic_callback);
  menuview.add("Flip Image (LR)", flip_image_lr_callback);
  menuview.add("Images as Color", display_images_as_color_callback);
  menuview.add("Project Points RPC", project_points_callback);
  menuview.add("Image as VRML Points", image_as_vrml_points_callback);

  //edit menu entries

  menuedit.add("Threshold", threshold_image_callback);
  menuedit.add("Harris", harris_corners_callback);
  menuedit.add("Beaudet", beaudet_callback);
  menuedit.add("Parallel Coverage", parallel_coverage_callback);

  menuedit.add("VD Edges", vd_edges_callback);
  menuedit.add("Third Order Edges", third_order_edges_callback);
  menuedit.add("Line Segments", fit_lines_callback);
  menuedit.add("Conic Segments", fit_conics_callback);
  menuedit.add("Overlay Conic Segments", fit_overlay_conics_callback);
  menuedit.add("MSER Conic Regions", mser_conics_callback);
  menuedit.add("Edgel Regions", regions_callback);
  menuedit.add("Non-Maximal Suppression", nonmaximal_suppression_callback);
  menuedit.add("Load Line Image", line_image_callback);

  //operation menu entries
  menuops.add("Create ROI:" , create_roi_callback);
  menuops.add("Convert To Greyscale:", convert_to_grey_callback);
  menuops.add("Crop Image:", crop_image_callback);
  menuops.add("Add Images:", add_images_callback);
  menuops.add("Subtract Images:", subtract_images_callback);
  menuops.add("Negate Image:", negate_image_callback);
  menuops.add("Gaussian Smoothing:", gaussian_callback);
  menuops.add("Absolute Value:", abs_value_callback);
  menuops.add("Entropy", entropy_callback);
  menuops.add("Mutal Information", minfo_callback);
  menuops.add("Max Trace Scale",  max_trace_scale_callback);
  menuops.add("Color Order",  color_order_callback);
  menuops.add("Extrema", extrema_callback);
  menuops.add("Rotational Extrema", rot_extrema_callback);
  menuops.add("GradMagAngle", gradient_mag_angle_callback);
  menuops.add("FFT", fft_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Image Ops", menuops);
  return menubar;
}
