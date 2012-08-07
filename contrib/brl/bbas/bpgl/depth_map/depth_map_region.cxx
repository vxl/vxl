#include "depth_map_region.h"
#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/vgl_closest_point.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vcl_cassert.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vpgl/algo/vpgl_ray.h>
#include <bpgl/bpgl_camera_utils.h>

// specified and fixed plane
vsol_polygon_3d_sptr depth_map_region::
region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d,
                vgl_plane_3d<double> const& region_plane,
                vpgl_perspective_camera<double> const& cam)
{
  unsigned nverts = region_2d->size();
  vcl_vector<vsol_point_3d_sptr> verts_3d;
  for (unsigned i = 0; i<nverts; ++i){
    vgl_point_2d<double> pimg = region_2d->vertex(i)->get_p();
    vgl_ray_3d<double> ray = cam.backproject_ray(pimg);
    vgl_point_3d<double> p3d;
    bool success =  vgl_intersection(ray, region_plane, p3d);
    assert(success);
    verts_3d.push_back(new vsol_point_3d(p3d));
  }
  vsol_polygon_3d_sptr poly_3d = new vsol_polygon_3d(verts_3d);
  return poly_3d;
}

vsol_polygon_3d_sptr depth_map_region::
region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d,
                vgl_vector_3d<double> const& region_normal,
                double depth,
                vpgl_perspective_camera<double> const& cam)
{
  unsigned nverts = region_2d->size();
  vgl_point_2d<double> centroid = (region_2d->centroid())->get_p();
  // ray from camera through centroid
  vgl_ray_3d<double> cray = cam.backproject_ray(centroid);
  // check if ray is orthogonal to the region normal, i.e.,
  // can't intersect the region plane with the back-projected centroid ray
  vgl_vector_3d<double> dir = cray.direction();
  double dot = dot_product(region_normal, dir);
  bool ortho = vcl_fabs(dot)<= vgl_tolerance<double>::position;
  assert(!ortho);
  vgl_vector_3d<double> vorg(cray.origin().x(), cray.origin().y(), cray.origin().z());
  //depth is along Y axis not camera ray
  double k = depth/cray.direction().y();
  //but move along camera ray
  vgl_vector_3d<double> p_on_ray = vorg + k*cray.direction();
  double d = -dot_product(p_on_ray, region_normal);
  vgl_plane_3d<double> plane(region_normal.x(),
                             region_normal.y(),
                             region_normal.z(),
                             d);
  vcl_vector<vsol_point_3d_sptr> verts_3d;
  for (unsigned i = 0; i<nverts; ++i){
    vgl_point_2d<double> pimg = region_2d->vertex(i)->get_p();
    vgl_ray_3d<double> ray = cam.backproject_ray(pimg);
    vgl_point_3d<double> p3d;
    bool success =  vgl_intersection(ray, plane, p3d);
    assert(success);
    verts_3d.push_back(new vsol_point_3d(p3d));
  }
  vsol_polygon_3d_sptr poly_3d = new vsol_polygon_3d(verts_3d);
  return poly_3d;
}

depth_map_region::depth_map_region() :
  orient_type_(NOT_DEF), name_(""), depth_(-1.0),
  min_depth_(0.0), max_depth_(vcl_numeric_limits<double>::max()),
  region_2d_(0), region_3d_(0)
{}

depth_map_region::depth_map_region(vsol_polygon_2d_sptr const& region,
                                   vgl_plane_3d<double> const& region_plane,
                                   double min_depth, double max_depth,
                                   vcl_string name,
                                   depth_map_region::orientation orient)
: orient_type_(orient), name_(name), depth_(-1.0),
  min_depth_(min_depth), max_depth_(max_depth),
 region_plane_(region_plane), region_2d_(region), region_3d_(0)
{
}

depth_map_region::depth_map_region(vsol_polygon_2d_sptr const& region,
                                   vgl_plane_3d<double> const& region_plane,
                                   vcl_string name,
                                   depth_map_region::orientation orient)
: orient_type_(orient), name_(name), depth_(-1.0),
  min_depth_(-1.0), max_depth_(-1.0),
 region_plane_(region_plane), region_2d_(region), region_3d_(0)
{
}

depth_map_region::depth_map_region(vsol_polygon_2d_sptr const& region,
                                   vcl_string name)
: orient_type_(INFINITE), name_(name), depth_(vcl_numeric_limits<double>::max()),
  min_depth_(vcl_numeric_limits<double>::max()),
  max_depth_(vcl_numeric_limits<double>::max()),
  region_2d_(region), region_3d_(0)
{
}

void depth_map_region::
set_region_3d(vpgl_perspective_camera<double> const& cam)
{
  region_3d_ =
    depth_map_region::region_2d_to_3d(region_2d_, region_plane_, cam);
}

void depth_map_region::
set_region_3d(double depth, vpgl_perspective_camera<double> const& cam)
{
  vgl_vector_3d<double> normal = region_plane_.normal();
  region_3d_ =
    depth_map_region::region_2d_to_3d(region_2d_, normal, depth, cam);
  depth_ = depth;
  region_plane_ = vgl_plane_3d<double>(region_3d_->plane());
}

static  vgl_vector_2d<double>
vector_to_closest_horizon_pt(vgl_point_2d<double> p,
                             vgl_line_2d<double> horiz)
{
  vgl_point_2d<double> pcp = vgl_closest_point(p, horiz);
  // get vector along positive z axis
  vgl_vector_2d<double> vp = p-pcp;
  return vp;
}

static vgl_point_2d<double>
move_to_depth(vgl_point_2d<double> img_pt, double max_depth,
              vpgl_perspective_camera<double> const& cam,
              vgl_plane_3d<double> region_plane)
{
  vgl_ray_3d<double> ray = cam.backproject_ray(img_pt);
  vgl_point_3d<double> p3d, org = ray.origin();
  bool success =  vgl_intersection(ray, region_plane, p3d);
  assert(success);
  //    double depth = vgl_distance(p3d, org);
  // move p3d along (v3dx,v3dy) so that depth = max_depth
  vgl_vector_3d<double> v3d = p3d-org;
  double depth = v3d.y();
#if 0
  double dd = max_depth*max_depth - depth*depth;
  dd /= (v3d.x()*v3d.x() + v3d.y()*v3d.y());
  double k = -1 + vcl_sqrt(1 + dd);
#endif
  double k = (max_depth - depth)/v3d.y();
  // project into image
  vgl_vector_3d<double> vmove(k*v3d.x(), k*v3d.y(), 0.0);
  vgl_point_3d<double> pmoved_3d = p3d+vmove;
  vgl_point_2d<double> pmoved_2d(cam.project(pmoved_3d));
  return pmoved_2d;
}

bool depth_map_region::
set_ground_plane_max_depth(double max_depth,
                           vpgl_perspective_camera<double> const& cam,
                           double proximity_scale_factor)
{
  if (orient_type_ != GROUND_PLANE)
    return false;
  double tol = vcl_sqrt(vgl_tolerance<double>::position);
  vgl_line_2d<double> horizon = bpgl_camera_utils::horizon(cam);
  vgl_point_2d<double> centroid = (region_2d_->centroid())->get_p();
  vgl_point_2d<double> cpcent = vgl_closest_point(centroid, horizon);
  vgl_vector_2d<double> vc = centroid-cpcent;
  double dc = vc.length();

  // first check if the polygon is entirely above the horizion
  // shouldn't happen
  vgl_point_2d<double> p0(cam.project(vgl_point_3d<double>(0.0, 100.0, 0.0)));
  vgl_point_2d<double> p1(cam.project(vgl_point_3d<double>(0.0, 100.0, 1.0)));
  vgl_vector_2d<double> v01 = p1-p0;
  unsigned nverts = region_2d_->size();
  bool needs_split = false;
  vcl_vector<vsol_point_2d_sptr> pts_above_on;
  vcl_vector<unsigned> pts_closer_than_centroid;
  double min_d = vcl_numeric_limits<double>::max();
  unsigned i_min = 0;
  for (unsigned i = 0; i<nverts; ++i) {
    vgl_point_2d<double> p = region_2d_->vertex(i)->get_p();
    vgl_vector_2d<double> vp = vector_to_closest_horizon_pt(p, horizon);
    double d = vp.length();
    if (d<dc)
      pts_closer_than_centroid.push_back(i);
    if (d <min_d){
      min_d = d;
      i_min = i;
    }
    double dp = dot_product(vp, v01);
    bool above = dp>0.0;
    bool on = vcl_fabs(dp)<tol;
    if (above || on){
      pts_above_on.push_back(region_2d_->vertex(i));
      needs_split = true;
    }
  }
  assert(pts_above_on.size()<nverts);
  // assert didn't happen and no points are above the horizon
  if (needs_split) return false;// don't handle this case yet
  double d_thresh = min_d*proximity_scale_factor;
  for (unsigned i = 0; i<nverts; ++i) {
    vgl_point_2d<double> p = region_2d_->vertex(i)->get_p();
    vgl_vector_2d<double> vp = vector_to_closest_horizon_pt(p, horizon);
    double d = vp.length();
    if (d < d_thresh)
    {
      vgl_point_2d<double> p_moved = move_to_depth(p, max_depth,cam,region_plane_);
      vsol_point_2d_sptr p_ptr = region_2d_->vertex(i);
      p_ptr->set_x(p_moved.x()); p_ptr->set_y(p_moved.y());
    }
  }
  this->set_region_3d(cam);
  return true;
}

bool depth_map_region::
update_depth_image(vil_image_view<double>& depth_image,
                   vpgl_perspective_camera<double> const& cam) const
{
  if (!region_3d_) return false;//depth is not initialized
  vcl_vector<vgl_point_2d<double> > verts_2d;
  unsigned nverts = region_2d_->size();
  for (unsigned i = 0; i<nverts; ++i)
    verts_2d.push_back(region_2d_->vertex(i)->get_p() );

  // since the regions are planar it follows that
  // depth = alpha u + beta v + gamma for image locations within the 2-d region
  // note this computation could be put into set_depth to be more efficient
  double alpha=0.0, beta=0.0, gamma = vcl_numeric_limits<double>::max();
  if (this->orient_type() != INFINITE)
  {
    if (!region_3d_) return false;
    vnl_matrix<double> A(nverts, 3, 1.0);
    vnl_vector<double> b(nverts);
    for (unsigned r = 0; r<nverts; ++r){
      A[r][0]=verts_2d[r].x(); A[r][1]=verts_2d[r].y();
      vgl_point_2d<double> pimg = verts_2d[r];
      vgl_ray_3d<double> ray = cam.backproject_ray(pimg);
      vgl_point_3d<double> p3d, org = ray.origin();
      bool success =  vgl_intersection(ray, region_plane_, p3d);
      assert(success);
      //        b[r] = vgl_distance(p3d, org);
      b[r] = vcl_fabs(p3d.y()-org.y());
    }
    vnl_svd<double> svd(A);
    vnl_vector<double> result = svd.solve(b);
    alpha = result[0]; beta = result[1]; gamma = result[2];
  }
  // now scan the region into the image
  vgl_polygon<double> vpoly(verts_2d);
  //scan convert depths
  int ni = depth_image.ni(), nj = depth_image.nj();
  vgl_polygon_scan_iterator<double> scan(vpoly);
  for (scan.reset(); scan.next(); ) {
    int v = scan.scany();
    for (int u = scan.startx(); u <= scan.endx(); ++u){
      if (u<0||u>=ni||v<0||v>=nj)
        continue;
      depth_image(u, v) = alpha*u + beta*v + gamma;
    }
  }
  return true;
}
