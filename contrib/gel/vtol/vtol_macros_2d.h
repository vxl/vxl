// .SECTION Modifications
// 05/13/98  RIH replaced append by insert_after to avoid n^2 behavior

#ifndef MACROS_2d_H
#define MACROS_2d_H
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>

#define SEL_SUP_2d(suptype,target)\
  vcl_vector<suptype *> *new_list=new vcl_vector<suptype *>();\
  vcl_vector<suptype *> *sublist;\
  vcl_list<vtol_topology_object_2d_ref>::iterator i;\
  vcl_vector<suptype*>::iterator m_i;\
  for(i=_superiors.begin();i!=_superiors.end();i++)\
      {\
       sublist=(*i)->target();\
        if(sublist->size())\
        { \
          for(m_i=sublist->begin();m_i!=sublist->end();m_i++){\
               new_list->push_back(*m_i);\
          }\
        }\
        delete sublist;\
      }\
       tagged_union((vcl_vector<vsol_spatial_object_2d *> *)new_list);\
      return new_list

#define SEL_INF_2d(inftype,target)\
  vcl_vector<inftype *> *new_list=new vcl_vector<inftype *>();\
  vcl_vector<inftype *> *sublist;\
  for(topology_list_2d::iterator i=_inferiors.begin();i!=_inferiors.end();i++)\
     {\
     sublist=(*i)->target();\
     if(sublist->size())\
       {\
        vcl_vector<inftype *>::iterator m_i;\
          for(m_i=sublist->begin();m_i!=sublist->end();m_i++){\
            new_list->push_back(*m_i);\
          }\
      }\
     delete sublist;\
    }\
    tagged_union((vcl_vector<vsol_spatial_object_2d *> *)new_list);\
    return new_list

#define SUBCHAIN_INF_2d(listnm, suptype, inftype, target) \
    vcl_vector<inftype*> *templist; \
    chain_list_2d::iterator hi; \
    for (hi=_chain_inferiors.begin(); hi != _chain_inferiors.end(); hi++ ) \
       { \
         templist = ((suptype *) ((*hi).ptr()))->target(); \
          vcl_vector<inftype*>::iterator m_i; \
          for(m_i=templist->begin();m_i!=templist->end();m_i++){ \
            listnm->push_back(*m_i); \
          } \
         delete templist;\
       } \
    tagged_union((vcl_vector<vsol_spatial_object_2d*> *)listnm); \
    return listnm


#define OUTSIDE_BOUNDARY_2d(targettype, inftype, target) \
   vcl_vector<targettype*> *newlist = new vcl_vector<targettype*>(); \
   vcl_vector<targettype*> *templist;  \
      for(topology_list_2d::iterator i = _inferiors.begin(); i != _inferiors.end(); i++) \
         { \
        templist = ((inftype*)((*i).ptr()))->outside_boundary_##target(); \
        vcl_vector<targettype*>::iterator m_i; \
        for(m_i=templist->begin();m_i!=templist->end();m_i++){ \
            newlist->push_back(*m_i); \
          } \
        delete templist;  \
      } \
      return newlist


#define LIST_SELF_2d(selftype) \
     selftype* v = (selftype*)this;\
   vcl_vector<selftype*> * new_list = new vcl_vector<selftype*>(); \
   new_list->push_back(v); \
   return new_list


#define COPY_SUP_2d(suptype)  \
   vcl_vector<suptype*> *new_list = new vcl_vector<suptype*>(); \
   for(topology_list_2d::iterator i = _superiors.begin(); i != _superiors.end(); i++) \
   {\
   new_list->push_back((suptype*)(*i)); \
   }\
   return new_list

#define COPY_INF_2d(inftype) \
   vcl_vector<inftype*> *new_list = new vcl_vector<inftype*>(); \
      for(topology_list_2d::iterator i = _inferiors.begin(); i != _inferiors.end(); i++) \
     {\
   new_list->push_back((inftype*)((*i).ptr())); \
   }\
   return new_list

#endif // MACROS_2d_H
