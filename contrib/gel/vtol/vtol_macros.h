// This is gel/vtol/vtol_macros.h
#ifndef VTOL_MACROS_H
#define VTOL_MACROS_H
//:
// \file
// \verbatim
//  Modifications
//   05/13/98  RIH replaced append by insert_after to avoid n^2 behavior
// \endverbatim

#include <vector>
#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define SEL_SUP(suptype,target)\
  std::vector<suptype *> *new_list=new std::vector<suptype *>();\
  std::vector<suptype *> *sublist;\
  std::list<vtol_topology_object*>::const_iterator i;\
  for (i=superiors_.begin(); i!=superiors_.end(); ++i)\
  {\
    sublist=(*i)->target();\
    std::vector<suptype*>::iterator m_i;\
    for (m_i=sublist->begin(); m_i!=sublist->end(); ++m_i)\
      new_list->push_back(*m_i);\
    delete sublist;\
  }\
  return tagged_union(new_list)

#define SEL_INF(inftype,target)\
  std::vector<inftype *> *new_list=new std::vector<inftype *>();\
  std::vector<inftype *> *sublist;\
  for (topology_list::iterator i=inferiors_.begin(); i!=inferiors_.end(); ++i)\
  {\
    sublist=(*i)->target();\
    std::vector<inftype *>::iterator m_i;\
    for (m_i=sublist->begin(); m_i!=sublist->end(); ++m_i)\
      new_list->push_back(*m_i);\
    delete sublist;\
  }\
  return tagged_union(new_list)

#define SUBCHAIN_INF(listnm, suptype, inftype, target)\
  std::vector<inftype*> *templist;\
  chain_list::iterator hi;\
  for (hi=chain_inferiors_.begin(); hi!=chain_inferiors_.end(); ++hi)\
  {\
    templist = (*hi)->cast_to_##suptype()->target();\
    std::vector<inftype*>::iterator m_i;\
    for (m_i=templist->begin(); m_i!=templist->end(); ++m_i)\
      listnm->push_back(*m_i);\
    delete templist;\
  }\
  return tagged_union(listnm)

#define OUTSIDE_BOUNDARY(targettype, inftype, target)\
  std::vector<targettype*> *newlist = new std::vector<targettype*>();\
  std::vector<targettype*> *templist;\
  for (topology_list::iterator i=inferiors_.begin(); i!=inferiors_.end(); ++i)\
  {\
    templist = (*i)->cast_to_##inftype()->outside_boundary_##target();\
    std::vector<targettype*>::iterator m_i;\
    for (m_i=templist->begin(); m_i!=templist->end(); ++m_i)\
      newlist->push_back(*m_i);\
    delete templist;\
  }\
  return newlist

#define LIST_SELF(selftype)\
  std::vector<selftype*> * new_list = new std::vector<selftype*>();\
  new_list->push_back(this);\
  return new_list

#define COPY_SUP(suptype)\
  std::vector<suptype*> *new_list = new std::vector<suptype*>();\
  std::list<vtol_topology_object*>::const_iterator i;\
  for (i=superiors_.begin(); i!=superiors_.end(); ++i)\
    new_list->push_back(static_cast<suptype*>(*i));\
  return new_list

#define COPY_INF(inftype)\
  std::vector<vtol_##inftype*> *new_list = new std::vector<vtol_##inftype*>();\
  for (topology_list::iterator i=inferiors_.begin(); i!=inferiors_.end(); ++i)\
    new_list->push_back((*i)->cast_to_##inftype());\
  return new_list

#endif // VTOL_MACROS_H
