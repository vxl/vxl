//:
//  \file

#include "vtol_list_functions.h"
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>

//: Note if you really want a tagged union...this is it...
// otherwise you can call remove_duplicates and get the
// same result....pav

void tagged_union(vcl_vector<vsol_spatial_object_2d*>* topolist)
{
  vcl_vector<vsol_spatial_object_2d*> temp;

  // Clearing the tags before processing...
  vcl_vector<vsol_spatial_object_2d*>::iterator i;

  for (i=topolist->begin();i!=topolist->end(); ++i)
    ((vsol_spatial_object_2d*)(*i))->unset_tagged_union_flag();

  // Performing the Union by looping over
  // the elements and removing duplicates.

  for (i=topolist->begin(); i!=topolist->end(); ++i)
    if (!((vsol_spatial_object_2d*)(*i))->get_tagged_union_flag())
      {
        ((vsol_spatial_object_2d*)(*i))->set_tagged_union_flag();
        temp.push_back(*i);
      }
  (*topolist)=temp;
}


void tagged_union(vcl_list<vtol_topology_object_sptr> *topolist)
{
  vcl_list<vtol_topology_object_sptr> temp;

  // Clearing the tags before processing...
  vcl_list<vtol_topology_object_sptr>::iterator i;

  for (i=topolist->begin();i!=topolist->end(); ++i)
    ((*i))->unset_tagged_union_flag();

  // Performing the Union by looping over
  // the elements and removing duplicates.

  for (i=topolist->begin(); i!=topolist->end(); ++i)
    if (!((*i))->get_tagged_union_flag())
      {
        ((*i))->set_tagged_union_flag();
        temp.push_back(*i);
      }
  (*topolist)=temp;
}
