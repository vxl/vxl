//*****************************************************************************
// File name: vidl_codec_ref.h
// Description: A smart pointer on a vidl_codec
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/26| Julien ESTEVE            |Creation
//*****************************************************************************
#ifndef vidl_codec_ref_h
#define vidl_codec_ref_h

//
// typedef for class vbl_smart_ptr<vidl_codec>
// Include this file to use the smart pointer vidl_codec_ref
//

class vidl_codec;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vidl_codec> vidl_codec_ref;

#endif // ifndef vidl_codec_ref_h

