#ifndef TopologyHierarchyNode_H
#define TopologyHierarchyNode_H


//
// Class : vtol_topology_hierarchy_node
//
// .SECTION Description
//  The vtol_topology_hierarchy_node is a base class inherited by top_hold.
//  It provides the data and methods for creating the doubly linked structure
//  of inferiors and superiors.
//
// .NAME        vtol_topology_hierarchy_node - Base class of top_hold (inferior/superior mechanics)
// .LIBRARY     vtol
// .HEADER SpatialObjects package
// .INCLUDE     Topology/vtol_topology_hierarchy_node.h
// .FILE        vtol_topology_hierarchy_node.h
// .FILE        vtol_topology_hierarchy_node.cxx
// .SECTION Author
//     Patricia A. Vrobel - ported Peter Tu
//
//-----------------------------------------------------------------------------

// #include <cool/ListP.h>
// #include <cool/ArrayP.h>

#include <vcl/vcl_vector.h>


class top_hold;

typedef vcl_vector<top_hold*> topology_list;

class vtol_topology_hierarchy_node
{
  // public:
  // TOPOLOGY_DLL_DATA static bool DEFER_SUPERIORS;

protected:

  topology_list _inferiors;
  topology_list _superiors;
  topology_list * inf() { return &_inferiors; }
  topology_list * sup() { return &_superiors; }

  // classes used to simplify the find and remove processes

  bool find(topology_list &list, top_hold *object);
  void remove(topology_list &list, top_hold *object);
    

public:
  // Constructors and Destructors

 vtol_topology_hierarchy_node();

//   vtol_topology_hierarchy_node(int num_inferiors, int num_superiors);
 ~vtol_topology_hierarchy_node();

  // Accessors

  int numinf() const { return _inferiors.size(); }
  int numsup() const { return _superiors.size(); }

  // New stuff

  //  void DescribeInferiors(ostream& strm=cout,int blanking=0);
  //  void DescribeSuperiors(ostream& strm=cout,int blanking=0);
    //  void Describe(ostream& strm=cout,int blanking=0);
  void Print(ostream& strm=cout);

  inline topology_list * get_inferiors() { return &_inferiors; }
  inline topology_list * get_superiors() { return &_superiors; }

  void set_inferiors(topology_list& newinf) { _inferiors = newinf; }
  void set_superiors(topology_list& newsup) { _superiors = newsup; }
  topology_list * inferiors() { return &_inferiors;}
  topology_list * superiors() { return &_superiors;}


  bool link_inferior(top_hold*, top_hold*);
  bool link_superior(top_hold*, top_hold*);
  bool link_inferior_oneway(top_hold*);
  bool link_superior_oneway(top_hold*);
  bool unlink_inferior(top_hold*, top_hold*);
  bool unlink_superior(top_hold*, top_hold*);

  void unlink_all_superiors(top_hold*);
  void unlink_all_inferiors(top_hold*);
  bool unlink_superior_simple(top_hold*);
  bool unlink_inferior_simple(top_hold*);
  void unlink_all_superiors_twoway( top_hold * );
  void unlink_all_inferiors_twoway( top_hold * );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_topology_hierarchy_node.




