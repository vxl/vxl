#ifndef vtol_hierarchy_node_3d_h_
#define vtol_hierarchy_node_3d_h_
//-----------------------------------------------------------------------------
//
// .NAME        vtol_hierarchy_node_3d - Base class of TwoChain and OneChain (representation of holes)
// .LIBRARY     vtol
// .HEADER vxl package
// .INCLUDE     vtol/vtol_hierarchy_node_3d.h
// .FILE        vtol_hierarchy_node_3d.cxx
//
// .SECTION Author
//     Patricia A. Vrobel
// .SECTION Description
//   The vtol_hierarchy_node_3d class is a base class of TwoChain and OneChain.  It
//   provides the data and methods for creating the doubly linked subhierarchy
//   of holes in Blocks and Faces respectively.  (Warning:: this class and
//   philosophy of holes may not be around after evaluation of the necessity
//   for Boolean operations....pav).
//
// .SECTION Author
//     Patricia A. Vrobel Peter Tu
//-----------------------------------------------------------------------------

#include <vcl_vector.h>

class vtol_hierarchy_node_3d;

typedef vcl_vector<vtol_hierarchy_node_3d*> hierarchy_node_list_3d;

//: Base class of vtol_two_chain_3d and vtol_one_chain_3d (representation of holes)

class vtol_hierarchy_node_3d
{
protected:

  hierarchy_node_list_3d _hierarchy_inferiors;
  hierarchy_node_list_3d _hierarchy_superiors;

public:

// Constructors and Destructors

//: \brief constructors
  vtol_hierarchy_node_3d();
  ~vtol_hierarchy_node_3d();

// Accessors

//: \brief accessors

  inline hierarchy_node_list_3d * get_hierarchy_inferiors() { return &_hierarchy_inferiors; }
  inline hierarchy_node_list_3d * get_hierarchy_superiors() { return &_hierarchy_superiors; }
  void set_hierarchy_inferiors(const hierarchy_node_list_3d&);
  void set_hierarchy_superiors(const hierarchy_node_list_3d&);

  hierarchy_node_list_3d* hierarchy_inferiors();
  hierarchy_node_list_3d* hierarchy_superiors();

  bool add_hierarchy_inferior(vtol_hierarchy_node_3d*);
  bool remove_hierarchy_inferior(vtol_hierarchy_node_3d*);
  bool add_hierarchy_superior(vtol_hierarchy_node_3d*);
  bool remove_hierarchy_superior(vtol_hierarchy_node_3d*);
  bool add_hierarchy_inferior_oneway(vtol_hierarchy_node_3d*);
  bool add_hierarchy_superior_oneway(vtol_hierarchy_node_3d*);

  int num_hierarchy_superiors( void ) { return _hierarchy_superiors.size(); }
  int num_hierarchy_inferiors( void ) { return _hierarchy_inferiors.size(); }

};


#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_hierarchy_node_3d.





