#ifndef vsol_volume_3d_sptr_h_
#define vsol_volume_3d_sptr_h_
//*****************************************************************************
// File name: vsol_volume_3d_sptr.h
// Description: Smart pointer on a vsol_volume_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| Peter Vanroose           |First version
//*****************************************************************************

class vsol_volume_3d;

#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<vsol_volume_3d> vsol_volume_3d_sptr;

#endif // vsol_volume_3d_sptr_h_
