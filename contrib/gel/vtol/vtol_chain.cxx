// This is gel/vtol/vtol_chain.cxx
#include "vtol_chain.h"
//:
//  \file

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
  return &_chain_inferiors;
}

//---------------------------------------------------------------------------
//: Return a pointer to the superiors (no copy)
//---------------------------------------------------------------------------
const chain_list *
vtol_chain::chain_superiors(void) const
{
  chain_list *result;
  vcl_list<vtol_chain_sptr>::const_iterator i;

  result=new chain_list();
  result->reserve(_chain_superiors.size());
  for (i=_chain_superiors.begin();i!=_chain_superiors.end();i++)
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
  bool result;
  vcl_vector<vtol_chain_sptr>::const_iterator i;

  for (i=_chain_inferiors.begin();
      (i!=_chain_inferiors.end())&&((*i).ptr()!=&chain_inferior);
      i++)
    ;
  result=i!=_chain_inferiors.end();

  return result;
}

//---------------------------------------------------------------------------
//: Is `superior' already a superior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_chain::is_chain_superior(const vtol_chain &chain_superior) const
{
  bool result;
  vcl_list<vtol_chain_sptr>::const_iterator i;


  for (i=_chain_superiors.begin();
      (i!=_chain_superiors.end())&&((*i).ptr()!=&chain_superior);
      i++);

  result=i!=_chain_superiors.end();

  return result;
}

//---------------------------------------------------------------------------
//: Return the number of superiors
//---------------------------------------------------------------------------
int vtol_chain::num_chain_superiors(void) const
{
  return _chain_superiors.size();
}

//---------------------------------------------------------------------------
//: Return the number of inferiors
//---------------------------------------------------------------------------
int vtol_chain::num_chain_inferiors(void) const
{
  return _chain_inferiors.size();
}

//---------------------------------------------------------------------------
//: Does `this' contain some sub chains ?
//---------------------------------------------------------------------------
bool vtol_chain::contains_sub_chains(void) const
{
  return _chain_inferiors.size()>0;
}

//---------------------------------------------------------------------------
//: Is `this' a sub chain ?
//---------------------------------------------------------------------------
bool vtol_chain::is_sub_chain(void) const
{
  return _chain_superiors.size()>0;
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

  _chain_inferiors.push_back(&chain_inferior);
  ref();
  chain_inferior.link_chain_superior(*this);
  ref_count--;
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

  vcl_vector<vtol_chain_sptr>::iterator i=_chain_inferiors.begin();
  while ((i!=_chain_inferiors.end())&&((*i).ptr()!=&chain_inferior))
    ++i;
  chain_inferior.unlink_chain_superior(*this);
  _chain_inferiors.erase(i);
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with all its chain inferiors
//---------------------------------------------------------------------------
void vtol_chain::unlink_all_chain_inferiors(void)
{
  while (_chain_inferiors.size()>0)
    {
      ref();
      (*_chain_inferiors.begin())->unlink_chain_superior(*this);
      _chain_inferiors.erase(_chain_inferiors.begin());
    }
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' of the network
//---------------------------------------------------------------------------
void vtol_chain::unlink(void)
{
  while (_chain_superiors.size()>0)
    (*_chain_superiors.begin())->unlink_chain_inferior(*this);
  unlink_all_chain_inferiors();
  vtol_topology_object::unlink();
}

//***************************************************************************
// WARNING: the 2 following methods are directly called only by the superior
// class. It is FORBIDDEN to use them directly
// If you want to link and unlink superior use sup.link_chain_inferior(*this)
// of sup.unlink_chain_inferior(*this)
//***************************************************************************

//---------------------------------------------------------------------------
//: Link `this' with a chain superior `chain_superior'
// Require: valid_chain_type(chain_superior)
//          and !is_chain_superior(chain_superior)
//---------------------------------------------------------------------------
void vtol_chain::link_chain_superior(vtol_chain &chain_superior)
{
  // require
  assert(valid_chain_type(chain_superior));
  assert(!is_chain_superior(chain_superior));

  vcl_list<vtol_chain_sptr>::iterator i;

  _chain_superiors.push_back(&chain_superior);
  i=_chain_superiors.end();
  i--;
//(*i).unprotect();
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with its chain superior `chain_superior'
// Require: valid_chain_type(chain_superior) and is_chain_superior(chain_superior)
//---------------------------------------------------------------------------
void vtol_chain::unlink_chain_superior(vtol_chain &chain_superior)
{
  //require


  assert(valid_chain_type(chain_superior));
  assert(is_chain_superior(chain_superior));

  vcl_list<vtol_chain_sptr>::iterator i=_chain_superiors.begin();
  while ((i!=_chain_superiors.end())&&((*i).ptr()!=&chain_superior))
      ++i;

  // check
  assert((*i).ptr()==&chain_superior);

  _chain_superiors.erase(i); // unlink
  touch();
}

//---------------------------------------------------------------------------
//: Is `this' a connected chain ?
//---------------------------------------------------------------------------
bool vtol_chain::is_cycle(void) const
{
  return _is_cycle;
}

//---------------------------------------------------------------------------
//: Set if `this' is a connected chain
//---------------------------------------------------------------------------
void vtol_chain::set_cycle(bool new_is_cycle)
{
  _is_cycle=new_is_cycle;
}

//---------------------------------------------------------------------------
//: Return the directions
//---------------------------------------------------------------------------
const vcl_vector<signed char> *vtol_chain::directions(void) const
{
  return &_directions;
}

//---------------------------------------------------------------------------
//: Return the directions
//---------------------------------------------------------------------------
vcl_vector<signed char> *vtol_chain::directions(void)
{
  return &_directions;
}

//---------------------------------------------------------------------------
//: Return the direction `i'
//---------------------------------------------------------------------------
int vtol_chain::dir(int i) const
{
  return _directions[i];
}

//---------------------------------------------------------------------------
//: Reset the chain
//---------------------------------------------------------------------------
void vtol_chain::clear(void)
{
  _directions.clear();
  unlink_all_chain_inferiors();
}
