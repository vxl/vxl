//*****************************************************************************
// File name: vidl_clip_sptr.h
// Description: A smart pointer on a vidl_clip
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/26| Julien ESTEVE            |Creation
//*****************************************************************************
#ifndef vidl_clip_sptr_h
#define vidl_clip_sptr_h

//
// typedef for class vbl_smart_ptr<vidl_clip>
// Include this file to use the smart pointer vidl_clip_sptr
//

class vidl_clip;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vidl_clip> vidl_clip_sptr;

#endif // ifndef vidl_clip_sptr_h

