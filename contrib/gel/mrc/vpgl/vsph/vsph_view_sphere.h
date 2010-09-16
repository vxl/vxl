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

  //: finds the nearest view to the given point already in the saved ones
  // if uid is -1, it is unsuccessful
  T find_closest(unsigned i, int &uid);

  //: finds the nearest view to the given arbitrary point 
  // if uid is -1, it is unsuccessful
  T find_closest(vgl_point_3d<double> p, int &uid);

  //: Iterators
  typedef T* iterator;
  iterator begin() { it_= views_.begin(); return &(it_->second); }
  iterator end() { it_=views_.end(); return 0; }
  iterator next() { it_++; if (it_!=views_.end()) return &(it_->second); else return 0; }

#if 0
  //: const Iterators
  typedef T const* const_iterator;
  const_iterator begin() const { const_it_= views_.begin(); return &(const_it_->second); }
  const_iterator end() const { const_it_=views_.end(); return 0; }
  const_iterator next() const { const_it_++; if (const_it_!=views_.end()) return &(const_it_->second); else return 0; }
#endif

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
  typename vcl_map<unsigned, T>::iterator it_;
  typename vcl_map<unsigned, T>::const_iterator const_it_;
};

#endif