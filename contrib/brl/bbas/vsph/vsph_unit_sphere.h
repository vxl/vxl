#ifndef vsph_unit_sphere_h_
#define vsph_unit_sphere_h_
//:
// \file
// \brief  3-d unit sphere with triangulated discrete surface positions
// \author J. L. Mundy
// \date January 27, 2013
//
// \verbatim
//  Modifications
//   2013/01/13 Initial version
// \endverbatim
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <iosfwd>
#include "vsph_sph_point_2d.h"
#include "vsph_sph_box_2d.h"
#include "vsph_grid_index_2d.h"
#include "vsph_defs.h"//DIST_TOL, MARGIN
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsph_edge
{
 public:
  vsph_edge(): vs_(-1), ve_(-1) {}
  vsph_edge(int vs, int ve): vs_(vs), ve_(ve) {}
  bool operator==(vsph_edge const& e) const {
    return (vs_==e.vs_ && ve_==e.ve_)
        || (vs_==e.ve_ && ve_==e.vs_);      }
  short version() const { return 1;}
  void b_read(vsl_b_istream& is);
  void b_write(vsl_b_ostream& os) const;
  void print(std::ostream& os) const { os << '(' << vs_ << ' ' << ve_ << ")\n"; }
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
  vsph_unit_sphere() : point_angle_(0.0), min_theta_(0.0), max_theta_(0.0) {}
  //: constructor, angles are in degrees
  // \p point_angle is the maximum angle between adjacent triangle vertices
  // \p min_theta and \p max_theta bound the points constructed on the sphere surface
  vsph_unit_sphere(double point_angle, double min_theta, double max_theta);

  //: destructor
  ~vsph_unit_sphere() override = default;

  //: accessors
  double point_angle() const {return point_angle_;}

  double min_theta() const {return min_theta_;}
  double max_theta() const {return max_theta_;}

  //: \returns the number of spherical vertices
  unsigned size() const { return sph_pts_.size(); }

  //: transforms a spherical coordinate to a Cartesian unit vector
  static vgl_vector_3d<double> cart_coord(vsph_sph_point_2d const& vp);

  //: transforms a Cartesian unit vector to a spherical coordinate
  // If \p in_radians == false units are in degrees
  static vsph_sph_point_2d spher_coord(vgl_vector_3d<double> const& cp,
                                       bool in_radians = true);

  //: spherical points
  // \returns a copy
  std::vector<vsph_sph_point_2d> sph_points() const { return sph_pts_; }
  // \returns a const reference
  const std::vector<vsph_sph_point_2d>& sph_points_ref() const { return sph_pts_; }

  //: Cartesian unit vectors
  // \returns a copy
  std::vector<vgl_vector_3d<double> > cart_vectors() const { return cart_pts_; }
  // \returns a const reference
  const std::vector<vgl_vector_3d<double> >& cart_vectors_ref() const { return cart_pts_; }

  //: get the triangle edges
  std::vector<vsph_edge> edges() const {return edges_;}

  //: find the nearest neighbors (connected by a single edge traversal)
  void find_neighbors();

  //: have neighbors been collected yet ?
  bool neighbors_valid() const {return neighbors_valid_;}

  //: get the neighboring vertices based on triangle edges (hex neighborhood)
  std::set<int> neighbors(int vert_id) const {return neighbors_[vert_id];}

  //: the tangent plane at the specified point
  static vgl_plane_3d<double> tangent_plane(vsph_sph_point_2d const& sph);

  //: display the vertices in a vrml format
  void display_vertices(std::string const & path) const;

  //: display the edges in a vrml format
  void display_edges(std::string const & path) const;

  //: display data values associated with spherical positions
  void display_data(std::string const & path,
                    std::vector<double> const& data,
                    vsph_sph_box_2d const& mask = vsph_sph_box_2d()) const;

  //: display a color distribution on the unit sphere
  void display_color(std::string const & path,
                     std::vector<std::vector<float> > const& cdata,
                     std::vector<float> const& skip_color =
                     std::vector<float>(3, -1.0f),
                     vsph_sph_box_2d const& mask = vsph_sph_box_2d()) const;

  //: display a set of axis aligned boxes on the sphere
  void display_boxes(std::string const & path,
                     std::vector<vsph_sph_box_2d> const& boxes);

  //: Iterator over the set of spherical points
  typedef std::vector<vsph_sph_point_2d>::iterator iterator;
  iterator begin() { return sph_pts_.begin(); }
  iterator end() { return sph_pts_.end(); }

  //: Const Iterator
  typedef std::vector<vsph_sph_point_2d>::const_iterator const_iterator;
  const_iterator begin() const { const_iterator it=sph_pts_.begin(); return it; }
  const_iterator end() const   { const_iterator it=sph_pts_.end();   return it; }
  bool operator==(const vsph_unit_sphere &other) const;

  void print(std::ostream& os) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os) const;

  short version() const { return 1; }

 protected:
  //: adds uniformly placed vertices on the sphere in the area defined by elevation angle \a cap_angle, and the vertices are at most \a point_angle apart
  // Algorithm uses iterative triangle division on octahedron until the point angle is achieved
  void add_uniform_views();

  //: eliminate vertices above min_theta and below max_theta in elevation
  void remove_top_and_bottom();

  //: construct Cartesian vectors from spherical points
  void set_cart_points();

  bool find_near_equal(vgl_vector_3d<double>const& p,int& id,double tol=DIST_TOL);
  bool find_edge(vsph_edge const&  e);
  void insert_edge(vsph_edge const&  e);

  //: views are associated with an id, all the view centers are on the sphere (r) of the coordinate system
  std::vector<vsph_sph_point_2d> sph_pts_;
  std::vector<vgl_vector_3d<double> > cart_pts_;
  std::vector<vsph_edge> edges_;
  std::vector<int> equivalent_ids_;
  std::vector<std::set<int> > neighbors_;
  vsph_grid_index_2d index_;
 private:
  bool neighbors_valid_;
  //: these angles are stored in degrees for convenient interpretation
  double point_angle_;
  double min_theta_;
  double max_theta_;
  //: returns true if all the angles between vertices of a triangle are smaller than \a angle.
  bool min_angle(std::vector<vgl_vector_3d<double> > list, double angle_rad);
};

#include "vsph_unit_sphere_sptr.h"

void vsl_b_read(vsl_b_istream& is, vsph_edge& e);

void vsl_b_write(vsl_b_ostream& os, vsph_edge const& e);

void vsl_print_summary(std::ostream& os, vsph_edge const& e);

void vsl_b_read(vsl_b_istream& is, vsph_unit_sphere& usph);

void vsl_b_write(vsl_b_ostream& os, vsph_unit_sphere const& usph);

std::ostream& operator<<(std::ostream& os, vsph_unit_sphere const& usph);

void vsl_b_write(vsl_b_ostream &os, vsph_unit_sphere const* usph_ptr);

void vsl_b_read(vsl_b_istream &is, vsph_unit_sphere*& usph_ptr);

void vsl_b_write(vsl_b_ostream &os, vsph_unit_sphere_sptr const&  usph_sptr);

void vsl_b_read(vsl_b_istream &is, vsph_unit_sphere_sptr& usph_sptr);

#endif // vsph_unit_sphere_h_
