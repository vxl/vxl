// This is mul/vil3d/vil3d_copy.h
#ifndef vil3d_copy_h_
#define vil3d_copy_h_
//:
// \file
// \brief Various image copying functions.
// \author Ian Scott - Manchester
// \date 2 Feb 2003

#include <vil3d/vil3d_fwd.h>


//: Copy src to dest, without changing dest's view parameters.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and types.
//  O(size).
// \relatesalso vil3d_image_view
template<class T>
void vil3d_copy_reformat(const vil3d_image_view<T> &src, vil3d_image_view<T> &dest);

//: Copy src to window in dest.
// Size of window is defined by src.
//  O(src.size).
// \relatesalso vil3d_image_view
template<class T>
void vil3d_copy_to_window(const vil3d_image_view<T> &src, vil3d_image_view<T> &dest,
                         unsigned i0, unsigned j0, unsigned k0);

//: Deep copy src to dest.
//  O(size).
// \relatesalso vil3d_image_view
template<class T>
void vil3d_copy_deep(const vil3d_image_view<T> &src, vil3d_image_view<T> &dest);

//: Create a deep copy of an image, with completely new underlying memory.
//  O(size).
// \relatesalso vil3d_image_view
template<class T>
vil3d_image_view<T> vil3d_copy_deep(const vil3d_image_view<T> &src);


//: Copy src to dest.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and pixel-types. Returns false if the copy
// failed.
//  O(size).
// \relatesalso vil3d_image_resource
bool vil3d_copy_deep(const vil3d_image_resource_sptr &src, vil3d_image_resource_sptr &dest);


#endif // vil3d_copy_h_
