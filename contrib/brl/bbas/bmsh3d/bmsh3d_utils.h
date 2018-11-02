#ifndef bmsh3d_utils_h_
#define bmsh3d_utils_h_
//:
// \file
// \author Ming-Ching Chang
// \date 2004-03-12
//  Ming: General 3D Utilities

#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "bmsh3d_fuzzy_boolean.h"

//: Setting of level of details of debug info.
//    0: off.
//    1: only critical info.
//    2: only important info.
//    3: some processing details.
//    4: more debugging details, somewhat verbose.
//    5: very verbose for debugging.
#define bmsh3d_DEBUG   1

  // ####################################################################
  //    THE GUI ELEMENT (THAT SUPPORT CLICK-N-GETINFO)
  // ####################################################################

//: visualization element: contains one void* pointer to the visualization object
class vispt_elm
{
 protected:
  // pointer to the visualization object
  const void* vis_pointer_;

  // ====== Constructor/Destructor ======

 public:
  vispt_elm()  { vis_pointer_ = nullptr; }
  virtual ~vispt_elm() = default;

  // ====== Data access functions ======

  void set_vis_pointer (const void* pointer) { vis_pointer_ = pointer; }
  const void* vis_pointer() const { return vis_pointer_; }

  //: Ming: don't make this function pure virtual. It eats up 4 bytes of mem for each derived object!
  virtual void getInfo (std::ostringstream& /*ostrm*/) {}
};

//##########################################################
//Color code to visualize several mesh objects.
typedef enum
{
  COLOR_NOT_SPECIFIED = 0,
  COLOR_BLACK         = 1,
  COLOR_RED           = 2,
  COLOR_GREEN         = 3,
  COLOR_BLUE          = 4,
  COLOR_YELLOW        = 5,
  COLOR_CYAN          = 6,
  COLOR_PINK          = 7,
  COLOR_GRAY          = 8,
  COLOR_DARKGRAY      = 9,
  COLOR_DARKRED       = 10,
  COLOR_DARKGREEN     = 11,
  COLOR_DARKBLUE      = 12,
  COLOR_DARKYELLOW    = 13,
  COLOR_DARKCYAN      = 14,
  COLOR_PURPLE        = 15,
  COLOR_WHITE         = 16,

  COLOR_LIGHTGRAY     = 17,
  COLOR_GRAYBLUE      = 18,

  COLOR_LIGHTGREEN    = 19,
  COLOR_ORANGE        = 20,

  COLOR_GOLD          = 41,
  COLOR_SILVER        = 42,
  COLOR_RANDOM        = 50,
} VIS_COLOR_CODE;

//##################################################################

#endif // bmsh3d_utils_h_
