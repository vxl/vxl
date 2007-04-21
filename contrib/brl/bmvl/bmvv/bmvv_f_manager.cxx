// This is brl/bmvl/bmvv/bmvv_f_manager.cxx
#include "bmvv_f_manager.h"
//:
// \file
// \author J.L. Mundy

#if 1 //JLM
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#endif
// include for project points menu option
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_fm_compute_ransac.h>
#include <vpgl/algo/vpgl_fm_compute_8_point.h>
#include <vpgl/algo/vpgl_fm_compute_2_point.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // sprintf
#include <vcl_fstream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vul/vul_file.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_view.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_property.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <bgui/bgui_vsol_soview2D.h>
#if 0
#ifdef HAS_XERCES
#include <bxml/bxml_vtol_io.h>
#endif
#endif
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
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_intensity_face.h>
#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_para_cvrg_params.h>
#include <brip/brip_para_cvrg.h>
#include <brip/brip_watershed_params.h>
#include <sdet/sdet_watershed_region_proc_params.h>
#include <sdet/sdet_watershed_region_proc.h>
#include <sdet/sdet_vehicle_finder_params.h>
#include <sdet/sdet_vehicle_finder.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <strk/strk_region_info_params.h>
#include <strk/strk_region_info.h>
#include <strk/strk_io.h>
#include <brct/brct_algos.h>

;
bmvv_f_manager *bmvv_f_manager::instance_ = 0;

bmvv_f_manager *bmvv_f_manager::instance()
{
  if (!instance_)
  {
    instance_ = new bmvv_f_manager();
    instance_->init();
  }
  return bmvv_f_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
bmvv_f_manager::bmvv_f_manager():vgui_wrapper_tableau()
{
  first_ = true;
}

bmvv_f_manager::~bmvv_f_manager()
{
  for (unsigned v = 0; v<vtabs_.size(); ++v)
  {
    bgui_vtol2D_tableau_sptr t = vtabs_[v];
    vpgl_proj_camera<double>* cam = cam_map_[v];
    cam_map_[v] = 0;
    delete cam;
  }
}

//: Set up the tableaux
void bmvv_f_manager::init()
{
  bgui_image_tableau_sptr itab = bgui_image_tableau_new();
  bgui_vtol2D_tableau_sptr t2D = bgui_vtol2D_tableau_new(itab);
  vtabs_.push_back(t2D);
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
vgui_range_map_params_sptr bmvv_f_manager::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;

  //Check if the image is blocked
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (bir)
  { gl_map = true; cache = false; }

  //Check if the image is a pyramid
  bool pyr = image->get_property(vil_property_pyramid, 0);
  if (pyr)
  { gl_map = true; cache = false; }
  //Get max min parameters

  double min=0, max=0;
  unsigned n_components = image->nplanes();
  vgui_range_map_params_sptr rmps;
  if (n_components == 1)
  {
    bgui_image_utils iu(image);
    iu.range(min, max);
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  }
  else if (n_components == 3)
  {
    min = 0; max = 255;//for now - ultimately need to compute color histogram
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  }
  return rmps;
}

//: set the image at the currently selected grid cell
void bmvv_f_manager::
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
void bmvv_f_manager::
add_image_at(vil_image_resource_sptr const& image,
             const unsigned col, const unsigned row,
             vgui_range_map_params_sptr const& rmps)
{
  vgui_range_map_params_sptr rmap = rmps;
  if (!rmps)
    rmap = range_params(image);
  bgui_image_tableau_sptr itab = bgui_image_tableau_new(image);
  itab->set_mapping(rmap);
  bgui_vtol2D_tableau_sptr t2D = bgui_vtol2D_tableau_new(itab);
  vtabs_.push_back(t2D);
  bgui_picker_tableau_sptr picktab = bgui_picker_tableau_new(t2D);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(picktab);
  grid_->add_at(v2D, col, row);
  itab->post_redraw();
}

//: Add an image to the currently selected grid cell
void bmvv_f_manager::
add_image(vil_image_resource_sptr const& image,
          vgui_range_map_params_sptr const& rmps)
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  this->add_image_at(image, col, row, rmps);
}

#if 0
//-----------------------------------------------------------------------------
//: remove the selected image
//-----------------------------------------------------------------------------
void bmvv_f_manager::remove_image()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  grid_->remove_at(col, row);
}

void bmvv_f_manager::convert_to_grey()
{
  vil_image_resource_sptr img = this->selected_image();
  if (!img)
    return;
  vil_image_view<unsigned char> grey =
    brip_vil_float_ops::convert_to_byte(img);
  vil_image_resource_sptr gimg = vil_new_image_resource_of_view(grey);
  this->add_image(gimg);
}
#endif

//: Get the image tableau for the currently selected grid cell
bgui_image_tableau_sptr bmvv_f_manager::selected_image_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_image_tableau_sptr itab;
    itab.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    vcl_string("vgui_image_tableau")));
    if (itab)
      return itab;
  }
  vcl_cout << "Unable to get bgui_image_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_image_tableau_sptr();
}

//: Get the vtol2D tableau at the specified grid cell
bgui_vtol2D_tableau_sptr
bmvv_f_manager::vtol2D_tab_at(const unsigned col,
                              const unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_vtol2D_tableau_sptr v2D;
    v2D.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                   vcl_string("bgui_vtol2D_tableau")));
    if (v2D)
      return v2D;
  }
  vcl_cout << "Unable to get bgui_vtol2D_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_vtol2D_tableau_sptr();
}

//: Get the vtol2D tableau for the currently selected grid cell
bgui_vtol2D_tableau_sptr bmvv_f_manager::selected_vtol2D_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->vtol2D_tab_at(col, row);
}

//: Get the picker tableau for the currently selected grid cell
bgui_picker_tableau_sptr bmvv_f_manager::selected_picker_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_picker_tableau_sptr pick;
    pick.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    vcl_string("bgui_picker_tableau")));
    if (pick)
      return pick;
  }
  vcl_cout << "Unable to get bgui_picker_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_picker_tableau_sptr();
}


vil_image_resource_sptr bmvv_f_manager::selected_image()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    return 0;
  return itab->get_image_resource();
}

vil_image_resource_sptr bmvv_f_manager::image_at(const unsigned col,
                                                 const unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (!top_tab)
    return 0;

  bgui_image_tableau_sptr itab;
  itab.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                  vcl_string("vgui_image_tableau")));
  if (!itab)
  {
    vcl_cout << "Unable to get bgui_image_tableau at (" << col
             << ", " << row << ")\n";
    return 0;
  }
  return itab->get_image_resource();
}

//-----------------------------------------------------------------------------
//: Clear spatial objects from the selected display
//-----------------------------------------------------------------------------
void bmvv_f_manager::clear_display()
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
}

//-----------------------------------------------------------------------------
//: Clear spatial objects from all spatial panes
//-----------------------------------------------------------------------------
void bmvv_f_manager::clear_all()
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
bmvv_f_manager::draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
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
    vcl_cout << "In bmvv_f_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
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
void bmvv_f_manager::
draw_polylines(vcl_vector<vsol_polyline_2d_sptr > const& polys,
               vgui_style_sptr style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In bmvv_f_manager::draw_polylines - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vsol_polyline_2d_sptr>::const_iterator pit = polys.begin();
       pit != polys.end(); pit++)
  {
    t2D->add_vsol_polyline_2d(*pit, style);
  }

  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw line segments on the tableau
//-----------------------------------------------------------------------------
void bmvv_f_manager::
draw_lines(vcl_vector<vsol_line_2d_sptr > const& lines,
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
    vcl_cout << "In bmvv_f_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    t2D->add_vsol_line_2d(*lit,style);
  }

  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw conic segments on the tableau
//-----------------------------------------------------------------------------
void bmvv_f_manager::
draw_conics(vcl_vector<vsol_conic_2d_sptr > const& conics,
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
    vcl_cout << "In bmvv_f_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vsol_conic_2d_sptr>::const_iterator lit = conics.begin();
       lit != conics.end(); lit++)
  {
    t2D->add_vsol_conic_2d(*lit,style);
  }
  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw points on the tableau
//-----------------------------------------------------------------------------
void bmvv_f_manager::
draw_points(vcl_vector<vsol_point_2d_sptr> const& points, const vgui_style_sptr& style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In bmvv_f_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
  {
    t2D->add_vsol_point_2d(*pit,style);
  }

  t2D->post_redraw();
}

void bmvv_f_manager::draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                                  bool verts)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  for (vcl_vector<vtol_intensity_face_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
  {
    vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
    t2D->add_face(f);
    if (verts)
    {
      vcl_vector<vtol_vertex_sptr> vts;
      f->vertices(vts);
      for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts.begin();
           vit != vts.end(); vit++)
      {
        vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
        t2D->add_vertex(v);
      }
    }
  }
  t2D->post_redraw();
}

void bmvv_f_manager::quit()
{
  this->clear_all();
  vgui::quit();
}

void bmvv_f_manager::load_image()
{
  static bool greyscale = false;
  static bool sblock = false;
  vgui_dialog load_image_dlg("Load image file");
  static vcl_string image_filename = "/home/dec/images/cal_image1.tif";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  load_image_dlg.checkbox("blocked?:", sblock);
  if (!load_image_dlg.ask())
    return;
  //first check to see if the filename is a directory
  //if so, then assume a pyramid image
  bool pyrm = false;
  vil_image_resource_sptr image;
  if (vul_file::is_directory(image_filename.c_str()))
  {
    vil_pyramid_image_resource_sptr pyr =
      vil_load_pyramid_resource(image_filename.c_str());
    if (pyr)
    {
      image = pyr.ptr();
      pyrm = true;
    }
  }
  if (!image)
    image = vil_load_image_resource(image_filename.c_str());

  if (!image)
    return;

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

void bmvv_f_manager::save_image()
{
  vgui_dialog file_dialog("Save Image");
  static vcl_string image_file;
  static vcl_string ext = "tif";
  static vcl_string type = "tiff";
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
    vcl_cerr << "Null image in bmvv_f_manager::save_image\n";
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
    vcl_cerr << "bmvv_f_manager::save_image operation failed\n";
}

void bmvv_f_manager::set_range_params()
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
    rmps = 0;
  itab->set_mapping(rmps);
}

void bmvv_f_manager::read_corrs()
{
  vgui_dialog corr_dlg("Read Correspondences");
  static vcl_string corr_file = "";
  static vcl_string corr_ext = "*.corr";
  corr_dlg.file("Corr File", corr_ext, corr_file);
  if (!corr_dlg.ask())
    return;
  vcl_ifstream corr_istr(corr_file.c_str());
  corrs_.clear();
  if (!brct_algos::read_brct_corrs(corr_istr, corrs_))
  {
    vcl_cout << "Read Failed\n";
    return;
  }
  for (unsigned i = 0; i< corrs_.size(); ++i)
    vcl_cout << "c[" << i << "]:" << *(corrs_[i]) << '\n';
}

void bmvv_f_manager::save_corrs()
{
  vcl_cout <<"Saving these corrs\n";
  for (unsigned i = 0; i< corrs_.size(); ++i)
    vcl_cout << "c[" << i << "]:" << *(corrs_[i]) << '\n';
  vgui_dialog corr_dlg("Save Correspondences");
  static vcl_string corr_file = "";
  static vcl_string corr_ext = "*.corr";
  corr_dlg.file("Corr File", corr_ext, corr_file);
  if (!corr_dlg.ask())
    return;
  vcl_ofstream corr_ostr(corr_file.c_str());
  if (!brct_algos::write_brct_corrs(corr_ostr, corrs_))
  {
    vcl_cout << "Read Failed\n";
    return;
  }
}

void bmvv_f_manager::read_f_matrix()
{
  vgui_dialog f_dlg("Read F Matrix");
  static vcl_string f_file = "";
  static vcl_string f_ext = "*.fm";
  f_dlg.file("F Matrix File", f_ext, f_file);
  if (!f_dlg.ask())
    return;
  vcl_ifstream f_istr(f_file.c_str());
  if (!f_istr.is_open()){
    vcl_cout << "Read Failed\n";
    return;
  }
  vnl_matrix_fixed<double, 3,3> m;
  f_istr >> m;
  fm_.set_matrix(m);
}

void bmvv_f_manager::save_f_matrix()
{
  vgui_dialog f_dlg("Save F Matrix");
  static vcl_string f_file = "";
  static vcl_string f_ext = "*.fm";
  f_dlg.file("F Matrix File", f_ext, f_file);
  if (!f_dlg.ask())
    return;
  vcl_ofstream f_ostr(f_file.c_str());
  if (!f_ostr.is_open()){
    vcl_cout << "Write Failed\n";
    return;
  }
  vnl_matrix_fixed<double, 3,3> m = fm_.get_matrix();
  f_ostr << m;
}

void bmvv_f_manager::display_corrs()
{
  unsigned n = corrs_.size();
  if (!n)
    return;
  brct_corr_sptr bc = corrs_[0];
  unsigned int ncams = bc->n_cams();
  unsigned cols = grid_->cols();
  if (cols!=ncams)
    return;
  //assume left image is in col 0 and right image is in col 1
  bgui_vtol2D_tableau_sptr t0 = this->vtol2D_tab_at(0, 0);
  if (!t0)
    return;
  bgui_vtol2D_tableau_sptr t1 = this->vtol2D_tab_at(1, 0);
  if (!t1)
    return;
  vgui_style_sptr st = vgui_style::new_style(0,1,0,3,1);
  //clear the map
  corr_map_.clear();
  for (unsigned i=0; i<n; ++i)
  {
    bc = corrs_[i];
    vgl_point_2d<double> m0(bc->match(0)), m1(bc->match(1));
    vsol_point_2d_sptr p0 = new vsol_point_2d(m0);
    vsol_point_2d_sptr p1 = new vsol_point_2d(m1);
    bgui_vsol_soview2D_point* sov = t0->add_vsol_point_2d(p0,st);
    int id = sov->get_id();
    int n = corrs_.size();
    corr_map_[id]=n-1;
    t1->add_vsol_point_2d(p1,st);
  }
  t0->post_redraw();
  t1->post_redraw();
}

void bmvv_f_manager::display_right_epi_lines()
{
  unsigned n = corrs_.size();
  if (!n)
    return;
  brct_corr_sptr bc = corrs_[0];
  unsigned int ncams = bc->n_cams();
  unsigned cols = grid_->cols();
  if (cols!=ncams)
    return;
  //assume left image is in col 0 and right image is in col 1
  bgui_vtol2D_tableau_sptr t1 = this->vtol2D_tab_at(1, 0);
  if (!t1)
    return;
  vgui_style_sptr st =vgui_style::new_style(0,1,0,1,2);
  for (unsigned i=0; i<n; ++i)
  {
    bc = corrs_[i];
    vgl_homg_point_2d<double> hpl=bc->match(0);
    vgl_homg_line_2d<double> hl = fm_.r_epipolar_line(hpl);
    t1->add_infinite_line(hl.a(), hl.b(), hl.c());
  }
  t1->post_redraw();
}

void bmvv_f_manager::display_picked_epi_line()
{
  //determine if left or right image
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  bgui_picker_tableau_sptr picktab = this->selected_picker_tab();
  if (!picktab)
    return;
  //get a point
  float x=0, y=0;
  picktab->pick_point(&x, &y);
  vgl_homg_point_2d<double> hp(x,y);
  vgl_homg_line_2d<double> hl;
  if (col==0)
  {
    bgui_vtol2D_tableau_sptr t1 = this->vtol2D_tab_at(1, 0);
    hl = fm_.r_epipolar_line(hp);
    t1->add_infinite_line(hl.a(), hl.b(), hl.c());
    t1->post_redraw();
    return;
  }
  if (col==1)
  {
    bgui_vtol2D_tableau_sptr t0 = this->vtol2D_tab_at(0, 0);
    hl = fm_.l_epipolar_line(hp);
    t0->add_infinite_line(hl.a(), hl.b(), hl.c());
    t0->post_redraw();
    return;
  }
  vcl_cout << "Draw failed\n";
}

brct_corr_sptr  bmvv_f_manager::get_selected_corr()
{
  bgui_vtol2D_tableau_sptr t0 = this->vtol2D_tab_at(0, 0);
  vcl_vector<unsigned> ids = t0->get_selected();
  //take the last selected
  int n = ids.size();
  if (!n)
  {
    vcl_cout << "Nothing selected\n";
    return 0;
  }
  unsigned int i = corr_map_[ids[n-1]];
  if (i<corrs_.size())
  {
    return corrs_[i];
  }
  vcl_cout << "Bogus correspondence\n";
  return 0;
}

void bmvv_f_manager::create_correspondence()
{
  //determine if left or right image
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  if (col!=0)
  {
    vcl_cout << "Select left pane and retry\n";
    return;
  }
  bgui_picker_tableau_sptr picktab = this->selected_picker_tab();
  if (!picktab)
    return;
  //get a point
  float x=0, y=0;
  picktab->pick_point(&x, &y);
  brct_corr_sptr bc = new brct_corr(2);
  bc->set_match(col, x, y);
  corrs_.push_back(bc);
  bgui_vtol2D_tableau_sptr t0 = this->vtol2D_tab_at(0, 0);
  vsol_point_2d_sptr p = new vsol_point_2d(x, y);
  vgui_style_sptr st = vgui_style::new_style(1,1,0,5,1);
  bgui_vsol_soview2D_point* sov = t0->add_vsol_point_2d(p,st);
  int id = sov->get_id();
  int n = corrs_.size();
  corr_map_[id]=n-1;
}

void bmvv_f_manager::pick_correspondence()
{
  brct_corr_sptr bc = this->get_selected_corr();
  if (!bc)
  {
    vcl_cout << "No correspondence selected\n";
    return;
  }
  //determine if left or right image
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  if (col!=1)
  {
    vcl_cout << "Select right pane and retry\n";
    return;
  }
  bgui_picker_tableau_sptr picktab = this->selected_picker_tab();
  if (!picktab)
    return;
  //get a point
  float x=0, y=0;
  picktab->pick_point(&x, &y);
  bc->set_match(col,x,y);
  bgui_vtol2D_tableau_sptr t1 = this->vtol2D_tab_at(1, 0);
  vsol_point_2d_sptr p = new vsol_point_2d(x, y);
  vgui_style_sptr st = vgui_style::new_style(1,1,0,5,1);
  t1->add_vsol_point_2d(p,st);
}

bool bmvv_f_manager::point_lists(vcl_vector<vgl_point_2d<double> >& lpts,
                                 vcl_vector<vgl_point_2d<double> >& rpts)
{
  unsigned n = corrs_.size();
  if (!n)
    return false;
  lpts.clear(); rpts.clear();
  for (unsigned i = 0; i<n; ++i)
  {
    brct_corr_sptr bc = corrs_[i];
    if (!bc->valid(0)||!bc->valid(1))
      continue;
    vgl_homg_point_2d<double> m0 = bc->match(0), m1 = bc->match(1);
    lpts.push_back(vgl_point_2d<double>(m0));
    rpts.push_back(vgl_point_2d<double>(m1));
  }
  return true;
}

void bmvv_f_manager::compute_f_matrix()
{
  vgui_dialog f_dlg("Compute F Matrix");
  static bool refine = false;
  static bool trans = false;
  f_dlg.checkbox("Refine F Matrix? ", refine);
  f_dlg.checkbox("Translation F Matrix? ", trans);
  if (!f_dlg.ask())
    return;
  vcl_vector<vgl_point_2d<double> > lpts, rpts;
  vcl_vector< vgl_homg_point_2d<double> > pir, pil;
  if (!this->point_lists(lpts, rpts))
  {
    vcl_cout << "No corresponding points to compute f matrix\n";
    return;
  }
  if (trans)
  {
    for (unsigned i = 0; i<lpts.size(); ++i)
    {
      pir.push_back(vgl_homg_point_2d<double>(rpts[i]));
      pil.push_back(vgl_homg_point_2d<double>(lpts[i]));
    }
    vpgl_fm_compute_2_point cl2;
    cl2.compute( pir, pil, fm_);
  }
  else
  {
    vpgl_fm_compute_ransac fmcr;
    fmcr.set_generate_all(true);
    fmcr.set_outlier_threshold(2);
    fmcr.compute( rpts, lpts, fm_);
    vnl_matrix_fixed<double,3,3> fm_vnl = fm_.get_matrix();
    vcl_cout << "\nRansac estimated fundamental matrix:\n" << fm_vnl << '\n';
    vcl_vector<bool> outliers = fmcr.outliers;
    vcl_vector<double> res = fmcr.residuals;
    vcl_cout << "\noutliers\n";
    for (unsigned i = 0; i<outliers.size(); ++i)
      vcl_cout << "O[" << i << "]= " << outliers[i]
               << "  e "<< res[i] <<  '\n';
    //Form new point sets throwing out outliers
    for (unsigned i = 0; i<lpts.size(); ++i)
    {
      if (outliers[i])
        continue;
      pir.push_back(vgl_homg_point_2d<double>(rpts[i]));
      pil.push_back(vgl_homg_point_2d<double>(lpts[i]));
    }
    if (refine){
      vpgl_fm_compute_8_point cl;
      cl.compute( pir, pil, fm_);
      vnl_matrix_fixed<double,3,3> fl_vnl = fm_.get_matrix();
      vcl_cerr << "\nLinear refined fundamental matrix:\n" << fl_vnl << '\n';
    }
  }
  vgl_homg_point_2d<double> er,  el;
  fm_.get_epipoles(er, el);
  vcl_cout << "Right Epipole " << er << '\n';
}

void bmvv_f_manager::intensity_profile()
{
  bgui_picker_tableau_sptr ptab = selected_picker_tab();
  float start_col=0, end_col=0, start_row=0, end_row=0;
  ptab->pick_line(&start_col, &start_row, &end_col, &end_row);
  bgui_image_tableau_sptr itab = selected_image_tab();
  vcl_vector<double> pos, vals;
  itab->image_line(start_col, start_row, end_col, end_row, pos, vals);
  bgui_graph_tableau_sptr g = bgui_graph_tableau_new(512, 512);
  g->update(pos, vals);
  //popup a profile graph
  char location[100];
  vcl_sprintf(location, "scan:(%d, %d)<->(%d, %d)",
              static_cast<unsigned>(start_col),
              static_cast<unsigned>(start_row),
              static_cast<unsigned>(end_col),
              static_cast<unsigned>(end_row));
  vgui_dialog* ip_dialog = g->popup_graph(location);
  if (!ip_dialog->ask())
  {
    delete ip_dialog;
    return;
  }
  delete ip_dialog;
}

void bmvv_f_manager::intensity_histogram()
{
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    vcl_cout << "In bmvv_f_manager::intensity_histogram() - no image\n";
    return;
  }
  bgui_image_utils iu(img);
  bgui_graph_tableau_sptr g = iu.hist_graph();

  if (!g)
  { vcl_cout << "In bmvv_f_manager::intensity_histogram()- color images not supported\n";
    return;
  }

  //popup a profile graph
  char location[100];
  vcl_sprintf(location, "Intensity Histogram");
  vgui_dialog* ip_dialog = g->popup_graph(location);
  if (!ip_dialog->ask())
  {
    delete ip_dialog;
    return;
  }
  delete ip_dialog;
}

void bmvv_f_manager::load_image_and_cam()
{
  static bool greyscale = false;
  static vcl_string cam_file = "";
  static vcl_string cam_ext = "*.cam";
  static vcl_string image_file = "";
  static vcl_string ext = "*.*";
  vgui_dialog image_cam_dlg("Read Image and Cam");
  image_cam_dlg.file("Image Filename:", ext, image_file);
  image_cam_dlg.file("Camera File", cam_ext, cam_file);
  image_cam_dlg.checkbox("greyscale ", greyscale);
  if (!image_cam_dlg.ask())
    return;
  //first check to see if the filename is a directory
  //if so, then assume a pyramid image
  bool pyrm = false;
  vil_image_resource_sptr image;
  if (vul_file::is_directory(image_file.c_str()))
  {
    vil_pyramid_image_resource_sptr pyr =
      vil_load_pyramid_resource(image_file.c_str());
    if (pyr)
    {
      image = pyr.ptr();
      pyrm = true;
    }
  }
  if (!image)
    image = vil_load_image_resource(image_file.c_str());

  if (!image)
    return;

  if (greyscale&&!pyrm)
  {
    vil_image_view<unsigned char> grey_view =
      brip_vil_float_ops::convert_to_grey(*image);
    image = vil_new_image_resource_of_view(grey_view);
  }

  vgui_range_map_params_sptr rmps = range_params(image);

  if (first_)
  {
    this->set_selected_grid_image(image, rmps);
    first_ = false;
  }
  else
    this->add_image(image, rmps);

  vcl_ifstream f_istr(cam_file.c_str());
  if (!f_istr.is_open()) {
    vcl_cout << "Camera read failed\n";
    return;
  }

  vpgl_proj_camera<double>* cam = new vpgl_proj_camera<double>;
  f_istr >> *cam;
  cam_map_[vtabs_.size()-1]=cam;
}

void bmvv_f_manager::load_world()
{
  vgui_dialog world_dlg("Load World Points3D");
  static vcl_string world_file = "";
  static vcl_string world_ext = "*.wd";
  world_dlg.file("World Point File", world_ext, world_file);
  if (!world_dlg.ask())
    return;
  vcl_ifstream world_istr(world_file.c_str());
  if (!brct_algos::read_world_points(world_istr, world_))
  {
    vcl_cout << "Failed to read world\n";
    return ;
  }
  this->project_world();
}

void bmvv_f_manager::save_world()
{
  vgui_dialog world_dlg("Save World Points3D");
  static vcl_string world_file = "";
  static vcl_string world_ext = "*.wd";
  world_dlg.file("World Point File", world_ext, world_file);
  if (!world_dlg.ask())
    return;
  vcl_ofstream world_ostr(world_file.c_str());
  brct_algos::write_world_points(world_ostr, world_);
}

void bmvv_f_manager::project_world()
{
  vgui_style_sptr st = vgui_style::new_style(1,1,0,5,1);
  for (unsigned v = 0; v<vtabs_.size(); ++v)
  {
    bgui_vtol2D_tableau_sptr t = vtabs_[v];
    vpgl_proj_camera<double>* cam = cam_map_[v];
    t->clear_all();
    for (unsigned i = 0; i<world_.size(); ++i)
    {
      vgl_point_2d<double> pg = cam->project(world_[i]);
      vsol_point_2d_sptr ps = new vsol_point_2d(pg);
      t->add_vsol_point_2d(ps,st);
    }
  }
}

void bmvv_f_manager::reconstruct_world()
{
  if (cam_map_.size()!=2)
  {
    vcl_cout << "Need exactly two cameras to reconstruct world\n";
    return;
  }
  brct_algos::reconstruct_corrs(corrs_, *(cam_map_[0]), *(cam_map_[1]), world_);
}
