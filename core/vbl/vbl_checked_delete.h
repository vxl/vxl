#ifndef vbl_checked_delete_h_
#define vbl_checked_delete_h_

//:
// \file
// \author Amitha Perera
// Checked delete function lifted from BOOST.

//
//  boost/checked_delete.hpp
//
//  Copyright (c) 1999, 2000, 2001, 2002 boost.org
//  Copyright (c) 2002, 2003 Peter Dimov
//  Copyright (c) 2003 Daniel Frey
//  Copyright (c) 2003 Howard Hinnant
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org/libs/utility/checked_delete.html for documentation.
//
//  Modified from the original boost sources to fit the VXL restrictions.

//: Checks that T is complete and deletes x.
// Not to be used on arrays!
template<class T>
inline
void vbl_checked_delete(T * x)
{
    // intentionally complex - simplification causes regressions
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete x;
}

//: Checks that T is complete and array deletes x.
template<class T>
inline
void vbl_checked_delete_array(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}

#endif  // vbl_checked_delete_h_
