//*****************************************************************************
//:
// \file
// \brief A smart pointer on a gkll_multi_view_data<vtol_vertex_2d_sptr>
//*****************************************************************************
#ifndef gkll_multi_view_data_vertex_sptr_h
#define gkll_multi_view_data_vertex_sptr_h

#include <vtol/vtol_vertex_2d_sptr.h>
#include <gkll/gkll_multi_view_data.h>
#include <vbl/vbl_smart_ptr.h>
typedef gkll_multi_view_data<vtol_vertex_2d_sptr> gkll_mvd_vertex;
typedef vbl_smart_ptr<gkll_mvd_vertex> gkll_multi_view_data_vertex_sptr;

#endif // #ifndef gkll_multi_view_data_vertex_sptr_h
