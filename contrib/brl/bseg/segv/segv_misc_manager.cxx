// This is brl/bseg/segv/segv_misc_manager.cxx
#include "segv_misc_manager.h"
//:
// \file
// \author J.L. Mundy

// include for project points menu option
#include <vpgl/vpgl_rational_camera.h>
#if 1 //JLM
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#endif
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // sprintf
#include <vcl_cmath.h> //sin, cos exp
#include <vcl_fstream.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vul/vul_file.h>
#include <vnl/vnl_matlab_read.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_copy.h>
#include <vil/vil_math.h>
#include <vil/vil_decimate.h>
#include <vil/vil_property.h>
#include <vil/vil_flip.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
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
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_range_adjuster_tableau.h>
#include <bgui/bgui_image_utils.h>
#include <bvgl/bvgl_articulated_poly.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_intensity_face.h>
#include <bsol/bsol_algs.h>
#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_para_cvrg_params.h>
#include <brip/brip_para_cvrg.h>
#include <brip/brip_watershed_params.h>
#include <sdet/sdet_watershed_region_proc_params.h>
#include <sdet/sdet_watershed_region_proc.h>
#include <sdet/sdet_vehicle_finder_params.h>
#include <sdet/sdet_vehicle_finder.h>
#include <bsol/bsol_hough_line_index.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <strk/strk_region_info_params.h>
#include <strk/strk_region_info.h>
#include <strk/strk_io.h>

;
segv_misc_manager *segv_misc_manager::instance_ = 0;

segv_misc_manager *segv_misc_manager::instance()
{
  if (!instance_)
    {
      instance_ = new segv_misc_manager();
      instance_->init();
    }
  return segv_misc_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
segv_misc_manager::segv_misc_manager():vgui_wrapper_tableau()
{
  first_ = true;
}

segv_misc_manager::~segv_misc_manager()
{
}

//: Set up the tableaux
void segv_misc_manager::init()
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
vgui_range_map_params_sptr segv_misc_manager::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;

  //Check if the image is blocked
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if(bir)
    { gl_map = true; cache = false;}

  //Check if the image is a pyramid
  bool pyr = image->get_property(vil_property_pyramid, 0);
  if(pyr)
    { gl_map = true; cache = false;}
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
void segv_misc_manager::
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
void segv_misc_manager::
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
  bgui_picker_tableau_sptr picktab = bgui_picker_tableau_new(t2D);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(picktab);
  grid_->add_at(v2D, col, row);
  itab->post_redraw();
}

//: Add an image to the currently selected grid cell
void segv_misc_manager::
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
void segv_misc_manager::remove_image()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  grid_->remove_at(col, row);
}

void segv_misc_manager::convert_to_grey()
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
bgui_image_tableau_sptr segv_misc_manager::selected_image_tab()
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
segv_misc_manager::vtol2D_tab_at(const unsigned col,
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
bgui_vtol2D_tableau_sptr segv_misc_manager::selected_vtol2D_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->vtol2D_tab_at(col, row);
}

//: Get the picker tableau for the currently selected grid cell
bgui_picker_tableau_sptr segv_misc_manager::selected_picker_tab()
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


vil_image_resource_sptr segv_misc_manager::selected_image()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    return 0;
  return itab->get_image_resource();
}

vil_image_resource_sptr segv_misc_manager::image_at(const unsigned col,
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
void segv_misc_manager::clear_display()
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
}

//-----------------------------------------------------------------------------
//: Clear spatial objects from all spatial panes
//-----------------------------------------------------------------------------
void segv_misc_manager::clear_all()
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
segv_misc_manager::draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
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
      vcl_cout << "In segv_misc_manager::draw_edges - null image tab\n";
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
void segv_misc_manager::
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
      vcl_cout << "In segv_misc_manager::draw_polylines - null image tab\n";
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
void segv_misc_manager::
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
      vcl_cout << "In segv_misc_manager::draw_edges - null image tab\n";
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
void segv_misc_manager::
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
      vcl_cout << "In segv_misc_manager::draw_edges - null image tab\n";
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
void segv_misc_manager::
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
      vcl_cout << "In segv_misc_manager::draw_edges - null image tab\n";
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

void segv_misc_manager::draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
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

void segv_misc_manager::quit()
{
  this->clear_all();
  vgui::quit();
}

void segv_misc_manager::load_image()
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

void segv_misc_manager::save_image()
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
      vcl_cerr << "Null image in segv_misc_manager::save_image\n";
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
    vcl_cerr << "segv_misc_manager::save_image operation failed\n";
}
#if 0
void segv_misc_manager::set_range_params()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if(!itab)
    return;
  vgui_range_map_params_sptr rmps = itab->map_params();
  if(!rmps)
    {
      vil_image_resource_sptr img = itab->get_image_resource();
      if(!img)
        return;
      rmps = range_params(img);
      if(!rmps)
        return;
    }
  unsigned nc = rmps->n_components_;
  static double min = static_cast<double>(rmps->min_L_),
    max = static_cast<double>(rmps->max_L_);
  static float gamma = rmps->gamma_L_;
  static bool invert = rmps->invert_;
  static bool gl_map = rmps->use_glPixelMap_;
  static bool cache = rmps->cache_mapped_pix_;
  if(nc==3)
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
  if(nc==1)
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
#endif
//: Transform a polyline according to a rotation and scale in the plane.
static vsol_polyline_2d_sptr 
trans_poly(const double alpha,
           const double theta,
           vsol_polyline_2d_sptr const & base_poly)
{ 
  if(!base_poly)
    return 0;
  double e = vnl_math::e;
  double scale = vcl_pow(e, alpha);
  vcl_vector<vsol_point_2d_sptr> verts, trans_verts;
  for(unsigned i=0; i<base_poly->size(); ++i)
    {
      vsol_point_2d_sptr bv = base_poly->vertex(i);
      double x = bv->x(), y = bv->y();
      double sx = x*scale, sy = y*scale;
      double rsx = sx*vcl_cos(theta)-sy*vcl_sin(theta);
      double rsy = sx*vcl_sin(theta)+sy*vcl_cos(theta);
      vsol_point_2d_sptr tv = new vsol_point_2d(rsx, rsy);
      trans_verts.push_back(tv);
    }
  return new vsol_polyline_2d(trans_verts);
}
//:Generate a specific polyline as 3 sides of a square
static vsol_polyline_2d_sptr base_poly()
{
  vsol_point_2d_sptr p0 = new vsol_point_2d(-100,-100);
  vsol_point_2d_sptr p1 = new vsol_point_2d(-100,+100);
  vsol_point_2d_sptr p2 = new vsol_point_2d(+100,+100);
  vsol_point_2d_sptr p3 = new vsol_point_2d(+100,-100);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p3);   verts.push_back(p2);
  verts.push_back(p1);   verts.push_back(p0);
  return new vsol_polyline_2d(verts);
}
//:the optimum projection onto a one-dimensional sub-manifold of 
// rotation and scale. alpha_hat and theta_hat are the parameters of the
// 1-d sub-manifold, alpha = t*alpha_hat, theta = t*theta_hat.
static double topt(const double alpha_hat, const double theta_hat,
                   const double alpha, const double theta)
{
  double t = alpha_hat*alpha + theta_hat*theta;
  t /= (alpha_hat*alpha_hat + theta_hat*theta_hat);
  return t;
}
// Experiment with projecting a shape onto a manifold
void segv_misc_manager::project_on_subm()
{
  this->clear_display();
  vgui_style_sptr mstyle = vgui_style::new_style(0.5f, 1.0f, 0.25f,
                                               1.0f, 5.0f);
  vgui_style_sptr pstyle = vgui_style::new_style(1.0f, 0.5f, 0.1f,
                                               1.0f, 5.0f);
    static double theta_hat = 0.25*vnl_math::pi;
  static double alpha_hat = 1.0;
  static double theta = 0.3*vnl_math::pi;
  static double alpha = 1.5;
  static double tinc = 0.1;
  static double tmax = 3.0;
  vgui_dialog project_dlg("Projection Parameters");
  project_dlg.field("Theta hat:", theta_hat);
  project_dlg.field("Alpha hat:", alpha_hat);
  project_dlg.field("tinc:", tinc);
  project_dlg.field("tmax:", tmax);
  project_dlg.field("Theta:", theta );
  project_dlg.field("Alpha:", alpha);
  if (!project_dlg.ask())
    return;
  vcl_vector<vsol_polyline_2d_sptr> tpolys;
  vsol_polyline_2d_sptr base = base_poly();
  vsol_polyline_2d_sptr tbase = trans_poly(alpha,theta,base);
  tpolys.push_back(tbase);
  this->draw_polylines(tpolys,mstyle);
  tpolys.clear();
  double to = topt(alpha_hat, theta_hat, alpha, theta);
  double alp = to*alpha_hat;
  double thet = to*theta_hat;
  vsol_polyline_2d_sptr popt = trans_poly(alp, thet, base);
  tpolys.push_back(popt);
  this->draw_polylines(tpolys,pstyle);
  tpolys.clear();
  for(double t = 0; t<=tmax; t+=tinc)
    {
      alp = t*alpha_hat;
      thet = t*theta_hat;
      vsol_polyline_2d_sptr p = trans_poly(alp, thet, base);
      tpolys.push_back(p);
    }
  this->draw_polylines(tpolys);
}  
void segv_misc_manager::project_on_articulation()
{
  this->clear_display();
  vgui_style_sptr mstyle = vgui_style::new_style(0.5f, 1.0f, 0.25f,
                                               1.0f, 5.0f);
  vgui_style_sptr pstyle = vgui_style::new_style(1.0f, 0.5f, 0.1f,
                                               1.0f, 5.0f);
  unsigned njoints = 4;
  static vcl_vector<double> joint_trans(njoints, 0);  
  static double basis_angle = 0.25*vnl_math::pi;
  joint_trans[0] = 0.25*vnl_math::pi;
  joint_trans[1] = 0.25*vnl_math::pi;
  joint_trans[2] = -0.25*vnl_math::pi;
  vgui_dialog project_dlg("Articulation Projection Parameters");
  project_dlg.field("Ang 0:", joint_trans[0]);
  project_dlg.field("Ang 1:", joint_trans[1]);
  project_dlg.field("Ang 2:", joint_trans[2]);
  project_dlg.field("Basis Angle:", basis_angle);
  if (!project_dlg.ask())
    return;
  vcl_vector<double> links(njoints-1, 100.0);
  bvgl_articulated_poly_sptr ap = new bvgl_articulated_poly(njoints, links);
  ap->transform(joint_trans);
  ap->print();

  vcl_vector<vsol_polyline_2d_sptr> tpolys;

  vcl_vector<double> basis(njoints, basis_angle);
  for(double t = - 2.5; t<=2.5; t+=0.25)
    {
      bvgl_articulated_poly_sptr manifold = 
        new bvgl_articulated_poly(njoints,links);
      manifold->sub_manifold_transform(t, basis);
      tpolys.push_back((vsol_polyline_2d*)(manifold.ptr()));
    }
  this->draw_polylines(tpolys);
  tpolys.clear();
  tpolys.push_back((vsol_polyline_2d*)(ap.ptr()));
  this->draw_polylines(tpolys, mstyle);
  //get the projection
  bvgl_articulated_poly_sptr proj = 
    bvgl_articulated_poly::projection(ap, basis);
  tpolys.clear();
  tpolys.push_back((vsol_polyline_2d*)(proj.ptr()));
  this->draw_polylines(tpolys, pstyle);
}
