#ifndef vsph_unit_sphere_h_
#define vsph_unit_sphere_h_
//:
// \file
// \brief  3D unit sphere with triangulated discrete surface positions
// \author J. L. Mundy
// \date January 27, 2013
//
// \verbatim
//  Modifications
//   2013/01/13 Initial version
// \endverbatim
#include <vbl/vbl_ref_count.h>
#include "vsph_sph_point_2d.h"
#include <vgl/vgl_point_3d.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_iosfwd.h>
class vsph_edge
{
 public:
 vsph_edge(int vs, int ve): vs_(vs), ve_(ve){}
  bool operator==(vsph_edge const& e) const{
    bool temp = (vs_==e.vs_) && (ve_==e.ve_);
    temp = temp || (vs_==e.ve_)&&(ve_==e.vs_);
    return temp;}
  //: start vertex
  int vs_;
  //: end vertex
  int ve_;
};
bool operator < (vsph_edge const& a, vsph_edge const& b);

class vsph_unit_sphere : public vbl_ref_count
{
 public:
  //: default constructor
 vsph_unit_sphere() : uid_(0) {}

  //: destructor
  ~vsph_unit_sphere() {}


  //: adds uniformly placed viewpoints on the sphere in the area defined by elevation angle \a cap_angle, and the viewpoints are at most \a point_angle apart
  // It uses iterative triangle division on octahedron until the point angle is achieved
  void add_uniform_views(double cap_angle, double point_angle);
  //: eliminate vertices above top_angle and below bottom_angle in elevation
  void remove_top_and_bottom(double top_angle, double bottom_angle);

  //: returns the number of view points kept
  unsigned size() const { return sph_pts_.size(); }

  //: transforms a spherical coordinate to a Cartesian unit vector
  vgl_vector_3d<double> cart_coord(vsph_sph_point_2d const& vp) const;

  //: transforms a Cartesian unit vector to a spherical coordinate
  vsph_sph_point_2d spher_coord(vgl_vector_3d<double> const& cp) const;

  //: get the triangle edges
  vcl_vector<vsph_edge> edges() const{return edges_;}


  //: display the vertices in a vrml format
  void display_vertices(vcl_string const & path) const;

  //: display the edges in a vrml format
  void display_edges(vcl_string const & path) const;

  //: Iterator

  typedef vcl_vector<vsph_sph_point_2d>::iterator iterator;
  iterator begin() { return sph_pts_.begin(); }
  iterator end() { return sph_pts_.end(); }

  //: Const Iterator
  typedef vcl_vector<vsph_sph_point_2d>::const_iterator const_iterator;
  const_iterator begin() const { const_iterator it=sph_pts_.begin(); return it; }
  const_iterator end() const   { const_iterator it=sph_pts_.end();   return it; }

  void print(vcl_ostream& os) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os) const;

  short version() const { return 1; }

 protected:
  bool find_near_equal(vgl_vector_3d<double>const& p,int& id,double tol=0.0001);
  void filter_intersecting_edges(double point_angle);

  //: views are associated with an id, all the view centers are on the sphere (r) of the coordinate system
  vcl_vector<vsph_sph_point_2d> sph_pts_;
  vcl_vector<vgl_vector_3d<double> > cart_pts_;
  vcl_vector<vsph_edge> edges_;
  vcl_map<int, int> equivalent_ids_;
 private:
  //: unique id for the map
  int uid_;

  int next_id() { return uid_++; }

  //: returns true if all the angles between vertices of a triangle are smaller than \a angle.
  bool min_angle(vcl_vector<vgl_vector_3d<double> > list, double angle);
};

void vsl_b_read(vsl_b_istream& is, vsph_unit_sphere& vs);

void vsl_b_write(vsl_b_ostream& os, vsph_unit_sphere const& vs);

vcl_ostream& operator<<(vcl_ostream& os, vsph_unit_sphere const& vs);

#endif // vsph_unit_sphere_h_
