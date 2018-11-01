// This is brl/bbas/bxml/bxml_find.h
#ifndef bxml_find_h_
#define bxml_find_h_
//:
// \file
// \brief functions to search XML documents
// \author Matt Leotta (Brown)
// \date   November 30, 2006
//
// \verbatim
//  Modifications
//   Ozge C Ozcanli Nov 4, 2007 - added find_by_name() method
// \endverbatim
//

#include "bxml_document.h"

//: Return true if \a elm has the same name and contains the same attributes as query
bool bxml_matches(const bxml_element& elm, const bxml_element& query);

//: Find the first element that matches
bxml_data_sptr bxml_find(const bxml_data_sptr& head,
                             const bxml_element& query);

bool bxml_matches_by_name(const bxml_element& elm, const bxml_element& query);

//: Find the first element that matches the query only by name
//  (no check on the attribute matching)
bxml_data_sptr bxml_find_by_name(const bxml_data_sptr& head,
                                     const bxml_element& query);

//: Find all elements that match the query only by name
//  (no check on the attribute matching)
std::vector<bxml_data_sptr> bxml_find_all_with_name(const bxml_data_sptr& head,
                                                   const bxml_element& query);

#endif // bxml_find_h_
