#ifndef vtol_chain_2d_h_
#define vtol_chain_2d_h_
//-----------------------------------------------------------------------------
//
// Class : vtol_chain_2d
//
//
// .NAME        vtol_chain_2d - Base class of TwoChain and OneChain (representation of holes)
// .LIBRARY     vtol
// .HEADER vxl package
// .INCLUDE     vtol/vtol_chain_2d.h
// .FILE        vtol_chain_2d.h
// .FILE        vtol_chain_2d.cxx
// .SECTION Author
//     Patricia A. Vrobel
// .SECTION Description
//   The vtol_chain_2d class is a base class of TwoChain and OneChain.  It
//   provides the data and methods for creating the doubly linked subhierarchy
//   of holes in Blocks and Faces respectively.  (Warning:: this class and
//   philosophy of holes may not be around after evaluation of the necessity
//   for Boolean operations....pav).
//
// .SECTION Author
//     Patricia A. Vrobel Peter Tu
//-----------------------------------------------------------------------------
#include <vtol/vtol_chain_2d_ref.h>

#include <vtol/vtol_topology_object_2d.h>
#include <vcl/vcl_vector.h>

//class vtol_hierarchy_node_2d;

typedef vcl_vector<vtol_chain_2d_ref> chain_list_2d;

//: Base class of vtol_two_chain_2d and vtol_one_chain_2d (representation of holes)

//*****************************************************************************
// ALL THE DERIVED AND NONE ABSTRACT CLASSES OF THIS CLASS MUST CALL
// unlink_all_chain_inferiors() IN THEIR DESTRUCTOR
// unlink_all_chain_inferiors() CANT BE CALLED DIRECTLY IN THIS CLASS,
// OTHERWISE BAD VERSIONS OF METHODS SHOULD BE CALLED (C++ IS STUPID !)
//*****************************************************************************

class vtol_chain_2d
  :public vtol_topology_object_2d
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************
  
  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_chain_2d(void);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_chain_2d();

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return a pointer to the inferiors (no copy)
  //---------------------------------------------------------------------------
  virtual const chain_list_2d *chain_inferiors(void) const;

  //---------------------------------------------------------------------------
  //: Return a pointer to the superiors (no copy)
  //---------------------------------------------------------------------------
  virtual const chain_list_2d *chain_superiors(void) const;
  
  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `chain_inf_sup' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_chain_type(const vtol_chain_2d &chain_inf_sup) const=0;

  //---------------------------------------------------------------------------
  //: Is `inferior' already an inferior of `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  is_chain_inferior(const vtol_chain_2d &chain_inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' already a superior of `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  is_chain_superior(const vtol_chain_2d &chain_superior) const;

  //---------------------------------------------------------------------------
  //: Number of superiors
  //---------------------------------------------------------------------------
  virtual int num_chain_superiors(void) const;

  //---------------------------------------------------------------------------
  //: Number of inferiors
  //---------------------------------------------------------------------------
  virtual int num_chain_inferiors(void) const;

  //---------------------------------------------------------------------------
  //: Does `this' contain some sub chains ?
  //---------------------------------------------------------------------------
  virtual bool contains_sub_chains(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' a sub chain ?
  //---------------------------------------------------------------------------
  virtual bool is_sub_chain(void) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Link `this' with an inferior `chain_inferior'
  //: REQUIRE: valid_chain_type(chain_inferior)
  //:          and !is_chain_inferior(chain_inferior)
  //---------------------------------------------------------------------------
  virtual void link_chain_inferior(vtol_chain_2d &chain_inferior);

  //---------------------------------------------------------------------------
  //: Unlink `this' with the chain_inferior `chain_inferior'
  //: REQUIRE: valid_chain_type(chain_inferior)
  //:          and is_chain_inferior(chain_inferior)
  //---------------------------------------------------------------------------
  virtual void unlink_chain_inferior(vtol_chain_2d &chain_inferior);
  
  //---------------------------------------------------------------------------
  //: Unlink `this' with all its chain inferiors
  //---------------------------------------------------------------------------
  virtual void unlink_all_chain_inferiors(void);

  //---------------------------------------------------------------------------
  //: Unlink `this' of the network
  //---------------------------------------------------------------------------
  virtual void unlink(void);

  //---------------------------------------------------------------------------
  //: Is `this' a connected chain ?
  //---------------------------------------------------------------------------
  virtual bool is_cycle(void) const;

  //---------------------------------------------------------------------------
  //: Return the directions
  //---------------------------------------------------------------------------
  virtual const vcl_vector<signed char> *directions(void) const;

  //---------------------------------------------------------------------------
  //: Return the directions
  //---------------------------------------------------------------------------
  virtual vcl_vector<signed char> *directions(void);

  //---------------------------------------------------------------------------
  //: Return the direction `i'
  //---------------------------------------------------------------------------
  virtual int dir(int i) const;

  //---------------------------------------------------------------------------
  //: Set if `this' is a connected chain
  //---------------------------------------------------------------------------
  virtual void set_cycle(bool new_is_cycle);

  //---------------------------------------------------------------------------
  // Task: Reset the chain
  //---------------------------------------------------------------------------
  virtual void clear(void);

private:
  //***************************************************************************
  // WARNING: the 2 following methods are directly called only by the superior
  // class. It is FORBIDDEN to use them directly
  // If you want to link and unlink superior use sup.link_chain_inferior(*this)
  // of sup.unlink_chain_inferior(*this)
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Link `this' with a chain superior `chain_superior'
  //: REQUIRE: valid_chain_type(chain_superior)
  //:          and !is_chain_superior(chain_superior)
  //---------------------------------------------------------------------------
  virtual void link_chain_superior(vtol_chain_2d &chain_superior);

  //---------------------------------------------------------------------------
  //: Unlink `this' with its chain superior `chain_superior'
  //: REQUIRE: valid_chain_type(chain_superior)
  //:          and is_chain_superior(chain_superior)
  //---------------------------------------------------------------------------
  virtual void unlink_chain_superior(vtol_chain_2d &chain_superior);

  //***************************************************************************
  //: Implementation
  //***************************************************************************
protected:
  //---------------------------------------------------------------------------
  // Description: array of the inferiors
  //---------------------------------------------------------------------------
  chain_list_2d _chain_inferiors;

  //---------------------------------------------------------------------------
  // Description: array of the superiors
  //---------------------------------------------------------------------------
  vcl_list<vtol_chain_2d_ref> _chain_superiors;

  bool _is_cycle; // True if `this' is a connected chain
  vcl_vector<signed char> _directions;
};
#endif
