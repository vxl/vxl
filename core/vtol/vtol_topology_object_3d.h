#ifndef TOPOLOGY_OBJECT_3D_H
#define TOPOLOGY_OBJECT_3D_H

//
// .NAME        vtol_topology_object_3d - base class for topology objects
// .LIBRARY     vtol
// .HEADER vxl package
// .INCLUDE     vtol/vtol_topology_object_3d.h
// .FILE        vtol_topology_object_3d.h
// .FILE        vtol_topology_object_3d.C
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


#include <vsol/vsol_spatial_object_3d.h>
#include <vtol/vtol_topology_hierarchy_node_3d.h>

class vtol_vertex_3d;
class vtol_zero_chain_3d;
class vtol_edge_3d;
class vtol_one_chain_3d;
class vtol_face_3d;
class vtol_block_3d;


class vtol_topology_cache_3d;


class vtol_topology_object_3d : public vsol_spatial_object_3d, public vtol_topology_hierarchy_node_3d

{
public:
  enum vtol_topology_object_3d_type
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
  virtual ~vtol_topology_object_3d();

public:

  //xtors and dtors
  //: \brief Constructor
  vtol_topology_object_3d(void)
  {
  }
  vtol_topology_object_3d(int num_inferiors,
                          int num_superiors);
  virtual void protected_destroy(void);
  static void destroy(vtol_topology_object_3d *);

  //: \brief safe casting methods
  virtual vtol_topology_object_3d *cast_to_topology_object_3d(void)
  {
    return this;
  }
  virtual vtol_vertex_3d *cast_to_vertex_3d(void)
  {
    return 0;
  }
  virtual vtol_zero_chain_3d *cast_to_zero_chain_3d(void)
  {
    return 0;
  }
  virtual vtol_edge_3d *cast_to_edge_3d(void)
  {
    return 0;
  }
  virtual vtol_one_chain_3d *cast_to_one_chain_3d(void)
  {
    return 0;
  }
  virtual vtol_face_3d *cast_to_face_3d(void)
  {
    return 0;
  }
  virtual vtol_two_chain_3d *cast_to_two_chain_3d(void)
  {
    return 0;
  }
  virtual vtol_block_3d *cast_to_block_3d(void)
  {
    return 0;
  } 


  //: \brief accessors

  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  virtual vertex_list_3d *vertices(void);
  virtual zero_chain_list_3d *zero_chains(void);
  virtual edge_list_3d *edges(void);
  virtual one_chain_list_3d *one_chains(void);
  virtual face_list_3d *faces(void)
  {
    return (face_list_3d*)0;
  }
  virtual two_chain_list_3d *two_chains(void)
  {
    return (two_chain_list_3d*)0;
  }
  virtual block_list_3d *blocks(void)
  {
    return (block_list_3d*)0;
  }

  void vertices(vertex_list_3d &);
  void zero_chains(zero_chain_list_3d &);
  void edges(edge_list_3d &);
  void one_chains(one_chain_list_3d &);
  void faces(face_list_3d &)
  {
  }
  void two_chains(two_chain_list_3d &)
  {
  }
  void blocks(block_list_3d &)
  {
  }


  //: \brief utilities
  inline bool link_inferior(vtol_topology_object_3d *child)
  {
    return vtol_topology_hierarchy_node_3d::link_inferior(this,child);
  }
  inline bool link_superior(vtol_topology_object_3d* parent)
  {
    return vtol_topology_hierarchy_node_3d::link_superior(parent,this);
  }
  inline bool unlink_inferior(vtol_topology_object_3d *child)
  {
    return vtol_topology_hierarchy_node_3d::unlink_inferior(this,child);
  }
  inline bool unlink_superior(vtol_topology_object_3d *parent)
  {
    return vtol_topology_hierarchy_node_3d::unlink_superior(parent,this);
  }
  virtual void print(ostream &strm=cout);
private:
  //: \brief members
  vtol_topology_cache_3d *inf_sup_cache;
};

#endif //TOPOLOGY_OBJECT_3D_H
