// This is brl/bbas/bxml/bxml_find.cxx
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

  for (auto a = query.attr_begin();
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
    return nullptr;
  auto* h_elm = static_cast<bxml_element*>(head.ptr());
  if ( bxml_matches(*h_elm, query) )
    return head;
  else {
    // recursively check nested elements
    for (auto i = h_elm->data_begin();
         i != h_elm->data_end();  ++i)
    {
      bxml_data_sptr result = bxml_find(*i, query);
      if (result)
        return result;
    }
  }
  return nullptr;
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
    return nullptr;
  auto* h_elm = static_cast<bxml_element*>(head.ptr());
  if ( bxml_matches_by_name(*h_elm, query) )
    return head;
  else{
    // recursively check nested elements
    for (auto i = h_elm->data_begin();
         i != h_elm->data_end();  ++i)
    {
      bxml_data_sptr result = bxml_find_by_name(*i, query);
      if (result)
        return result;
    }
  }
  return nullptr;
}

//: Find all elements that match
std::vector<bxml_data_sptr> bxml_find_all_with_name(const bxml_data_sptr& head,
                                                   const bxml_element& query)
{
  std::vector<bxml_data_sptr> all_results;

  if (head->type() != bxml_data::ELEMENT)
    return all_results;
  auto* h_elm = static_cast<bxml_element*>(head.ptr());
  if ( bxml_matches_by_name(*h_elm, query) )
    return all_results;
  else{
    // recursively check nested elements
    for (auto i = h_elm->data_begin();
         i != h_elm->data_end();  ++i)
    {
      bxml_data_sptr result = bxml_find_by_name(*i, query);
      if (result)
        all_results.push_back(result);
    }
  }
  return all_results;
}
