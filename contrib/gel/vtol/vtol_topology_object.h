#ifndef topology_object_h
#define topology_object_h

//:
//  \file
// \brief base class for topology objects
//
//  The TopologyObject class is the interface base class for all
//  topological entites. There are only access methods in this class.
//  TopologyObject inherits both SpatialObject and TopologyHierarchyNode.
//  SpatialObject is the base class for all Topology and Geometry.
//  The Topology class hierarchy:
// \verbatim
//                             TopologyObject+
//                                   |
//             ---------------------------------------------------
//             |        |       |       |       |       |        |
//          Vertex+ ZeroChain+ Edge+ OneChain+ Face+ TwoChain+ Block+
//
//Incidence
//     of: (IUPoint)          (Curve)        (Surface)        (Volume)
//Directed
//Sequence of:        Vertex           Edge            Face
//
// + checked bounding box and timestamp -JLM
// \endverbatim
//
// The Vertex, Edge and Face entities account for incidence between
// points, curves and surfaces, respectively. For example, two Edge(s)
// are incident at exactly one Vertex.  That is, an Edge does not
// intersect itself or another Edge, except at a Vertex.
// The xxChain entities are directed sequences which define composite
// structures. Each element of the chain has a sign associated with
// with it denoting the "direction" of use in a boundary traversal.
// For example, a closed OneChain (OneCycle) forms the boundary of
// a surface region. Two adjacent surface regions (Face(s)) use an
// Edge in the opposite sense in their bounding OneChain(s).
//
// \author
//    Patricia A. Vrobel.
//    ported by Luis E. Galup

#include <vtol/vtol_topology_object_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>

#include <vcl_vector.h>
#include <vcl_list.h>

#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_two_chain_sptr.h>
#include <vtol/vtol_block_sptr.h>
class vtol_topology_cache; //#include <vtol/vtol_topology_cache_sptr.h>


// Useful typedefs
#if 1
typedef vcl_vector<vtol_topology_object_sptr> topology_list;
typedef vcl_vector<vtol_vertex_sptr>          vertex_list;
typedef vcl_vector<vtol_edge_sptr>            edge_list;
typedef vcl_vector<vtol_one_chain_sptr>       one_chain_list;
typedef vcl_vector<vtol_zero_chain_sptr>      zero_chain_list;
typedef vcl_vector<vtol_face_sptr>            face_list;
typedef vcl_vector<vtol_two_chain_sptr>       two_chain_list;
typedef vcl_vector<vtol_block_sptr>           block_list;
#else
typedef vcl_vector<vtol_topology_object *> topology_list;
typedef vcl_vector<vtol_vertex *>          vertex_list;
typedef vcl_vector<vtol_edge *>            edge_list;
typedef vcl_vector<vtol_one_chain *>       one_chain_list;
typedef vcl_vector<vtol_zero_chain *>      zero_chain_list;
typedef vcl_vector<vtol_face *>            face_list;
typedef vcl_vector<vtol_two_chain *>       two_chain_list;
typedef vcl_vector<vtol_block *>           block_list;

#endif

//*****************************************************************************
// ALL THE DERIVED AND NON-ABSTRACT CLASSES OF THIS CLASS MUST CALL
// unlink_all_inferiors() IN THEIR DESTRUCTOR
// unlink_all_inferiors() CANT BE CALLED DIRECTLY IN THIS CLASS, OTHERWISE
// BAD VERSIONS OF METHODS SHOULD BE CALLED (C++ IS STUPID !)
//*****************************************************************************

class vtol_topology_object
  : public vsol_spatial_object_2d
{
public:
  enum vtol_topology_object_type
  { TOPOLOGY_NO_TYPE=0,
    VERTEX,
    ZEROCHAIN,
    EDGE,
    ONECHAIN,
    FACE,
    TRIFACE,
    INTENSITYFACE,
    INTENSITYFACE3D,
    DDBINTENSITYFACE,
    TWOCHAIN,
    TRIMESHTWOCHAIN,
    BLOCK,
    NUM_TOPOLOGYOBJECT_TYPES
  };

public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_topology_object(void);

  //---------------------------------------------------------------------------
  //: Constructor with given sizes for arrays of inferiors and superiors
  //---------------------------------------------------------------------------
  explicit vtol_topology_object(int num_inferiors,
                                   int num_superiors);
protected:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_topology_object();

public:
  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************
  virtual vtol_topology_object *cast_to_topology_object(void) { return this; }
  virtual const vtol_topology_object*cast_to_topology_object(void)const{return this;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_vertex *cast_to_vertex(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vertex, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_vertex *cast_to_vertex(void) { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_zero_chain *cast_to_zero_chain(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a zero_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_zero_chain *cast_to_zero_chain(void) { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge *cast_to_edge(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge *cast_to_edge(void) { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an one_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_one_chain *cast_to_one_chain(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an one_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_one_chain *cast_to_one_chain(void) { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_face *cast_to_face(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_face *cast_to_face(void) { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_two_chain *cast_to_two_chain(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a two_chain, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_two_chain *cast_to_two_chain(void) { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_block *cast_to_block(void) const { return 0; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a block, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_block *cast_to_block(void) { return 0; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_inferior_type(const vtol_topology_object &inferior) const=0;

  //---------------------------------------------------------------------------
  //: Is `superior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_superior_type(const vtol_topology_object &superior) const=0;

  //---------------------------------------------------------------------------
  //: Is `inferior' already an inferior of `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  is_inferior(const vtol_topology_object &inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' already a superior of `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  is_superior(const vtol_topology_object &superior) const;

  //---------------------------------------------------------------------------
  //: Number of inferiors
  //---------------------------------------------------------------------------
  virtual int numinf(void) const;

  //---------------------------------------------------------------------------
  //: Number of superiors
  //---------------------------------------------------------------------------
  virtual int numsup(void) const;

  //---------------------------------------------------------------------------
  //: Return the superiors list
  //---------------------------------------------------------------------------
  virtual const vcl_vector<vtol_topology_object_sptr> *superiors(void) const;

  //---------------------------------------------------------------------------
  //: Return the inferiors list
  //---------------------------------------------------------------------------
  virtual       vcl_vector<vtol_topology_object_sptr> *inferiors(void);
  virtual const vcl_vector<vtol_topology_object_sptr> *inferiors(void) const;

  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_type topology_type(void) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Link `this' with an inferior `inferior'
  //  REQUIRE: valid_inferior_type(inferior) and !is_inferior(inferior)
  //---------------------------------------------------------------------------
  virtual void link_inferior(vtol_topology_object &inferior);

  //---------------------------------------------------------------------------
  //: Unlink `this' with the inferior `inferior'
  //  REQUIRE: valid_inferior_type(inferior) and is_inferior(inferior)
  //---------------------------------------------------------------------------
  virtual void unlink_inferior(vtol_topology_object &inferior);

  //---------------------------------------------------------------------------
  //: Unlink `this' with all its inferiors
  //---------------------------------------------------------------------------
  virtual void unlink_all_inferiors(void);

  //---------------------------------------------------------------------------
  //: Unlink `this' of the network
  //---------------------------------------------------------------------------
  virtual void unlink(void);


  //---------------------------------------------------------------------------
  //: Get lists of vertices
  //---------------------------------------------------------------------------
  vertex_list *vertices(void);
  void vertices(vertex_list &list);

  //---------------------------------------------------------------------------
  //: Get lists of zero chains
  //---------------------------------------------------------------------------
  zero_chain_list *zero_chains(void);
  void zero_chains(zero_chain_list &list);

  //---------------------------------------------------------------------------
  //: Get lists of edges
  //---------------------------------------------------------------------------
  edge_list *edges(void);
  void edges(edge_list &list);


  //---------------------------------------------------------------------------
  //: Get lists of one chains
  //---------------------------------------------------------------------------
  one_chain_list *one_chains(void);
  void one_chains(one_chain_list &list);

  //---------------------------------------------------------------------------
  //: Get lists of faces
  //---------------------------------------------------------------------------
  face_list *faces(void);
  void faces(face_list &list);

  //---------------------------------------------------------------------------
  //: Get lists of two chains
  //---------------------------------------------------------------------------
  two_chain_list *two_chains(void);
  void two_chains(two_chain_list &list);

  //---------------------------------------------------------------------------
  //: Get lists of blocks
  //---------------------------------------------------------------------------
  block_list *blocks(void);
  void blocks(block_list &list);

  //---------------------------------------------------------------------------
  //: print and describe the objects
  //---------------------------------------------------------------------------
  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe_inferiors(vcl_ostream &strm=vcl_cout,
                                  int blanking=0) const;
  virtual void describe_superiors(vcl_ostream &strm=vcl_cout,
                                  int blanking=0) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;
private:
  //***************************************************************************
  // WARNING: the 2 following methods are directly called only by the superior
  // class. It is FORBIDDEN to use them directly
  // If you want to link and unlink superior use sup.link_inferior(*this)
  // of sup.unlink_inferior(*this)
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Link `this' with a superior `superior'
  //  REQUIRE: valid_superior_type(superior) and !is_superior(superior)
  //---------------------------------------------------------------------------
  virtual void link_superior(vtol_topology_object &superior);

  //---------------------------------------------------------------------------
  //: Unlink `this' with its superior `superior'
  //  REQUIRE: valid_superior_type(superior) and is_superior(superior)
  //---------------------------------------------------------------------------
  virtual void unlink_superior(vtol_topology_object &superior);

public:

  //---------------------------------------------------------------------------
  //: compute lists of vertices
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_vertex*> *compute_vertices(void);

  //---------------------------------------------------------------------------
  //: compute lists of zero chains
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_zero_chain*> *compute_zero_chains(void);

  //---------------------------------------------------------------------------
  //: compute lists of edges
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_edge*> *compute_edges(void);

  //---------------------------------------------------------------------------
  //: compute lists of one chains
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_one_chain*> *compute_one_chains(void);

  //---------------------------------------------------------------------------
  //: compute lists of faces
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_face*> *compute_faces(void);

  //---------------------------------------------------------------------------
  //: compute lists of two chains
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_two_chain*> *compute_two_chains(void);

  //---------------------------------------------------------------------------
  //: compute lists of blocks
  // WARNING should not be used by clients
  //---------------------------------------------------------------------------
  virtual vcl_vector<vtol_block*> *compute_blocks(void);

protected:

  //---------------------------------------------------------------------------
  // Description: array of superiors
  //---------------------------------------------------------------------------
  vcl_list<vtol_topology_object_sptr> _superiors;

  //---------------------------------------------------------------------------
  // Description: array of inferiors
  //---------------------------------------------------------------------------
  vcl_vector<vtol_topology_object_sptr> _inferiors;

private:
  //---------------------------------------------------------------------------
  // Description: cache system
  //---------------------------------------------------------------------------
  vtol_topology_cache *inf_sup_cache;

  // declare a freind class
  friend class vtol_topology_cache;
};

#endif // topology_object_h
