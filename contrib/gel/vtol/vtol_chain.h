// This is gel/vtol/vtol_chain.h
#ifndef vtol_chain_h_
#define vtol_chain_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Base class for representation of holes
//
//  The vtol_chain class is a base class of vtol_one_chain and vtol_two_chain.
//  It provides the data and methods for creating the doubly linked subhierarchy
//  of holes in Blocks and Faces respectively.  (Warning:: this class and
//  philosophy of holes may not be around after evaluation of the necessity
//  for Boolean operations....pav).
//
// \author
//     Patricia A. Vrobel
//-----------------------------------------------------------------------------

#include <vtol/vtol_chain_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vcl_vector.h>

typedef vcl_vector<vtol_chain_sptr> chain_list;

//: Base class of vtol_two_chain and vtol_one_chain (representation of holes)

//*****************************************************************************
// ALL THE DERIVED AND NONE ABSTRACT CLASSES OF THIS CLASS MUST CALL
// unlink_all_chain_inferiors() IN THEIR DESTRUCTOR
// unlink_all_chain_inferiors() CANT BE CALLED DIRECTLY IN THIS CLASS,
// OTHERWISE BAD VERSIONS OF METHODS SHOULD BE CALLED (C++ IS STUPID !)
//*****************************************************************************

class vtol_chain
  :public vtol_topology_object
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_chain(void);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_chain();

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return a pointer to the inferiors (no copy)
  //---------------------------------------------------------------------------
  virtual const chain_list *chain_inferiors(void) const;

  //---------------------------------------------------------------------------
  //: Return a copy of the chain_superiors list
  // The return value must be deleted by the caller
  //---------------------------------------------------------------------------
  virtual const chain_list *chain_superiors(void) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `chain_inf_sup' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_chain_type(const vtol_chain &chain_inf_sup) const=0;

  //---------------------------------------------------------------------------
  //: Is `inferior' already an inferior of `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  is_chain_inferior(const vtol_chain &chain_inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' already a superior of `this' ?
  //---------------------------------------------------------------------------
  virtual bool is_chain_superior(const vtol_chain &chain_superior) const;

  //---------------------------------------------------------------------------
  //: Return the number of superiors
  //---------------------------------------------------------------------------
  int num_chain_superiors(void) const {return chain_superiors_.size();}

  //---------------------------------------------------------------------------
  //: Return the number of inferiors
  //---------------------------------------------------------------------------
  int num_chain_inferiors(void) const {return chain_inferiors_.size();}

  //---------------------------------------------------------------------------
  //: Does `this' contain some sub chains ?
  //---------------------------------------------------------------------------
  bool contains_sub_chains(void) const { return chain_inferiors_.size()>0; }

  //---------------------------------------------------------------------------
  //: Is `this' a sub chain ?
  //---------------------------------------------------------------------------
  bool is_sub_chain(void) const { return chain_superiors_.size()>0; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Link `this' with an inferior `chain_inferior'
  //  REQUIRE: valid_chain_type(chain_inferior)
  //           and !is_chain_inferior(chain_inferior)
  //---------------------------------------------------------------------------
  virtual void link_chain_inferior(vtol_chain &chain_inferior);

  //---------------------------------------------------------------------------
  //: Unlink `this' with the chain_inferior `chain_inferior'
  //  REQUIRE: valid_chain_type(chain_inferior)
  //           and is_chain_inferior(chain_inferior)
  //---------------------------------------------------------------------------
  virtual void unlink_chain_inferior(vtol_chain &chain_inferior);

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
  const vcl_vector<signed char> *directions(void) const { return &directions_; }

  //---------------------------------------------------------------------------
  //: Return the directions
  //---------------------------------------------------------------------------
  vcl_vector<signed char> *directions(void) { return &directions_; }

  //---------------------------------------------------------------------------
  //: Return the direction `i'
  //---------------------------------------------------------------------------
  int dir(int i) const { return directions_[i]; }

  //---------------------------------------------------------------------------
  //: Set if `this' is a connected chain
  //---------------------------------------------------------------------------
  void set_cycle(bool new_is_cycle) { is_cycle_=new_is_cycle; }

  //---------------------------------------------------------------------------
  // Task: Reset the chain
  //---------------------------------------------------------------------------
  virtual void clear(void);

  //***************************************************************************
  // Implementation
  //***************************************************************************
 protected:
  //---------------------------------------------------------------------------
  // Description: array of the inferiors
  //---------------------------------------------------------------------------
  chain_list chain_inferiors_;

  //---------------------------------------------------------------------------
  // Description: array of the superiors
  //---------------------------------------------------------------------------
  vcl_list<vtol_chain*> chain_superiors_;

  bool is_cycle_; // True if `this' is a connected chain
  vcl_vector<signed char> directions_;
};

#endif // vtol_chain_h_
