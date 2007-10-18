#include "bwm_algo.h"

#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_point_3d.h>

//#include <vpgl/vpgl_proj_camera.h>
//#include <vpgl/vpgl_rational_camera.h>

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

//: Finds the center of the sphere with r that is tangent to the given three
// planes
vgl_point_3d<double> bwm_algo::fit_sphere_to_corner(vgl_point_3d<double> P1, vgl_vector_3d<double> N1,
                                          vgl_point_3d<double> P2, vgl_vector_3d<double> N2,
                                          vgl_point_3d<double> P3, vgl_vector_3d<double> N3,
                                          double r)
{
  

  vgl_vector_3d<double> v1 = vgl_vector_3d<double> (P1.x(), P1.y(), P1.z()) +  (r*N1);
  vgl_point_3d<double> p1(v1.x(), v1.y(), v1.z());
  vgl_homg_plane_3d<double> plane1(N1, vgl_homg_point_3d<double>(p1));

  vgl_vector_3d<double> v2 = vgl_vector_3d<double> (P2.x(), P2.y(), P2.z()) +  (r*N2);
  vgl_point_3d<double> p2(v2.x(), v2.y(), v2.z());
  vgl_homg_plane_3d<double> plane2(N2, vgl_homg_point_3d<double>(p2));

  vgl_vector_3d<double> v3 = vgl_vector_3d<double> (P3.x(), P3.y(), P3.z()) +  (r*N3);
  vgl_point_3d<double> p3(v3.x(), v3.y(), v3.z());
  vgl_homg_plane_3d<double> plane3(N3, vgl_homg_point_3d<double>(p3));

  vgl_homg_point_3d<double> Q = vgl_homg_operators_3d<double>::intersection(plane1, plane2, plane3);
  
  return (vgl_point_3d<double> (Q.x()/Q.w(), Q.y()/Q.w(), Q.z()/Q.w()));
}
