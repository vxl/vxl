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
#include <vgl/vgl_box_3d.h>

#include <vsl/vsl_binary_io.h>

template <class T>
class vsph_view_sphere
{
 public:
  //: default constructor
  vsph_view_sphere() : uid_(0) {}

  //:constructor, creates with a spherical coordinate system
  vsph_view_sphere(vsph_spherical_coord_sptr cs)
    : coord_sys_(cs),uid_(0) { }

  //: constructor, creates a sphere from a bounding box, centered at the box center
  vsph_view_sphere(vgl_box_3d<double> bb, double radius);

  //: copy constructor
  vsph_view_sphere(vsph_view_sphere<T> const& rhs)
    : coord_sys_(rhs.coord_sys_), views_(rhs.views_), uid_(rhs.uid_) {}

  //: destructor
  ~vsph_view_sphere() {}

  //: returning the id of the newly added view
  unsigned add_view(T view);

  //: creates a view point at a given 3d point, it will be translated to the surface of the sphere
  unsigned add_view(vgl_point_3d<double> center);

  //: adds uniformly placed viewpoints on the sphere in the area defined by elevation angle cap_angle, and the viewpoints are at most point_angle apart
  // It uses iterative triangle division on octahedron until the point angle is achieved
  void add_uniform_views(double cap_angle, double point_angle);

  //: removes the view with the given id
  bool remove_view(unsigned id);

  //: returns the number of view points kept
  unsigned size() const { return views_.size(); }

  //: returns the view point associated with unique id (uid), returns false if uid is non-existant
  bool view_point(unsigned uid, T& vp)  const; 

  //: finds the nearest view to the given point already in the saved ones
  // If uid is -1, it is unsuccessful
  T find_closest(unsigned i, int &uid, double& dist);

  //: finds the nearest view to the given arbitrary point
  // If uid is -1, it is unsuccessful
  T find_closest(vgl_point_3d<double> p, int &uid, double& dist);

  //: finds the closest neighbors of the view point in the distance dist and 
  // return them
  void find_neighbors(unsigned uid, vcl_vector<T>& neighbors);

  //: assignment operator
  vsph_view_sphere<T>& operator=(vsph_view_sphere<T> const& rhs);

  //: Iterators
  typedef typename vcl_map<unsigned, T>::iterator iterator;
  iterator begin() { return views_.begin(); }
  iterator end() { return views_.end(); }

  //: Const Iterator
  typedef typename vcl_map<unsigned, T>::const_iterator const_iterator;
  const_iterator begin() const { const_iterator it=views_.begin(); return it; }
  const_iterator end() const   { const_iterator it=views_.end();   return it; }

  void print(vcl_ostream& os) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os) const;

  short version() const { return 1; }

 protected:

  //: spherical coordinate system
  vsph_spherical_coord_sptr coord_sys_;

  //: views are associated with an id, all the view centers are on the sphere (r) of the coordinate system
  vcl_map<unsigned, T> views_;

 private:
  //: unique id for the map
  unsigned uid_;

  unsigned next_id() { return uid_++; }

  //: returns true if all the the angles between vertices of a triangle is smaller than angle
  // list.size() should be 3
  bool min_angle(vcl_vector<vgl_point_3d<double> > list, double angle);
};

template <class T>
void vsl_b_read(vsl_b_istream& is, vsph_view_sphere<T>& vs);

template <class T>
void vsl_b_write(vsl_b_ostream& os, vsph_view_sphere<T> const& vs);

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, vsph_view_sphere<T> const& vs);

#endif
