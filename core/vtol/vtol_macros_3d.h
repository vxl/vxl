// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

// 05/13/98  RIH replaced append by insert_after to avoid n^2 behavior

#ifndef MACROS_3d_H
#define MACROS_3d_H
#include<vcl/vcl_vector.h>
#include<vcl/vcl_algorithm.h>

#define SEL_SUP_3d(suptype,target) \
  vcl_vector<suptype*> *new_list = new vcl_vector<suptype*>(), *sublist; \
  for(topology_list_3d::iterator i = _superiors.begin(); i != _superiors.end(); i++) \
      {\
       sublist = (*i)->target();\
        if (sublist->size())\
	{ \
          vcl_vector<suptype*>::iterator m_i; \
          for(m_i=sublist->begin();m_i!=sublist->end();m_i++){ \
               new_list->push_back(*m_i); \
          } \
	}\
	delete sublist; \
      }\
       tagged_union((vcl_vector<vsol_spatial_object_3d*> *)new_list); \
      return new_list


#define SEL_INF_3d(inftype, target) \
  vcl_vector<inftype*> *new_list = new vcl_vector<inftype*>(), * sublist; \
  for(topology_list_3d::iterator i = _inferiors.begin(); i != _inferiors.end(); i++) \
     {                                    \
     sublist = (*i)->target();         \
     if (sublist->size()) \
       {\
        vcl_vector<inftype*>::iterator m_i; \
          for(m_i=sublist->begin(); m_i!=sublist->end();m_i++){ \
            new_list->push_back(*m_i); \
          } \
      }\
     delete sublist; \
    }\
    tagged_union((vcl_vector<vsol_spatial_object_3d*> *)new_list); \
    return new_list




#define SUBCHAIN_INF_3d(listnm, suptype, inftype, target) \
    vcl_vector<inftype*> *templist; \
    hierarchy_node_list_3d::iterator hi; \
    for (hi=_hierarchy_inferiors.begin(); hi != _hierarchy_inferiors.end(); hi++ ) \
       { \
	 templist = ((suptype *) (*hi))->target(); \
          vcl_vector<inftype*>::iterator m_i; \
          for(m_i=templist->begin();m_i!=templist->end();m_i++){ \
            listnm->push_back(*m_i); \
          } \
	 delete templist;\
       } \
    tagged_union((vcl_vector<vsol_spatial_object_3d*> *)listnm); \
    return listnm


 
#define OUTSIDE_BOUNDARY_3d(targettype, inftype, target) \
   vcl_vector<targettype*> *newlist = new vcl_vector<targettype*>(); \
   vcl_vector<targettype*> *templist;  \
      for(topology_list_3d::iterator i = _inferiors.begin(); i != _inferiors.end(); i++) \
         { \
	templist = ((inftype*)(*i))->outside_boundary_##target(); \
        vcl_vector<targettype*>::iterator m_i; \
        for(m_i=templist->begin();m_i!=templist->end();m_i++){ \
            newlist->push_back(*m_i); \
          } \
        delete templist;  \
      } \
      return newlist
 


#define LIST_SELF_3d(selftype) \
     selftype* v = (selftype*)this;\
   vcl_vector<selftype*> * new_list = new vcl_vector<selftype*>(); \
   new_list->push_back(v); \
   return new_list


#define COPY_SUP_3d(suptype)  \
   vcl_vector<suptype*> *new_list = new vcl_vector<suptype*>(); \
   for(topology_list_3d::iterator i = _superiors.begin(); i != _superiors.end(); i++) \
   {\
   new_list->push_back((suptype*)(*i)); \
   }\
   return new_list       

#define COPY_INF_3d(inftype) \
   vcl_vector<inftype*> *new_list = new vcl_vector<inftype*>(); \
      for(topology_list_3d::iterator i = _inferiors.begin(); i != _inferiors.end(); i++) \
     {\
   new_list->push_back((inftype*)(*i)); \
   }\
   return new_list       


#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS macros.







