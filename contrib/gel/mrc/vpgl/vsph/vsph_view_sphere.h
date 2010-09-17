#ifndef vsph_view_sphere_h_
#define vsph_view_sphere_h_
//:
// \file
// \brief  3D view sphere contains view points in a spherical coordinate system
// \author Gamze Tunali
//
// \verbatim
//  Modifications
//   2010/09/10 Initial version
// \endverbatim

#include <vcl_map.h>

#include "vsph_spherical_coord_sptr.h"

#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>

template <class T> 
class vsph_view_sphere {
public:
  vsph_view_sphere() : uid_(0) {}
  vsph_view_sphere(vsph_spherical_coord_sptr cs) : coord_sys_(cs),uid_(0) { }
  ~vsph_view_sphere() {}

  //: returning the id of the newly added view 
  unsigned add_view(T view);

  //: creates a view point at a given 3d point, it will be translated to the surface
  // of the sphere
  unsigned add_view(vgl_point_3d<double> center);

  //: removes the view with the given id
  bool remove_view(unsigned id);

  unsigned size() const { return views_.size(); }

  //: finds the nearest view to the given point already in the saved ones
  // if uid is -1, it is unsuccessful
  T find_closest(unsigned i, int &uid);

  //: finds the nearest view to the given arbitrary point 
  // if uid is -1, it is unsuccessful
  T find_closest(vgl_point_3d<double> p, int &uid);

  //: Iterators
  typedef typename vcl_map<unsigned, T>::iterator iterator;
  iterator begin() { return views_.begin(); } 
  iterator end() { return views_.end(); }
  
  //: Const Iterators
  typedef typename vcl_map<unsigned, T>::const_iterator const_iterator;
  const_iterator begin() const { vcl_map<unsigned, T>::const_iterator it=views_.begin(); return it; } 
  const_iterator end() const { vcl_map<unsigned, T>::const_iterator it=views_.end(); return it; }

  void print(vcl_ostream& os) const;

protected:

  //: spherical coordinate system
  vsph_spherical_coord_sptr coord_sys_;

  //: views are associated with an id, all the view centers 
  // are on the sphere (r) of the coordinate system
  vcl_map<unsigned, T> views_;
  
private:
  //: unique id for the map
  unsigned uid_;       

  unsigned next_id() { return uid_++; }

  //: keeps the current location of the iterator
 // typename vcl_map<unsigned, T>::iterator it_;
 // typename vcl_map<unsigned, T>::const_iterator const_it_;
};

template <class T> 
vcl_ostream& operator<<(vcl_ostream& os, vsph_view_sphere<T> const& vs) 
{ 
  vs.print(os); 
  return os; 
}

#endif