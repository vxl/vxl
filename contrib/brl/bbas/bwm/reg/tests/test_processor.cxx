// This is brl/bbas/bwm/reg/tests/test_processor.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/vnl_numeric_traits.h>
#include <bwm/reg/bwm_reg_processor.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_rational_camera.h>

static void print_edges(vcl_vector<vsol_digital_curve_2d_sptr> const& edges)
{
  for (unsigned cv = 0; cv<edges.size(); ++cv)
  {
    vsol_digital_curve_2d_sptr dc = edges[cv];
    vcl_cout << "dc[" << cv << "](" << dc->p0()->x()<< ' '
             << dc->p0()->y() << ")(" << dc->p1()->x()<< ' '
             << dc->p0()->y() << ")\n";
  }
}

static
void display_edges(vcl_vector<vsol_digital_curve_2d_sptr> const& edges)
{
  //get the bounds on the model edges
  double dcmin = vnl_numeric_traits<double>::maxval, dcmax = 0;
  double drmin = dcmin, drmax = 0;
  vcl_vector<vsol_digital_curve_2d_sptr>::const_iterator cit =
    edges.begin();
  for (; cit != edges.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x(), r = p->y();
      if (c<dcmin) dcmin = c;
      if (c>dcmax) dcmax = c;
      if (r<drmin) drmin = r;
      if (r>drmax) drmax = r;
    }
  unsigned cmin = static_cast<unsigned>(dcmin);
  unsigned cmax = static_cast<unsigned>(dcmax);
  unsigned rmin = static_cast<unsigned>(drmin);
  unsigned rmax = static_cast<unsigned>(drmax);
  unsigned ncols = cmax - cmin +1;
  unsigned nrows = rmax - rmin +1;
  vcl_vector<vcl_vector<bool> > edge_map(nrows);
  for (unsigned r = 0; r<nrows; ++r)
    edge_map[r]=vcl_vector<bool>(ncols, false);
  cit = edges.begin();
  for (; cit != edges.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x(), r = p->y();
      c-= cmin; r-=rmin;
      unsigned ic=static_cast<unsigned>(c), ir=static_cast<unsigned>(r);
      edge_map[ir][ic] = true;
    }
  for (unsigned r = 0; r<nrows; ++r)
  {
    for (unsigned c = 0; c<ncols; ++c)
      if (edge_map[r][c])
        vcl_cout << '1' ;
      else
        vcl_cout << ' ' ;
    vcl_cout << '\n';
  }
}

void test_processor()
{
#ifdef SanDiegoExists
  vcl_string model_image_file =
    "C:/images/SanDiego/SanDiegoA/po_39942_pan_0000010000/po_39942_pan_0000010000_0.NTF";
  vil_image_resource_sptr model_image =
    vil_load_image_resource(model_image_file.c_str());

  vcl_string model_cam_file = "C:/images/SanDiego/SanDiegoA/po_39942_pan_0000010000/camera/po_39942_pan_0000010000.RPB";
  vpgl_rational_camera<double>* model_cam =
    read_rational_camera<double>(model_cam_file);

  vpgl_rational_camera<double> search_cam = *model_cam;

  double uoff, voff;
  search_cam.image_offset(uoff, voff);
  uoff+= 21; voff+=14;
  search_cam.set_image_offset(uoff, voff);

  vgl_point_3d<double> wpt(-117.157366999, 32.70183, -29.642173);
  vgl_plane_3d<double> wpl(0, 0, 1, 29.642173);

  bwm_reg_processor brp(model_image, *model_cam, wpt, wpl, model_image,
                        search_cam);

  vgl_point_2d<double> model_point(799.1443, 2795.08);

  int tcol, trow;
  bool success = brp.match(model_point, 40, 100, 1e8, tcol, trow);
  delete model_cam;
#endif
}

TESTMAIN(test_processor);
