#ifndef vtol_list_functions_txx_
#define vtol_list_functions_txx_
//:
// \file
#include "vtol_list_functions.h"

//: Note if you really want a tagged union...this is it...
// otherwise you can call remove_duplicates and get the
// same result....pav
// Also note that T must be a pointer or smart pointer to a derived class of
// vsol_spatial_object_[23]d as {s,g}et_tagged_union_flag() must exist.

template <class T>
vcl_vector<T>* tagged_union(vcl_vector<T>* topolist)
{
  if (!topolist) return 0; // null pointer invalid
  vcl_vector<T> temp;
  typename vcl_vector<T>::iterator i;

  // Clearing the tags before processing...
  for (i=topolist->begin();i!=topolist->end(); ++i)
    (*i)->unset_tagged_union_flag();

  // Performing the Union by looping over
  // the elements and removing duplicates.

  for (i=topolist->begin(); i!=topolist->end(); ++i)
    if (! (*i)->get_tagged_union_flag())
    {
      (*i)->set_tagged_union_flag();
      temp.push_back(*i);
    }
  // now overwrite the original list with the new one:
  (*topolist)=temp;
  return topolist;
}

template <class T>
vcl_list<T>* tagged_union(vcl_list<T>* topolist)
{
  if (!topolist) return 0; // null pointer invalid
  vcl_list<T> temp;
  typename vcl_list<T>::iterator i;

  // Clearing the tags before processing...
  for (i=topolist->begin();i!=topolist->end(); ++i)
    (*i)->unset_tagged_union_flag();

  // Performing the Union by looping over
  // the elements and removing duplicates.

  for (i=topolist->begin(); i!=topolist->end(); ++i)
    if (! (*i)->get_tagged_union_flag())
    {
      (*i)->set_tagged_union_flag();
      temp.push_back(*i);
    }
  // now overwrite the original list with the new one:
  (*topolist)=temp;
  return topolist;
}

#endif // vtol_list_functions_txx_
