// This is core/vgui/vgui_cache_wizard.h
#ifndef vgui_cache_wizard_h_
#define vgui_cache_wizard_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Marko Bacic
// \date   17 Aug 2000
// \brief Provides support for caching of large images
//
//  Contains classes: vgui_cache_wizard
//
// \verbatim
//  Modifications
//   17-AUG-2000 Initial version. Marko Bacic,Oxford RRG
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_utility.h>

#include <vil1/vil1_image.h>
#include <vgui/vgui_gl.h>

//: Provides support for caching of large images.
class vgui_cache_wizard
{
 public:
  //: Each image consists of MxN quadrants. M,N is to be determined on runtime
  typedef vcl_vector <GLuint> image_cache_quadrants;
  typedef vcl_pair<vil1_image,image_cache_quadrants *> wizard_image;
  typedef vcl_pair<int,int> dimension;

  //: Loads an image into the memory
  int load_image(vil1_image);

  //: Get the texture names for the current viewport
  bool get_section(int id, int x, int y,int width,int height,
                   image_cache_quadrants *quadrants,dimension *pos,dimension *size);

  //:
  void TexImage2D_Brownie(vil1_image img);

  //: Get the image quadrant width
  int get_quadrant_width() const { return quadrant_width_; }

  //: Get the image quadrant height
  int get_quadrant_height() const { return quadrant_height_; }

  //: Get the class instance
  static vgui_cache_wizard *Instance();

  //: Constructor - set the image quadrant size here.
  vgui_cache_wizard(int quadrant_width, //= DEFAULT_QUADRANT_WIDTH,
                    int quadrant_height); //= DEFAULT_QUADRANT_HEIGHT);

  //: Destructor.
  ~vgui_cache_wizard();

 private:
  //: Hold a vector of images that have been loaded
  vcl_vector <wizard_image *> images_;

  //: Dimensions of images in quadrants units
  vcl_vector <dimension *>  dimensions_;

  //:
  int quadrant_width_;

  int quadrant_height_;

  //:
  // Maximum number of textures(limited by memory size - driver dependent,
  // as some drivers will do their one caching so that the number of textures
  // is only limited by the size of virtual memory
  unsigned int max_texture_num_;

  GLuint *texture_names_;

  //: Cache queue
  vcl_list <GLuint> cache_queue_;

  //: Only one instance of a class is allowed
  static vgui_cache_wizard *instance_;
};

#endif // vgui_cache_wizard_h_
