// This is gel/vtol/vtol_chain.cxx
#include "vtol_chain.h"
//:
// \file

#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_chain::vtol_chain(void)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_chain::~vtol_chain()
{
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return a pointer to the inferiors (no copy)
//---------------------------------------------------------------------------
const chain_list *
vtol_chain::chain_inferiors(void) const
{
  return &chain_inferiors_;
}

//---------------------------------------------------------------------------
//: Return a copy of the chain_superiors list
// The return value must be deleted by the caller
//---------------------------------------------------------------------------
const chain_list *
vtol_chain::chain_superiors(void) const
{
  chain_list *result=new chain_list;
  result->reserve(chain_superiors_.size());
  vcl_list<vtol_chain*>::const_iterator i;
  for (i=chain_superiors_.begin();i!=chain_superiors_.end();i++)
    result->push_back(*i);
  return result;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' already an inferior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_chain::is_chain_inferior(const vtol_chain &chain_inferior) const
{
  chain_list::const_iterator i;

  for (i=chain_inferiors_.begin();
       i!=chain_inferiors_.end() && (*i).ptr()!=&chain_inferior;
       i++)
    ;
  return i!=chain_inferiors_.end();
}

//---------------------------------------------------------------------------
//: Is `superior' already a superior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_chain::is_chain_superior(const vtol_chain &chain_superior) const
{
  vcl_list<vtol_chain*>::const_iterator i;
  for (i=chain_superiors_.begin();
       i!=chain_superiors_.end() && (*i)!=&chain_superior;
       ++i);

  return i!=chain_superiors_.end();
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Link `this' with an inferior `chain_inferior'
// Require: valid_chain_type(chain_inferior)
//          and !is_chain_inferior(chain_inferior)
//---------------------------------------------------------------------------
void vtol_chain::link_chain_inferior(vtol_chain &chain_inferior)
{
  // require
  assert(valid_chain_type(chain_inferior));
  assert(!is_chain_inferior(chain_inferior));
  assert(!chain_inferior.is_chain_superior(*this));

  chain_inferiors_.push_back(&chain_inferior);
  chain_inferior.chain_superiors_.push_back(this);
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with the chain_inferior `chain_inferior'
// Require: valid_chain_type(chain_inferior)
//          and is_chain_inferior(chain_inferior)
//---------------------------------------------------------------------------
void vtol_chain::unlink_chain_inferior(vtol_chain &chain_inferior)
{
  // require
  assert(valid_chain_type(chain_inferior));
  assert(is_chain_inferior(chain_inferior));
  assert(chain_inferior.is_chain_superior(*this));

  vcl_list<vtol_chain*>::iterator i=chain_inferior.chain_superiors_.begin();
  while ( i!=chain_inferior.chain_superiors_.end() && *i!=this ) ++i;
  // check presence in "chain_superiors_" list of chain_inferior:
  assert(*i==this);

  // unlink "this" from chain_superiors_ list of chain_inferior:
  chain_inferior.chain_superiors_.erase(i);

  chain_list::iterator j=chain_inferiors_.begin();
  while ( j!=chain_inferiors_.end() && (*j).ptr()!=&chain_inferior ) ++j;
  // check presence in "chain_inferiors_" list:
  assert((*j).ptr()==&chain_inferior);

  chain_inferiors_.erase(j);
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with all its chain inferiors
//---------------------------------------------------------------------------
void vtol_chain::unlink_all_chain_inferiors(void)
{
  while (chain_inferiors_.size()>0)
    unlink_chain_inferior(*(chain_inferiors_.back()));
}

//---------------------------------------------------------------------------
//: Unlink `this' of the network
//---------------------------------------------------------------------------
void vtol_chain::unlink(void)
{
  while (chain_superiors_.size()>0)
    (*chain_superiors_.begin())->unlink_chain_inferior(*this);
  unlink_all_chain_inferiors();
  vtol_topology_object::unlink();
}

//---------------------------------------------------------------------------
//: Is `this' a connected chain ?
//---------------------------------------------------------------------------
bool vtol_chain::is_cycle(void) const
{
  return is_cycle_;
}

//---------------------------------------------------------------------------
//: Reset the chain
//---------------------------------------------------------------------------
void vtol_chain::clear(void)
{
  directions_.clear();
  unlink_all_chain_inferiors();
}
