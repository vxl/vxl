#include <vtol/vtol_chain_2d.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Name: vtol_chain_2d
// Task: Default constructor
//---------------------------------------------------------------------------
vtol_chain_2d::vtol_chain_2d(void)
{
}

//---------------------------------------------------------------------------
// Name: ~vtol_chain_2d
// Task: Destructor
//---------------------------------------------------------------------------
vtol_chain_2d::~vtol_chain_2d()
{
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// Name: chain_inferiors
// Task: Return a pointer to the inferiors (no copy)
//---------------------------------------------------------------------------
const chain_list_2d *
vtol_chain_2d::chain_inferiors(void) const
{
  return &_chain_inferiors;
}

//---------------------------------------------------------------------------
// Name: chain_superiors
// Task: Return a pointer to the superiors (no copy)
//---------------------------------------------------------------------------
const chain_list_2d *
vtol_chain_2d::chain_superiors(void) const
{
  chain_list_2d *result;
  vcl_list<vtol_chain_2d_ref>::const_iterator i;

  result=new chain_list_2d();
  result->reserve(_chain_superiors.size());
  for(i=_chain_superiors.begin();i!=_chain_superiors.end();i++)
    result->push_back(*i);
  return result;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Name: is_chain_inferior
// Task: Is `inferior' already an inferior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_chain_2d::is_chain_inferior(const vtol_chain_2d &chain_inferior) const
{
  bool result;
  vcl_vector<vtol_chain_2d_ref>::const_iterator i;
  
  for(i=_chain_inferiors.begin();
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
vtol_chain_2d::is_chain_superior(const vtol_chain_2d &chain_superior) const
{
  bool result;
  vcl_list<vtol_chain_2d_ref>::const_iterator i;
  
  for(i=_chain_superiors.begin();
      (i!=_chain_superiors.end())&&((*i).ptr()!=&chain_superior);
      i++)
    ;
  result=i!=_chain_superiors.end();

  return result;
}

//---------------------------------------------------------------------------
// Name: num_chain_superiors
// Task: Return the number of superiors
//---------------------------------------------------------------------------
int vtol_chain_2d::num_chain_superiors(void) const
{
  return _chain_superiors.size();
}

//---------------------------------------------------------------------------
// Name: num_chain_inferiors
// Task: Return the number of inferiors
//---------------------------------------------------------------------------
int vtol_chain_2d::num_chain_inferiors(void) const
{
  return _chain_inferiors.size();
}

//---------------------------------------------------------------------------
// Name: contains_sub_chains
// Task: Does `this' contain some sub chains ?
//---------------------------------------------------------------------------
bool vtol_chain_2d::contains_sub_chains(void) const
{
  return _chain_inferiors.size()>0;
}

//---------------------------------------------------------------------------
// Name: is_sub_chain
// Task: Is `this' a sub chain ?
//---------------------------------------------------------------------------
bool vtol_chain_2d::is_sub_chain(void) const
{
  return _chain_superiors.size()>0;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Name: link_chain_inferior
// Task: Link `this' with an inferior `chain_inferior'
// Require: valid_chain_type(chain_inferior)
//          and !is_chain_inferior(chain_inferior)
//---------------------------------------------------------------------------
void vtol_chain_2d::link_chain_inferior(vtol_chain_2d &chain_inferior)
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
// Name: unlink_chain_inferior
// Task: Unlink `this' with the chain_inferior `chain_inferior'
// Require: valid_chain_type(chain_inferior)
//          and is_chain_inferior(chain_inferior)
//---------------------------------------------------------------------------
void vtol_chain_2d::unlink_chain_inferior(vtol_chain_2d &chain_inferior)
{
  // require
  assert(valid_chain_type(chain_inferior));
  assert(is_chain_inferior(chain_inferior));
  
  vcl_vector<vtol_chain_2d_ref>::iterator i;
  
  for(i=_chain_inferiors.begin();
      (i!=_chain_inferiors.end())&&((*i).ptr()!=&chain_inferior);
      i++)
    ;
  chain_inferior.unlink_chain_superior(*this);
  _chain_inferiors.erase(i);
  touch();
}
  
//---------------------------------------------------------------------------
// Name: unlink_all_chain_inferiors
// Task: Unlink `this' with all its chain inferiors
//---------------------------------------------------------------------------
void vtol_chain_2d::unlink_all_chain_inferiors(void)
{
  while(_chain_inferiors.size()>0)
    {
      ref();
      (*_chain_inferiors.begin())->unlink_chain_superior(*this);
      _chain_inferiors.erase(_chain_inferiors.begin());
    }
  touch();
}

//---------------------------------------------------------------------------
// Name: unlink
// Task: Unlink `this' of the network
//---------------------------------------------------------------------------
void vtol_chain_2d::unlink(void)
{
  while(_chain_superiors.size()>0)
    (*_chain_superiors.begin())->unlink_chain_inferior(*this);
  unlink_all_chain_inferiors();
  vtol_topology_object_2d::unlink();
}

//***************************************************************************
// WARNING: the 2 following methods are directly called only by the superior
// class. It is FORBIDDEN to use them directly
// If you want to link and unlink superior use sup.link_chain_inferior(*this)
// of sup.unlink_chain_inferior(*this)
//***************************************************************************

//---------------------------------------------------------------------------
// Name: link_chain_superior
// Task: Link `this' with a chain superior `chain_superior'
// Require: valid_chain_type(chain_superior)
//          and !is_chain_superior(chain_superior)
//---------------------------------------------------------------------------
void vtol_chain_2d::link_chain_superior(vtol_chain_2d &chain_superior)
{
  // require
  assert(valid_chain_type(chain_superior));
  assert(!is_chain_superior(chain_superior));

  vcl_list<vtol_chain_2d_ref>::iterator i;

  _chain_superiors.push_back(&chain_superior);
  i=_chain_superiors.end();
  i--;
  (*i).unprotect();
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with its chain superior `chain_superior'
//: REQUIRE: valid_chain_type(chain_superior)
//:          and is_chain_superior(chain_superior)
//---------------------------------------------------------------------------
void vtol_chain_2d::unlink_chain_superior(vtol_chain_2d &chain_superior)
{
  //require
  assert(valid_chain_type(chain_superior));
  assert(is_superior(chain_superior));

  vcl_list<vtol_chain_2d_ref>::iterator i;

  for(i=_chain_superiors.begin();
      (i!=_chain_superiors.end())&&((*i).ptr()!=&chain_superior);
      i++)
    ;
  
  // check
  assert((*i).ptr()==&chain_superior);

  _chain_superiors.erase(i); // unlink
  touch();
}

//---------------------------------------------------------------------------
// Name: is_cycle
// Task: Is `this' a connected chain ?
//---------------------------------------------------------------------------
bool vtol_chain_2d::is_cycle(void) const
{
  return _is_cycle;
}

//---------------------------------------------------------------------------
// Name: set_cycle
// Task: Set if `this' is a connected chain
//---------------------------------------------------------------------------
void vtol_chain_2d::set_cycle(bool new_is_cycle)
{
  _is_cycle=new_is_cycle;
}

//---------------------------------------------------------------------------
// Name: directions
// Task: Return the directions
//---------------------------------------------------------------------------
const vcl_vector<signed char> *vtol_chain_2d::directions(void) const
{
  return &_directions;
}

//---------------------------------------------------------------------------
// Name: directions
// Task: Return the directions
//---------------------------------------------------------------------------
vcl_vector<signed char> *vtol_chain_2d::directions(void)
{
  return &_directions;
}

//---------------------------------------------------------------------------
// Name: dir
// Task: Return the direction `i'
//---------------------------------------------------------------------------
int vtol_chain_2d::dir(int i) const
{
  return _directions[i];
}

//---------------------------------------------------------------------------
// Name: clear
// Task: Reset the chain
//---------------------------------------------------------------------------
void vtol_chain_2d::clear(void)
{
  _directions.clear();
  unlink_all_inferiors();
}

#include <vcl/vcl_rel_ops.h> // gcc 2.7
VCL_INSTANTIATE_INLINE(bool operator!=(vtol_chain_2d const &, vtol_chain_2d const &));
