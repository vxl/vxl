#ifndef VTOL_VERTEX_2D_H
#define VTOL_VERTEX_2D_H
// .NAME vtol_vertex_2d - Topological container for a spatial point
// .LIBRARY vtol
// .HEADER vxl Package
// .INCLUDE vtol/vtol_vertex_2d.h
// .FILE vtol_vertex_2d.h
// .FILE vtol_vertex_2d.cxx
// .SECTION Description
//  The vtol_vertex_2d class is used to represent either a 2D or 2D point on
//  a topological structure.  A vtol_vertex_2d maintains a pointer to the IUPoint
//  which is the actual spatial point.
//
// .SECTION Modifications:
//     JLM December 1995, Added timeStamp (touch) to
//                        operations which affect bounds.
//
//     JLM October 1996,  Added the method EuclideanDistance(vtol_vertex_2d &)
//     to permit Charlie Rothwell's Polyhedra code to be more
//     generic.  Note this is distance, NOT squared distance.
//     LEG May 2000. ported to vxl
//
/// .EXAMPLE vtol_vertex_2d.example
#include <vtol/vtol_vertex_2d_ref.h>

#include <vtol/vtol_topology_object_2d.h>
#include <vnl/vnl_double_2.h>
#include <vsol/vsol_point_2d_ref.h>

class vtol_vertex_2d
  : public vtol_topology_object_2d
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************
  
  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor from a point (the point is not copied)
  //: REQUIRE: new_point!=0
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(vsol_point_2d &new_point);

  //---------------------------------------------------------------------------
  //: Constructor from a vector
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(const vnl_double_2 &v);

  //---------------------------------------------------------------------------
  //: Constructor from abscissa `new_x' and ordinate `new_y' of the point
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(const double new_x,
                          const double new_y);

  //---------------------------------------------------------------------------
  //: Copy constructor. Copy the point but not the links
  //---------------------------------------------------------------------------
  explicit vtol_vertex_2d(const vtol_vertex_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_vertex_2d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

 //  explicit vtol_vertex_2d(const vsol_point_2d &);   // for some peculiar cases.

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  //---------------------------------------------------------------------------
  //: Return the point
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_ref point(void) const;

  //---------------------------------------------------------------------------
  //: Set the point (the point is not copied)
  //: REQUIRE: new_point.ptr()!=0
  //---------------------------------------------------------------------------
  virtual void set_point(vsol_point_2d &new_point);
 

  virtual void explore_vertex(vertex_list_2d &);
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

  //  bool operator==(const vsol_spatial_object_2d &obj) const; // virtual of vsol_spatial_object_2d.

  //---------------------------------------------------------------------------
  //: Is `this' has the same coordinates for its point than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_vertex_2d &other) const;

  //---------------------------------------------------------------------------
  //: Assignment of `this' with `other' (copy the point not the links)
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d& operator=(const vtol_vertex_2d &other);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex_2d *cast_to_vertex(void) const;

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d *cast_to_vertex(void);

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_inferior_type(const vtol_topology_object_2d &inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_superior_type(const vtol_topology_object_2d &superior) const;

  //---------------------------------------------------------------------------
  //: Is `this' connected with `v2' ?
  //: ie has a superior of `this' `v2' as inferior ?
  //---------------------------------------------------------------------------
  virtual bool is_connected(const vtol_vertex_2d &v2);

  //---------------------------------------------------------------------------
  //: Create a line edge from `this' and `other' only if this edge does not
  //: exist. Otherwise it just returns the existing edge
  //: REQUIRE: other.ptr()!=0 and other.ptr()!=this
  //---------------------------------------------------------------------------
  virtual vtol_edge_2d *new_edge(vtol_vertex_2d &other);

  vtol_vertex_2d *vertex_diff (vtol_vertex_2d &);
  bool is_endpointp (const vtol_edge_2d &);
  double distance_from(const vnl_double_2 &);
  double euclidean_distance(vtol_vertex_2d &v); //actual distance, not squared - JLM
  //  void merge_references(vtol_vertex_2d &);
  //  void calculate_average_normal(IUE_vector<double>&);


  void print(ostream &strm=cout) const;
  void describe(ostream &strm=cout, int blanking=0) const;

protected:
  //---------------------------------------------------------------------------
  // Description: point associated to the vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_ref _point;


public:
  
  // : Warning - should not be used by clients
  virtual vcl_vector<vtol_vertex_2d*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge_2d*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain_2d*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block_2d*> *compute_blocks(void);
};

#endif
