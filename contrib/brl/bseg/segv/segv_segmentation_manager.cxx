// This is brl/bseg/segv/segv_segmentation_manager.cxx
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
#ifdef HAS_XERCES
#include <bxml/bxml_vtol_io.h>
#endif
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
#include <vgui/vgui_image_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_soview.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <gevd/gevd_float_operators.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <segv/segv_segmentation_manager.h>
//static live_video_manager instance
segv_segmentation_manager *segv_segmentation_manager::instance_ = 0;


segv_segmentation_manager *segv_segmentation_manager::instance()
{
  if (!instance_)
    {
      instance_ = new segv_segmentation_manager();
      instance_->init();
    }
  return segv_segmentation_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
segv_segmentation_manager::segv_segmentation_manager():vgui_wrapper_tableau()
{
}

segv_segmentation_manager::~segv_segmentation_manager()
{
}
//: Set up the tableaux
void segv_segmentation_manager::init()
{
  vgui_image_tableau_sptr itab = bgui_image_tableau_new();
  t2D_ = bgui_vtol2D_tableau_new(itab);
  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(t2D_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2d);
  this->add_child(shell);
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
  t2D_->get_image_tableau()->set_image(img_);
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
      t2D_->add_edge(*eit);
      //optionally display the edge vertices
      if (verts)
        {
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
   for (vcl_vector<vdgl_intensity_face_sptr>::iterator rit = regions.begin();
        rit != regions.end(); rit++)
     {
       vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
       t2D_->add_face(f);
       if (verts)
         {
           vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
           for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
                vit != vts->end(); vit++)
             {
               vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
               t2D_->add_vertex(v);
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
  if (!vd_dialog->ask())
    return;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  bool debug = false;
  sdet_region_proc_params rpp(debug, true, dp);
  sdet_region_proc rp(rpp);
  rp.set_image(img_);
  rp.extract_regions();
  if (debug)
    {
      vil_image ed_img = rp.get_edge_image();
      vgui_image_tableau_sptr itab =  t2D_->get_image_tableau();
      if (!itab)
      {
          vcl_cout << "In segv_segmentation_manager::regions() - null image tableau\n";
          return;
      }
      itab->set_image(ed_img);
    }
  if (!debug)
    {
   vcl_vector<vdgl_intensity_face_sptr>& regions = rp.get_regions();
   this->draw_regions(regions, true);
    }
}

#ifdef HAS_XERCES
void segv_segmentation_manager::read_xml_edges()
{
  vgui_dialog load_image_dlg("Load XML edges");
  static vcl_string xml_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("XML filename:", ext, xml_filename);
  if (!load_image_dlg.ask())
    return;
  vcl_vector<vtol_edge_2d_sptr> edges;
  if (bxml_vtol_io::read_edges(xml_filename, edges))
    this->draw_edges(edges, true);
}
#endif

void segv_segmentation_manager::test_face()
{
  if(!img_)
    return;
  int sx = img_.cols(), sy = img_.rows();
  if(sx<10||sy<10)
    return;
  t2D_->set_foreground(0.0, 1.0, 0.0);
  vsol_point_2d_sptr pa = new vsol_point_2d(1,1);
  vsol_point_2d_sptr pb = new vsol_point_2d(sx-2,1);  
  vsol_point_2d_sptr pc = new vsol_point_2d(sx-2,sy-2);  
  vsol_point_2d_sptr pd = new vsol_point_2d(1,sy-2);
  vsol_curve_2d_sptr cab = new vdgl_digital_curve(*pa, *pb);
  vsol_curve_2d_sptr cbc = new vdgl_digital_curve(*pb, *pc);
  vsol_curve_2d_sptr ccd = new vdgl_digital_curve(*pc, *pd);
  vsol_curve_2d_sptr cda = new vdgl_digital_curve(*pd, *pa);
  vtol_vertex_2d_sptr va = new vtol_vertex_2d(*pa);
  vtol_vertex_2d_sptr vb = new vtol_vertex_2d(*pb);
  vtol_vertex_2d_sptr vc = new vtol_vertex_2d(*pc);
  vtol_vertex_2d_sptr vd = new vtol_vertex_2d(*pd);
  vtol_edge_2d_sptr eab = new vtol_edge_2d(*va, *vb, cab);
  vtol_edge_2d_sptr ebc = new vtol_edge_2d(*vb, *vc, cbc);
  vtol_edge_2d_sptr ecd = new vtol_edge_2d(*vc, *vd, ccd);
  vtol_edge_2d_sptr eda = new vtol_edge_2d(*vd, *va, cda);
  vcl_vector<vtol_edge_sptr> edges;
  edges.push_back(eab->cast_to_edge());   
  edges.push_back(ebc->cast_to_edge());   
  edges.push_back(ecd->cast_to_edge());   
  edges.push_back(eda->cast_to_edge());   
  vtol_one_chain_sptr b_onch = new vtol_one_chain(edges,true);
  vtol_face_2d_sptr b_f = new  vtol_face_2d(*b_onch);
  int px = sx/2, py = sy/2;

  vsol_point_2d_sptr p1 = new vsol_point_2d(px+3,py+3);  
  vsol_point_2d_sptr p2 = new vsol_point_2d(px,py-3);
  vsol_point_2d_sptr p3 = new vsol_point_2d(px-3,py+3);  
  vsol_curve_2d_sptr c12 = new vdgl_digital_curve(*p1, *p2);
  vsol_curve_2d_sptr c23 = new vdgl_digital_curve(*p2, *p3);
  vsol_curve_2d_sptr c31 = new vdgl_digital_curve(*p3, *p1);
  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(*p1);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(*p2);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(*p3);
  vtol_edge_2d_sptr e12 = new vtol_edge_2d(*v1, *v2, c12);
  vtol_edge_2d_sptr e23 = new vtol_edge_2d(*v2, *v3, c23);
  vtol_edge_2d_sptr e31 = new vtol_edge_2d(*v3, *v1, c31);
  edges.clear();
  edges.push_back(e12->cast_to_edge());   
  edges.push_back(e23->cast_to_edge());   
  edges.push_back(e31->cast_to_edge());   
  vtol_one_chain_sptr t_onch = new vtol_one_chain(edges,true);
  vtol_face_2d_sptr t_f = new  vtol_face_2d(*t_onch);
  t2D_->add_face(b_f);   t2D_->add_face(t_f);
  t2D_->set_foreground(1.0, 0.0, 0.0);
  for(int x = 0; x<sx; x+=20)
    for(int y = 0; y<sy; y+=20)
      {
        vtol_vertex_2d_sptr v = new vtol_vertex_2d(x, y);
        t2D_->add_vertex(v);
      }
}
