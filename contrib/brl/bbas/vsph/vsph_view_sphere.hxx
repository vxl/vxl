#ifndef vsph_view_sphere_hxx_
#define vsph_view_sphere_hxx_

#include "vsph_view_sphere.h"
#include "vsph_view_point.h"
#include "vsph_spherical_coord.h"
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>

template <class T>
vsph_view_sphere<T>::vsph_view_sphere(vgl_point_3d<double> const& center, double radius)
  : coord_sys_(new vsph_spherical_coord(center ,radius)),uid_(0) { }

template <class T>
vsph_view_sphere<T>::vsph_view_sphere(vgl_box_3d<double> bb, double radius)
  : coord_sys_(new vsph_spherical_coord(bb.centroid(),radius)),uid_(0) { }

template <class T>
vgl_point_3d<double> vsph_view_sphere<T>::cart_coord(vsph_sph_point_3d const& vp) const
{
  return coord_sys_->cart_coord(vp);
}

template <class T>
vsph_sph_point_3d vsph_view_sphere<T>::spher_coord(vgl_point_3d<double> const& cp) const
{
  vsph_sph_point_3d sp;
  coord_sys_->spherical_coord(cp, sp);
  return sp;
}

template <class T>
unsigned vsph_view_sphere<T>::add_view(T view, unsigned ni, unsigned nj)
{
  // make sure that the view point is on the sphere
  vsph_sph_point_3d p = view.view_point();
  coord_sys_->move_point(p);
  view.set_view_point(p);

  vpgl_perspective_camera<double>* cam = new vpgl_perspective_camera<double>();
  vgl_point_3d<double> camera_center = coord_sys_->cart_coord(p);
  cam->set_camera_center(camera_center);
  vgl_point_2d<double> pp(ni/2., nj/2.);
  vpgl_calibration_matrix<double> K(1871.0,pp);
  cam->set_calibration(K);
  vgl_vector_3d<double> up(0.0, 1.0, 0.0);
  if (std::fabs(p.theta_)<1.0e-3)
    cam->look_at(vgl_homg_point_3d<double>(coord_sys_->origin()), up);
  else
    cam->look_at(vgl_homg_point_3d<double>(coord_sys_->origin()));
  view.set_camera(cam);

  // generate a new id
  unsigned id = next_id();
  views_[id] = view;
  return id;
}

template <class T>
bool vsph_view_sphere<T>::view_point(unsigned uid, T*& vp)
{
  typename std::map<unsigned, T>::iterator it = views_.find(uid);
  if (it != views_.end()) {
    vp = &(it->second);
    return true;
  }
  else {
    std::cerr << " vsph_view_sphere<T>::view_point(uid) -- View with ID=" << uid << "does not exist\n";
    return false;
  }
}

template <class T>
unsigned vsph_view_sphere<T>::add_view(vgl_point_3d<double> center, unsigned ni, unsigned nj)
{
  // convert to spherical coordinates
  vsph_sph_point_3d sp;
  coord_sys_->spherical_coord(center, sp);

  T view(sp);
  return add_view(view,ni,nj);
}


template <class T>
void vsph_view_sphere<T>::add_uniform_views(double cap_angle, double point_angle, unsigned ni, unsigned nj)
{
  // create a octahedron on the sphere, define 6 points for the vertices of the triangles
  double radius = coord_sys_->radius();
  vgl_point_3d<double> center = coord_sys_->origin();

  std::vector<vgl_point_3d<double> > verts;
  vgl_point_3d<double> v1(center.x(),center.y(),center.z()+radius); verts.push_back(v1);
  vgl_point_3d<double> v2(center.x(),center.y(),center.z()-radius); verts.push_back(v2);
  vgl_point_3d<double> v3(center.x()+radius,center.y(),center.z()); verts.push_back(v3);
  vgl_point_3d<double> v4(center.x()-radius,center.y(),center.z()); verts.push_back(v4);
  vgl_point_3d<double> v5(center.x(),center.y()+radius,center.z()); verts.push_back(v5);
  vgl_point_3d<double> v6(center.x(),center.y()-radius,center.z()); verts.push_back(v6);

  // vector of triangles (vector of 3 points, only indices of the vertices kept)
  std::vector<std::vector<int> > triangles;

  std::vector<int> tri1;
  tri1.push_back(0); tri1.push_back(2); tri1.push_back(4); triangles.push_back(tri1);

  std::vector<int> tri2;
  tri2.push_back(0); tri2.push_back(4); tri2.push_back(3); triangles.push_back(tri2);

  std::vector<int> tri3;
  tri3.push_back(0); tri3.push_back(3); tri3.push_back(5); triangles.push_back(tri3);

  std::vector<int> tri4;
  tri4.push_back(0); tri4.push_back(5); tri4.push_back(2); triangles.push_back(tri4);

  std::vector<int> tri5;
  tri5.push_back(1); tri5.push_back(2); tri5.push_back(4); triangles.push_back(tri5);

  std::vector<int> tri6;
  tri6.push_back(1); tri6.push_back(3); tri6.push_back(4); triangles.push_back(tri6);

  std::vector<int> tri7;
  tri7.push_back(1); tri7.push_back(5); tri7.push_back(3); triangles.push_back(tri7);

  std::vector<int> tri8;
  tri8.push_back(1); tri8.push_back(2); tri8.push_back(5); triangles.push_back(tri8);

  // iteratively refine the triangles
  // check the angle between two vertices (of the same triangle),
  // use the center of the spherical coordinate system
  // vgl_vector_3d<double> vector1=verts[triangles[0][0]]-center;
  // vgl_vector_3d<double> vector2=verts[triangles[0][1]]-center;

  bool done=false;
  while (!done) {
    std::vector<std::vector<int> >  new_triangles;
    int ntri=triangles.size();
    for (int i=0; i<ntri; i++) {
      std::vector<int> points;
      for (int j=0; j<3; j++) {
        // find the mid points of edges
        int next=j+1; if (next == 3) next=0;
        vgl_line_segment_3d<double> edge1(verts[triangles[i][j]],verts[triangles[i][next]]);
        vgl_point_3d<double> mid=edge1.point_t(0.5);

        // move the point onto the surface of the sphere
        vsph_sph_point_3d sv;
        coord_sys_->spherical_coord(mid, sv);
        coord_sys_->move_point(sv);
        mid = coord_sys_->cart_coord(sv);

        // add a new vertex for mid points of the edges of the triangle
        int idx = verts.size();
        verts.push_back(mid);

        points.push_back(triangles[i][j]);  // existing vertex of the bigger triangle
        points.push_back(idx);              // new mid-point vertex
      }

      // add new samller 4 triangles instead of the old big one
      /******************************
                   /\
                  /  \
                 /    \
                /      \
               /--------\
              / \      / \
             /   \    /   \
            /     \  /     \
           /       \/       \
           -------------------
      *******************************/
      done=true;
      std::vector<int> list(3); list[0]=points[0]; list[1]=points[5]; list[2]=points[1];
      new_triangles.push_back(list);
      // check for point_angles
      std::vector<vgl_point_3d<double> > triangle;
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle)) done=false;

      list[0]=points[1]; list[1]=points[3]; list[2]=points[2];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle)) done=false;

      list[0]=points[3]; list[1]=points[5]; list[2]=points[4];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle)) done=false;

      list[0]=points[1]; list[1]=points[5]; list[2]=points[3];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle)) done=false;
    }
    // check the angle again to see if the threashold is met
    //vgl_vector_3d<double> vector1=verts[new_triangles[0][0]]-center;
    //vgl_vector_3d<double> vector2=verts[new_triangles[0][1]]-center;
    triangles.clear();
    triangles=new_triangles;
  }

  // refine the vertices to points, eliminate duplicate ones and also eliminate the ones below given elevation
  int ntri=triangles.size();
  for (int i=0; i<ntri; i++) {
    for (int j=0; j<3; j++) {
      vsph_sph_point_3d sv;
      coord_sys_->spherical_coord(verts[triangles[i][j]], sv);
      //if ((sv.theta_ < cap_angle) && (sv.theta_ > 3.0*cap_angle/4.0)) {
      if (sv.theta_ < cap_angle) {
        int uid;
        double dist;
        T neighb = find_closest(verts[triangles[i][j]],uid,dist);
        if (uid >-1) {
          // add if not already added
          if (dist > 0.0001) {
            T view(sv);
            add_view(view,ni,nj);
          }
          // no views yet, add the first one..
        }
        else {
          T view(sv);
          add_view(view,ni,nj);
        }
      }
    }
  }
}

template <class T>
bool vsph_view_sphere<T>::remove_view(unsigned id)
{
  int num =views_.erase(id);
  // check if it exists, so it should return 1
  if (num == 1)
    return true;
  return false;
}

template <class T>
T vsph_view_sphere<T>::find_closest(unsigned id, int& uid, double& dist)
{
  typename std::map<unsigned, T>::iterator it = views_.find(id);
  if (it != views_.end()) {
    vsph_sph_point_3d vp = it->second.view_point();
    vgl_point_3d<double> p = coord_sys_->cart_coord(vp);

    // make sure you do not compare with itself
    double min_dist=1e20;
    uid=-1;
    it = views_.begin();
    while (it != views_.end()) {
      if (it->first != id) {
        vsph_sph_point_3d vp=it->second.view_point();
        vgl_point_3d<double> cp = coord_sys_->cart_coord(vp);
        double dist = vgl_distance(cp,p);
        if (dist < min_dist) {
          min_dist = dist;
          uid = it->first;
        }
      }
      it++;
    }
    dist=min_dist;
  }
  if (uid > -1)
    return views_[uid];
  else
    return T();
}

template <class T>
T vsph_view_sphere<T>::find_closest(vgl_point_3d<double> p, int &uid, double& dist)
{
  typename std::map<unsigned, T>::iterator it = views_.begin();
  double min_dist=1e20;
  uid=-1;

  while (it != views_.end()) {
    vsph_sph_point_3d vp=it->second.view_point();
    vgl_point_3d<double> cp = coord_sys_->cart_coord(vp);
    double dist = vgl_distance(cp,p);
    if (dist < min_dist) {
      min_dist = dist;
      uid = it->first;
    }
    it++;
  }
  dist=min_dist;
  if (uid > -1)
    return views_[uid];
  else
    return T();
}

template <class T>
void vsph_view_sphere<T>::print(std::ostream& os) const
{
  os << "vsph_view_sphere: " << size() << std::endl;
  const_iterator it = views_.begin();

  while (it != views_.end()) {
    os << '(' << it->first << ") " << it->second << std::endl;
    it++;
  }
  os << std::endl;
}

template <class T>
void vsph_view_sphere<T>::
print_relative_cams(vpgl_camera_double_sptr const& target_cam,
                    double distance_thresh)
{
  vpgl_perspective_camera<double>* pcam=
    dynamic_cast<vpgl_perspective_camera<double>*>(target_cam.as_pointer());
  typename std::map<unsigned, T>::iterator it = views_.begin();
  vgl_point_3d<double> t = pcam->camera_center();
  while (it != views_.end()) {
    vgl_rotation_3d<double> rel_rot;
    vgl_vector_3d<double> rel_trans;
    unsigned vp_id = it->first;
    vsph_sph_point_3d vp = (it->second).view_point();
    vgl_point_3d<double> p_3d = this->cart_coord(vp);
    vgl_vector_3d<double> vv(p_3d-t);
    if (vv.length() < distance_thresh) {
      (it->second).relative_transf(target_cam, rel_rot, rel_trans);
      vnl_vector_fixed<double, 3> rod = rel_rot.as_rodrigues();
      const double deg_per_rad = vnl_math::deg_per_rad;
      std::cout <<"***************************************\n"
               << "Viewpoint " << vp_id << '\n'
               << vp.theta_*deg_per_rad << ' ' << vp.phi_*deg_per_rad<< '\n'
               << "Rel Rot[ " << rod.magnitude() <<" ]= " << rod << '\n'
               << "Rel trans[ "<< rel_trans.length()<<" ]= "<< rel_trans << '\n'
               <<"***************************************" << std::endl;
    }
    it++;
  }
}

template <class T>
void vsph_view_sphere<T>::find_neighbors(unsigned id, std::vector<T >& neighbors)
{
  typename std::map<unsigned, T>::iterator it = views_.find(id);
  int closest_uid=-1;
  std::map<unsigned,double> distances;
  double min_dist=1e20;

  if (it != views_.end()) {
    vsph_sph_point_3d vp = it->second.view_point();
    vgl_point_3d<double> p = coord_sys_->cart_coord(vp);

    // make sure you do not compare with itself
    it = views_.begin();
    while (it != views_.end()) {
      if (it->first != id) {
        vsph_sph_point_3d vp=it->second.view_point();
        vgl_point_3d<double> cp = coord_sys_->cart_coord(vp);
        double dist = vgl_distance(cp,p);
        distances[it->first] = dist;
        if (dist < min_dist) {
          min_dist = dist;
          closest_uid = it->first;
        }
      }
      it++;
    }
  }
  if (closest_uid > -1) {
    // examine the list of distances collected, add the ones close enough to the resulting vector
    std::map<unsigned,double>::iterator it = distances.begin();
    double threshold=min_dist/3.0;
    while (it != distances.end()) {
      double diff = it->second - min_dist; // this should be positive
      if (diff < threshold) {
        neighbors.push_back(views_[it->first]);
      }
      it++;
    }
  }

  // else, an empty list will be returned
}

template <class T>
vsph_view_sphere<T>& vsph_view_sphere<T>::operator=(vsph_view_sphere<T> const& rhs)
{
  if (this == &rhs)
    return *this;

  this->coord_sys_ = new vsph_spherical_coord(*rhs.coord_sys_);
  this->views_ = rhs.views_;
  this->uid_ = rhs.uid_;
  return *this;
}

template <class T>
bool vsph_view_sphere<T>::min_angle(std::vector<vgl_point_3d<double> > list, double point_angle)
{
  if (list.size() < 2)
    return false;

  vgl_point_3d<double> center = coord_sys_->origin();
  for (unsigned i=0; i<list.size(); i++) {
    unsigned next = i+1;
    if (next == list.size()) next = 0;
    vgl_vector_3d<double> vector1=list[i]-center;
    vgl_vector_3d<double> vector2=list[next]-center;
    if (angle(vector1, vector2) > point_angle)
      return false;
  }

  return true;
}

template <class T>
void vsph_view_sphere<T>::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
  switch (version) {
   case 1:
    {
      if (!coord_sys_) coord_sys_ = new vsph_spherical_coord();
      coord_sys_->b_read(is);
      unsigned size, uid;
      T view;
      vsl_b_read(is, size);
      for (unsigned i=0; i<size; i++) {
        vsl_b_read(is, uid);
        view.b_read(is);
        views_[uid] = view;
      }
      vsl_b_read(is, uid_);
      break;
    }
   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vsph_view_sphere<T>&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

template <class T>
void vsph_view_sphere<T>::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  coord_sys_->b_write(os);

  typename std::map<unsigned, T>::const_iterator it = views_.begin();

  // write each view point
  vsl_b_write(os, size());
  while (it != views_.end()) {
    unsigned uid=it->first;
    T vp=it->second;
    vsl_b_write(os, uid);
    vp.b_write(os);
    ++it;
  }
  vsl_b_write(os, uid_);
}

template <class T>
void vsl_b_read(vsl_b_istream& is, vsph_view_sphere<T>& vs)
{
  vs.b_read(is);
}

template <class T>
void vsl_b_write(vsl_b_ostream& os, vsph_view_sphere<T> const& vs)
{
  vs.b_write(os);
}

template <class T>
std::ostream& operator<<(std::ostream& os, vsph_view_sphere<T> const& vs)
{
  vs.print(os);
  return os;
}

#define VSPH_VIEW_SPHERE_INSTANTIATE(T) \
template class vsph_view_sphere<T >; \
template void vsl_b_read(vsl_b_istream&, vsph_view_sphere<T >&); \
template void vsl_b_write(vsl_b_ostream&, vsph_view_sphere<T > const&); \
template std::ostream& operator<<(std::ostream&, vsph_view_sphere<T > const&)

#endif  //vsph_view_sphere_hxx_
