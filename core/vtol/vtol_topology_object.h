#ifndef TOPOLOGY_OBJECT_H
#define TOPOLOGY_OBJECT_H

//#include <vsbl/vsol_spatial_object.h>
//#include <vtol/vtol_topology_hierarchy.h>

//for testing purposes, we will use the following classes
#include <vsol/vsol_spatial_object.h>
#include <vtol/vtol_topology_hierarchy_node.h>

class vtol_zero_chain;
class vtol_vertex;
class vtol_one_chain;
class vtol_vertex;
class vtol_face;
class two_chain;
class vtol_block;
class vtol_curve;
#undef vertices
class vtol_topology_cache;


class vtol_topology_object : public vsol_spatial_object, public vtol_topology_hierarchy_node

{

protected:
  virtual ~vtol_topology_object();

public:

  //xtors and dtors
  vtol_topology_object(int num_inferiors, int num_superiors);
  virtual void ProtectedDestroy();
  static void Destroy(vtol_topology_object*);

  //safe casting methods
  vtol_topology_object* CastToTopologyObject() { return this;}
  virtual vtol_vertex * CastToVertex() { return 0; }
  virtual vtol_zero_chain * CastToZeroChain() { return 0; }
  virtual vtol_vertex * CastToEdge() { return 0; }
  
  //accessors
  virtual vcl_vector<vtol_vertex*>* Vertices();
  void Vertices(vcl_vector<vtol_vertex*>&);
  virtual vcl_vector<vtol_zero_chain*>* ZeroChains();
  void ZeroChains( vcl_vector<vtol_zero_chain*>&);
  virtual vcl_vector<vtol_vertex*>* Edges();
  void Edges(vcl_vector<vtol_vertex*>&);

  //utilities
    inline bool link_inferior(vtol_topology_object* child)
    { return topology_hierarchy_node::link_inferior(this, child); }
  inline bool link_superior(vtol_topology_object* parent)
    { return topology_hierarchy_node::link_superior(parent, this); }
  inline bool unlink_inferior(vtol_topology_object* child)
    { return topology_hierarchy_node::unlink_inferior(this, child); }
  inline bool unlink_superior(vtol_topology_object* parent)
    { return topology_hierarchy_node::unlink_superior(parent, this); }

  virtual void Describe(ostream& strm=cout,int blanking=0);
  virtual void Print (ostream& strm =cout);


private:
  //members
  vtol_topology_cache * inf_sup_cache;



};


















#endif //TOPOLOGY_OBJECT_H
