// *****************************************************************************
// :
// \file
// \brief A smart pointer on a vmal_multi_view_data<vtol_edge_2d_sptr>
// *****************************************************************************
#ifndef vmal_multi_view_data_edge_sptr_h
#define vmal_multi_view_data_edge_sptr_h

#include <vtol/vtol_edge_2d_sptr.h>
#include <vmal/vmal_multi_view_data.h>
#include <vbl/vbl_smart_ptr.h>
typedef vmal_multi_view_data<vtol_edge_2d_sptr> vmal_mvd_edge;
typedef vbl_smart_ptr<vmal_mvd_edge>            vmal_multi_view_data_edge_sptr;

#endif // #ifndef vmal_multi_view_data_edge_sptr_h
