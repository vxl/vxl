// This is core/vgui/vgui_satellite_tableau.h
#ifndef vgui_satellite_tableau_h_
#define vgui_satellite_tableau_h_
//:
// \file
// \author fsm
// \brief  Tableau to turn a non-tableau into a multi-tableau.
//
//  Contains classes: vgui_satellite_tableau_t  vgui_satellite_tableau
//                    vgui_satellite_tableau_t_new vgui_satellite_tableau_new
//
// \verbatim
//  Modifications
//   02-OCT-2002 A.W.Fitzgibbon - Changed overview documentation.
// \endverbatim

#include <vgui/vgui_menu.h>
#include <vgui/vgui_tableau.h>

//----------------------------------------------------------------------------
//: Tableau to turn a non-tableau into a multi-tableau (with client data).
//
// PURPOSE: To turn a non-tableau into a multi-tableau, or
//          to put one tableau into two parts of the tree
//          simultaneously.
//
// EXAMPLE:
//          We are displaying two images, each in its own zoomer
//          and we'd like to have a tableau which takes mouse events
//          from one image and draws a line on the other.  This
//          introduces a "crossover" in the tree which is difficult
//          to handle without vgui_satellite_tableau.
//
// The basic layout we want is like this:
// \verbatim
//                          Adaptor
//                             |
//                             |
//                            grid
//                            / \                         .
//                           /   \                        .
//                          /     \                       .
//                        zoom    zoom
//                         |       |
//                          \     /
//              draw_on_both_children_tableau ("dob")
//                          /     \                       .
//                         |       |
//                       image   image
// \endverbatim
//
// But a single tableau cannot have multiple paths through
// "handle", so we might split "dob" into two helper tableaux,
// which each hold a pointer to the data required to implement
// the drawing.  Then the "handle" methods of each of the helpers
// are just small stubs which call the handle method on "dob",
// telling it whether it's in the "left" tree or the "right" tree.
// \verbatim
//                          Adaptor
//                             |
//                             |
//                      ---- grid ----
//                     /              \                   .
//                  zoom              zoom
//                    |                |
//                    |                |
//                  dob1 - -> dob <- - dob2
//                           /   \                        .
//                          /     \                       .
//            "handle(e,1)"+       +"handle(e,2)"
//                         |       |
//                         |       |
//                       image   image
// \endverbatim
//
// However, this is a small bit of hassle for the writer of "dob", which
// vgui_satellite_tableau makes easier.  Add an extra argument to
// dob::handle(event), which indicates which side the event is on, and then
// put a
//   vgui_satellite_tableau_t<type_of_dob, int>(dob, 1)
// in the left tree and a
//   vgui_satellite_tableau_t<type_of_dob, int>(dob, 2)
// in the right tree.  Then whenever an event goes down the tree
// the satellites call dob::handle with a 1 or a 2, so dob knows
// which way to pass it on...
template <class object, class data>
struct vgui_satellite_tableau_t : public vgui_tableau
{
  typedef bool (object::*method)(vgui_event const &, data );

  //: The 'object' type must have a method type_name().
  //  It is used to implement the type_name() methods on the satellites.
  object *p;

  method  m;

  //: Client data.
  //  The 'data' parameter may seem superfluous because one could
  //  achieve the same result by having two methods which were used
  //  to initialize the satellites. However, if the number of satellites
  //  is unknown at compile time, or if there are many of them and it
  //  is easier to generate them in code, then the 'data' parameter is
  //  necessary.
  data    d;

  //: Name.
  vcl_string n;

  //: Constructor - don't use this, use vgui_satellite_tableau_t_new.
  //  There is no vgui_satellite_tableau_t_sptr for this tableau.
  vgui_satellite_tableau_t(object *p_, method m_, data const &d_,
                           vcl_string const &n_ = "")
    : p(p_), m(m_), d(d_), n(n_) { }

  bool handle(vgui_event const &e) { return (p && m) && (p->*m)(e, d); }

  vcl_string type_name() const
    { return vcl_string("vgui_satellite_tableau_t[") + n + vcl_string("]"); }

  vgui_menu a_menu;
  void add_popup(vgui_menu &m) { m.include(a_menu); }

 protected:
  ~vgui_satellite_tableau_t() { p = 0; m = 0; }
};

//----------------------------------------------------------------------------
//: Tableau to turn a non-tableau into a multi-tableau (without client data).
//
//  See vgui_satellite_tableau_t to see what a satellite does.
template <class object>
struct vgui_satellite_tableau : public vgui_tableau
{
  typedef bool (object::*method)(vgui_event const &);

  //:The 'object' type must have a method type_name().
  // It is used to implement the type_name() methods on the satellites.
  object *p;

  method  m;

  //: Name.
  vcl_string n;

  //: Constructor - don't use this, use vgui_satellite_tableau_new.
  //  There is no vgui_satellite_tableau_sptr for this tableau.
  vgui_satellite_tableau(object *p_, method m_, vcl_string const &n_ = "")
    : p(p_), m(m_), n(n_) { }

  bool handle(vgui_event const &e) { return (p && m) && (p->*m)(e); }

  vcl_string type_name() const
    { return vcl_string("vgui_satellite_tableau[") + n + vcl_string("]"); }

 protected:
  ~vgui_satellite_tableau() { p = 0; m = 0; }
};

//----------------------------------------------------------------------------
#ifdef VCL_SUNPRO_CC_50
# define quirk(x) , x
#else
# define quirk(x) /* */
#endif

template <class object, class data>
struct vgui_satellite_tableau_t_new : public vgui_tableau_sptr_t<vgui_satellite_tableau_t<object, data> quirk(vgui_tableau_sptr)>
{
  // no vgui_make_sptr: this file must be maintained manually.
  typedef vgui_satellite_tableau_t<object, data> impl;
  typedef vgui_tableau_sptr_t<impl quirk(vgui_tableau_sptr)> base;
  typedef typename impl::method method;
  vgui_satellite_tableau_t_new(object *p, method m, data const &d,
                               vcl_string const&n=""):base(new impl(p,m,d,n)) {}
};

//----------------------------------------------------------------------------
template <class object>
struct vgui_satellite_tableau_new : public vgui_tableau_sptr_t<vgui_satellite_tableau<object> quirk(vgui_tableau_sptr)>
{
  // no vgui_make_sptr: this file must be maintained manually.
  typedef vgui_satellite_tableau<object> impl;
  typedef vgui_tableau_sptr_t<impl quirk(vgui_tableau_sptr)> base;
  typedef typename impl::method method;
  vgui_satellite_tableau_new(object *p, method m, vcl_string const &n = "")
    : base(new impl(p, m, n)) { }
};
#undef quirk

//----------------------------------------------------------------------------
// these could be in the .txx file but there would be no point to that.
#undef VGUI_SATELLITE_T_INSTANTIATE
#define VGUI_SATELLITE_T_INSTANTIATE(C, A) \
template struct vgui_satellite_tableau_t<C, A >; \
template struct vgui_satellite_tableau_t_new<C, A >

#undef VGUI_SATELLITE_INSTANTIATE
#define VGUI_SATELLITE_INSTANTIATE(C) \
template struct vgui_satellite_tableau<C >; \
template struct vgui_satellite_tableau_new<C >

#endif // vgui_satellite_tableau_h_
