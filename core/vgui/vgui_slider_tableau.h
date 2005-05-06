#ifndef vgui_slider_tableau_h_
#define vgui_slider_tableau_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2005

#include <vcl_list.h>

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_button.h>

#include "vgui_slider_tableau_sptr.h"

//: A slider implementation
//
// Implments a tableau that can be used as a slider. This can be used
// to provide platform-independent scrollbar-like capabilities. (It
// may be stretching the design parameters of vgui, but hey...)
//
// One use of it is in conjunction with vgui_poly_tableau to place the
// slider somewhere. For example:
// \code
//   vgui_tableau_sptr main_tab; // some tableau
//   vgui_slider_tableau_new slider_h( vgui_slider_tableau::horiz );
//   vgui_slider_tableau_new slider_v( vgui_slider_tableau::vert );
//   vgui_poly_tableau_new poly;
//   poly->add( easy_tab, 0.1, 0.1, 0.9, 0.9 );
//   poly->add( slider_h, 0.1, 0.0, 0.9, 0.1 );
//   poly->add( slider_v, 0.0, 0.1, 0.1, 0.9 );
// \endcode
//
class vgui_slider_tableau
  : public vgui_tableau
{
 public:
  //: Direction of slider
  enum slider_type { horiz, vert };

  //: Callback function type
  //
  // The parameter \a tab will be a pointer to the slider tableau
  // performing the callback. \a data contains data that was specified
  // when this callback was registered.
  typedef void (*callback)( vgui_slider_tableau* tab, void* data );


  // Internal structure used to store the callback pointer and
  // associated data. It needs to be public because it is used to
  // instantiate a vcl_list. It also needs to be complete at the time
  // of instantiation, so we can't simply forward declare it.
  struct callback_info {
    callback func_;
    void* data_;
    callback_info( callback f, void* d ) : func_(f), data_(d) { }
  };


  //: A handle used to refer to callback functions added to this slider
  typedef vcl_list< callback_info >::iterator cb_handle;

  //: Add a callback.
  //
  // The callback function \a cb will be called whenever the slider
  // value changes. The callback function will be provided the data in
  // \a data as the second parameter.
  cb_handle add_motion_callback( callback cb, void* data );

  //: Add a callback.
  //
  // The callback function \a cb will be called whenever user finished
  // picking a new slider value. The callback function will be
  // provided the data in \a data as the second parameter.
  cb_handle add_final_callback( callback cb, void* data );

  //: Remove a callback
  //
  // \a cbh is the handle returned by add_motion_callback() when the callback
  // was added to this slider.
  void remove_motion_callback( cb_handle cbh );

  //: Remove a callback
  //
  // \a cbh is the handle returned by add_final_callback() when the callback
  // was added to this slider.
  void remove_final_callback( cb_handle cbh );

  //: Current value of the slider, in [0,1]
  float value() const { return loc_; }

  //: Set the slider to value \a v.
  //
  // \a v will be clipped to [0,1]. The callbacks associated with the
  // slider will be called, as if the slider was changed
  // interactively.
  void set_value( float v );

  //: Set the slider to value \a v.
  //
  // \a v will be clipped to [0,1]. The callbacks will not be called.
  void set_value_no_callbacks( float v );

  ~vgui_slider_tableau();

  virtual bool handle(const vgui_event&);

 private:
  //: Draw the slider at the current position
  void draw_bar() const;

  //: Call each of the callbacks in \a cbs
  void call_callbacks( vcl_list< callback_info > const& cbs );

  //: Update the slider location
  // \a newx and \a newy give the latest mouse position in window coordinates
  void update_location( int newx, int newy );

  friend struct vgui_slider_tableau_new;

  //: Constructor - don't use this, use vgui_slider_tableau_new
  vgui_slider_tableau( slider_type type );

  //: Current location of slider, in [0:1]
  float loc_;

  //: Slider is horizontal or vertical?
  bool horiz_;

  //: Mouse is currently pressed?
  bool down_;

  //: Location of bar when mouse was pressed
  float last_loc_;

  //: Window coords of last mouse press
  int last_x_, last_y_;

  //: Callbacks called on every change of the slider
  vcl_list< callback_info > motion_callbacks_;

  //: Callbacks callled only at the final position of the slider
  vcl_list< callback_info > final_callbacks_;
};

//: Create a smart-pointer to a vgui_displaybase_tableau tableau.
struct vgui_slider_tableau_new : public vgui_slider_tableau_sptr
{
  typedef vgui_slider_tableau_sptr base;
  vgui_slider_tableau_new( vgui_slider_tableau::slider_type type )
    : base( new vgui_slider_tableau( type ) ) {}
};


#endif // vgui_slider_tableau_h_
