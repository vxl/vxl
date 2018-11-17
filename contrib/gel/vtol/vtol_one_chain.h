// This is gel/vtol/vtol_one_chain.h
#ifndef vtol_one_chain_h_
#define vtol_one_chain_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Represents a connected chain of edges
//
//  The vtol_one_chain class is used to represent a set of edges on a topological
//  structure.  A vtol_one_chain consists of its inferior edges and the superiors
//  on which it lies.  A vtol_one_chain may or may not be an ordered cycle.  If
//  the chain of edges encloses an area, then the vtol_one_chain may be used as
//  the boundary of a topological face in a 3D structure.
//
// \author
//     Patricia A. Vrobel
//
// \verbatim
//  Modifications:
//   JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//   JLM Dec 1995, no local method for ComputeBoundingBox
//                 Should use edge geometry recursively to be proper.
//                 Currently reverts to bounds on vertices from
//                 TopologyObject::ComputeBoundingBox()
//   JLM Jan 1998  Added method to get direction of an edge
//   JLM Feb 1999  Added correct method for ComputeBoundingBox()
//   PTU May 2000  ported to vxl
//   JLM Nov 2002  Modified the compute_bounding_box method to use
//                 box::grow_minmax_bounds for uniformity and to
//                 avoid dependence on dimension.  Old method was strictly 2-d.
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_chain.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
class vtol_edge;
class vtol_vertex;
class vtol_face;
class vtol_block;
class vtol_zero_chain;
class vtol_two_chain;

//: The class represents a collection of edges and orientations

class vtol_one_chain : public vtol_chain
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  void link_chain_inferior(const vtol_one_chain_sptr& chain_inferior);
  void unlink_chain_inferior(const vtol_one_chain_sptr& chain_inferior);

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_one_chain() { set_cycle(false); }

  //---------------------------------------------------------------------------
  //: Constructor from an array of edges
  //---------------------------------------------------------------------------
  explicit vtol_one_chain(edge_list const&,
                          bool new_is_cycle=false);

  //---------------------------------------------------------------------------
  //: Constructor from an array of edges and an array of directions
  //---------------------------------------------------------------------------
  vtol_one_chain(edge_list const&,
                 std::vector<signed char> const&,
                 bool new_is_cycle=false);

  //---------------------------------------------------------------------------
  //: Pseudo copy constructor.  Deep copy.
  //---------------------------------------------------------------------------
  vtol_one_chain(vtol_one_chain_sptr const& other);
 private:
  //---------------------------------------------------------------------------
  //: Copy constructor.  Deep copy.  Deprecated.
  //---------------------------------------------------------------------------
  vtol_one_chain(vtol_one_chain const& other) = delete;
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vtol_one_chain() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vtol_one_chain"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vtol_chain::is_class(cls); }

  // Access methods

 private: // has been superseded by is_a()
  //: Return the topology type
  vtol_topology_object_type topology_type() const override {return ONECHAIN;}

 public:
  virtual signed char direction(vtol_edge const& e) const;

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a one_chain, 0 otherwise
  //---------------------------------------------------------------------------
  const vtol_one_chain *cast_to_one_chain() const override { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a one_chain, 0 otherwise
  //---------------------------------------------------------------------------
  vtol_one_chain *cast_to_one_chain() override { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(const vtol_edge_sptr& inf);
  void unlink_inferior(const vtol_edge_sptr& inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  bool valid_inferior_type(vtol_topology_object const* inferior) const override
  { return inferior->cast_to_edge() != nullptr; }
  bool valid_inferior_type(vtol_edge_sptr const& )    const { return true; }
  bool valid_inferior_type(vtol_edge_2d_sptr const& ) const { return true; }
  bool valid_superior_type(vtol_face_sptr const& )    const { return true; }
  bool valid_superior_type(vtol_face_2d_sptr const& ) const { return true; }

  //---------------------------------------------------------------------------
  //: Is `chain_inf_sup' type valid for `this' ?
  //---------------------------------------------------------------------------
  bool valid_chain_type(vtol_chain_sptr chain_inf_sup) const override
  { return chain_inf_sup->cast_to_one_chain() != nullptr; }
  bool valid_chain_type(vtol_one_chain_sptr const& ) const { return true; }

  //: accessors for outside boundary elements

  virtual vertex_list *outside_boundary_vertices();
  virtual zero_chain_list *outside_boundary_zero_chains();
  virtual edge_list *outside_boundary_edges();
  virtual one_chain_list *outside_boundary_one_chains();

  //: accessors to inferiors and superiors
  virtual one_chain_list *inferior_one_chains();
  virtual one_chain_list *superior_one_chains();

  // Utilities

  virtual void reverse_directions();

  virtual vtol_one_chain * copy_with_arrays(topology_list &verts,
                                            topology_list &edges) const;

  void compute_bounding_box() const override; //A local implementation

  virtual vtol_edge_sptr edge(int i) const;
  int num_edges() const { return numinf(); }

  virtual void determine_edge_directions();
  virtual void add_edge(vtol_edge_sptr const&, bool);
  virtual void add_edge(vtol_edge_2d_sptr const&, bool);
  virtual void remove_edge(vtol_edge_sptr const&, bool);
  virtual void remove_edge(vtol_edge_2d_sptr const&, bool);
 private:
  // Deprecated:
  virtual void add_edge(vtol_edge &, bool);
  virtual void remove_edge(vtol_edge &, bool);
 public:
  // Operators

  virtual bool operator==(vtol_one_chain const& other) const;
  inline bool operator!=(const vtol_one_chain &other)const{return !operator==(other);}
  bool operator==(vsol_spatial_object_2d const& obj) const override; // virtual of vsol_spatial_object_2d

  void print(std::ostream &strm=std::cout) const override;
  virtual void describe_directions(std::ostream &strm=std::cout, int blanking=0) const;
  void describe(std::ostream &strm=std::cout, int blanking=0) const override;

 protected:
  // \warning clients should not use these methods
  // The returned pointers must be deleted after use.

  std::vector<vtol_vertex*> *compute_vertices() override;
  std::vector<vtol_edge*> *compute_edges() override;
  std::vector<vtol_zero_chain*> *compute_zero_chains() override;
  std::vector<vtol_one_chain*> *compute_one_chains() override;
  std::vector<vtol_face*> *compute_faces() override;
  std::vector<vtol_two_chain*> *compute_two_chains() override;
  std::vector<vtol_block*> *compute_blocks() override;

 public:
  virtual std::vector<vtol_vertex*> *outside_boundary_compute_vertices();
  virtual std::vector<vtol_zero_chain*> *outside_boundary_compute_zero_chains();
  virtual std::vector<vtol_edge*> *outside_boundary_compute_edges();
  virtual std::vector<vtol_one_chain*> *outside_boundary_compute_one_chains();
};

#endif // vtol_one_chain_h_
