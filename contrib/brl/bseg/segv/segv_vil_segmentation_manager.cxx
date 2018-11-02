// This is brl/bseg/segv/segv_vil_segmentation_manager.cxx
#include <iostream>
#include <cstdio>
#include <fstream>
#include "segv_vil_segmentation_manager.h"
//:
// \file
// \author J.L. Mundy

#if 1 //JLM
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#endif
// include for project points menu option
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_copy.h>
#include <vil/vil_property.h>
#include <vil/vil_flip.h>
#include <vil/vil_resample_bicub.h>
#include <vil/vil_convert.h>
#include <vil/vil_config.h>
#if HAS_J2K
#include <vil/file_formats/vil_j2k_nitf2_pyramid_image_resource.h>
#include <vil/file_formats/vil_j2k_pyramid_image_resource.h>
#endif // HAS_J2K
#include <vil/algo/vil_sobel_1x3.h>
#include <vgl/vgl_vector_2d.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_harris_detector_params.h>
#include <sdet/sdet_harris_detector.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_fit_lines.h>
#include <sdet/sdet_fit_conics.h>
#include <sdet/sdet_grid_finder_params.h>
#include <sdet/sdet_grid_finder.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_range_adjuster_tableau.h>
#include <bgui/bgui_image_utils.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_intensity_face.h>
#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_para_cvrg_params.h>
#include <brip/brip_roi.h>
#include <brip/brip_para_cvrg.h>
#include <brip/brip_watershed_params.h>
#include <brip/brip_max_scale_response.h>
#include <sdet/sdet_vrml_display.h>
#include <sdet/sdet_watershed_region_proc_params.h>
#include <sdet/sdet_watershed_region_proc.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <sdet/sdet_third_order_edge_det_params.h>
#include <sdet/sdet_third_order_edge_det.h>
segv_vil_segmentation_manager *segv_vil_segmentation_manager::instance_ = nullptr;

segv_vil_segmentation_manager *segv_vil_segmentation_manager::instance()
{
  if (!instance_)
  {
    instance_ = new segv_vil_segmentation_manager();
    instance_->init();
  }
  return segv_vil_segmentation_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
segv_vil_segmentation_manager::segv_vil_segmentation_manager():vgui_wrapper_tableau()
{
  first_ = true;
}

segv_vil_segmentation_manager::~segv_vil_segmentation_manager()
{
}

//: Set up the tableaux
void segv_vil_segmentation_manager::init()
{
  bgui_image_tableau_sptr itab = bgui_image_tableau_new();
  bgui_vtol2D_tableau_sptr t2D = bgui_vtol2D_tableau_new(itab);
  bgui_picker_tableau_sptr picktab = bgui_picker_tableau_new(t2D);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(picktab);
  grid_ = vgui_grid_tableau_new(1,1);
  grid_->set_grid_size_changeable(true);
  grid_->add_at(v2D, 0, 0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);
  first_ = true;
}

//: Calculate the range parameters for the input image
vgui_range_map_params_sptr segv_vil_segmentation_manager::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;

  //Check if the image is blocked
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (bir)
  { gl_map = true; cache = true;}

  //Check if the image is a pyramid
  bool pyr = image->get_property(vil_property_pyramid, nullptr);
  if (pyr)
  { gl_map = true; cache = true;}

  bgui_image_utils iu(image);
  iu.set_percent_limit(0.001);

  vgui_range_map_params_sptr rmps;
  if (iu.range_map_from_hist((float)gamma, invert, gl_map, cache, rmps))
    return rmps;
  if (iu.default_range_map(rmps, gamma, invert, gl_map, cache))
    return rmps;
  return nullptr;
}

//: set the image at the currently selected grid cell
void segv_vil_segmentation_manager::
set_selected_grid_image(vil_image_resource_sptr const& image,
                        vgui_range_map_params_sptr const& rmps)
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    this->add_image(image, rmps);
  else
  {
    itab->set_image_resource(image);
    itab->set_mapping(rmps);
  }
  itab->post_redraw();
}

//: Add an image at the specified grid cell
void segv_vil_segmentation_manager::
add_image_at(vil_image_resource_sptr const& image,
             const unsigned col, const unsigned row,
             vgui_range_map_params_sptr const& rmps)
{
  vgui_range_map_params_sptr rmap = rmps;
  if (!rmps)
    rmap = range_params(image);
  bgui_image_tableau_sptr itab = bgui_image_tableau_new(image,rmps);
  //  itab->set_mapping(rmap);
  bgui_vtol2D_tableau_sptr t2D = bgui_vtol2D_tableau_new(itab);
  bgui_picker_tableau_sptr picktab = bgui_picker_tableau_new(t2D);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(picktab);
  grid_->add_at(v2D, col, row);
  itab->post_redraw();
}

//: Add an image to the currently selected grid cell
void segv_vil_segmentation_manager::
add_image(vil_image_resource_sptr const& image,
          vgui_range_map_params_sptr const& rmps)
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  this->add_image_at(image, col, row, rmps);
}

//-----------------------------------------------------------------------------
//: remove the selected image
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::remove_image()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  grid_->remove_at(col, row);
}

void segv_vil_segmentation_manager::convert_to_grey()
{
  vil_image_resource_sptr img = this->selected_image();
  if (!img)
    return;
  vil_image_view<unsigned char> grey =
    brip_vil_float_ops::convert_to_byte(img);
  vil_image_resource_sptr gimg = vil_new_image_resource_of_view(grey);
  this->add_image(gimg);
}

//: Get the image tableau for the currently selected grid cell
bgui_image_tableau_sptr segv_vil_segmentation_manager::selected_image_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_image_tableau_sptr itab;
    itab.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    std::string("vgui_image_tableau")));
    if (itab)
      return itab;
  }
  std::cout << "Unable to get bgui_image_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_image_tableau_sptr();
}

//: Get the vtol2D tableau at the specified grid cell
bgui_vtol2D_tableau_sptr
segv_vil_segmentation_manager::vtol2D_tab_at(const unsigned col,
                                             const unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_vtol2D_tableau_sptr v2D;
    v2D.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                   std::string("bgui_vtol2D_tableau")));
    if (v2D)
      return v2D;
  }
  std::cout << "Unable to get bgui_vtol2D_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_vtol2D_tableau_sptr();
}

//: Get the vtol2D tableau for the currently selected grid cell
bgui_vtol2D_tableau_sptr segv_vil_segmentation_manager::selected_vtol2D_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->vtol2D_tab_at(col, row);
}

//: Get the picker tableau for the currently selected grid cell
bgui_picker_tableau_sptr segv_vil_segmentation_manager::selected_picker_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_picker_tableau_sptr pick;
    pick.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    std::string("bgui_picker_tableau")));
    if (pick)
      return pick;
  }
  std::cout << "Unable to get bgui_picker_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_picker_tableau_sptr();
}


vil_image_resource_sptr segv_vil_segmentation_manager::selected_image()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    return nullptr;
  return itab->get_image_resource();
}

vil_image_resource_sptr segv_vil_segmentation_manager::image_at(const unsigned col,
                                                                const unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (!top_tab)
    return nullptr;

  bgui_image_tableau_sptr itab;
  itab.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                  std::string("vgui_image_tableau")));
  if (!itab)
  {
    std::cout << "Unable to get bgui_image_tableau at (" << col
             << ", " << row << ")\n";
    return nullptr;
  }
  return itab->get_image_resource();
}

//-----------------------------------------------------------------------------
//: Clear spatial objects from the selected display
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::clear_display()
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
}

//-----------------------------------------------------------------------------
//: Clear spatial objects from all spatial panes
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::clear_all()
{
  unsigned ncols = grid_->cols(), nrows = grid_->rows();
  for (unsigned r=0; r<nrows; ++r)
    for (unsigned c=0; c<ncols; ++c)
    {
      bgui_vtol2D_tableau_sptr t = this->vtol2D_tab_at(c, r);
      if (t)
        t->clear_all();
    }
}

//-----------------------------------------------------------------------------
//: Draw edges onto the tableau
//-----------------------------------------------------------------------------
void
segv_vil_segmentation_manager::draw_edges(std::vector<vtol_edge_2d_sptr>& edges,
                                          bool verts)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    std::cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (std::vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    t2D->add_edge(*eit);
    //optionally display the edge vertices
    if (verts)
    {
      if ((*eit)->v1())
      {
        vtol_vertex_2d_sptr v1 = (*eit)->v1()->cast_to_vertex_2d();
        t2D->add_vertex(v1);
      }
      if ((*eit)->v2())
      {
        vtol_vertex_2d_sptr v2 = (*eit)->v2()->cast_to_vertex_2d();
        t2D->add_vertex(v2);
      }
    }
  }
  t2D->post_redraw();
}


//-----------------------------------------------------------------------------
//: Draw polylines on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_polylines(std::vector<vsol_polyline_2d_sptr > const& polys)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    std::cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (std::vector<vsol_polyline_2d_sptr>::const_iterator pit = polys.begin();
       pit != polys.end(); pit++)
  {
    t2D->add_vsol_polyline_2d(*pit);
  }

  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw line segments on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_lines(std::vector<vsol_line_2d_sptr > const& lines,
           const vgui_style_sptr& style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    std::cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (std::vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    t2D->add_vsol_line_2d(*lit,style);
  }

  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw conic segments on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_conics(std::vector<vsol_conic_2d_sptr > const& conics,
            const vgui_style_sptr& style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    std::cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (std::vector<vsol_conic_2d_sptr>::const_iterator lit = conics.begin();
       lit != conics.end(); lit++)
  {
    t2D->add_vsol_conic_2d(*lit,style);
  }
  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw points on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_points(std::vector<vsol_point_2d_sptr> const& points, const vgui_style_sptr& style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    std::cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (std::vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
  {
    t2D->add_vsol_point_2d(*pit,style);
  }

  t2D->post_redraw();
}

void segv_vil_segmentation_manager::draw_regions(std::vector<vtol_intensity_face_sptr>& regions,
                                                 bool verts)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  for (std::vector<vtol_intensity_face_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
  {
    vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
    t2D->add_face(f);
    if (verts)
    {
      std::vector<vtol_vertex_sptr> vts;
      f->vertices(vts);
      for (std::vector<vtol_vertex_sptr>::iterator vit = vts.begin();
           vit != vts.end(); vit++)
      {
        vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
        t2D->add_vertex(v);
      }
    }
  }
  t2D->post_redraw();
}

void segv_vil_segmentation_manager::quit()
{
  this->clear_all();
  vgui::quit();
}

void segv_vil_segmentation_manager::load_image()
{
  static bool greyscale = false;
  static bool sblock = false;
  vgui_dialog load_image_dlg("Load image file");
  static std::string image_filename = "/home/dec/images/cal_image1.tif";
  static std::string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  load_image_dlg.checkbox("blocked?:", sblock);
  if (!load_image_dlg.ask())
    return;
  //first check to see if the image is a pyramid
  bool pyrm = false;
  vil_image_resource_sptr image;
  vil_pyramid_image_resource_sptr pyr =
      vil_load_pyramid_resource(image_filename.c_str(), false);
  if (pyr)
  {
    image = pyr.ptr();
    pyrm = true;
  }

  if (!image)
    image = vil_load_image_resource(image_filename.c_str(), false);

  if (!image) {
    std::cout << "Failed to load image path " << image_filename << '\n';
    return;
  }
#if HAS_J2K
  // determine if the image can be made into a J2K-nitf pyramid
  char const* fmtp = image->file_format();
  std::string file_fmt = "";
  if (fmtp) file_fmt = fmtp;//fmtp can be 0 for undefined formats
  if (file_fmt == "nitf21")
  {
    vil_nitf2_image* nitf_resc = static_cast<vil_nitf2_image*>(image.ptr());
    if (nitf_resc->is_jpeg_2000_compressed())
    {
      vil_j2k_nitf2_pyramid_image_resource* j2k_nitf =
        new vil_j2k_nitf2_pyramid_image_resource(image);
      image = j2k_nitf;
      pyrm = true;
    }
  }
  else if (file_fmt == "j2k") {
    vil_j2k_pyramid_image_resource* j2k_pyr =
      new vil_j2k_pyramid_image_resource(image);
    image = j2k_pyr;
    pyrm = true;
  }
#endif //HAS_J2K
  if (greyscale&&!pyrm)
  {
    vil_image_view<unsigned char> grey_view =
      brip_vil_float_ops::convert_to_grey(*image);
    image = vil_new_image_resource_of_view(grey_view);
  }

  if (sblock&&!pyrm)
  {
    vil_blocked_image_resource_sptr bimage = vil_new_blocked_image_facade(image);
    image = (vil_image_resource*)(vil_new_cached_image_resource(bimage)).ptr();
  }

  vgui_range_map_params_sptr rmps = range_params(image);

  if (first_)
  {
    this->set_selected_grid_image(image, rmps);
    first_ = false;
  }
  else
    this->add_image(image, rmps);
}

void segv_vil_segmentation_manager::load_image_nomenu(std::string const& path)
{
  bool pyrm = false;
  vil_image_resource_sptr image;
  vil_pyramid_image_resource_sptr pyr =
      vil_load_pyramid_resource(path.c_str(), false);
  if (pyr)
  {
    image = pyr.ptr();
    pyrm = true;
  }

  if (!image)
    image = vil_load_image_resource(path.c_str(), false);

  if (!image) {
    std::cout << "Failed to load image path " << path << '\n';
    return;
  }
#if HAS_J2K
// determine if the image can be made into a J2K-nitf pyramid
  char const* fmtp = image->file_format();
  std::string file_fmt = "";
  if (fmtp) file_fmt = fmtp;//fmtp can be 0 for undefined formats
  if (file_fmt == "nitf21")
  {
    vil_nitf2_image* nitf_resc = static_cast<vil_nitf2_image*>(image.ptr());
    if (nitf_resc->is_jpeg_2000_compressed())
    {
      vil_j2k_nitf2_pyramid_image_resource* j2k_nitf =
        new vil_j2k_nitf2_pyramid_image_resource(image);
      image = j2k_nitf;
      pyrm = true;
    }
  }
  else if (file_fmt == "j2k") {
    vil_j2k_pyramid_image_resource* j2k_pyr =
      new vil_j2k_pyramid_image_resource(image);
    image = j2k_pyr;
    pyrm = true;
  }
#endif //HAS_J2K
  vgui_range_map_params_sptr rmps = range_params(image);

  if (first_)
  {
    this->set_selected_grid_image(image, rmps);
    first_ = false;
  }
  else
    this->add_image(image, rmps);
}

void segv_vil_segmentation_manager::save_image()
{
  vgui_dialog file_dialog("Save Image");
  static std::string image_file;
  static std::string ext = "tif";
  static std::string type = "tiff";
  static unsigned size_block = 0;
  static bool byte = false;
  file_dialog.file("Image Filename:", ext, image_file);
  file_dialog.field("Image Format: ", type);
  file_dialog.field("BlockSize", size_block);
  file_dialog.checkbox("Convert to byte image", byte);
  if (!file_dialog.ask())
    return;
  vil_image_resource_sptr img = this->selected_image();
  if (!img)
  {
    std::cerr << "Null image in segv_vil_segmentation_manager::save_image\n";
    return;
  }
  vil_image_resource_sptr save_image = img;
  if (byte)
  {
    vil_image_view<unsigned char> byte_view = brip_vil_float_ops::convert_to_byte(img);
    save_image = vil_new_image_resource_of_view(byte_view);
  }
  if (size_block>0)
  {
    vil_blocked_image_resource_sptr bim =
      vil_new_blocked_image_resource(image_file.c_str(),
                                     save_image->ni(), save_image->nj(),
                                     save_image->nplanes(),
                                     save_image->pixel_format(),
                                     size_block, size_block,
                                     "tiff");
    vil_image_view_base_sptr view = save_image->get_view();
    if (view)
      bim->vil_image_resource::put_view(*view);
    return;
  }

  if (!vil_save_image_resource(save_image, image_file.c_str(), type.c_str()))
    std::cerr << "segv_vil_segmentation_manager::save_image operation failed\n";
}

void segv_vil_segmentation_manager::save_nitf_camera()
{
  vil_image_resource_sptr img = this->selected_image();
  if (!img)
  {
    std::cerr << "Null image in segv_vil_segmentation_manager::save_camera\n";
    return;
  }

  vil_nitf2_image* nitf = nullptr;
  std::string format = img->file_format();
  std::string prefix = format.substr(0,4);
  if (prefix == "nitf") {
    nitf = (vil_nitf2_image*)img.ptr();
    vgui_dialog file_dialog("Save NITF Camera");
    static std::string image_file;
    static std::string ext = "rpc";
    file_dialog.file("Image Filename:", ext, image_file);
    if (!file_dialog.ask())
      return;
    vpgl_nitf_rational_camera rpcam(nitf, true);
    rpcam.save(image_file);
  }
}

void segv_vil_segmentation_manager::set_range_params()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    return;
  vgui_range_map_params_sptr rmps = itab->map_params();
  if (!rmps)
  {
    vil_image_resource_sptr img = itab->get_image_resource();
    if (!img)
      return;
    rmps = range_params(img);
    if (!rmps)
      return;
  }
  unsigned nc = rmps->n_components_;
  static double min = static_cast<double>(rmps->min_L_),
    max = static_cast<double>(rmps->max_L_);
  static float gamma = rmps->gamma_L_;
  static bool invert = rmps->invert_;
  static bool gl_map = rmps->use_glPixelMap_;
  static bool cache = rmps->cache_mapped_pix_;
  if (nc==3)
  {
    min = static_cast<double>(rmps->min_R_);
    max = static_cast<double>(rmps->max_R_);
    gamma = rmps->gamma_R_;
  }
  vgui_dialog range_dlg("Set Range Map Params");
  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  range_dlg.checkbox("Use GL Mapping", gl_map);
  range_dlg.checkbox("Cache Pixels", cache);
  if (!range_dlg.ask())
    return;
  if (nc==1)
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  else if (nc == 3)
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  else
    rmps = nullptr;
  itab->set_mapping(rmps);
}

void segv_vil_segmentation_manager::threshold_image()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_segmentation_manager::threshold_image - no image\n";
    return;
  }
  static float thresh = 128.0f;
  vgui_dialog thresh_dlg("Threshold Image");
  thresh_dlg.field("Threshold", thresh);
  if (!thresh_dlg.ask())
    return;
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(*img);
  vil_image_view<float> timage =
    brip_vil_float_ops::threshold(fimage, thresh, 255);
  vil_image_view<unsigned char> cimage = brip_vil_float_ops::convert_to_byte(timage, 0, 255);
  this->add_image(vil_new_image_resource_of_view(cimage));
}

void segv_vil_segmentation_manager::harris_corners()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_segmentation_manager::harris_measure) - no image\n";
    return;
  }
  static sdet_harris_detector_params hdp;
  vgui_dialog harris_dialog("harris");
  harris_dialog.field("sigma", hdp.sigma_);
  harris_dialog.field("thresh", hdp.thresh_);
  harris_dialog.field("N = 2n+1, (n)", hdp.n_);
  harris_dialog.field("Max No Corners(percent)", hdp.percent_corners_);
  harris_dialog.field("scale_factor", hdp.scale_factor_);
  harris_dialog.checkbox("Use vil harris\n corner strength ", hdp.use_vil_harris_);
  if (!harris_dialog.ask())
    return;
  sdet_harris_detector hd(hdp);
  hd.set_image_resource(img);
  hd.extract_corners();
  std::vector<vsol_point_2d_sptr>& points = hd.get_points();
  int N = points.size();
  if (!N)
    return;
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
  for (int i=0; i<N; i++)
    t2D->add_vsol_point_2d(points[i]);
  t2D->post_redraw();
}

void segv_vil_segmentation_manager::nonmaximal_suppression()
{
  bool show_lines = false;
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_segmentation_manager::nonmaximal_suppression) - no image\n";
    return;
  }
  static sdet_nonmax_suppression_params nsp;
  vgui_dialog nonmax_dialog("non-maximal suppression");
  nonmax_dialog.field("Gradient Magnitude Percentage Threshold ", nsp.thresh_);
  nonmax_dialog.choice("Number of Points Used for Parabola Fit ", "3", "9", nsp.pfit_type_);
  nonmax_dialog.checkbox("Show lines at the edge points? ", show_lines);
  if (!nonmax_dialog.ask())
    return;

  // prepare input for the nonmax suppression
  vil_image_view<vxl_byte> input, input_grey;
  vil_image_view<double> grad_i, grad_j, grad_mag_img;
  vbl_array_2d<double> grad_x, grad_y, grad_mag;
  vbl_array_2d<vgl_vector_2d <double> > input_directions;

  // vil_convert_cast(img->get_view(), input);
  switch ( img->get_view()->pixel_format() )
  {
#define macro(F , T) \
    case F: vil_convert_cast( vil_image_view<T >(img->get_view()), input ); break;
    macro( VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32 )
    macro( VIL_PIXEL_FORMAT_INT_32, vxl_int_32 )
    macro( VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16 )
    macro( VIL_PIXEL_FORMAT_INT_16, vxl_int_16 )
    macro( VIL_PIXEL_FORMAT_BYTE, vxl_byte )
    macro( VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte )
    macro( VIL_PIXEL_FORMAT_FLOAT, float )
    macro( VIL_PIXEL_FORMAT_DOUBLE, double )
    macro( VIL_PIXEL_FORMAT_BOOL, bool )
    default: img->get_view() = nullptr;
#undef macro
  }

  int ni = input.ni();
  int nj = input.nj();

  grad_i.set_size(ni,nj);
  grad_j.set_size(ni,nj);
  grad_mag_img.set_size(ni,nj);
  input_grey.set_size(ni,nj);
  grad_x.resize(ni,nj);
  grad_y.resize(ni,nj);
  grad_mag.resize(ni,nj);
  input_directions.resize(ni,nj);

  if (input.nplanes() > 1)
    vil_convert_planes_to_grey(input, input_grey);
  else
    input_grey = input;

  vil_sobel_1x3 <vxl_byte, double> (input_grey, grad_i, grad_j);
  for (int j=0;j<nj; j++)
  {
    for (int i=0;i<ni; i++)
    {
      double xval = grad_i(i,j);
      double yval = grad_j(i,j);
      grad_x(i,j) = xval;
      grad_y(i,j) = yval;
      double val = std::sqrt(std::pow(xval,2.0) + std::pow(yval,2.0));
      grad_mag(i,j) = val;
      grad_mag_img(i,j) = val;
      vgl_vector_2d<double> dir(xval, yval);
      input_directions(i,j) = dir;
    }
  }

  // Below is to demonstrate how to initialize the non-maximal suppression in different ways
//  sdet_nonmax_suppression ns(nsp, grad_mag, input_directions);
  sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
//  sdet_nonmax_suppression ns(nsp, grad_i, grad_j);
//  sdet_nonmax_suppression ns(nsp, grad_mag_img, input_directions);
  ns.apply();
  std::vector<vsol_point_2d_sptr>& points = ns.get_points();
  std::vector<vsol_line_2d_sptr>& lines = ns.get_lines();
  // not used below, just for demonstration purposes
  //std::vector<vgl_vector_2d<double> >& directions = ns.get_directions();
  int N = points.size();
  if (!N)
    return;
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
  if (!show_lines)
  {
    for (int i=0; i<N; i++)
      t2D->add_vsol_point_2d(points[i]);
  }
  if (show_lines)
  {
    for (int i=0; i<N; i++)
      t2D->add_vsol_line_2d(lines[i]);
  }
  t2D->post_redraw();
}

void segv_vil_segmentation_manager::vd_edges()
{
  this->clear_display();
  static bool agr = true;
  static sdet_detector_params dp;
  static float nm = 2.0;

  vgui_dialog vd_dialog("VD Edges");
  vd_dialog.field("Gaussian sigma", dp.smooth);
  vd_dialog.field("Noise Threshold", nm);
  vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  vd_dialog.checkbox("Aggressive Closure", agr);
  vd_dialog.checkbox("Compute Junctions", dp.junctionp);
  vd_dialog.checkbox("DetectPeaks", dp.peaks_only);
  if (!vd_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }
  sdet_detector det(dp);
  det.SetImage(img);

  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (edges)
    this->draw_edges(*edges, true);
}

void segv_vil_segmentation_manager::third_order_edges()
{
  this->clear_display();
  static sdet_third_order_edge_det_params dp;
  vgui_dialog todr_dialog("Third Order Edges");
  todr_dialog.field("Sigma", dp.sigma_);
  todr_dialog.field("Threshold", dp.thresh_);
  todr_dialog.field("Parabola Type", dp.pfit_type_);
  todr_dialog.field("Gradient Oper", dp.grad_op_);
  todr_dialog.field("Convolution Algo", dp.conv_algo_);
  if (!todr_dialog.ask())
    return;
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::third_order_edges() - no image\n";
    return;
  }
  sdet_third_order_edge_det det(dp);
  det.apply(img->get_view());
  std::vector<vsol_line_2d_sptr> lines;
  det.line_segs(lines);
  if (lines.size())
    this->draw_lines(lines);
}

void segv_vil_segmentation_manager::fit_lines()
{
  this->clear_display();
  static sdet_detector_params dp;
  static bool agr = true;
  static float nm = 2.0;

  static sdet_fit_lines_params flp;

  vgui_dialog lf_dialog("Fit Lines");
  lf_dialog.field("Gaussian sigma", dp.smooth);
  lf_dialog.field("Noise Threshold", nm);
  lf_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  lf_dialog.checkbox("Aggressive Closure", agr);
  lf_dialog.checkbox("Compute Junctions", dp.junctionp);
  lf_dialog.field("Min Fit Length", flp.min_fit_length_);
  lf_dialog.field("RMS Distance", flp.rms_distance_);

  if (!lf_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  dp.borderp = false;
  sdet_detector det(dp);

  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }

  det.SetImage(img);

  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges)
  {
    std::cout << "No edges to fit lines\n";
    return;
  }
  sdet_fit_lines fl(flp);
  fl.set_edges(*edges);
  fl.fit_lines();
  std::vector<vsol_line_2d_sptr> lines = fl.get_line_segs();
  this->draw_lines(lines);
}

void segv_vil_segmentation_manager::fit_conics()
{
  this->clear_display();
  static sdet_detector_params dp;
  static bool agr = true;
  static float nm = 2.0;

  static sdet_fit_conics_params fcp;

  vgui_dialog lf_dialog("Fit Conics");
  lf_dialog.field("Gaussian sigma", dp.smooth);
  lf_dialog.field("Noise Threshold", nm);
  lf_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  lf_dialog.checkbox("Aggressive Closure", agr);
  lf_dialog.checkbox("Compute Junctions", dp.junctionp);
  lf_dialog.field("Min Fit Length", fcp.min_fit_length_);
  lf_dialog.field("RMS Distance", fcp.rms_distance_);

  if (!lf_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  dp.borderp = false;
  sdet_detector det(dp);

  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }

  det.SetImage(img);

  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges)
  {
    std::cout << "No edges to fit conics\n";
    return;
  }
  sdet_fit_conics fl(fcp);
  fl.set_edges(*edges);
  fl.fit_conics();
  std::vector<vsol_conic_2d_sptr> conics = fl.get_conic_segs();
  this->draw_conics(conics);

  std::vector<vsol_point_2d_sptr> center_points;
  double cx,cy,phi,width,height;
  // draw the center points of the conics
  for (unsigned int i=0; i<conics.size(); i++) {
    if (conics[i]->is_real_ellipse()) {
      conics[i]->ellipse_parameters(cx,cy,phi,width,height);
      vsol_point_2d_sptr p = new vsol_point_2d(cx, cy);
      std::cout << i << " center (" << cx << ',' << cy << ')' << std::endl;
      center_points.push_back(p);
    }
  }
  vgui_style_sptr style = vgui_style::new_style(1.0f,0.0f,0.0f,5.0f,1.0f);

  this->draw_points(center_points, style);
}

void segv_vil_segmentation_manager::fit_overlay_conics()
{
  //this->clear_display();
  static sdet_detector_params dp;
  static bool agr = true;
  static float nm = 2.0;

  static sdet_fit_conics_params fcp;

  vgui_dialog lf_dialog("Fit overlay Conics");
  static std::string image_filename = "/home/dec/images/cal_image1.tif";
  static std::string ext = "*.*";
  lf_dialog.file("Image Filename:", ext, image_filename);
  lf_dialog.field("Gaussian sigma", dp.smooth);
  lf_dialog.field("Noise Threshold", nm);
  lf_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  lf_dialog.checkbox("Aggressive Closure", agr);
  lf_dialog.checkbox("Compute Junctions", dp.junctionp);
  lf_dialog.field("Min Fit Length", fcp.min_fit_length_);
  lf_dialog.field("RMS Distance", fcp.rms_distance_);

  if (!lf_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  dp.borderp = false;
  sdet_detector det(dp);

  vil_image_resource_sptr img = vil_load_image_resource(image_filename.c_str());
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }

  det.SetImage(img);

  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges)
  {
    std::cout << "No edges to fit conics\n";
    return;
  }
  sdet_fit_conics fl(fcp);
  fl.set_edges(*edges);
  fl.fit_conics();
  std::vector<vsol_conic_2d_sptr> conics = fl.get_conic_segs();
  vgui_style_sptr style = vgui_style::new_style(1.0f,1.0f,0.0f,5.0f,1.0f);
  this->draw_conics(conics, style);

  std::vector<vsol_point_2d_sptr> center_points;
  double cx,cy,phi,width,height;
  // draw the center points of the conics
  for (unsigned int i=0; i<conics.size(); i++) {
    if (conics[i]->is_real_ellipse()) {
      conics[i]->ellipse_parameters(cx,cy,phi,width,height);
      vsol_point_2d_sptr p = new vsol_point_2d(cx, cy);
      std::cout << i << " center (" << cx << ',' << cy << ')' << std::endl;
      center_points.push_back(p);
    }
  }
  vgui_style_sptr style2 = vgui_style::new_style(1.0f,0.0f,1.0f,5.0f,1.0f);

  this->draw_points(center_points, style2);
}

// ####################################################################
// Added by J. Green to project 3D points into image using rpc camera

void segv_vil_segmentation_manager::project_points()
{
  this->clear_display(); // apparently this call is needed?
  vil_image_resource_sptr img = this->selected_image();
  vil_nitf2_image* nitf = nullptr;
  std::string format = img->file_format();
  std::string prefix = format.substr(0,4);
  if (prefix == "nitf")
    nitf = (vil_nitf2_image*)img.ptr();
  else
  {
    //Check if the image is a pyramid
    bool pyr = img->get_property(vil_property_pyramid, nullptr);
    if (!pyr)
    {
      std::cout << "Current image is not a NITF image\n";
      return;
    }
    //Get the base image
    vil_pyramid_image_resource* pimage =
      (vil_pyramid_image_resource*)img.ptr();
    vil_image_resource_sptr base = pimage->get_resource(0);
    format = base->file_format();
    if (format == "nitf" || format =="nitf20" )
      nitf = (vil_nitf2_image*)base.ptr();
    else
    {
      std::cout << "Current image is not a NITF image\n";
      return;
    }
  }
  //cast to an nitf2_image

  static double lat=32.722;  // Latitude
  static double lon=-117.15; // Longitude
  static double elev=43;     // Elevation

  vgui_dialog lf_dialog("Project Points");
  lf_dialog.field("Latitude", lat);
  lf_dialog.field("Longitude", lon);
  lf_dialog.field("Elevation", elev);

  if (!lf_dialog.ask())
  {
    std::cerr << "In project_points() dialog failed; returning.\n";
    return;
  }
  // calculate point location (x1, y1) for 1st camera
  double u = 0;
  double v = 0;
  vpgl_nitf_rational_camera rpcam(nitf, true);
  rpcam.project(lon, lat, elev, u,  v);
  std::cout << " camera projects to <" << u << ", " << v << '>' << std::endl;
  std::vector<vsol_point_2d_sptr> points;
  vsol_point_2d_sptr p1 = new vsol_point_2d(u, v);
  points.push_back(p1);
  vgui_style_sptr style1 = vgui_style::new_style(1.0f,0.0f,0.0f,10.0f,1.0f);   // first style, red
  // draw point
  this->draw_points(points, style1);
}  // end of project_points method

void segv_vil_segmentation_manager::regions()
{
  this->clear_display();
  static bool debug = false;
  static bool agr = true;
  static bool residual = false;
  static sdet_detector_params dp;
  static float nm = 1.0;
  vgui_dialog region_dialog("Edgel Regions");
  region_dialog.field("Gaussian sigma", dp.smooth);
  region_dialog.field("Noise Threshold", nm);
  region_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  region_dialog.checkbox("Aggressive Closure", agr);
  region_dialog.checkbox("Compute Junctions", dp.junctionp);
  region_dialog.checkbox("Debug", debug);
  region_dialog.checkbox("Residual Image", residual);
  if (!region_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }

  sdet_region_proc_params rpp(dp, true, debug, 2);
  sdet_region_proc rp(rpp);
  rp.set_image_resource(img);
  rp.extract_regions();
  if (debug)
  {
    vil1_image ed_img = rp.get_edge_image();
#if 0
    vgui_image_tableau_sptr itab =  t2D->get_image_tableau();
    if (!itab)
    {
      std::cout << "In segv_vil_segmentation_manager::regions() - null image tableau\n";
      return;
    }
    itab->set_image(ed_img);
    itab->post_redraw();
#endif
//    this->add_image(ed_img);
  }
  if (!debug)
  {
    std::vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
    this->draw_regions(regions, true);
  }
  if (residual)
  {
    vil_image_view<float> res_img = rp.get_residual_image_view();
    if (!res_img)
      return;
    this->add_image(vil_new_image_resource_of_view(res_img));
  }
}

//: Show combined greyscale images in up to three planes as a color image.
// Assume images are arranged by columns
void segv_vil_segmentation_manager::display_images_as_color()
{
  unsigned ncols =grid_->cols();
  if (ncols<2)
  {
    std::cout << "In segv_vil_segmentation_manager::display_images_as_color() -"
             << " not enough active panes\n";
    return;
  }
  vil_image_resource_sptr img0 = this->image_at(0,0);
  vil_image_resource_sptr img1 = this->image_at(1,0);
  if (!img0||!img1)
  {
    std::cout << "In segv_vil_segmentation_manager::display_images_as_color()() -"
             << " some input images are null\n";
    return;
  }
  vil_image_view<unsigned char> cimage0 =
    brip_vil_float_ops::convert_to_byte(img0);
  vil_image_view<unsigned char> cimage1 =
    brip_vil_float_ops::convert_to_byte(img1);
  vil_image_view<unsigned char> cimage2;
  if (ncols==3&&this->image_at(2,0))
  {
    vil_image_resource_sptr img2 = this->image_at(2,0);
    cimage2 = brip_vil_float_ops::convert_to_byte(img2);
  }
  else
  {
    unsigned w = cimage0.ni(), h = cimage0.nj();
    cimage2.set_size(w, h);
    cimage2.fill(0);
  }
  vil_image_view<vil_rgb<vxl_byte> > rgb = brip_vil_float_ops::combine_color_planes(cimage0, cimage1, cimage2);
  vil_image_resource_sptr color = vil_new_image_resource_of_view(rgb);
  if (ncols<3)
    grid_->add_column();
  unsigned col = 2, row = 0;
  this->add_image_at(color,col,row);
}

void segv_vil_segmentation_manager::intensity_profile()
{
  bgui_image_tableau_sptr itab = selected_image_tab();
 itab->lock_linenum(true);
  bgui_picker_tableau_sptr ptab = selected_picker_tab();
  float start_col=0, end_col=0, start_row=0, end_row=0;
  ptab->pick_line(&start_col, &start_row, &end_col, &end_row);
  std::vector<double> pos, vals;
  itab->image_line(start_col, start_row, end_col, end_row, pos, vals);
  bgui_graph_tableau_sptr g = bgui_graph_tableau_new(512, 512);
  g->update(pos, vals);
  //popup a profile graph
  char location[100];
  std::sprintf(location, "scan:(%d, %d)<->(%d, %d)",
              static_cast<unsigned>(start_col),
              static_cast<unsigned>(start_row),
              static_cast<unsigned>(end_col),
              static_cast<unsigned>(end_row));
  vgui_dialog* ip_dialog = g->popup_graph(location);
  if (!ip_dialog->ask())
  {
    delete ip_dialog;
    itab->lock_linenum(false);
    return;
  }
delete ip_dialog;
 itab->lock_linenum(false);
}

void segv_vil_segmentation_manager::display_roi()
{
  if (!roi_) {
    std::cout << " Null ROI\n";
    return;
  }
  //assume only one region
  if (roi_->n_regions() != 1)
  {
    std::cout << " Can't handle a roi with more than one region\n";
    return;
  }
  int cmin=roi_->cmin(0), cmax=roi_->cmax(0),
    rmin=roi_->rmin(0), rmax = roi_->rmax(0);

  // display the roi as a vsol polygon
  vsol_point_2d_sptr p0 = new vsol_point_2d(cmin, rmin);
  vsol_point_2d_sptr p1 = new vsol_point_2d(cmax, rmin);
  vsol_point_2d_sptr p2 = new vsol_point_2d(cmax, rmax);
  vsol_point_2d_sptr p3 = new vsol_point_2d(cmin, rmax);
  std::vector<vsol_point_2d_sptr> pts;
  pts.push_back(p0);pts.push_back(p1);pts.push_back(p2);pts.push_back(p3);
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(pts);
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
  t2D->add_vsol_polygon_2d(poly);
}

void segv_vil_segmentation_manager::create_roi()
{
  bgui_picker_tableau_sptr ptab = selected_picker_tab();
  float start_col=0, end_col=0, start_row=0, end_row=0;
  ptab->pick_box(&start_col, &start_row, &end_col, &end_row);
  roi_ = new brip_roi();
  roi_->add_region(int(start_col+0.5f), int(start_row+0.5f),
                   int(end_col-start_col+0.5f), int(end_row-start_row+0.5f));
  this->display_roi();
}

void segv_vil_segmentation_manager::crop_image()
{
  vil_image_resource_sptr img = this->selected_image();
  if (!img) {
    std::cout << "No image to crop\n";
    return;
  }
  if (!roi_) {
    std::cout << "No crop roi specified\n";
    return;
  }
  vil_image_resource_sptr chip;
  if (!brip_vil_float_ops::chip(img, roi_, chip))
  {
    std::cout << "Crop operation failed\n";
    return;
  }
  this->add_image(chip);
}

void segv_vil_segmentation_manager::gaussian()
{
  vil_image_resource_sptr img = this->selected_image();
  if (!img) {
    std::cout << "No image to smooth\n";
    return;
  }
  vil_image_view<float> view = brip_vil_float_ops::convert_to_float(img);
  static float sigma = 1.0f;
  vgui_dialog gauss_dialog("Gaussian Smoothing");
  gauss_dialog.field("Gauss sigma", sigma);
  if (!gauss_dialog.ask())
    return;
  vil_image_view<float> gauss = brip_vil_float_ops::gaussian(view, sigma);
  vil_image_resource_sptr gaussr = vil_new_image_resource_of_view(gauss);
  this->add_image(gaussr);
}

void segv_vil_segmentation_manager::abs_value()
{
  vil_image_resource_sptr img = this->selected_image();
  if (!img) {
    std::cout << "No image to for absolute value\n";
    return;
  }
  vil_image_view<float> view = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> abs = brip_vil_float_ops::absolute_value(view);
  vil_image_resource_sptr absr = vil_new_image_resource_of_view(abs);
  this->add_image(absr);
}

void segv_vil_segmentation_manager::inline_viewer()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  bgui_range_adjuster_tableau_sptr h= bgui_range_adjuster_tableau_new(itab);
  h->set_hardware(true);
  h->update();
  vgui_viewer2D_tableau_sptr v = vgui_viewer2D_tableau_new(h);
  vgui_shell_tableau_sptr s = vgui_shell_tableau_new(v);
  //popup adjuster
  vgui_dialog test_inline("Histogram Range Adjuster");
  test_inline.inline_tableau(s, 280, 200);
  if (!test_inline.ask())
    return;
}

void segv_vil_segmentation_manager::intensity_histogram()
{
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::intensity_histogram() - no image\n";
    return;
  }
  bgui_image_utils iu(img);
  bgui_graph_tableau_sptr g = iu.hist_graph();

  if (!g)
  { std::cout << "In segv_vil_segmentation_manager::intensity_histogram()- color images not supported\n";
    return;
  }

  //popup a profile graph
  char location[100];
  std::sprintf(location, "Intensity Histogram");
  vgui_dialog* ip_dialog = g->popup_graph(location);
  if (!ip_dialog->ask())
  {
    delete ip_dialog;
    return;
  }
  delete ip_dialog;
}

//=== Image Arithmetic (Uses the Image Stack)
//Add the image in pane 0 to the image in pane 1. Result in pane 2.
void segv_vil_segmentation_manager::add_images()
{
  unsigned ncols =grid_->cols();
  if (ncols<2)
  {
    std::cout << "In segv_vil_segmentation_manager::add_images() -"
             << " not enough active panes\n";
    return;
  }
  vil_image_resource_sptr img0 = this->image_at(0,0);
  vil_image_resource_sptr img1 = this->image_at(1,0);
  if (!img0||!img1)
  {
    std::cout << "In segv_vil_segmentation_manager::add_images() -"
             << " one or both input images are null\n";
    return;
  }
  vil_image_resource_sptr sum = brip_vil_float_ops::sum(img0, img1);
  vgui_range_map_params_sptr rmps = range_params(sum);
  if (ncols<3)
    grid_->add_column();
  unsigned col = 2, row = 0;
  this->add_image_at(sum,col,row, rmps);
}

//subtract the image in pane 1 from the image in pane 0. Result in pane 2
void segv_vil_segmentation_manager::subtract_images()
{
  unsigned ncols =grid_->cols();
  if (ncols<2)
  {
    std::cout << "In segv_vil_segmentation_manager::subtract_images() -"
             << " not enough active panes\n";
    return;
  }
  vil_image_resource_sptr img0 = this->image_at(0,0);
  vil_image_resource_sptr img1 = this->image_at(1,0);
  if (!img0||!img1)
  {
    std::cout << "In segv_vil_segmentation_manager::subtract_images() -"
             << " one or both input images are null\n";
    return;
  }
  vil_image_resource_sptr diff = brip_vil_float_ops::difference(img0, img1);
  vgui_range_map_params_sptr rmps = range_params(diff);
  if (ncols<3)
    grid_->add_column();
  unsigned col = 2, row = 0;
  this->add_image_at(diff,col,row);
}

void segv_vil_segmentation_manager::negate_image()
{
    vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_segmentation_manager::negate_image - no image\n";
    return;
  }
  vil_image_resource_sptr neg = brip_vil_float_ops::negate(img);
  if (neg)
    this->add_image(neg);
}

void segv_vil_segmentation_manager::entropy()
{
  vgui_dialog entropy_dlg("Entropy of Image");
  static unsigned xrad = 15, yrad = 15, step = 10;
  static float sigma = 1.0f;
  static bool inten = true;
  static bool grad = true;
  static bool color = false;
  entropy_dlg.field("Region x radius",xrad);
  entropy_dlg.field("Region y radius",yrad);
  entropy_dlg.field("Step Size", step);
  entropy_dlg.field("Sigma", sigma);
  entropy_dlg.checkbox("Intensity", inten);
  entropy_dlg.checkbox("Gradient", grad);
  entropy_dlg.checkbox("Color", color);
  if (!entropy_dlg.ask())
    return;
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  vil_image_resource_sptr img = itab->get_image_resource();
  vil_image_view<float> entropy =
    brip_vil_float_ops::entropy(xrad, yrad, step, img,
                                sigma, inten, grad, color);
  vil_image_view<unsigned char> cent =
    brip_vil_float_ops::convert_to_byte(entropy);

  this->add_image(vil_new_image_resource_of_view(cent));
}

void segv_vil_segmentation_manager::minfo()
{
  vgui_dialog minfo_dlg("Minfo of Image");
  static unsigned xrad = 15, yrad = 15, step = 10;
  static float sigma = 1.0f;
  static bool inten = true;
  static bool grad = true;
  static bool color = false;
  minfo_dlg.field("Region x radius",xrad);
  minfo_dlg.field("Region y radius",yrad);
  minfo_dlg.field("Step Size", step);
  minfo_dlg.field("Sigma", sigma);
  minfo_dlg.checkbox("Intensity", inten);
  minfo_dlg.checkbox("Gradient", grad);
  minfo_dlg.checkbox("Color", color);
  if (!minfo_dlg.ask())
    return;
  vil_image_resource_sptr img0 = this->image_at(0,0);
  vil_image_resource_sptr img1 = this->image_at(1,0);
  if (!img0||!img1)
  {
    std::cout << "In segv_vil_segmentation_manager::minfo() -"
             << " one or both input images are null\n";
    return;
  }
  vil_image_view<float> MI0, MI1;
  if (!brip_vil_float_ops::minfo(xrad, yrad, step, img0, img1, MI0, MI1,
                                sigma, inten, grad, color))
    return;
  vil_image_view<unsigned char> MI0_char =
    brip_vil_float_ops::convert_to_byte(MI0);

  vil_image_view<unsigned char> MI1_char =
    brip_vil_float_ops::convert_to_byte(MI1);

  this->add_image_at(vil_new_image_resource_of_view(MI0_char), 0, 0);
  this->add_image_at(vil_new_image_resource_of_view(MI1_char), 1, 0);
}

void segv_vil_segmentation_manager::rotate_image()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_vil_segmentation_manager::rotate_image - no image\n";
    return;
  }
  vil_image_view<float> flt =
    brip_vil_float_ops::convert_to_float(img);
  static double angle = 0;
  vgui_dialog rotate_dialog("Rotate Image");
  rotate_dialog.field("Rotation Angle (deg)", angle);
  if (!rotate_dialog.ask())
    return;

  vil_image_view<float> temp = brip_vil_float_ops::rotate(flt, angle);
#if 0
  vil_image_view<unsigned char> tempr =
    brip_vil_float_ops::convert_to_byte(temp, 0, 255);
#endif
  vil_image_resource_sptr out_image = vil_new_image_resource_of_view(temp);
  this->add_image(out_image);
}

void segv_vil_segmentation_manager::reduce_image()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_vil_segmentation_manager::reduce_image - no image\n";
    return;
  }
  static float coef=0.6f;
  vgui_dialog expand_dialog("Reduce Image");
  expand_dialog.field("Filter coef", coef);
  if (!expand_dialog.ask())
    return;

  vil_image_view<float> flt =
    brip_vil_float_ops::convert_to_float(img);

  vil_image_view<float> reduced =
    brip_vil_float_ops::half_resolution(flt, coef);

  vil_image_resource_sptr out_image = vil_new_image_resource_of_view(reduced);
  this->add_image(out_image);
}

void segv_vil_segmentation_manager::reduce_image_bicubic()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_vil_segmentation_manager::reduce_image_bicubic - no image\n";
    return;
  }
  vil_image_view<float> flt =
    brip_vil_float_ops::convert_to_float(img);

  vil_image_view<float> reduced;
  vil_resample_bicub(flt, reduced, flt.ni()/2, flt.nj()/2);

  vil_image_resource_sptr out_image =
    vil_new_image_resource_of_view(reduced);
  this->add_image(out_image);
}

void segv_vil_segmentation_manager::expand_image()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_vil_segmentation_manager::expand_image - no image\n";
    return;
  }
  static float coef=0.6f;
  vgui_dialog expand_dialog("Expand Image");
  expand_dialog.field("Filter coef", coef);
  if (!expand_dialog.ask())
    return;

  vil_image_view<float> flt =
    brip_vil_float_ops::convert_to_float(img);

  vil_image_view<float> expanded =
    brip_vil_float_ops::double_resolution(flt, coef);

  vil_image_resource_sptr out_image = vil_new_image_resource_of_view(expanded);
  this->add_image(out_image);
}

void segv_vil_segmentation_manager::expand_image_bicubic()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_vil_segmentation_manager::expand_image_bicubic - no image\n";
    return;
  }
  vil_image_view<float> flt =
    brip_vil_float_ops::convert_to_float(img);

  vil_image_view<float> expanded;
  vil_resample_bicub(flt, expanded, 2*flt.ni(), 2*flt.nj());

  vil_image_resource_sptr out_image = vil_new_image_resource_of_view(expanded);
  this->add_image(out_image);
}

void segv_vil_segmentation_manager::flip_image_lr()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout << "In segv_vil_segmentation_manager::flip_image - no image\n";
    return;
  }

  vil_image_resource_sptr flipr = vil_flip_lr(img);
  vil_image_resource_sptr flipc = vil_new_image_resource(img->ni(), img->nj(),
                                                         flipr);
  vil_copy_deep(flipr, flipc);
  this->add_image(flipc);
}

void segv_vil_segmentation_manager::max_trace_scale()
{
  static double scale_ratio = vnl_math::sqrt2;
  static double max_scale = 16.0f;
  vgui_dialog scale_dialog("Max Trace Scale");
  scale_dialog.field("Scale Ratio", scale_ratio);
  scale_dialog.field("Maximum Scale", max_scale);
  if (!scale_dialog.ask())
    return;

  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout<< "In segv_vil_segmentation_manager::max_trace_scale - no image\n";
    return;
  }
  vil_image_view<float> fimg = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> scale_image;
  brip_max_scale_response<float> msr(fimg, scale_ratio, max_scale);
  scale_image = msr.scale_base();
  this->add_image(vil_new_image_resource_of_view(scale_image));
}

void segv_vil_segmentation_manager::color_order()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout<< "In segv_vil_segmentation_manager::color order - no image\n";
    return;
  }
  static float equal_tol = 0.1f;
  vgui_dialog order_dialog("Color Order");
  order_dialog.field("Equal Tol", equal_tol);
  if (!order_dialog.ask())
    return;

  vil_image_view_base_sptr vb = img->get_view();

  //retains the image as color
  vil_image_view<float> fimg = *vil_convert_cast(float(), vb);
  //scale to 0,1 so tolerance is meaningful
  if (vb->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimg,1.0/255.0);

  vil_image_view<unsigned char> order_codes =
    brip_vil_float_ops::color_order(fimg, equal_tol);

  this->add_image(vil_new_image_resource_of_view(order_codes));
}

void segv_vil_segmentation_manager::create_polygon()
{
  bgui_picker_tableau_sptr ptab = selected_picker_tab();
  if (!ptab) {
    std::cerr << "In segv_vil_segmentation_managerd::create_polygon() - no picker tableau\n";
    return;
  }
  vsol_polygon_2d_sptr poly2d;
  ptab->pick_polygon(poly2d);
  if (!poly2d)
  {
    std::cerr << "In segv_vil_segmentation_manager::create_polygon() - picking failed\n";
    return;
  }
  bgui_vtol2D_tableau_sptr btab = selected_vtol2D_tab();
  if (!btab) {
    std::cerr << "In segv_vil_segmentation_managerd::create_polygon() - no vtol2D tableau\n";
    return;
  }
//  btab->add(poly2d);
  mask_.push_back(poly2d);
  btab->post_redraw();
}

void segv_vil_segmentation_manager::clear_mask()
{
  mask_.clear();
}

void segv_vil_segmentation_manager::save_mask()
{
  mask_.clear();
}

void segv_vil_segmentation_manager::mser_conics()
{
  vgui_dialog mser_dialog("Fit overlay Conics");
  static std::string conic_filename = "";
  static std::string ext = "*.*";
  mser_dialog.file("MSER Conic Filename:", ext, conic_filename);
  if (!mser_dialog.ask())
    return;
  std::ifstream istr(conic_filename.c_str());
  if (!istr.is_open())
    return;
  std::vector<vsol_conic_2d_sptr> conics;
#if 0 //check in later
  sdet_read_mser_regions::read_mser_conics(istr, conics);
#endif
  if (!conics.size())
    return;
  this->draw_conics(conics);
}

void segv_vil_segmentation_manager::image_as_vrml_points()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout<< "In segv_vil_segmentation_manager::image_as_vrml_points - no image\n";
    return;
  }
  vgui_dialog vrml_dialog("VRML Intensity Display");
  static std::string vrml_filename = "";
  static std::string ext = "*.*";
  vrml_dialog.file("VRML Filename:", ext, vrml_filename);
  if (!vrml_dialog.ask())
    return;
  std::ofstream ostr(vrml_filename.c_str());
  if (!ostr.is_open())
    return;
  vil_image_view<float> fimg =
    brip_vil_float_ops::convert_to_float(img);
#if 1
  sdet_vrml_display::write_vrml_header(ostr);
  sdet_vrml_display::write_vrml_height_map(ostr, fimg);
#endif
}

void segv_vil_segmentation_manager::extrema()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout<< "In segv_vil_segmentation_manager::extrema - no image\n";
    return;
  }
  static float lambda0 = 1.0f;
  static float lambda1 = 1.0f;
  static float theta = 0.0f;
  static bool bright = true;
  static bool color_overlay = true;
  static bool fast = true;
  static int choice = 1;
  std::vector<std::string> choices;
  choices.push_back("Point Response Only");
  choices.push_back("Point & Mask");
  choices.push_back("Point & Unclipped");
  choices.push_back("SignedPerPixel");
  choices.push_back("AbsolutePerPixel");
  vgui_dialog extrema_dialog("Detect Extrema");
  extrema_dialog.field("lambda0",lambda0);
  extrema_dialog.field("lambda1",lambda1);
  extrema_dialog.field("theta",theta);
  extrema_dialog.checkbox("Bright Extrema?(check)",bright);
  extrema_dialog.checkbox("ColorOverlay?(check)",color_overlay);
  extrema_dialog.choice("Display Mode", choices, choice);
  extrema_dialog.checkbox("Fast Alg.(check)", fast);
  if (!extrema_dialog.ask())
    return;
  vul_timer t;
  vil_image_view<float> fimg =
    brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> extr;
  bool output_mask = false, output_unclipped = false, mag_only = false;
  bool scale_invariant = false, non_max_suppress = true;
  if (choice == 1) output_mask = true;
  if (choice == 2) output_unclipped = true;
  if (choice == 3) {
    scale_invariant = true;
    output_unclipped = true;
    non_max_suppress = false;
  }
  if (choice == 4)  {
    scale_invariant = true;
    mag_only = true;
    output_unclipped = false;
    non_max_suppress = false;
  }
  if (fast)
    extr = brip_vil_float_ops::fast_extrema(fimg, lambda0, lambda1, theta, bright, mag_only, output_mask, output_unclipped, scale_invariant, non_max_suppress);
  else
    extr = brip_vil_float_ops::extrema(fimg, lambda0, lambda1, theta, bright, mag_only, output_mask, output_unclipped, scale_invariant, non_max_suppress);

  std::cout << "Extrema computation time " << t.real() << " msec\n";
  if (choice ==3 || choice == 4) {
    vil_image_resource_sptr resc = vil_new_image_resource_of_view(extr);
    this->add_image(resc);
    return;
  }
  unsigned ni = extr.ni(), nj = extr.nj(), np = extr.nplanes();
  if (choice==0&&!color_overlay) {
    if (np!=1)
      return;
    vil_image_resource_sptr resc = vil_new_image_resource_of_view(extr);
    this->add_image(resc);
    return;
  }
  if (choice==0&&color_overlay) {
    if (np!=1)
      return;
    vil_image_resource_sptr resc = vil_new_image_resource_of_view(extr);
    vil_image_view<vil_rgb<vxl_byte> > rgb =
      brip_vil_float_ops::combine_color_planes(img, resc, img);
    this->add_image(vil_new_image_resource_of_view(rgb));
  }
  if (choice>0)
  {
    if (np!=2)
      return;
    vil_image_view<float> res(ni, nj), mask(ni, nj);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
      {
        res(i,j) = extr(i,j,0);
        mask(i,j) = extr(i,j,1);
      }
    if (color_overlay) {
      vil_image_resource_sptr res_resc = vil_new_image_resource_of_view(res);
      vil_image_resource_sptr msk_resc = vil_new_image_resource_of_view(mask);
      vil_image_view<vil_rgb<vxl_byte> > rgb =
        brip_vil_float_ops::combine_color_planes(img, res_resc, msk_resc);
      this->add_image(vil_new_image_resource_of_view(rgb));
    }
    if (output_mask&&!color_overlay)
    {
     this->add_image(vil_new_image_resource_of_view(res));
      this->add_image(vil_new_image_resource_of_view(mask));
    }
  }
}

void segv_vil_segmentation_manager::rot_extrema()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout<< "In segv_vil_segmentation_manager::extrema - no image\n";
    return;
  }
  static float lambda0 = 1.0f;
  static float lambda1 = 1.0f;
  static float theta_inc = 0.0f;
  static bool bright = true;
  vgui_dialog extrema_dialog("Detect Extrema");
  extrema_dialog.field("lambda0",lambda0);
  extrema_dialog.field("lambda1",lambda1);
  extrema_dialog.field("theta increment",theta_inc);
  extrema_dialog.checkbox("Bright Extrema?(check)",bright);
  if (!extrema_dialog.ask())
    return;
  vul_timer t;
  vil_image_view<float> fimg = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> output = brip_vil_float_ops::extrema_rotational(fimg, lambda0, lambda1, theta_inc, bright);
  unsigned ni = output.ni(), nj = output.nj();
  vil_image_view<float> res(ni, nj), mask(ni, nj);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      res(i,j) = output(i,j,0);
      mask(i,j) = output(i,j,2);
    }

  vil_image_resource_sptr res_resc = vil_new_image_resource_of_view(res);
  vil_image_resource_sptr msk_resc = vil_new_image_resource_of_view(mask);
  vil_image_view<vil_rgb<vxl_byte> > rgb = brip_vil_float_ops::combine_color_planes(img, res_resc, msk_resc);
  this->add_image(vil_new_image_resource_of_view(rgb));
}

void segv_vil_segmentation_manager::beaudet()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    std::cout<< "In segv_vil_segmentation_manager::beaudet - no image\n";
    return;
  }
  static float sigma = 1.0f;
  static bool determinant = true;
  vgui_dialog beaudet_dialog("beaudet");
  beaudet_dialog.field("sigma", sigma);
  beaudet_dialog.checkbox("Determinant(or Trace)", determinant);
  if (!beaudet_dialog.ask())
    return;
  int ni = img->ni(), nj = img->nj();
  vil_image_view<float> fimg = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> smooth = brip_vil_float_ops::gaussian(fimg, sigma);
  vil_image_view<float> Ixx(ni,nj), Ixy(ni, nj), Iyy(ni, nj);
  brip_vil_float_ops::hessian_3x3(smooth, Ixx, Ixy, Iyy);
  vil_image_view<float> beau =
    brip_vil_float_ops::beaudet(Ixx, Ixy, Iyy, determinant);
  this->add_image(vil_new_image_resource_of_view(beau));
}

void segv_vil_segmentation_manager::parallel_coverage()
{
  static brip_para_cvrg_params pcp;
  static bool combined=true;
  vgui_dialog para_dialog("Parallel Coverage");
  para_dialog.field("Sigma", pcp.sigma_);
  para_dialog.field("Projection Width", pcp.proj_width_);
  para_dialog.field("Projection Height", pcp.proj_height_);
  para_dialog.checkbox("Display Coverage and Direction Combined", combined);
  para_dialog.checkbox("Verbose", pcp.verbose_);
  if (!para_dialog.ask())
    return;
  vil_image_resource_sptr img = selected_image();
  brip_para_cvrg pc(pcp);
  pc.do_coverage(img);
  vil_image_resource_sptr cov_res;
  if (combined)
    cov_res = vil_new_image_resource_of_view(pc.get_combined_image());
  else
    cov_res = vil_new_image_resource_of_view(pc.get_detection_image());
  this->add_image(cov_res);
}

//: it receives an image of line definitions and draws the lines on the tableau.
// The image is expected to have three planes to save (x,y,theta) of each line.
// (x,y) is the position of the edge and theta is the direction angle in radians
void segv_vil_segmentation_manager::draw_line_image()
{
  vgui_dialog file_dlg("Edge File");
  static std::string filename = "";
  static std::string ext = "*.*";
  file_dlg.file("Edge filename:", ext, filename);
  if (!file_dlg.ask())
    return;
  vil_image_view_base_sptr img_sptr = vil_load(filename.c_str());
  if (img_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> edge_image(img_sptr);
    if (edge_image.nplanes() < 3) {
      std::cout << "The image should have at least 3 planes" << std::endl;
      return;
    }
    bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
    if (!t2D)
      return;

    for (unsigned i=0; i<edge_image.ni(); i++) {
      for (unsigned j=0; j<edge_image.nj(); j++) {
        double x = edge_image(i,j,0);
        double y = edge_image(i,j,1);
        double theta = edge_image(i,j,2);
        double a,b;
        a = x-0.5*std::cos(theta);
        b = y-0.5*std::sin(theta);
        vgl_point_2d<double> p0(a,b);
        a = x+0.5*std::cos(theta);
        b = y+0.5*std::sin(theta);
        vgl_point_2d<double> p1(a,b);
        // define a line
        vsol_line_2d_sptr line = new vsol_line_2d(p0,p1);
        t2D->add_vsol_line_2d(line);
      }
    }
  }
  else
    std::cout << "Pixel format: " << img_sptr->pixel_format() << " is not implemented yet" << std::endl;
}

void segv_vil_segmentation_manager::gradient_mag_angle()
{
  static vgui_style_sptr style =
    vgui_style::new_style(0.8f, 0.2f, 0.9f, 1.0f, 3.0f);

  static float sigma = 1.0f;
  static bool sep_mag_displ = false;
  static bool display_on_image = true;
  static unsigned display_interval = 4;
  static double vector_scale = 0.7;
  vgui_dialog grad_mag_ang_dialog("grad_mag_ang");
  grad_mag_ang_dialog.field("sigma", sigma);
  grad_mag_ang_dialog.field("DisplayInterval", display_interval);
  grad_mag_ang_dialog.field("VectorScale", vector_scale);
  grad_mag_ang_dialog.checkbox("Display Mag as Separate Img", sep_mag_displ);
  grad_mag_ang_dialog.checkbox("Display Vectors on Img", display_on_image);
  grad_mag_ang_dialog.field("Red", style->rgba[0]);
  grad_mag_ang_dialog.field("Green", style->rgba[1]);
  grad_mag_ang_dialog.field("Blue", style->rgba[2]);
  if (!grad_mag_ang_dialog.ask())
    return;

  this->clear_display();
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::gradient_mag_angle() -  no image\n";
    return;
  }
  unsigned ni = img->ni(), nj = img->nj();
  vil_image_view<float> fview = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> smooth = brip_vil_float_ops::gaussian(fview, sigma);
  vil_image_view<float> mag(ni, nj), gx(ni, nj), gy(ni, nj);
  brip_vil_float_ops::gradient_mag_comp_3x3(smooth, mag, gx, gy);
  std::vector<vsol_line_2d_sptr > lines;
  if (sep_mag_displ) {
    for (unsigned j = 2; j<nj-2; j+=display_interval)
      for (unsigned i = 2; i<ni-2; i+=display_interval) {
        double cx =  i, cy = j;
        vsol_point_2d_sptr c = new vsol_point_2d(cx, cy);
        double dx = 0, dy = 0;
        float m = mag(i,j);
        float ggx = gx(i,j), ggy = gy(i,j);
        if (m>0.1) {
          dx = ggx/m; dy = ggy/m;
        }
        vsol_point_2d_sptr e = new vsol_point_2d(cx+vector_scale*dx,
                                                 cy+vector_scale*dy);
        vsol_line_2d_sptr l = new vsol_line_2d(c, e);
        lines.push_back(l);
      }
    if (!display_on_image) {
      vil_image_view<float> blank(ni, nj);
      blank.fill(0.0f);
      this->add_image(vil_new_image_resource_of_view(blank));
    }
    this->draw_lines(lines, style);
    this->add_image(vil_new_image_resource_of_view(mag));
    return;
  }
  double gmax = 0.0;
  for (unsigned j = 2; j<nj-2; j+=display_interval)
    for (unsigned i = 2; i<ni-2; i+=display_interval)
      if (mag(i,j)>gmax) gmax = mag(i,j);

  for (unsigned j = 2; j<nj-2; j+=display_interval)
    for (unsigned i = 2; i<ni-2; i+=display_interval)
    {
      double cx =  i, cy = j;
      vsol_point_2d_sptr c = new vsol_point_2d(cx, cy);
      double dx = gx(i,j)/gmax, dy = gy(i,j)/gmax;
      vsol_point_2d_sptr e = new vsol_point_2d(cx+vector_scale*dx,
                                               cy+vector_scale*dy);
      vsol_line_2d_sptr l = new vsol_line_2d(c, e);
      lines.push_back(l);
    }
  if (!display_on_image) {
    vil_image_view<float> blank(ni, nj);
    blank.fill(0.0f);
    this->add_image(vil_new_image_resource_of_view(blank));
  }
  this->draw_lines(lines, style);
}

void segv_vil_segmentation_manager::fft()
{
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    std::cout << "In segv_vil_segmentation_manager::fft() - no image\n";
    return;
  }
  vgui_dialog dlg("FFT");
  static bool use_mag = true;//else phase
  dlg.checkbox("Display FFT Mag?", use_mag);
  if (!dlg.ask())
    return;
  vil_image_view<float> fview = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> fview_p2;
  if (!brip_vil_float_ops::resize_to_power_of_two(fview, fview_p2))
    return;
  vil_image_view<float> magr, phase;
  brip_vil_float_ops::fourier_transform(fview_p2, magr, phase);
  if (use_mag)
    this->add_image(vil_new_image_resource_of_view(magr));
  else
    this->add_image(vil_new_image_resource_of_view(phase));
}
