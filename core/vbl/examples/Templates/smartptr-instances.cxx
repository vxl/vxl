/*
  fsm@robots.ox.ac.uk
*/
#include <vbl/examples/vbl_smart_ptr_example.h>
#include <vbl/vbl_smart_ptr.txx>
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_list.txx>
#include <vcl/vcl_algorithm.txx>

VBL_SMART_PTR_INSTANTIATE(example_sp);
VBL_SMART_PTR_INSTANTIATE(bigmatrix_impl);

VCL_OPERATOR_NE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_CONTAINABLE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_LIST_INSTANTIATE(vbl_smart_ptr<example_sp>);

