// This is gel/vtol/vtol_chain.cxx
#include "vtol_chain.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_chain::vtol_chain(void) = default;

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_chain::~vtol_chain() = default;

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
//
// Deprecated.
//---------------------------------------------------------------------------
const chain_list *
vtol_chain::chain_superiors(void) const
{
  auto *result=new chain_list;
  result->reserve(chain_superiors_.size());
  std::list<vtol_chain*>::const_iterator i;
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
vtol_chain::is_chain_inferior(vtol_chain_sptr chain_inferior) const
{
  chain_list::const_iterator i;

  for (i=chain_inferiors_.begin();
       i!=chain_inferiors_.end() && (*i)!=chain_inferior;
       ++i)
    ;
  return i!=chain_inferiors_.end();
}

//---------------------------------------------------------------------------
//: Is `superior' already a superior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_chain::is_chain_superior(vtol_chain const* chain_superior) const
{
  std::list<vtol_chain*>::const_iterator i;
  for (i=chain_superiors_.begin();
       i!=chain_superiors_.end() && (*i)!=chain_superior;
       ++i)
    /*nothing*/;

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
void vtol_chain::link_chain_inferior(const vtol_chain_sptr& chain_inferior)
{
  // require
  assert(valid_chain_type(chain_inferior));
  assert(!is_chain_inferior(chain_inferior));
  assert(!chain_inferior->is_chain_superior(this));

  chain_inferiors_.push_back(chain_inferior);
  chain_inferior->chain_superiors_.push_back(this);
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with the chain_inferior `chain_inferior'
// Require: valid_chain_type(chain_inferior)
//          and is_chain_inferior(chain_inferior)
//---------------------------------------------------------------------------
void vtol_chain::unlink_chain_inferior(const vtol_chain_sptr& chain_inferior)
{
  // require
  assert(valid_chain_type(chain_inferior));
  assert(is_chain_inferior(chain_inferior));
  assert(chain_inferior->is_chain_superior(this));

  auto i=chain_inferior->chain_superiors_.begin();
  while ( i!=chain_inferior->chain_superiors_.end() && *i!=this ) ++i;
  // check presence in "chain_superiors_" list of chain_inferior:
  assert(*i==this);

  // unlink "this" from chain_superiors_ list of chain_inferior:
  chain_inferior->chain_superiors_.erase(i);

  auto j=chain_inferiors_.begin();
  while ( j!=chain_inferiors_.end() && (*j)!=chain_inferior ) ++j;
  // check presence in "chain_inferiors_" list:
  assert((*j)==chain_inferior);

  chain_inferiors_.erase(j);
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with all its chain inferiors
//---------------------------------------------------------------------------
void vtol_chain::unlink_all_chain_inferiors(void)
{
  while (chain_inferiors_.size()>0)
    unlink_chain_inferior(chain_inferiors_.back());
}

//---------------------------------------------------------------------------
//: Unlink `this' of the network
//---------------------------------------------------------------------------
void vtol_chain::unlink(void)
{
  while (chain_superiors_.size()>0)
    chain_superiors_.front()->unlink_chain_inferior(this);
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
