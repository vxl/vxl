//this-sets-emacs-to-*-c++-*-mode
#ifndef bxml_generic_ptr_h_
#define bxml_generic_ptr_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A generic pointer mechanism so that all class hierarchies of interest
//        can be represented without requiring a common base class.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 07, 2002    Initial version.
//   Based on the original TargetJr design by R. Hoffman and J. Liu
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_ostream.h>

//forward declarations
class vcl_vector;
class vsol_spatial_object_2d;

class bxml_generic_ptr
{
 public:
  ~bxml_generic_ptr(){};
  //:type enumeration
  enum ptr_type{NO_TYPE=0, VECT, VSOL_SO}; 
  ptr_type type() const{return _type;}

  //:Constructing generic pointers from specific class hierarchies
  bxml_generic_ptr():_type(NO_TYPE){_ptrs._anyobject = 0;}
  bxml_generic_ptr(vcl_vector* v):_type(VECT){_ptrs._vec_ptr =v;}
  bxml_generic_ptr(vsol_spatial_object_2d* so):_type(VSOL_SO){_ptrs._vso_ptr =so;}

  //:Accessing specific classes from a generic pointer
  inline void* get_anyobject() const {return _ptrs._anyobject;}
  inline vcl_vector* get_vector() const; 
  inline vsol_spatial_object_2d* get_vsol_spatial_object() const; 

  //:stream print
  inline friend vcl_ostream& operator<<(vcl_ostream&, const bxml_generic_ptr&);
 private:
  union type_union
  {
    void*  _anyobject;
    vcl_vector* _vec_ptr;
    vsol_spatial_object_2d* _vso_ptr;
  };
  //:utility functions

  //:members
  ptr_type _type;
  type_union _ptrs;
};

//: Ostream inline function implementation
  inline vcl_ostream& operator<<(vcl_ostream& os, const bxml_generic_ptr& ptr) 
  {
  os << " bxml_generic_ptr type  " << int(ptr.type()) << vcl_endl;
  return os;
  }
//: access for vcl_vector
inline vcl_vector* bxml_generic_ptr::get_vector() const 
{
  if(_type==VECT)
    return _ptrs._vec_ptr;
  else
    return 0;
}

//: access for vsol_spatial_object
inline vsol_spatial_object_2d* bxml_generic_ptr::get_vsol_spatial_object() const 
{
  if(_type==VSOL_SO)
    return _ptrs._vso_ptr;
  else
    return 0;
}
#endif // bxml_generic_ptr_h_
