//*****************************************************************************
// File name: vidl_frame_sptr.h
// Description: A smart pointer on a vidl_frame
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/26| Julien ESTEVE            |Creation
//*****************************************************************************

#ifndef vidl_frame_sptr_h
#define vidl_frame_sptr_h

//
// typedef for class vbl_smart_ptr<vidl_frame>
// Include this file to use the smart pointer vidl_frame_sptr
//

class vidl_frame;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vidl_frame> vidl_frame_sptr;

#endif // ifndef vidl_frame_sptr_h

