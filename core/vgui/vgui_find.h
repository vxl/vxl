// This is oxl/vgui/vgui_find.h
#ifndef vgui_find_h_
#define vgui_find_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  fsm
// \brief   Functions to search for tableaux in the tableau hierachy.

#include <vcl_string.h>
#include <vgui/vgui_tableau_sptr.h>

//: Search for a tableau by type in the hierarchy.
//  If direction_down is true, it will search below param
//  start. Otherwise, it will search above param start. Returns a
//  null pointer if a matching tableau is not found.
vgui_tableau_sptr vgui_find_by_type_name(vgui_tableau_sptr const& start, vcl_string const &tn, bool direction_down);

//: Search for a tableau by type in the hierarchy.
//  This is a convenience function. It calls vgui_find_by_type_name().
//  \sa vgui_find_by_type_name()
inline
vgui_tableau_sptr vgui_find_below_by_type_name(vgui_tableau_sptr const& start, vcl_string const &tn)
{ return vgui_find_by_type_name(start, tn, true); }

//: Search for a tableau by type in the hierarchy.
//  This is a convenience function. It calls vgui_find_by_type_name().
//  \sa vgui_find_by_type_name()
inline
vgui_tableau_sptr vgui_find_above_by_type_name(vgui_tableau_sptr const& start, vcl_string const &tn)
{ return vgui_find_by_type_name(start, tn, false); }


//: Search for a tableau by name in the hierarchy.
//  If direction_down is true, it will search below param
//  start. Otherwise, it will search above param start. Returns a
//  null pointer if a matching tableau is not found.
vgui_tableau_sptr vgui_find_by_name(vgui_tableau_sptr const& start, vcl_string const &name, bool direction_down);

//: Search for a tableau by name in the hierarchy.
//  This is a convenience function. It calls vgui_find_by_name().
//  \sa vgui_find_by_name()
inline
vgui_tableau_sptr vgui_find_below_by_name(vgui_tableau_sptr const& start, vcl_string const &name)
{ return vgui_find_by_name(start, name, true); }

//: Search for a tableau by name in the hierarchy.
//  This is a convenience function. It calls vgui_find_by_name().
//  \sa vgui_find_by_name()
inline
vgui_tableau_sptr vgui_find_above_by_name(vgui_tableau_sptr const& start, vcl_string const &name)
{ return vgui_find_by_name(start, name, false); }

#endif // vgui_find_h_
