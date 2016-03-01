/*
  fsm
*/
#include "../vbl_smart_ptr_example.h"
#include <vbl/vbl_smart_ptr.hxx>
#include <vcl_list.hxx>
#include <vcl_algorithm.hxx>

VBL_SMART_PTR_INSTANTIATE(example_sp);
VBL_SMART_PTR_INSTANTIATE(bigmatrix_impl);

VCL_OPERATOR_NE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_CONTAINABLE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_LIST_INSTANTIATE(vbl_smart_ptr<example_sp>);

