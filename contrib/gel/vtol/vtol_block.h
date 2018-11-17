// This is gel/vtol/vtol_block.h
#ifndef vtol_block_h_
#define vtol_block_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Highest node in a 3D topological structure
//
//   The vtol_block class is used to represent the highest node in a 3D topological
//   structure. A vtol_block consists of its inferior 2-chains which must be
//   cyclic, and no topological superiors.
//
// \author
//     Patricia A. Vrobel
//
// \verbatim
//  Modifications:
//   JLM Dec 1995, Added timeStamp (Touch) to operations which affect bounds.
//   JLM Dec 1995  (Need to decide proper policy for curved volumes
//                 and possibly inconsistent polyhedra volume geometry)
//   JNS Dec 1998, Added is_within functions (used in 2D display of a block,
//                 these functions must be wrong for 3D use)
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_two_chain.h>
class vtol_vertex;
class vtol_edge;
class vtol_zero_chain;
class vtol_one_chain;
class vtol_face;
class vtol_two_chain;

class vtol_block : public vtol_topology_object
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_block() = default;

  //---------------------------------------------------------------------------
  //: Constructor from a two-chain (inferior)
  //---------------------------------------------------------------------------
  explicit vtol_block(vtol_two_chain_sptr const& faceloop);
 private:
  // Deprecated
  explicit vtol_block(vtol_two_chain &faceloop);
 public:
  //---------------------------------------------------------------------------
  //: Constructor from a list of two-chains.
  //  The first two-chain is the outside boundary.
  //  The remaining two-chains are considered holes inside the outside volume.
  //---------------------------------------------------------------------------
  explicit vtol_block(two_chain_list const& faceloops);

  //---------------------------------------------------------------------------
  //: Constructor from a list of faces
  //---------------------------------------------------------------------------
  explicit vtol_block(face_list const& new_face_list);

  //---------------------------------------------------------------------------
  //: Pseudo copy constructor. Deep copy.
  //---------------------------------------------------------------------------
  vtol_block(vtol_block_sptr const& other);
 private:
  //---------------------------------------------------------------------------
  //: Copy constructor. Deep copy. Deprecated.
  //---------------------------------------------------------------------------
  vtol_block(const vtol_block &other);
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vtol_block() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vtol_block"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }

  // Accessors

 private: // has been superseded by is_a()
  //: Return the topology type
  vtol_topology_object_type topology_type() const override { return BLOCK; }

 public:
  virtual vtol_two_chain_sptr get_boundary_cycle();

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  const vtol_block *cast_to_block() const override { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  vtol_block *cast_to_block() override { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(const vtol_two_chain_sptr& inf);
  void unlink_inferior(const vtol_two_chain_sptr& inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  bool valid_inferior_type(vtol_topology_object const* inferior) const override
  { return inferior->cast_to_two_chain()!=nullptr; }
  bool valid_inferior_type(vtol_two_chain_sptr const& ) const { return true; }

  virtual vertex_list *outside_boundary_vertices();
  virtual zero_chain_list *outside_boundary_zero_chains();
  virtual edge_list *outside_boundary_edges();
  virtual one_chain_list *outside_boundary_one_chains();
  virtual face_list *outside_boundary_faces();
  virtual two_chain_list *outside_boundary_two_chains();

 protected:
  // \warning these methods should not be used by clients
  // The returned pointers must be deleted after use.

  std::vector<vtol_vertex *> *compute_vertices() override;
  std::vector<vtol_edge *> *compute_edges() override;
  std::vector<vtol_zero_chain *> *compute_zero_chains() override;
  std::vector<vtol_one_chain *> *compute_one_chains() override;
  std::vector<vtol_face *> *compute_faces() override;
  std::vector<vtol_two_chain *> *compute_two_chains() override;
  std::vector<vtol_block *> *compute_blocks() override;

  virtual std::vector<vtol_vertex *> *outside_boundary_compute_vertices();
  virtual std::vector<vtol_zero_chain *> *outside_boundary_compute_zero_chains();
  virtual std::vector<vtol_edge *> *outside_boundary_compute_edges();
  virtual std::vector<vtol_one_chain *> *outside_boundary_compute_one_chains();
  virtual std::vector<vtol_face *> *outside_boundary_compute_faces();
  virtual std::vector<vtol_two_chain *> *outside_boundary_compute_two_chains();

 public:
  virtual two_chain_list *hole_cycles() const;

  //: Utility and Editing Functions
  virtual bool add_hole_cycle(vtol_two_chain_sptr new_hole);

  //  virtual void update();
  virtual bool operator==(const vtol_block &other) const;
  inline bool operator!=(const vtol_block &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  //: Printing Functions
  void print(std::ostream &strm=std::cout) const override;
  void describe(std::ostream &strm=std::cout,
                        int blanking=0) const override;
};

#endif // vtol_block_h_
