// This is gel/vtol/vtol_macros.h
#ifndef VTOL_MACROS_H
#define VTOL_MACROS_H
//:
// \file
// \verbatim
//  Modifications
//   05/13/98  RIH replaced append by insert_after to avoid n^2 behavior
// \endverbatim

#include <vcl_vector.h>
#include <vcl_list.h>

#define SEL_SUP(suptype,target)\
  vcl_vector<suptype *> *new_list=new vcl_vector<suptype *>();\
  vcl_vector<suptype *> *sublist;\
  vcl_list<vtol_topology_object*>::const_iterator i;\
  for (i=superiors_.begin(); i!=superiors_.end(); ++i)\
  {\
    sublist=(*i)->target();\
    vcl_vector<suptype*>::iterator m_i;\
    for (m_i=sublist->begin(); m_i!=sublist->end(); ++m_i)\
      new_list->push_back(*m_i);\
    delete sublist;\
  }\
  return tagged_union(new_list)

#define SEL_INF(inftype,target)\
  vcl_vector<inftype *> *new_list=new vcl_vector<inftype *>();\
  vcl_vector<inftype *> *sublist;\
  for (topology_list::iterator i=inferiors_.begin(); i!=inferiors_.end(); ++i)\
  {\
    sublist=(*i)->target();\
    vcl_vector<inftype *>::iterator m_i;\
    for (m_i=sublist->begin(); m_i!=sublist->end(); ++m_i)\
      new_list->push_back(*m_i);\
    delete sublist;\
  }\
  return tagged_union(new_list)

#define SUBCHAIN_INF(listnm, suptype, inftype, target)\
  vcl_vector<inftype*> *templist;\
  chain_list::iterator hi;\
  for (hi=chain_inferiors_.begin(); hi!=chain_inferiors_.end(); ++hi)\
  {\
    templist = (*hi)->cast_to_##suptype()->target();\
    vcl_vector<inftype*>::iterator m_i;\
    for (m_i=templist->begin(); m_i!=templist->end(); ++m_i)\
      listnm->push_back(*m_i);\
    delete templist;\
  }\
  return tagged_union(listnm)

#define OUTSIDE_BOUNDARY(targettype, inftype, target)\
  vcl_vector<targettype*> *newlist = new vcl_vector<targettype*>();\
  vcl_vector<targettype*> *templist;\
  for (topology_list::iterator i=inferiors_.begin(); i!=inferiors_.end(); ++i)\
  {\
    templist = (*i)->cast_to_##inftype()->outside_boundary_##target();\
    vcl_vector<targettype*>::iterator m_i;\
    for (m_i=templist->begin(); m_i!=templist->end(); ++m_i)\
      newlist->push_back(*m_i);\
    delete templist;\
  }\
  return newlist

#define LIST_SELF(selftype)\
  vcl_vector<selftype*> * new_list = new vcl_vector<selftype*>();\
  new_list->push_back(this);\
  return new_list

#define COPY_SUP(suptype)\
  vcl_vector<suptype*> *new_list = new vcl_vector<suptype*>();\
  vcl_list<vtol_topology_object*>::const_iterator i;\
  for (i=superiors_.begin(); i!=superiors_.end(); ++i)\
    new_list->push_back(static_cast<suptype*>(*i));\
  return new_list

#define COPY_INF(inftype)\
  vcl_vector<vtol_##inftype*> *new_list = new vcl_vector<vtol_##inftype*>();\
  for (topology_list::iterator i=inferiors_.begin(); i!=inferiors_.end(); ++i)\
    new_list->push_back((*i)->cast_to_##inftype());\
  return new_list

#endif // VTOL_MACROS_H
