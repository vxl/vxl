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

#include <vcl_iosfwd.h>
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
  vtol_vertex_2d(void);

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
  vtol_vertex_2d(const vtol_vertex_2d &other);
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_vertex_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;
  
  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the point
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr point(void) const;

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
  virtual double x(void) const;

  //---------------------------------------------------------------------------
  //: Return the ordinate of the point
  //---------------------------------------------------------------------------
  virtual double y(void) const;

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
  virtual bool operator== (const vtol_vertex &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Assignment of `this' with `other' (copy the point not the links)
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d& operator=(const vtol_vertex_2d &other);
  vtol_vertex& operator=(const vtol_vertex &other); // virtual of vtol_vertex

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex_2d *cast_to_vertex_2d(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d *cast_to_vertex_2d(void) { return this; }

  //---------------------------------------------------------------------------
  //: Create a line edge from `this' and `other' only if this edge does not exist.
  //  Otherwise it just returns the existing edge
  //  REQUIRE: other!=*this
  //---------------------------------------------------------------------------
  virtual vtol_edge_sptr new_edge(vtol_vertex_sptr const& other);
  vtol_edge_sptr new_edge(vtol_vertex_2d_sptr const& v);

  double distance_from(const vnl_double_2 &);

  double euclidean_distance(vtol_vertex_2d &v); //actual distance, not squared - JLM

  void print(vcl_ostream &strm=vcl_cout) const;
  void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;
  virtual void compute_bounding_box(void) const; //A local implementation

  //:  copy the geometry
  virtual void copy_geometry(const vtol_vertex &other);

  //: compare the geometry
  virtual bool compare_geometry(const vtol_vertex &other) const;
};

#endif // vtol_vertex_2d_h_
