#ifndef vsph_view_sphere_txx_
#define vsph_view_sphere_txx_

#include "vsph_view_sphere.h"
#include <vgl/vgl_distance.h>

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
bool vsph_view_sphere<T>::remove_view(unsigned id)
{
  int num =views_.erase(id);
  // check if it exists, so it should return 1
  if (num == 1)
    return true;
  return false;
}

template <class T>
T vsph_view_sphere<T>::find_closest(unsigned id, int& uid)
{
  vcl_map<unsigned, T>::iterator it = views_.find(id);
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
  }
  if (uid > -1)
    return views_[uid];
  else 
    return T();  
}

template <class T>
T vsph_view_sphere<T>::find_closest(vgl_point_3d<double> p, int &uid) 
{
  vcl_map<unsigned, T>::iterator it = views_.begin();
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
  if (uid > -1)
    return views_[uid];
  else 
    return T();  
}

template <class T>
void vsph_view_sphere<T>::print(vcl_ostream& os) const
{
  os << "vsph_view_sphere: " << size() << "\n";
  vcl_map<unsigned, T>::const_iterator it = views_.begin();
  
  while (it != views_.end()) {
    os << "(" << it->first << ") " << it->second << vcl_endl;
    it++;
  }
  os << vcl_endl;
}
#if 0
template <class T> 
vcl_ostream& operator<<(vcl_ostream& os, vsph_view_sphere<T> const& vs)
{
  vs.print(os);
  return os;
}
#endif
#define VSPH_VIEW_SPHERE_INSTANTIATE(T) \
template class vsph_view_sphere<T>
//template vcl_ostream& operator<<(vcl_ostream&, vsph_view_sphere<T> const&)

#endif  //vsph_view_sphere_txx_