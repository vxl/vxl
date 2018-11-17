// This is gel/vtol/vtol_zero_chain.h
#ifndef vtol_zero_chain_h_
#define vtol_zero_chain_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Represents a set of vertices
//
//  The vtol_zero_chain class is used to represent a set of vertices on
//  a topological structure. A vtol_zero_chain maintains only the inferiors and
//  superiors, not the geometry. It is the topological inferior of an edge.
//
// \author
//     Patricia A. Vrobel
//
// \verbatim
//  Modifications:
//   JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//   Peter Vanroose - 02-26-97 Added implementation for virtual Transform()
//   PTU may 2000 - ported to vxl
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
class vtol_vertex;
class vtol_edge;
class vtol_one_chain;
class vtol_face;
class vtol_two_chain;
class vtol_block;

class vtol_zero_chain : public vtol_topology_object
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Empty zero-chain
  //---------------------------------------------------------------------------
  vtol_zero_chain() = default;

  //---------------------------------------------------------------------------
  //: Constructor from two vertices (to make edge creation easier)
  //  REQUIRE: v1!=v2
  //---------------------------------------------------------------------------
  vtol_zero_chain(vtol_vertex_sptr const& v1,
                  vtol_vertex_sptr const& v2);

  vtol_zero_chain(vtol_vertex_2d_sptr const& v1,
                  vtol_vertex_2d_sptr const& v2);
 private:
  // Deprecated
  vtol_zero_chain(vtol_vertex &v1, vtol_vertex &v2);

 public:
  //---------------------------------------------------------------------------
  //: Constructor from an array of vertices
  //  REQUIRE: new_vertices.size()>0
  //---------------------------------------------------------------------------
  explicit vtol_zero_chain(const vertex_list &new_vertices);

  //---------------------------------------------------------------------------
  //: Pseudo copy constructor. Deep copy.
  //---------------------------------------------------------------------------
  vtol_zero_chain(vtol_zero_chain_sptr const& other);
 private:
  //---------------------------------------------------------------------------
  //: Copy constructor. Copy the vertices and the links.  Deprecated.
  //---------------------------------------------------------------------------
  vtol_zero_chain(const vtol_zero_chain &other);
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vtol_zero_chain() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vtol_zero_chain"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }

 private: // has been superseded by is_a()
  //: Return the topology type
  vtol_topology_object_type topology_type() const override{return ZEROCHAIN;}

 public:
  //---------------------------------------------------------------------------
  //: Return the first vertex of `this'. If it does not exist, return 0
  //---------------------------------------------------------------------------
  virtual vtol_vertex_sptr v0() const;

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  const vtol_zero_chain *cast_to_zero_chain() const override { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  vtol_zero_chain *cast_to_zero_chain() override { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(const vtol_vertex_sptr& inf);
  void unlink_inferior(const vtol_vertex_sptr& inf);
  void link_inferior(const vtol_vertex_2d_sptr& inf);
  void unlink_inferior(const vtol_vertex_2d_sptr& inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  bool valid_inferior_type(vtol_topology_object const* inferior) const override
  { return inferior->cast_to_vertex()!=nullptr; }
  bool valid_inferior_type(vtol_vertex_sptr const& )  const { return true; }
  bool valid_superior_type(vtol_edge_sptr const& )    const { return true; }
  bool valid_superior_type(vtol_edge_2d_sptr const& ) const { return true; }

  //---------------------------------------------------------------------------
  //: Return the length of the zero-chain
  //---------------------------------------------------------------------------
  int length() const { return numinf(); }

  //---------------------------------------------------------------------------
  //: Is `this' equal to `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_zero_chain &other) const;
  inline bool operator!=(const vtol_zero_chain &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  void print(std::ostream &strm=std::cout) const override;
  void describe(std::ostream &strm=std::cout,
                        int blanking=0) const override;

  //: \warning these should not be used by clients
 protected:
  std::vector<vtol_vertex*> *compute_vertices() override;
  std::vector<vtol_edge*> *compute_edges() override;
  std::vector<vtol_zero_chain*> *compute_zero_chains() override;
  std::vector<vtol_one_chain*> *compute_one_chains() override;
  std::vector<vtol_face*> *compute_faces() override;
  std::vector<vtol_two_chain*> *compute_two_chains() override;
  std::vector<vtol_block*> *compute_blocks() override;
};

#endif // vtol_zero_chain_h_
