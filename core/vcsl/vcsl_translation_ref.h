//*****************************************************************************
// File name: vcsl_translation_ref.h
// Description: Smart pointer on a vcsl_translation
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/04| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_TRANSLATION_REF_H
#define VCSL_TRANSLATION_REF_H

class vcsl_translation;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_translation> vcsl_translation_ref;

#endif // #ifndef VCSL_TRANSLATION_REF_H
