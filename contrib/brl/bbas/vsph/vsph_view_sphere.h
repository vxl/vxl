#ifndef vsph_view_sphere_h_
#define vsph_view_sphere_h_
//:
// \file
// \brief  3D view sphere contains view points in a spherical coordinate system
// \author Gamze Tunali
// \date September 10, 2010
//
// \verbatim
//  Modifications
//   2010/09/10 Initial version
// \endverbatim

#include <vector>
#include <map>
#include <iostream>
#include <iosfwd>
#include "vsph_spherical_coord_sptr.h"
#include "vsph_sph_point_3d.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vsph_view_sphere
{
 public:
  //: default constructor
  vsph_view_sphere() : uid_(0) {}

  //: constructor, creates with a spherical coordinate system
  vsph_view_sphere(vsph_spherical_coord_sptr cs)
    : coord_sys_(cs),uid_(0) { }

  //: constructor, creates a sphere from the center point and radius
  vsph_view_sphere(vgl_point_3d<double> const& center, double radius);

  //: constructor, creates a sphere from a bounding box, centered at the box center
  vsph_view_sphere(vgl_box_3d<double> bb, double radius);



  //: copy constructor
  vsph_view_sphere(vsph_view_sphere<T> const& rhs)
    : coord_sys_(rhs.coord_sys_), views_(rhs.views_), uid_(rhs.uid_) {}

  //: destructor
  ~vsph_view_sphere() = default;

  //: returning the id of the newly added view
  unsigned add_view(T view, unsigned ni, unsigned nj);

  //: creates a view point at a given 3d point.
  //  It will be translated to the surface of the sphere
  unsigned add_view(vgl_point_3d<double> center, unsigned ni, unsigned nj);

  //: adds uniformly placed viewpoints on the sphere in the area defined by elevation angle \a cap_angle, and the viewpoints are at most \a point_angle apart
  // It uses iterative triangle division on octahedron until the point angle is achieved
  void add_uniform_views(double cap_angle, double point_angle, unsigned ni, unsigned nj);

  //: removes the view with the given \a id
  bool remove_view(unsigned id);

  //: returns the number of view points kept
  unsigned size() const { return views_.size(); }

  //: returns the view point associated with unique id \a uid.
  //  Returns false if uid is non-existent
  bool view_point(unsigned uid, T*& vp);

  //: transforms a spherical coordinate to a cartesian coordinate
  vgl_point_3d<double> cart_coord(vsph_sph_point_3d const& vp) const;

  //: transforms a cartesian coordinate to a spherical coordinate
  vsph_sph_point_3d spher_coord(vgl_point_3d<double> const& cp) const;

  //: finds the nearest view to the i-th saved point
  // If the returned \a uid is -1, the search is unsuccessful
  T find_closest(unsigned i, int &uid, double& dist);

  //: finds the nearest view to the given arbitrary point \a p.
  // If the returned \a uid is -1, the search is unsuccessful
  T find_closest(vgl_point_3d<double> p, int &uid, double& dist);

  //: finds the closest neighbors of the view point \a uid and returns them in the second argument
  void find_neighbors(unsigned uid, std::vector<T>& neighbors);

  //: assignment operator
  vsph_view_sphere<T>& operator=(vsph_view_sphere<T> const& rhs);

  //: Iterator
  typedef typename std::map<unsigned, T>::iterator iterator;
  iterator begin() { return views_.begin(); }
  iterator end() { return views_.end(); }

  //: Const Iterator
  typedef typename std::map<unsigned, T>::const_iterator const_iterator;
  const_iterator begin() const { const_iterator it=views_.begin(); return it; }
  const_iterator end() const   { const_iterator it=views_.end();   return it; }

  void print(std::ostream& os) const;

  //: for debug purposes
  void print_relative_cams(vpgl_camera_double_sptr const& target_cam,
                           double distance_thresh = 1.0);


   void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os) const;

  short version() const { return 1; }

 protected:

  //: spherical coordinate system
  vsph_spherical_coord_sptr coord_sys_;

  //: views are associated with an id, all the view centers are on the sphere (r) of the coordinate system
  std::map<unsigned, T> views_;

 private:
  //: unique id for the map
  unsigned uid_;

  unsigned next_id() { return uid_++; }

  //: returns true if all the angles between vertices of a triangle are smaller than \a angle.
  bool min_angle(std::vector<vgl_point_3d<double> > list, double angle);
};

template <class T>
void vsl_b_read(vsl_b_istream& is, vsph_view_sphere<T>& vs);

template <class T>
void vsl_b_write(vsl_b_ostream& os, vsph_view_sphere<T> const& vs);

template <class T>
std::ostream& operator<<(std::ostream& os, vsph_view_sphere<T> const& vs);

#endif
