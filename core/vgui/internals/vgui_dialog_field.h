#ifndef vgui_dialog_field_h_
#define vgui_dialog_field_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_dialog_field - represents a field in a dialog
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/internals/vgui_dialog_field.h
// .FILE internals/vgui_dialog_field.cxx
//
// .SECTION Description
//
// vgui_dialog_field represents a field in a vgui_dialog. It is the base class for 
// all dialog fields so far. It has abstract functions for synchronising its 
// stored value with a vcl_string  
//
// .SECTION Author
//              Philip C. Pritchett, 23 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>

class vgui_dialog_field 
{
public:
  vcl_string label;
  // Constructors/Destructors--------------------------------------------------
  
  vgui_dialog_field(const char*);
  virtual ~vgui_dialog_field();
  
  virtual vcl_string current_value() const = 0; 
  virtual bool update_value(const vcl_string& s) = 0; 
};

#endif // vgui_dialog_field_h_
