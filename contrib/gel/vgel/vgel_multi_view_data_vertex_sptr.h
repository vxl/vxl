//*****************************************************************************
// File name: vgel_multi_view_data_vertex_sptr.h
// Description: A smart pointer on a vgel_multi_view_data<vtol_vertex_2d_sptr>
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |04/23/2001| Laurent Guichard         |Creation
//*****************************************************************************
#ifndef vgel_multi_view_data_vertex_sptr_h_
#define vgel_multi_view_data_vertex_sptr_h_

#include <vtol/vtol_vertex_2d_sptr.h>

#include <vgel/vgel_multi_view_data.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

//typedef vbl_smart_ptr<vgel_multi_view_data<>> vgel_multi_view_data_sptr;


typedef vgel_multi_view_data<vtol_vertex_2d_sptr> vgel_mvd_vertex;

typedef vbl_smart_ptr<vgel_mvd_vertex> vgel_multi_view_data_vertex_sptr;
#endif // #ifndef vgel_multi_view_data_vertex_sptr_h_
