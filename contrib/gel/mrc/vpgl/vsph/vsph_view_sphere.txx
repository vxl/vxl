#ifndef vsph_view_sphere_txx_
#define vsph_view_sphere_txx_

#include "vsph_view_sphere.h"
#include "vsph_view_point.h"
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_3d.h>


template <class T>
unsigned vsph_view_sphere<T>::add_view(T view)
{
  // generate a new id
  unsigned id = next_id();
  views_[id] = view;

  // make sure that the view point is on the sphere
  vsph_sph_point_3d p = view.view_point();
  coord_sys_->move_point(p);
  view.set_view_point(p);
  return id;
}

template <class T>
unsigned vsph_view_sphere<T>::add_view(vgl_point_3d<double> center)
{
  // check if the point on the sphere
  vsph_sph_point_3d sp;
  coord_sys_->spherical_coord(center, sp);

  T view(sp);
  return add_view(view);
}

template <class T>
void vsph_view_sphere<T>::add_uniform_views(double cap_angle, double point_angle)
{
  // create a octahedron on the sphere, define 6 points for the vertices of the triangles
  double radius = coord_sys_->radius();
  vgl_point_3d<double> center = coord_sys_->origin();
  
  vcl_vector<vgl_point_3d<double> > verts;
  vgl_point_3d<double> v1(center.x(),center.y(),center.z()+radius); verts.push_back(v1);
  vgl_point_3d<double> v2(center.x(),center.y(),center.z()-radius); verts.push_back(v2);
  vgl_point_3d<double> v3(center.x()+radius,center.y(),center.z()); verts.push_back(v3);
  vgl_point_3d<double> v4(-1*radius+center.x(),center.y(),center.z()); verts.push_back(v4);
  vgl_point_3d<double> v5(center.x(),center.y()+radius,center.z()); verts.push_back(v5);
  vgl_point_3d<double> v6(center.x(),center.y()-radius,center.z()); verts.push_back(v6);
  
  // vector of triangles (vector of 3 points, only indices of the vertices kept)
  vcl_vector<vcl_vector<int> > triangles;
  
  vcl_vector<int> tri1;
  tri1.push_back(0); tri1.push_back(2); tri1.push_back(4); triangles.push_back(tri1);
  
  vcl_vector<int> tri2;
  tri2.push_back(0); tri2.push_back(4); tri2.push_back(3); triangles.push_back(tri2);
  
  vcl_vector<int> tri3;
  tri3.push_back(0); tri3.push_back(3); tri3.push_back(5); triangles.push_back(tri3);
  
  vcl_vector<int> tri4;
  tri4.push_back(0); tri4.push_back(5); tri4.push_back(2); triangles.push_back(tri4);
  
  vcl_vector<int> tri5;
  tri5.push_back(1); tri5.push_back(2); tri5.push_back(4); triangles.push_back(tri5);
  
  vcl_vector<int> tri6;
  tri6.push_back(1); tri6.push_back(3); tri6.push_back(4); triangles.push_back(tri6);
  
  vcl_vector<int> tri7;
  tri7.push_back(1); tri7.push_back(5); tri7.push_back(3); triangles.push_back(tri7);
  
  vcl_vector<int> tri8;
  tri8.push_back(1); tri8.push_back(2); tri8.push_back(5); triangles.push_back(tri8);

  // iteratively refine the triangles
  // check the angle between two vertices (of the same triangle),
  // use the center of the spheriacl coordinate system
  vgl_vector_3d<double> vector1=verts[triangles[0][0]]-center; 
  vgl_vector_3d<double> vector2=verts[triangles[0][1]]-center; 
  double a = angle(vector1, vector2);
  
  while (a > point_angle) {
    vcl_vector<vcl_vector<int> >  new_triangles;
    int ntri=triangles.size();
    for (int i=0; i<ntri; i++) {
      vcl_vector<int> points;
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
      
      vcl_vector<int> list(3); list[0]=points[0]; list[1]=points[5]; list[2]=points[1];
      new_triangles.push_back(list);
      list[0]=points[1]; list[1]=points[3]; list[2]=points[2];
      new_triangles.push_back(list);
      list[0]=points[3]; list[1]=points[5]; list[2]=points[4];
      new_triangles.push_back(list);
      list[0]=points[1]; list[1]=points[5]; list[2]=points[3];
      new_triangles.push_back(list);
    }
    // check the angle again to see if the threashold is met
    vgl_vector_3d<double> vector1=verts[new_triangles[0][0]]-center; 
    vgl_vector_3d<double> vector2=verts[new_triangles[0][1]]-center; 
    a = angle(vector1, vector2);
    triangles.clear();
    triangles=new_triangles;
  }

  // refine the vertices to points, eliminate duplicate ones and also eliminate the ones below given elevation
  int ntri=triangles.size();
  for (int i=0; i<ntri; i++) {
    for (int j=0; j<3; j++) {
      vsph_sph_point_3d sv;
      coord_sys_->spherical_coord(verts[triangles[i][j]], sv);
      if (sv.theta_ < cap_angle) {
        int uid;
        double dist;
        T neighb = find_closest(verts[triangles[i][j]],uid,dist);
        if (uid >-1) {
          // add if not already added
          if (dist > 0.0001) {
            T view(sv);
            add_view(view);
          }
        // no views yet, add the first one..
        } else {
          T view(sv);
          add_view(view);
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
  typename vcl_map<unsigned, T>::iterator it = views_.find(id);
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
  typename vcl_map<unsigned, T>::iterator it = views_.begin();
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
void vsph_view_sphere<T>::print(vcl_ostream& os) const
{
  os << "vsph_view_sphere: " << size() << vcl_endl;
  const_iterator it = views_.begin();

  while (it != views_.end()) {
    os << '(' << it->first << ") " << it->second << vcl_endl;
    it++;
  }
  os << vcl_endl;
}

template <class T>
vsph_view_sphere<T>& vsph_view_sphere<T>::operator=(vsph_view_sphere<T> const& rhs)
{
  this->coord_sys_ = new vsph_spherical_coord(*rhs.coord_sys_);
  this->views_ = rhs.views_;
  this->uid_ = rhs.uid_;
  return *this;
}

#define VSPH_VIEW_SPHERE_INSTANTIATE(T) \
template class vsph_view_sphere<T >

#endif  //vsph_view_sphere_txx_
