// This is gel/vtol/vtol_vertex_2d.h
#ifndef vtol_vertex_2d_h_
#define vtol_vertex_2d_h_
//:
// \file
// \brief Topological container for a spatial point, with 2d geometry (location)
//
//  The vtol_vertex_2d class is used to represent either a 2D or 2D point on
//  a topological structure.  A vtol_vertex_2d maintains a pointer to the point
//  which is the actual spatial location.
//
// \verbatim
//  Modifications:
//   JLM December 1995, Added timeStamp (touch) to
//                      operations which affect bounds.
//
//   JLM October 1996,  Added the method EuclideanDistance(vtol_vertex_2d &)
//      to permit Charlie Rothwell's Polyhedra code to be more
//      generic.  Note this is distance, NOT squared distance.
//   LEG May 2000. ported to vxl
//   JLM November 2002 - added local bounding_box method
//  Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
//  JLM November 2003 - set_x and set_y were creating a new point_ which
//                      invalidated curve endpoint geometry.
// \endverbatim

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_2.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d_sptr.h>

class vtol_vertex_2d : public vtol_vertex
{
  //***************************************************************************
  // Data members
  //***************************************************************************

 protected:
  //---------------------------------------------------------------------------
  // Description: point associated to the vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr point_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_vertex_2d();

  //---------------------------------------------------------------------------
  //: Constructor from a point (the point is copied, not stored)
  //  REQUIRE: new_point!=0
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(vsol_point_2d &new_point);

  //---------------------------------------------------------------------------
  //: Constructor from a vector
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(const vnl_double_2 &v);

  //---------------------------------------------------------------------------
  //: Constructor from abscissa `new_x' and ordinate `new_y' of the point
  //---------------------------------------------------------------------------
  vtol_vertex_2d(double new_x, double new_y);

  //---------------------------------------------------------------------------
  //: Pseudo copy constructor.  Deep copy.
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(vtol_vertex_2d_sptr const& other);
 private:
  //---------------------------------------------------------------------------
  //: Copy constructor. Copy the point but not the links.  Deprecated.
  //---------------------------------------------------------------------------
  vtol_vertex_2d(const vtol_vertex_2d &other) = delete;
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vtol_vertex_2d() override = default;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vtol_vertex_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vtol_vertex::is_class(cls); }

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the point
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr point() const;

  //---------------------------------------------------------------------------
  //: Set the point (the point is not copied)
  //  REQUIRE: new_point!=0
  //---------------------------------------------------------------------------
  virtual void set_point(vsol_point_2d_sptr const& new_point);

  // Methods called on Vertex
  // for vsol_point_2d.   These are here
  // during the transition period.
  // Looks like forever now - JLM

  //---------------------------------------------------------------------------
  //: Return the abscissa of the point
  //---------------------------------------------------------------------------
  virtual double x() const;

  //---------------------------------------------------------------------------
  //: Return the ordinate of the point
  //---------------------------------------------------------------------------
  virtual double y() const;

  //---------------------------------------------------------------------------
  //: Set the abscissa of the point with `new_x'
  //---------------------------------------------------------------------------
  virtual void set_x(const double new_x);

  //---------------------------------------------------------------------------
  //: Set the ordinate of the point with `new_y'
  //---------------------------------------------------------------------------
  virtual void set_y(const double new_y);

  //---------------------------------------------------------------------------
  //: Is `this' has the same coordinates for its point than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_vertex_2d &other) const;
  inline bool operator!=(const vtol_vertex_2d &other)const{return !operator==(other);}
  bool operator== (const vtol_vertex &other) const override;
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Assignment of `this' with `other' (copy the point not the links)
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d& operator=(const vtol_vertex_2d &other);
  vtol_vertex_2d& operator=(const vtol_vertex &other) override; // virtual of vtol_vertex

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  const vtol_vertex_2d *cast_to_vertex_2d() const override { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  vtol_vertex_2d *cast_to_vertex_2d() override { return this; }

  //---------------------------------------------------------------------------
  //: Create a line edge from `this' and `other' only if this edge does not exist.
  //  Otherwise it just returns the existing edge
  //  REQUIRE: other!=*this
  //---------------------------------------------------------------------------
  vtol_edge_sptr new_edge(vtol_vertex_sptr const& other) override;
  vtol_edge_sptr new_edge(vtol_vertex_2d_sptr const& v);

  double distance_from(const vnl_double_2 &);

  double euclidean_distance(vtol_vertex_2d &v); //actual distance, not squared - JLM

  void print(std::ostream &strm=std::cout) const override;
  void describe(std::ostream &strm=std::cout, int blanking=0) const override;
  void compute_bounding_box() const override; //A local implementation

  //:  copy the geometry
  void copy_geometry(const vtol_vertex &other) override;

  //: compare the geometry
  bool compare_geometry(const vtol_vertex &other) const override;
};

#endif // vtol_vertex_2d_h_
