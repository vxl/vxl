// This is brl/bseg/segv/segv_segmentation_manager.cxx
#include "segv_segmentation_manager.h"
//:
// \file
// \author J.L. Mundy

#include <math.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vil/vil_load.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <bxml/bxml_vtol_io.h>
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
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <segv/segv_vtol2D_tableau.h>

//static live_video_manager instance
segv_segmentation_manager *segv_segmentation_manager::instance_ = 0;


segv_segmentation_manager *segv_segmentation_manager::instance()
{
  if (!instance_)
    instance_ = new segv_segmentation_manager();
  return segv_segmentation_manager::instance_;
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
  img_ = vil_load(image_filename.c_str());
  vgui_image_tableau_sptr itab = vgui_image_tableau_new(img_);
  //  e2D_ = vgui_easy2D_tableau_new(itab);
  t2D_ = segv_vtol2D_tableau_new(itab);
  //vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(e2D_);
  vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(t2D_);
  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(stab);
  unsigned col=0, row=0;
  this->add_at(v2d, col, row);
}

//-----------------------------------------------------------------------------
//: Clear the display
//-----------------------------------------------------------------------------
void segv_segmentation_manager::clear_display()
{
  if (!t2D_)
    return;
  t2D_->clear();
}

//-----------------------------------------------------------------------------
//: Draw edges onto the tableau
//-----------------------------------------------------------------------------
void
segv_segmentation_manager::draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
                                      bool verts)
{
  if (!t2D_)
    return;
  t2D_->set_line_width(3.0);
  t2D_->set_point_radius(5.0);
  this->clear_display();
  vgui_image_tableau_sptr itab = t2D_->get_image_tableau();
  if (!itab)
    {
      vcl_cout << "In segv_segmentation_manager::draw_edges - null image tab"
               << vcl_endl;
      return;
    }
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      t2D_->set_foreground(0.5,0.5,0.0);
      t2D_->add_edge(*eit);
      //optionally display the edge vertices
      if (verts)
        {
          t2D_->set_foreground(1.0,0.0,0.0);
          if ((*eit)->v1())
            {
              vtol_vertex_2d_sptr v1 =
                (vtol_vertex_2d*)(*eit)->v1()->cast_to_vertex_2d();
              t2D_->add_vertex(v1);
            }
          if ((*eit)->v2())
            {
              vtol_vertex_2d_sptr v2 =
                (vtol_vertex_2d*)(*eit)->v2()->cast_to_vertex_2d();
              t2D_->add_vertex(v2);
            }
        }
    }
  t2D_->post_redraw();
}

void segv_segmentation_manager::draw_regions(vcl_vector<vdgl_intensity_face_sptr>& regions,
											 bool verts)
{
   for(vcl_vector<vdgl_intensity_face_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
     {
       vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
       _t2D->set_foreground(0.0, 1.0, 0.0);
       _t2D->add_face(f);
       if(verts)
         {
           vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
           for(vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
               vit != vts->end(); vit++)
             {
               vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
               _t2D->set_foreground(1.0, 0, 0.0);
               _t2D->add_vertex(v);
             }
		delete vts;
         }
     }
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
  if (!vd_dialog->ask())
    return;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  sdet_detector det(dp);
  det.SetImage(img_);
  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  this->draw_edges(*edges, true);
  //display test verts
  vcl_vector<vtol_vertex_2d_sptr> test_verts = det.get_test_verts();
  t2D_->set_foreground(0.0,1.0,0.0);
  for (vcl_vector<vtol_vertex_2d_sptr>::iterator vit = test_verts.begin();
      vit != test_verts.end(); vit++)
    if ((*vit))
      t2D_->add_vertex(*vit);
}

void segv_segmentation_manager::regions()
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
  bool debug = false;
  sdet_region_proc_params rpp(debug, true, dp);
  sdet_region_proc rp(rpp);
  rp.set_image(_img);
  rp.extract_regions();
  if(debug)
    {
      vil_image ed_img = rp.get_edge_image();
      vgui_image_tableau_sptr itab =  _t2D->get_image_tableau();
	  if(!itab)
	  {
		  vcl_cout << "In segv_segmentation_manager::regions() - null image tableau"
			  << vcl_endl;
		  return;
	  }
      itab->set_image(ed_img);
    }
  if(!debug)
    {
   vcl_vector<vdgl_intensity_face_sptr>& regions = rp.get_regions();
   this->draw_regions(regions, true);
    }
}

void segv_segmentation_manager::read_xml_edges()
{
  vgui_dialog load_image_dlg("Load XML edges");
  static vcl_string xml_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("XML filename:", ext, xml_filename);
  if (!load_image_dlg.ask())
    return;
  vcl_vector<vtol_edge_2d_sptr> edges;
  if(bxml_vtol_io::read_edges(xml_filename, edges))
    this->draw_edges(edges, true);
}
