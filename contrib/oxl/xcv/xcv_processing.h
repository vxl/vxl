// This is oxl/xcv/xcv_processing.h
#ifndef xcv_processing_h_
#define xcv_processing_h_
//:
// \file
// \author  K.Y.McGaul
// \brief   Interface to image processing operations.
//
//  Creates a menu for, and provides an interface to image processing operations
//  contained in vxl.
//
// \verbatim
//  Modifications
//    K.Y.McGaul     26-SEP-2000    Initial version.
// \endverbatim

class vgui_menu;

class xcv_processing
{
 public:
  static void xcv_processing_dup();
  static vgui_menu create_processing_menu();
};

#endif // xcv_processing_h_
