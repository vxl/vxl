//*****************************************************************************
// File name: gkll_multi_view_data_vertex_sptr.h
// Description: A smart pointer on a gkll_multi_view_data<vtol_vertex_2d_sptr>
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |04/23/2001| Laurent Guichard         |Creation
//*****************************************************************************
#ifndef gkll_multi_view_data_vertex_sptr_h_
#define gkll_multi_view_data_vertex_sptr_h_

#include <vtol/vtol_vertex_2d_sptr.h>

#include <gkll/gkll_multi_view_data.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

//typedef vbl_smart_ptr<gkll_multi_view_data<>> gkll_multi_view_data_sptr;


typedef gkll_multi_view_data<vtol_vertex_2d_sptr> gkll_mvd_vertex;

typedef vbl_smart_ptr<gkll_mvd_vertex> gkll_multi_view_data_vertex_sptr;
#endif // #ifndef gkll_multi_view_data_vertex_sptr_h_
