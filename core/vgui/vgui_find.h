#ifndef vgui_find_h_
#define vgui_find_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl_string.h>
#include <vgui/vgui_tableau_sptr.h>

//: Searching for tableaux in the hierarchy.

vgui_tableau_sptr vgui_find_by_type_name(vgui_tableau_sptr const& start, vcl_string const &tn, bool direction_down);

inline
vgui_tableau_sptr vgui_find_below_by_type_name(vgui_tableau_sptr const& start, vcl_string const &tn)
{ return vgui_find_by_type_name(start, tn, true); }

inline
vgui_tableau_sptr vgui_find_above_by_type_name(vgui_tableau_sptr const& start, vcl_string const &tn)
{ return vgui_find_by_type_name(start, tn, false); }

#endif
