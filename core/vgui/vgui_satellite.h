#ifndef vgui_satellite_h_
#define vgui_satellite_h_
// .NAME vgui_satellite
// .LIBRARY vgui
// .INCLUDE vgui/vgui_satellite.h
// .FILE    vgui_satellite.txx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//
// .SECTION Description
// Purpose: To turn a non-tableau into a multi-tableau.
//
// Explanation: Sometimes it is handy to have a tableau with two
// handle methods, or a handle method with an extra argument.
// For example, if a single object holds some state which is
// to be manipulated from two windows, it is convenient to have
// two tableaux which can be inserted into the vgui hierarchy
// separately, but to have a single handle method which accepts
// an argument that tells it which window the event came from.
//
// Of course, this can't be done, but the vgui_satellite_t<> class
// template can be used to create the illusion. It works as
// follows: create an object which has a member signature
//   bool method(vgui_event const &, T );
// where the name ('method') is irrelevant. To arrange for your
// object to receive events as if it were a tableau, simply
// create a vgui_satellite_t<object, T>, using the address of
// your object, the address of the member function which will
// handle the event and a piece of "client data" of type T.
// The vgui_satellite_t can now be inserted in any tableau context
// and will behave as your object would if it were a tableau
// whose handle method is the one you gave to the satellite. The
// client data will be passed back to you.
//
// For example, an application which needs to maintain some state
// which is used in, and responds to, two windows could use a pair
// of vgui_satellite<>s as follows:
//    struct my_object
//    {
//      vgui_satellite_t_new<my_object, int> window_0;
//      vgui_satellite_t_new<my_object, int> window_1;
//      ... add common state here ...
//
//      my_object()
//      : window_0(this, &my_object::handle, 0)
//      , window_1(this, &my_object::handle, 1) { }
//
//      bool handle(vgui_event const &e, int const &which_window) {
//        vcl_cerr << "event in window " << which_window << vcl_endl;
//        ... now use/manipulate common state ...
//        return true;
//      }
//
//      vcl_string type_name() const { return "my_object"; }
//    };
// All the state of this two-window application can now be held
// in a single object, while each window's tableau interface can
// be used independently. The satellite tableaux need not be
// members of the object that holds the state, but it is convenient
// if they are, e.g. for posting redraws etc.
//
// Another use of vgui_satellite_t<> is to avoid derivation from
// vgui_tableau (by having a single vgui_satellite_t<> inside the
// application object).
//
// The 'data' parameter may seem superfluous because one could
// achieve the same result by having two methods which were used
// to initialize the satellites. However, if the number of satellites
// is unknown at compile time, or if there are many of them and it
// is easier to generate them in code, then the 'data' parameter is
// necessary.
//
// The 'object' type must have a method type_name(); it is used to
// implement the type_name() methods on the satellites.

#include <vgui/vgui_menu.h>
#include <vgui/vgui_tableau.h>

// satellite with client data.
template <class object, class data>
struct vgui_satellite_t : vgui_tableau
{
  typedef bool (object::*method)(vgui_event const &, data );

  object *p;
  method  m;
  data    d;
vcl_string n;

  vgui_satellite_t(object *p_, method m_, data const &d_, vcl_string const &n_ = "")
    : p(p_), m(m_), d(d_), n(n_) { }

  bool handle(vgui_event const &e) { return (p && m) && (p->*m)(e, d); }

  vcl_string type_name() const { return vcl_string("vgui_satellite_t[") + n + vcl_string("]"); }

  vgui_menu a_menu;
  void add_popup(vgui_menu &m) { m.include(a_menu); }

protected:
  ~vgui_satellite_t() { p = 0; m = 0; }
};

// satellite without client data.
template <class object>
struct vgui_satellite : vgui_tableau
{
  typedef bool (object::*method)(vgui_event const &);

  object *p;
  method  m;
  vcl_string n;

  vgui_satellite(object *p_, method m_, vcl_string const &n_ = "")
    : p(p_), m(m_), n(n_) { }

  bool handle(vgui_event const &e) { return (p && m) && (p->*m)(e); }

  vcl_string type_name() const { return vcl_string("vgui_satellite[") + n + vcl_string("]"); }

protected:
  ~vgui_satellite() { p = 0; m = 0; }
};

#ifdef VCL_SUNPRO_CC_50
# define quirk(x) , x
#else
# define quirk(x) /* */
#endif
template <class object, class data>
struct vgui_satellite_t_new : vgui_tableau_sptr_t<vgui_satellite_t<object, data> quirk(vgui_tableau_sptr)>
{
// no vgui_make_sptr: this file must be maintained manually.
  typedef vgui_satellite_t<object, data> impl;
  typedef vgui_tableau_sptr_t<impl quirk(vgui_tableau_sptr)> base;
  typedef typename impl::method method;
  vgui_satellite_t_new(object *p, method m, data const &d, vcl_string const &n = "") : base(new impl(p, m, d, n)) { }
};

template <class object>
struct vgui_satellite_new : vgui_tableau_sptr_t<vgui_satellite<object> quirk(vgui_tableau_sptr)>
{
// no vgui_make_sptr: this file must be maintained manually.
  typedef vgui_satellite<object> impl;
  typedef vgui_tableau_sptr_t<impl quirk(vgui_tableau_sptr)> base;
  typedef typename impl::method method;
  vgui_satellite_new(object *p, method m, vcl_string const &n = "") : base(new impl(p, m, n)) { }
};
#undef quirk

// these could be in the .txx file but there would be no point to that.
#undef VGUI_SATELLITE_T_INSTANTIATE
#define VGUI_SATELLITE_T_INSTANTIATE(C, A) \
template struct vgui_satellite_t<C, A >; \
template struct vgui_satellite_t_new<C, A >

#undef VGUI_SATELLITE_INSTANTIATE
#define VGUI_SATELLITE_INSTANTIATE(C) \
template struct vgui_satellite<C >; \
template struct vgui_satellite_new<C >

#endif // vgui_satellite_h_
