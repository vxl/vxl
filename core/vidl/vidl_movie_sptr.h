//*****************************************************************************
// File name: vidl_movie_sptr.h
// Description: A smart pointer on a vidl_movie
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/26| Julien ESTEVE            |Creation
//*****************************************************************************
#ifndef vidl_movie_sptr_h
#define vidl_movie_sptr_h

//
// typedef for class vbl_smart_ptr<vidl_movie>
// Include this file to use the smart pointer vidl_movie_sptr
//

class vidl_movie;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vidl_movie> vidl_movie_sptr;

#endif // ifndef vidl_movie_sptr_h

