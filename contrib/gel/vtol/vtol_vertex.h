// This is gel/vtol/vtol_vertex.h
#ifndef vtol_vertex_h_
#define vtol_vertex_h_
//:
// \file
// \brief Topological container for a spatial point, without geometry info
//
//  The vtol_vertex class is used to represent either a 2D or 3D point on
//  a topological structure.  A vtol_vertex does not actually maintain a pointer
//  to the vsol_point which is the actual spatial point, since this could either
//  be a vsol_point_2d or a vsol_point_3d.  See vtol_vertex_2d for this purpose.
//
// \verbatim
//  Modifications:
//   JLM December 1995, Added timeStamp(touch) to operations which affect bounds
//   JLM October 1996,  Added the method EuclideanDistance(vtol_vertex &)
//                      to permit Charlie Rothwell's Polyhedra code to be more
//                      generic.  Note this is distance, NOT squared distance.
//   LEG May 2000. ported to vxl
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
//   Sept.2004, Peter Vanroose -is_endpoint() now accepts smart pointer argument
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vtol/vtol_topology_object.h>
#include <vcl_compiler.h>
class vtol_vertex_2d;
class vtol_edge;
class vtol_zero_chain;
class vtol_one_chain;
class vtol_face;
class vtol_two_chain;
class vtol_block;

class vtol_vertex : public vtol_topology_object
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_vertex() {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_vertex();

  // Accessors

 private: // has been superseded by is_a()
  //: Return the topology type
  virtual vtol_topology_object_type topology_type() const { return VERTEX; }

 public:
  //---------------------------------------------------------------------------
  //: create a list of all connected vertices
  //---------------------------------------------------------------------------
  virtual void explore_vertex(vertex_list &);

  //---------------------------------------------------------------------------
  //: Is `this' has the same coordinates for its point than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_vertex &other) const;
  inline bool operator!=(const vtol_vertex &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Assignment of `this' with `other' (copy the point not the links)
  //---------------------------------------------------------------------------
  virtual vtol_vertex& operator=(const vtol_vertex &other);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex *cast_to_vertex() const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex *cast_to_vertex() { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a 2D vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex_2d *cast_to_vertex_2d() const {return 0;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a 2D vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex_2d *cast_to_vertex_2d() {return 0;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* /*inf*/) const
  { return false; } // a vertex can never have an inferior
  bool valid_superior_type(vtol_zero_chain_sptr const& ) const { return true; }

  //---------------------------------------------------------------------------
  //: Is `this' connected with `v2' ?
  //  ie has a superior of `this' `v2' as inferior ?
  //---------------------------------------------------------------------------
  virtual bool is_connected(vtol_vertex_sptr const& v2) const;

  // methods that will be defined by inherited classes

  //---------------------------------------------------------------------------
  //: Create a line edge from `this' and `other' only if this edge does not exist.
  //  Otherwise it just returns the existing edge
  //  REQUIRE: other!=*this
  //---------------------------------------------------------------------------

  virtual vtol_edge_sptr new_edge(vtol_vertex_sptr const& other)=0;
 private: // deprecated:
  vtol_edge_sptr new_edge(vtol_vertex const& other);
 public:

  //: check to see if the vertex is part of the edge
  bool is_endpoint(vtol_edge_sptr const&) const;

  void print(std::ostream &strm=std::cout) const;
  void describe(std::ostream &strm=std::cout, int blanking=0) const;

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const { return std::string("vtol_vertex"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }

  //: have the inherited classes copy the geometry
  virtual void copy_geometry(const vtol_vertex &other)=0;
  virtual bool compare_geometry(const vtol_vertex &other) const =0;

 protected:
  // \warning these should not be used by clients

  virtual std::vector<vtol_vertex*> *compute_vertices();
  virtual std::vector<vtol_edge*> *compute_edges();
  virtual std::vector<vtol_zero_chain*> *compute_zero_chains();
  virtual std::vector<vtol_one_chain*> *compute_one_chains();
  virtual std::vector<vtol_face*> *compute_faces();
  virtual std::vector<vtol_two_chain*> *compute_two_chains();
  virtual std::vector<vtol_block*> *compute_blocks();
};

#endif // vtol_vertex_h_
