#ifndef xcv_processing_h_
#define xcv_processing_h_
 
//--------------------------------------------------------------------------------
// .NAME	xcv_processing - xcv menu and interface for image processing
// .INCLUDE	xcv/xcv_processing.h
// .FILE	xcv_processing.cxx
// .SECTION Description
//    Creates a menu for, and provides an interface to image processing operations
//    contained in vxl.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications
//   K.Y.McGaul     26-SEP-2000    Initial version.
//--------------------------------------------------------------------------------

class vgui_menu;
class vsl_harris_params;
class vsl_canny_ox_params;
class vgui_easy2D;
class vsl_edge;

class xcv_processing
{
public:
  static void xcv_processing_dup();
  static vgui_menu create_processing_menu();
};

#endif // xcv_processing_
