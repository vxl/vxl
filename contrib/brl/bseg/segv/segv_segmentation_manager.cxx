//this-sets-emacs-to-*-c++-*-mode

//:
// \file
// \author J.L. Mundy

#include <math.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_load.h>
#include <vsol/vsol_curve_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <gevd/gevd_clean_edgels.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <gevd/gevd_float_operators.h>
#include <segv/segv_vtol2D_tableau.h>
#include <segv/segv_segmentation_manager.h>
//static live_video_manager instance
segv_segmentation_manager *segv_segmentation_manager::_instance = 0;


segv_segmentation_manager *segv_segmentation_manager::instance()
{
  if (!_instance)
    _instance = new segv_segmentation_manager();
  return segv_segmentation_manager::_instance;
}

//-----------------------------------------------------------
// constructors/destructor
//
segv_segmentation_manager::
segv_segmentation_manager() : vgui_grid_tableau(1,1)
{
  this->set_grid_size_changeable(true);
}
segv_segmentation_manager::~segv_segmentation_manager()
{
  //this->clear_display();
}

//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool segv_segmentation_manager::handle(const vgui_event &e)
{
  return vgui_grid_tableau::handle(e);
}
void segv_segmentation_manager::quit()
{
  vcl_exit(1);
}

void segv_segmentation_manager::load_image()
{
  bool greyscale = false;
  vgui_dialog load_image_dlg("Load image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;
  _img = vil_load(image_filename.c_str());
  vgui_image_tableau_sptr itab = vgui_image_tableau_new(_img);
  //  _e2D = vgui_easy2D_tableau_new(itab);
  _t2D = segv_vtol2D_tableau_new(itab);
  //vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(_e2D);
  vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(_t2D);
  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(stab);
  unsigned col=0, row=0;
  this->add_at(v2d, col, row);
}
//-----------------------------------------------------------------------------
//: Clear the display
//-----------------------------------------------------------------------------
void segv_segmentation_manager::clear_display()
{
  if(!_t2D)
    return;
  _t2D->clear();
}
//-----------------------------------------------------------------------------
//: Draw the given edges onto the given location.
//-----------------------------------------------------------------------------
void 
segv_segmentation_manager::draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
                                      bool verts)
{
  if (!_t2D)
    return;
  _t2D->set_line_width(3.0);
  _t2D->set_point_radius(5.0);
  this->clear_display();
  vgui_image_tableau_sptr itab = _t2D->get_image_tableau();
  if(!itab)
    {
      vcl_cout << "In segv_segmentation_manager::draw_edges - null image tab"
               << vcl_endl;
      return;
    }
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {     
      _t2D->set_foreground(0.5,0.5,0.0);
      _t2D->add_edge(*eit);
      //optionally display the edge vertices
      if(verts)
        {
          _t2D->set_foreground(1.0,0.0,0.0);
          if((*eit)->v1())
            {
              vtol_vertex_2d_sptr v1 =
                (vtol_vertex_2d*)(*eit)->v1()->cast_to_vertex_2d();
              _t2D->add_vertex(v1);
            }
          if((*eit)->v2())
            {	
              vtol_vertex_2d_sptr v2 =
                (vtol_vertex_2d*)(*eit)->v2()->cast_to_vertex_2d();
              _t2D->add_vertex(v2);
            }
        }

    }
  _t2D->post_redraw();
}
void segv_segmentation_manager::vd_edges()
{
  this->clear_display();
  static bool agr = true;
  static sdet_detector_params dp;
  vgui_dialog* vd_dialog = new vgui_dialog("VD Edges");
  vd_dialog->field("Gaussian sigma", dp.smooth);
  vd_dialog->field("Noise Threshold", dp.noise_multiplier);
  vd_dialog->checkbox("Automatic Threshold", dp.automatic_threshold);
  vd_dialog->checkbox("Agressive Closure", agr);
  if(!vd_dialog->ask())
    return;
  if(agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  sdet_detector det(dp);
  det.SetImage(_img);
  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  this->draw_edges(*edges, true);
  //display test verts
  vcl_vector<vtol_vertex_2d_sptr> test_verts = det.get_test_verts();
  _t2D->set_foreground(0.0,1.0,0.0);
  for(vcl_vector<vtol_vertex_2d_sptr>::iterator vit = test_verts.begin();
      vit != test_verts.end(); vit++)
    if((*vit))
      _t2D->add_vertex(*vit);
}

void segv_segmentation_manager::clean_vd_edges()
{
  sdet_detector det(_img);
  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  vcl_vector<vtol_edge_2d_sptr> broken_edges;
  det.corner_angle=70.0;
  // det.DoBreakCorners(*edges, broken_edges);
  gevd_clean_edgels clean;
  vcl_vector<vtol_edge_2d_sptr> clean_edges;
  clean.DoCleanEdgelChains(broken_edges, clean_edges);
  this->draw_edges(clean_edges, true);
}
