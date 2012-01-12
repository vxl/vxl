//*****************************************************************************
//:
// \file
// \brief A smart pointer on a vgel_multi_view_data<vtol_vertex_2d_sptr>
//*****************************************************************************
#ifndef vgel_multi_view_data_vertex_sptr_h
#define vgel_multi_view_data_vertex_sptr_h

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vgel/vgel_multi_view_data.h>
#include <vbl/vbl_smart_ptr.h>
typedef vgel_multi_view_data<vtol_vertex_2d_sptr> vgel_mvd_vertex;
typedef vbl_smart_ptr<vgel_mvd_vertex> vgel_multi_view_data_vertex_sptr;

#endif // vgel_multi_view_data_vertex_sptr_h
