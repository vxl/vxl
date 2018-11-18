// This is mul/vimt/vimt_save.h
#ifndef vimt_save_h_
#define vimt_save_h_
//:
// \file
// \author Ian Scott, Kevin de Souza
// \note Adapted from vimt3d_save


#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_fwd.h>
class vimt_transform_2d;
class vimt_image_2d;

//: Save values from a transform to an image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
void vimt_save_transform(vil_image_resource_sptr &ir,
                         const vimt_transform_2d& trans,
                         bool use_millimetres =false);

//: Save image from path into given image (forcing to given pixel type)
bool vimt_save(const std::string& path,
               const vimt_image_2d& image,
               bool use_millimetres =false);

#endif // vimt_save_h_
