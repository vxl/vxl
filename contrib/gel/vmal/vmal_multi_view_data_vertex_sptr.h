//*****************************************************************************
// File name: vmal_multi_view_data_vertex_sptr.h
// Description: A smart pointer on a vmal_multi_view_data<vtol_vertex_2d_sptr>
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |04/23/2001| Laurent Guichard         |Creation
//*****************************************************************************
#ifndef vmal_multi_view_data_vertex_sptr_h_
#define vmal_multi_view_data_vertex_sptr_h_

#include <vtol/vtol_vertex_2d_sptr.h>

#include <vmal/vmal_multi_view_data.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

//typedef vbl_smart_ptr<vmal_multi_view_data<>> vmal_multi_view_data_sptr;

typedef vmal_multi_view_data<vtol_vertex_2d_sptr> vmal_mvd_vertex;

typedef vbl_smart_ptr<vmal_mvd_vertex> vmal_multi_view_data_vertex_sptr;
#endif // #ifndef vmal_multi_view_data_vertex_sptr_h_
