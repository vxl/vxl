#include "bwm_algo.h"
//:
// \file
#include <vgl/vgl_closest_point.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

#include <vil/vil_load.h>

//: returns the vertex (x,y) values of a 2D polygon in seperate x and y arrays
void bwm_algo::get_vertices_xy(vsol_polygon_2d_sptr poly2d,
                               float **x, float **y)
{
  int n = poly2d->size();
  *x = (float*) malloc(sizeof(float) * n);
  *y = (float*) malloc(sizeof(float) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = (float) poly2d->vertex(i)->x();
    (*y)[i] = (float) poly2d->vertex(i)->y();
   //vcl_cout << "X=" << poly2d->vertex(i)->x() << " Y=" << poly2d->vertex(i)->y() << vcl_endl;
  }
}

//: returns the vertex (x,y) values of a 2D polygon in seperate x and y arrays
void bwm_algo::get_vertices_xy(vsol_polygon_2d_sptr poly2d,
                               double **x, double **y)
{
  int n = poly2d->size();
  *x = (double*) malloc(sizeof(double) * n);
  *y = (double*) malloc(sizeof(double) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = (double) poly2d->vertex(i)->x();
    (*y)[i] = (double) poly2d->vertex(i)->y();
   //vcl_cout << "X=" << poly2d->vertex(i)->x() << " Y=" << poly2d->vertex(i)->y() << vcl_endl;
  }
}

//: returns the vertex (x,y) values of a 2D polygon in seperate x and y arrays
void bwm_algo::get_vertices_xy(vsol_polyline_2d_sptr poly2d,
                               float **x, float **y)
{
  int n = poly2d->size();
  *x = (float*) malloc(sizeof(float) * n);
  *y = (float*) malloc(sizeof(float) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = (float) poly2d->vertex(i)->x();
    (*y)[i] = (float) poly2d->vertex(i)->y();
   //vcl_cout << "X=" << poly2d->vertex(i)->x() << " Y=" << poly2d->vertex(i)->y() << vcl_endl;
  }
}

void bwm_algo::get_vertices_xyz(vsol_polygon_3d_sptr poly3d,
                                          double **x, double **y, double **z)
{
  int n = poly3d->size();
  *x = (double*) malloc(sizeof(double) * n);
  *y = (double*) malloc(sizeof(double) * n);
  *z = (double*) malloc(sizeof(double) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = poly3d->vertex(i)->x();
    (*y)[i] = poly3d->vertex(i)->y();
    (*z)[i] = poly3d->vertex(i)->z();
    //vcl_cout << i << ' ' << *(poly3d->vertex(i)) << vcl_endl;
  }
}

vsol_polygon_3d_sptr bwm_algo::move_points_to_plane(vsol_polygon_3d_sptr polygon)
{
  vgl_fit_plane_3d<double> fitter;
  for (unsigned i=0; i<polygon->size(); i++) {
    fitter.add_point(polygon->vertex(i)->x(),
      polygon->vertex(i)->y(), polygon->vertex(i)->z());
  }

  vgl_homg_plane_3d<double> plane;

  if (fitter.fit(0.1)) {
  //fitter.fit();

  plane = fitter.get_plane();
  } else {
    vcl_cout << "NO FITTING" << vcl_endl;
    return 0;
  }


  // find the closest point on the plane and replace it for each point
  vcl_vector<vsol_point_3d_sptr> points;
  for (unsigned i=0; i<polygon->size(); i++) {
    vgl_homg_point_3d<double> hp(polygon->vertex(i)->x(),
      polygon->vertex(i)->y(), polygon->vertex(i)->z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    points.push_back(new vsol_point_3d(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
  }
  vsol_polygon_3d_sptr new_polygon = new vsol_polygon_3d(points);
  return new_polygon;
}

vsol_polygon_3d_sptr bwm_algo::move_points_to_plane(vcl_vector<vsol_point_3d_sptr> points)
{
  vgl_fit_plane_3d<double> fitter;
  vcl_cout << "fitting----------------" << vcl_endl;
  for (unsigned i=0; i<points.size(); i++) {
    fitter.add_point(points[i]->x(),
      points[i]->y(), points[i]->z());
    vcl_cout << *(points[i]) << vcl_endl;
  }

  vgl_homg_plane_3d<double> plane;

  if (fitter.fit(1.0)) {
  // fitter.fit();
  plane = fitter.get_plane();
  } else {
    vcl_cout << "NO FITTING" << vcl_endl;
    return 0;
  }


  // find the closest point on the plane and replace it for each point
  vcl_vector<vsol_point_3d_sptr> new_points;
  for (unsigned i=0; i<points.size(); i++) {
    vgl_homg_point_3d<double> hp(points[i]->x(), points[i]->y(), points[i]->z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    new_points.push_back(new vsol_point_3d(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
  }
  vsol_polygon_3d_sptr polygon = new vsol_polygon_3d(new_points);
  return polygon;
}

//: Finds the center of the sphere with radius r that is tangent to the given three planes
vgl_point_3d<double>
bwm_algo::fit_sphere_to_corner(vgl_point_3d<double> P1, vgl_vector_3d<double> N1,
                               vgl_point_3d<double> P2, vgl_vector_3d<double> N2,
                               vgl_point_3d<double> P3, vgl_vector_3d<double> N3,
                               double r)
{
  vgl_vector_3d<double> v1 = vgl_vector_3d<double> (P1.x(), P1.y(), P1.z()) + (r*N1);
  vgl_point_3d<double> p1(v1.x(), v1.y(), v1.z());
  vgl_homg_plane_3d<double> plane1(N1, vgl_homg_point_3d<double>(p1));

  vgl_vector_3d<double> v2 = vgl_vector_3d<double> (P2.x(), P2.y(), P2.z()) + (r*N2);
  vgl_point_3d<double> p2(v2.x(), v2.y(), v2.z());
  vgl_homg_plane_3d<double> plane2(N2, vgl_homg_point_3d<double>(p2));

  vgl_vector_3d<double> v3 = vgl_vector_3d<double> (P3.x(), P3.y(), P3.z()) + (r*N3);
  vgl_point_3d<double> p3(v3.x(), v3.y(), v3.z());
  vgl_homg_plane_3d<double> plane3(N3, vgl_homg_point_3d<double>(p3));

  vgl_homg_point_3d<double> Q = vgl_homg_operators_3d<double>::intersection(plane1, plane2, plane3);

  return (vgl_point_3d<double> (Q.x()/Q.w(), Q.y()/Q.w(), Q.z()/Q.w()));
}

vpgl_rational_camera<double> *
bwm_algo::extract_nitf_camera(vil_image_resource_sptr img)
{
  if (!img)
  {
    vcl_cerr << "Null image in bwm_tableau_mgr::extract_nitf_camera\n";
    return 0;
  }

  vil_nitf2_image* nitf = 0;
  vcl_string format = img->file_format();
  vcl_string prefix = format.substr(0,4);
  if (prefix == "nitf") {
    nitf = (vil_nitf2_image*)img.ptr();
    vpgl_nitf_rational_camera* rpcam = new vpgl_nitf_rational_camera(nitf, true);
    return rpcam;
  } else {
    vcl_cout << "The image is not an NITF" << vcl_endl;
    return 0;
  }
}

vpgl_rational_camera<double> *
bwm_algo::extract_nitf_camera(vcl_string img_path)
{
  vil_image_resource_sptr img_res = vil_load_image_resource(img_path.c_str());
  //vil_image_resource_sptr img_res = load_image(img_path, params);
  return extract_nitf_camera(img_res);
}
