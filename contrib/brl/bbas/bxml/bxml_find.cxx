// This is brl/bbas/bxml/bxml_find.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   November 30, 2006

#include "bxml_find.h"


//: Return true if elm has the same name and contains the same attributes as query
bool bxml_matches(const bxml_element& elm, const bxml_element& query)
{
  if (elm.name() != query.name())
    return false;

  if (elm.num_attributes() < query.num_attributes())
    return false;

  for (bxml_element::const_attr_iterator a = query.attr_begin();
       a != query.attr_end();  ++a)
  {
    if (elm.attribute(a->first) != a->second)
      return false;
  }
  return true;
}


//: Find the first element that matches
bxml_data_sptr bxml_find(const bxml_data_sptr& head,
                             const bxml_element& query)
{
  if (head->type() != bxml_data::ELEMENT)
    return NULL;
  bxml_element* h_elm = static_cast<bxml_element*>(head.ptr());
  if ( bxml_matches(*h_elm, query) )
    return head;
  else {
    // recursively check nested elements
    for (bxml_element::const_data_iterator i = h_elm->data_begin();
         i != h_elm->data_end();  ++i)
    {
      bxml_data_sptr result = bxml_find(*i, query);
      if (result)
        return result;
    }
  }
  return NULL;
}

//: Return true if elm has the same name and contains the same attributes as query
bool bxml_matches_by_name(const bxml_element& elm, const bxml_element& query)
{
  if (elm.name() != query.name())
    return false;

  return true;
}


//: Find the first element that matches
bxml_data_sptr bxml_find_by_name(const bxml_data_sptr& head,
                             const bxml_element& query)
{
  if (head->type() != bxml_data::ELEMENT)
    return NULL;
  bxml_element* h_elm = static_cast<bxml_element*>(head.ptr());
  if ( bxml_matches_by_name(*h_elm, query) )
    return head;
  else{
    // recursively check nested elements
    for (bxml_element::const_data_iterator i = h_elm->data_begin();
         i != h_elm->data_end();  ++i)
    {
      bxml_data_sptr result = bxml_find_by_name(*i, query);
      if (result)
        return result;
    }
  }
  return NULL;
}

