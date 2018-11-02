// This is brl/bbas/bwm/reg/tests/test_processor.cxx
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_numeric_traits.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <bwm/reg/bwm_reg_processor.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_rational_camera.h>

#if 0 // unused!!
static void print_edges(std::vector<vsol_digital_curve_2d_sptr> const& edges)
{
  for (unsigned cv = 0; cv<edges.size(); ++cv)
  {
    vsol_digital_curve_2d_sptr dc = edges[cv];
    std::cout << "dc[" << cv << "](" << dc->p0()->x()<< ' '
             << dc->p0()->y() << ")(" << dc->p1()->x()<< ' '
             << dc->p0()->y() << ")\n";
  }
}
#endif // 0

#if 0 // unused!!
static void display_edges(std::vector<vsol_digital_curve_2d_sptr> const& edges)
{
  //get the bounds on the model edges
  double dcmin = vnl_numeric_traits<double>::maxval, dcmax = 0;
  double drmin = dcmin, drmax = 0;
  std::vector<vsol_digital_curve_2d_sptr>::const_iterator cit =
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
  std::vector<std::vector<bool> > edge_map(nrows);
  for (unsigned r = 0; r<nrows; ++r)
    edge_map[r]=std::vector<bool>(ncols, false);
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
        std::cout << '1' ;
      else
        std::cout << ' ' ;
    std::cout << '\n';
  }
}
#endif // 0

void test_processor()
{
#ifdef SanDiegoExists
  std::string model_image_file =
    "C:/images/SanDiego/SanDiegoA/po_39942_pan_0000010000/po_39942_pan_0000010000_0.NTF";
  vil_image_resource_sptr model_image =
    vil_load_image_resource(model_image_file.c_str());

  std::string model_cam_file = "C:/images/SanDiego/SanDiegoA/po_39942_pan_0000010000/camera/po_39942_pan_0000010000.RPB";
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
