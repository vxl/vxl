// This is vxl/vbl/vbl_br_impl.txx

#include <vbl/vbl_br_impl.h>
#include <vbl/vbl_br_default.h>
#include <vcl_iostream.h>

//----------------------------------------------------------------------
//: Return the current factory for this class.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_factory<T1,T2,T3,T4,T5>* vbl_br_impl<T1,T2,T3,T4,T5>::GetFactory()
{
  return factory();
}

//----------------------------------------------------------------------
//: Set the current factory for this class.
template <class T1, class T2, class T3, class T4, class T5>
void vbl_br_impl<T1,T2,T3,T4,T5>::SetFactory(vbl_basic_relation_factory<T1,T2,T3,T4,T5>* f)
{
  factory() = f;
}

//------------------------------------------------------------
//: Use a static method with local static to get around the lack of
// static members for templates under gcc.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_factory<T1,T2,T3,T4,T5>*& vbl_br_impl<T1,T2,T3,T4,T5>::factory()
{
  static vbl_basic_relation_factory<T1,T2,T3,T4,T5>* factory 
    = new vbl_br_default_factory<T1,T2,T3,T4,T5>;
  return factory;
}
