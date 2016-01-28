#include "boxm2_vecf_orbit_tableau.h"
#include "../boxm2_vecf_plot_orbit.h"
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vgui/vgui_style.h>
#include <vil/vil_load.h>
// : Set up the tableaux
void boxm2_vecf_orbit_tableau::init()
{
  img_tab_ = bgui_image_tableau_new();
  vsol_tab_ = bgui_vsol2D_tableau_new(img_tab_);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(vsol_tab_);
  vgui_shell_tableau_sptr    shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
}

bool boxm2_vecf_orbit_tableau::set_image(vcl_string const& image_path)
{
  vil_image_resource_sptr res = vil_load_image_resource(image_path.c_str() );

  if( !res )
    {
    return false;
    }
  img_tab_->set_image_resource(res);
  return true;
}

void boxm2_vecf_orbit_tableau::set_params(vcl_string const& param_path, bool is_right)
{
  vcl_ifstream istr(param_path.c_str() );

  if( is_right )
    {
    istr >> right_params_;
    }
  else
    {
    istr >> left_params_;
    }
}

bool boxm2_vecf_orbit_tableau::set_dlib_parts(vcl_string const& dlib_path)
{
  return fo_.read_dlib_part_file(dlib_path);
}

void boxm2_vecf_orbit_tableau::draw_orbit(bool is_right)
{
  // get parameter bounds and model to image transformation parameters
  boxm2_vecf_orbit_params params = left_params_;

  bool           export_points = export_fname_base_ == "" ? false : true;
  vcl_ofstream * sup_points = 0;
  vcl_ofstream * inf_points = 0;
  vcl_ofstream * cre_points = 0;

  if( export_points )
    {
    vcl_string sup_fname = is_right ? export_fname_base_ + "/right_sup_points.txt" : export_fname_base_
      + "/left_sup_points.txt";
    vcl_string inf_fname = is_right ? export_fname_base_ + "/right_inf_points.txt" : export_fname_base_
      + "/left_inf_points.txt";
    vcl_string cre_fname = is_right ? export_fname_base_ + "/right_cre_points.txt" : export_fname_base_
      + "/left_cre_points.txt";
    sup_points = new vcl_ofstream(sup_fname.c_str() );
    inf_points = new vcl_ofstream(inf_fname.c_str() );
    cre_points = new vcl_ofstream(cre_fname.c_str() );
    if( !sup_points->is_open() )
      {
      vcl_cout << "point file" << sup_fname << " is not open" << vcl_endl;
      export_points = false;
      }
    else
      {
      vcl_cout << "Exporting superior margin points into " << sup_fname << vcl_endl;
      }

    if( !inf_points->is_open() )
      {
      vcl_cout << "point file" << inf_fname << " is not open" << vcl_endl;
      export_points = false;
      }
    else
      {
      vcl_cout << "Exporting inferior margin points into " << inf_fname << vcl_endl;
      }

    if( !cre_points->is_open() )
      {
      vcl_cout << "point file" << cre_fname << " is not open" << vcl_endl;
      export_points = false;
      }
    else
      {
      vcl_cout << "Exporting crease points into " << cre_fname << vcl_endl;
      }
    }
  if( is_right )
    {
    params = right_params_;
    }
  double xm_min = params.x_min() - 10.0;
  double xm_max = params.x_max() + 10.0;
  double xtr = params.x_trans();
  double ytr = params.y_trans();
  double mm_per_pix = params.mm_per_pix_;
  double image_height = params.image_height_;
  // scan the margin and crease polynomial curves
  vcl_vector<vgl_point_3d<double> > crease_pts, inf_pts, sup_pts;
  boxm2_vecf_plot_orbit::plot_inferior_margin(params, is_right, xm_min, xm_max, inf_pts);
  boxm2_vecf_plot_orbit::plot_superior_margin(params, is_right, xm_min, xm_max, sup_pts);
  boxm2_vecf_plot_orbit::plot_crease(params, is_right, xm_min, xm_max, crease_pts);
  int  imin = -1, imax = -1;
  bool good = boxm2_vecf_plot_orbit::plot_limits(inf_pts, sup_pts, imin, imax);
  if( !good )
    {
    vcl_cout << "determine plot limits failed \n";
    return;
    }
  // shouldn't happen but just in case...
  if( imin == -1 ) {imin = 0; }
  if( imax == -1 ) {imax = static_cast<int>(inf_pts.size() ); }

  // prepare vsol polyline curves for tableau display
  vcl_vector<vsol_point_2d_sptr> vsol_pts;
  for( int i = imin; i <= imax; ++i )
    {
    double x = crease_pts[i].x();
    double y = crease_pts[i].y();
    // convert to image coordinates
    x = (x + xtr) / mm_per_pix;
    y = (y + ytr) / mm_per_pix;
    y = image_height - y;
    vsol_pts.push_back(new vsol_point_2d(x, y) );
    if( export_points )
      {
      *cre_points << x << " " << y << vcl_endl;
      }
    }
  vsol_polyline_2d_sptr cre_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(cre_pline);

  vsol_pts.clear();
  for( int i = imin; i <= imax; ++i )
    {
    // convert to image coordinates
    double x = inf_pts[i].x();
    double y = inf_pts[i].y();
    x = (x + xtr) / mm_per_pix;
    y = (y + ytr) / mm_per_pix;
    y = image_height - y;
    vsol_pts.push_back(new vsol_point_2d(x, y) );
    if( export_points )
      {
      *inf_points << x << " " << y << vcl_endl;
      }
    }
  vsol_polyline_2d_sptr sup_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(sup_pline);

  vsol_pts.clear();
  for( int i = imin; i <= imax; ++i )
    {
    double x = sup_pts[i].x();
    double y = sup_pts[i].y();
    // convert to image coordinates
    x = (x + xtr) / mm_per_pix;
    y = (y + ytr) / mm_per_pix;
    y = image_height - y;
    vsol_pts.push_back(new vsol_point_2d(x, y) );
    if( export_points )
      {
      *sup_points << x << " " << y << vcl_endl;
      }
    }
  vsol_polyline_2d_sptr inf_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(inf_pline);
  vsol_tab_->post_redraw();
  if( export_points )
    {
    sup_points->close();
    inf_points->close();
    cre_points->close();
    delete sup_points;
    delete inf_points;
    delete cre_points;
    }

}

void boxm2_vecf_orbit_tableau::draw_dlib_parts(bool is_right)
{
  vgui_style_sptr lat_style    = vgui_style::new_style(0.0f, 1.0f, 0.0f, 7.5f, 1.0f);
  vgui_style_sptr med_style    = vgui_style::new_style(1.0f, 0.0f, 1.0f, 7.5f, 1.0f);
  vgui_style_sptr inf_style    = vgui_style::new_style(1.0f, 1.0f, 0.0f, 7.5f, 1.0f);
  vgui_style_sptr sup_style    = vgui_style::new_style(1.0f, 0.0f, 0.0f, 7.5f, 1.0f);
  vgui_style_sptr crease_style = vgui_style::new_style(0.0f, 1.0f, 1.0f, 7.5f, 1.0f);

  boxm2_vecf_orbit_params params = left_params_;
  vcl_string              mcs = "left_eye_medial_canthus";
  vcl_string              lcs = "left_eye_lateral_canthus";
  vcl_string              infs = "left_eye_inferior_margin";
  vcl_string              sups = "left_eye_superior_margin";
  vcl_string              creases = "left_eye_superior_crease";

  if( is_right )
    {
    params = right_params_;
    mcs = "right_eye_medial_canthus";
    lcs = "right_eye_lateral_canthus";
    infs = "right_eye_inferior_margin";
    sups = "right_eye_superior_margin";
    creases = "right_eye_superior_crease";
    }
  double                            image_height = params.image_height_;
  vgl_point_3d<double>              mc;
  vgl_point_3d<double>              lc;
  vcl_vector<vgl_point_3d<double> > inf_pts;
  vcl_vector<vgl_point_3d<double> > sup_pts;
  vcl_vector<vgl_point_3d<double> > crease_pts;
  bool                              good = fo_.lab_point(mcs, mc);
  if( !good )
    {
    vcl_cout << "no " + mcs << '\n';
    return;
    }
  good = fo_.lab_point(lcs, lc);
  if( !good )
    {
    vcl_cout << "no " + lcs << '\n';
    return;
    }
  inf_pts = fo_.orbit_data(infs);
  if( !inf_pts.size() )
    {
    vcl_cout << "no " + infs << '\n';
    return;
    }
  sup_pts = fo_.orbit_data(sups);
  if( !sup_pts.size() )
    {
    vcl_cout << "no " + sups << '\n';
    return;
    }
  crease_pts = fo_.orbit_data(creases);
  if( !crease_pts.size() )
    {
    vcl_cout << "no " + creases << '\n';
    return;
    }
  // now draw the dlib points
  vsol_point_2d_sptr plc = new vsol_point_2d(lc.x(), lc.y() );
  vsol_tab_->add_vsol_point_2d(plc, lat_style);

  vsol_point_2d_sptr pmc = new vsol_point_2d(mc.x(), mc.y() );
  vsol_tab_->add_vsol_point_2d(pmc, med_style);
  for( vcl_vector<vgl_point_3d<double> >::iterator iit =  inf_pts.begin();
       iit !=  inf_pts.end(); ++iit )
    {
    vsol_point_2d_sptr p = new vsol_point_2d(iit->x(), iit->y() );
    vsol_tab_->add_vsol_point_2d(p, inf_style);
    }
  for( vcl_vector<vgl_point_3d<double> >::iterator sit =  sup_pts.begin();
       sit !=  sup_pts.end(); ++sit )
    {
    vsol_point_2d_sptr p = new vsol_point_2d(sit->x(), sit->y() );
    vsol_tab_->add_vsol_point_2d(p, sup_style);
    }
  for( vcl_vector<vgl_point_3d<double> >::iterator cit =  crease_pts.begin();
       cit !=  crease_pts.end(); ++cit )
    {
    vsol_point_2d_sptr p = new vsol_point_2d(cit->x(), cit->y() );
    vsol_tab_->add_vsol_point_2d(p, crease_style);
    }
}
