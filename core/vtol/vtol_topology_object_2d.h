#ifndef TOPOLOGY_OBJECT_2D_H
#define TOPOLOGY_OBJECT_2D_H

//
// .NAME        vtol_topology_object_2d - base class for topology objects
// .LIBRARY     vtol
// .HEADER vxl package
// .INCLUDE     vtol/vtol_topology_object_2d.h
// .FILE        vtol_topology_object_2d.h
// .FILE        vtol_topology_object_2d.C
// .SECTION Description
//  The TopologyObject class is the interface base class for all
//  topological entites. There are only access methods in this class.
//  TopologyObject inherits both SpatialObject and TopologyHierarchyNode.
//  SpatialObject is the base class for all Topology and Geometry.
//  The Topology class hierarchy:
//
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
// .SECTION Author
//    Patricia A. Vrobel.
//    ported by Luis E. Galup


#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_hierarchy_node_2d.h>

class vtol_vertex_2d;
class vtol_zero_chain_2d;
class vtol_edge_2d;
class vtol_one_chain_2d;
class vtol_face_2d;
class vtol_block_2d;


class vtol_topology_cache_2d;


class vtol_topology_object_2d : public vsol_spatial_object_2d, public vtol_topology_hierarchy_node_2d

{
public:
  enum vtol_topology_object_2d_type
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

protected:
  virtual ~vtol_topology_object_2d();

public:

  //xtors and dtors
  //: \brief Constructor
  vtol_topology_object_2d(void)
  {
  }
  vtol_topology_object_2d(int num_inferiors,
                          int num_superiors);
  virtual void protected_destroy(void);
  static void destroy(vtol_topology_object_2d *);

  //: \brief safe casting methods
  virtual vtol_topology_object_2d *cast_to_topology_object_2d(void)
  {
    return this;
  }
  virtual vtol_vertex_2d *cast_to_vertex_2d(void)
  {
    return 0;
  }
  virtual vtol_zero_chain_2d *cast_to_zero_chain_2d(void)
  {
    return 0;
  }
  virtual vtol_edge_2d *cast_to_edge_2d(void)
  {
    return 0;
  }
  virtual vtol_one_chain_2d *cast_to_one_chain_2d(void)
  {
    return 0;
  }
  virtual vtol_face_2d *cast_to_face_2d(void)
  {
    return 0;
  }
  virtual vtol_two_chain_2d *cast_to_two_chain_2d(void)
  {
    return 0;
  }
  virtual vtol_block_2d *cast_to_block_2d(void)
  {
    return 0;
  } 


  //: \brief accessors

  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  virtual vertex_list_2d *vertices(void);
  virtual zero_chain_list_2d *zero_chains(void);
  virtual edge_list_2d *edges(void);
  virtual one_chain_list_2d *one_chains(void);
  virtual face_list_2d *faces(void)
  {
    return (face_list_2d*)0;
  }
  virtual two_chain_list_2d *two_chains(void)
  {
    return (two_chain_list_2d*)0;
  }
  virtual block_list_2d *blocks(void)
  {
    return (block_list_2d*)0;
  }

  void vertices(vertex_list_2d &);
  void zero_chains(zero_chain_list_2d &);
  void edges(edge_list_2d &);
  void one_chains(one_chain_list_2d &);
  void faces(face_list_2d &)
  {
  }
  void two_chains(two_chain_list_2d &)
  {
  }
  void blocks(block_list_2d &)
  {
  }


  //: \brief utilities
  inline bool link_inferior(vtol_topology_object_2d *child)
  {
    return vtol_topology_hierarchy_node_2d::link_inferior(this,child);
  }
  inline bool link_superior(vtol_topology_object_2d* parent)
  {
    return vtol_topology_hierarchy_node_2d::link_superior(parent,this);
  }
  inline bool unlink_inferior(vtol_topology_object_2d *child)
  {
    return vtol_topology_hierarchy_node_2d::unlink_inferior(this,child);
  }
  inline bool unlink_superior(vtol_topology_object_2d *parent)
  {
    return vtol_topology_hierarchy_node_2d::unlink_superior(parent,this);
  }
  virtual void print(ostream &strm=cout);
private:
  //: \brief members
  vtol_topology_cache_2d *inf_sup_cache;
};

#endif //TOPOLOGY_OBJECT_2D_H
