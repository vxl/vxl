// This is core/vgui/vgui_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  See vgui_tableau.h for a description of this file.
//
// \verbatim
//  Modifications
//   11-SEP-1999 P.Pritchett - Initial version.
//   08-OCT-2002 K.Y.McGaul - Removed unused adopt and disown functions.
// \endverbatim

#include "vgui_tableau.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vgui/vgui_macro.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_popup_params.h>

// static data
//------------
// Every tableau is on this array.
// It must be a ptr as must live longer than any vgui_tableau_sptr
static vgui_DLLDATA vcl_vector<vgui_tableau*>* all = 0;

//-----------------------------------------------------------------------------
//: Constructor.
//  Don't use the constructor for a tableau, use vgui_tableau_sptr to get
//  a smart-pointer to your tableau.
vgui_tableau::vgui_tableau()
  : references(0)
{
#ifdef DEBUG
  vcl_cerr << "vgui_tableau constructor: this = " << (void*)this << '\n';
#endif
  // register :
  if (all == 0) {
    all = new vcl_vector<vgui_tableau*>;
  }
  all->push_back(this);
}

//-----------------------------------------------------------------------------
//: Destructor - called by vgui_tableau_sptr when ref count is zero.
vgui_tableau::~vgui_tableau()
{
#ifdef DEBUG
  vcl_cerr << "vgui_tableau destructor : this = " << (void*)this << '\n';
#endif

  if (references != 0)
    vgui_macro_warning << "there are still " << references
                       << " references. this=" << (void*)this << vcl_endl;

  // deregister :
  vcl_vector<vgui_tableau*>::iterator i=vcl_find(all->begin(), all->end(), this);
  assert(i != all->end());
  all->erase(i);
  if (all->size() == 0)
  {
    delete all;
    all = 0;
  }
}

//-----------------------------------------------------------------------------
//: Increase the reference count by one (for smart-pointers).
void vgui_tableau::ref() const
{
  ++ const_cast<int &>(references);
}

//-----------------------------------------------------------------------------
//: Decrease the reference count by one (for smart-pointers).
//  If the reference count reaches zero then delete the object.
void vgui_tableau::unref() const
{
  assert(references > 0); // fatal if not

  if (-- const_cast<int &>(references) == 0) {
    delete const_cast<vgui_tableau*>(this);
  }
}

//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
//  Override in subclasses to give the tableau some appearance and behaviour.
bool vgui_tableau::handle(vgui_event const &event) {
  vgui_macro_report_errors;

  switch (event.type) {
  case vgui_DRAW:
    return draw();

  case vgui_BUTTON_DOWN:
    return mouse_down (event.wx, event.wy, event.button, event.modifier);

  case vgui_MOTION:
    return motion     (event.wx, event.wy);

  case vgui_BUTTON_UP:
    return mouse_up   (event.wx, event.wy, event.button, event.modifier);

  case vgui_KEY_PRESS:
    if (event.key == '?' || event.key == '/')
      return help();
    else
      return key_press(event.wx, event.wy, event.key, event.modifier);

  default:
    return false;
  }
}

//-----------------------------------------------------------------------------
//: Called by default handle when it receives a mouse down event.
bool vgui_tableau::mouse_down(int, int, vgui_button, vgui_modifier) {
#ifdef DEBUG
  vcl_cerr << "vgui_tableau::mouse_down\n";
#endif
  return false;
}

//-----------------------------------------------------------------------------
//: Called by default handle when it receives a mouse up event.
bool vgui_tableau::mouse_up(int, int, vgui_button, vgui_modifier) {
#ifdef DEBUG
  vcl_cerr << "vgui_tableau::mouse_up\n";
#endif
  return false;
}

//-----------------------------------------------------------------------------
//: Called by default handle when it receives a mouse motion event.
bool vgui_tableau::motion(int, int) {
#ifdef DEBUG
  vcl_cerr << "vgui_tableau::motion\n";
#endif
  return false;
}

//-----------------------------------------------------------------------------
//: Caled by default handle when it receives a key press event.
bool vgui_tableau::key_press(int, int, vgui_key, vgui_modifier) {
#ifdef DEBUG
  vcl_cerr << "vgui_tableau::key_press\n";
#endif
  return false;
}

//-----------------------------------------------------------------------------
//: Called by default handle when it receives a '?' pressed event.
bool vgui_tableau::help() {
#ifdef DEBUG
  vcl_cerr << "vgui_tableau::help\n";
#endif
  return false;
}

//-----------------------------------------------------------------------------
//: Called by default handle when it receives a draw event.
bool vgui_tableau::draw() {
#ifdef DEBUG
  vcl_cerr << "vgui_tableau::draw\n";
#endif
  return false;
}


//-----------------------------------------------------------------------------
//: Return the bounding box of this tableau.
//  If infinite in extent, or nothing is drawn, or you can't be bothered to
//  implement it, return false.
bool vgui_tableau::get_bounding_box(float /*low*/[3], float /*high*/[3]) const {
  return false;
}

//-----------------------------------------------------------------------------
//: Post a message event.
void vgui_tableau::post_message(char const *msg, void const *data) {
  vcl_vector<vgui_tableau_sptr> ps;
  get_parents(&ps);
  for (unsigned i=0; i<ps.size(); ++i)
    ps[i]->post_message(msg, data);
}

//-----------------------------------------------------------------------------
//: Post a draw event.
void vgui_tableau::post_redraw() {
  vcl_vector<vgui_tableau_sptr> ps;
  get_parents(&ps);
  for (unsigned i=0; i<ps.size(); ++i)
    ps[i]->post_redraw();
}

//-----------------------------------------------------------------------------
//: Post a overlay redraw event.
void vgui_tableau::post_overlay_redraw() {
  vcl_vector<vgui_tableau_sptr> ps;
  get_parents(&ps);
  for (unsigned i=0; i<ps.size(); ++i)
    ps[i]->post_overlay_redraw();
}

//-----------------------------------------------------------------------------
//: Return the name of the tableau.
vcl_string vgui_tableau::name() const {
  return file_name();
}

//-----------------------------------------------------------------------------
//: Return the name of a file associated with some tableau below.
//  Virtual function.
vcl_string vgui_tableau::file_name() const {
  return "(none)";
}

//-----------------------------------------------------------------------------
//: Used to provide an informative name for printouts, debugging etc.
//  Often it's type_name() + some representation of the essential state.
//  Virtual function.
vcl_string vgui_tableau::pretty_name() const {
  return type_name();
}

//-----------------------------------------------------------------------------
//: Return the name of the most derived (tableau) class.
//  Virtual. This ought never to be called as derived classes should
//  implement type_name().
vcl_string vgui_tableau::type_name() const {
  static bool warned=false;
  if (!warned) {
    vgui_macro_warning << "WARNING: vgui_tableau::type_name() called\n";
    warned=true;
  }
  return "vgui_tableau";
}

//-----------------------------------------------------------------------------
//: Push parents onto the given vcl_vector.
void vgui_tableau::get_parents(vcl_vector<vgui_tableau_sptr> *v) const {
  vgui_parent_child_link::get_parents_of(const_cast<vgui_tableau*>(this),v);
}

//-----------------------------------------------------------------------------
//: Push children onto the given vcl_vector.
void vgui_tableau::get_children(vcl_vector<vgui_tableau_sptr> *v) const {
  vgui_parent_child_link::get_children_of(const_cast<vgui_tableau*>(this),v);
}

//-----------------------------------------------------------------------------
//: Get the ith child, or return 0.
vgui_tableau_sptr vgui_tableau::get_child(unsigned i) const {
  vcl_vector<vgui_tableau_sptr> children;
  get_children(&children);
  return i<children.size() ? children[i] : vgui_tableau_sptr();
}

//-----------------------------------------------------------------------------
//: Add the given tableau to the list of child tableaux.
//  Virtual overridden by consenting parents.
bool vgui_tableau::add_child(vgui_tableau_sptr const &) {
  return false;
}

//-----------------------------------------------------------------------------
//: Remove the given tableau from the list of child tableaux.
bool vgui_tableau::remove_child(vgui_tableau_sptr const&) {
  return false;
}

//-----------------------------------------------------------------------------
//: Called when a child of this tableau is forcibly replaced.
//  This method is called when some part of the program (typically the
//  parent_child_link mechanism) is about to forcibly replace a child of this
//  tableau.
//  The canonical reason to override this is in order to invalidate caches.
bool vgui_tableau::notify_replaced_child(vgui_tableau_sptr const& /*old_child*/,
                                         vgui_tableau_sptr const& /*new_child*/)
{
  return true;
}

//-----------------------------------------------------------------------------
//: Add given menu to the tableau popup menu.
//  This method is for tableaux to implement if they want to _add_ some items to
//  the popup menu. They can assign to or clear 'menu', but that is not
//  recommended as it would remove what other tableaux put there.
//  The recommended usage is to .add() items or to .include() another menu.
//
//  ** This is an interface method. it abstracts a behaviour. **
void vgui_tableau::add_popup(vgui_menu &/*menu*/) {
  // do nothing by default.
}

//-----------------------------------------------------------------------------
//: Gets popup menu for this tableau.
// If recurse is, true, recursively add the popup menus for children and
// children's children etc.
//
// ** this is a mixin method. it does some work for you. **
void vgui_tableau::get_popup(vgui_popup_params const &params, vgui_menu &menu) {
  // extract this tableau's popup menu into 'submenu'.
  vgui_menu submenu;
  add_popup(submenu);

  if (params.nested) { // nested menu style.
    // get list of children of this tableau.
    vcl_vector<vgui_tableau_sptr> children;
    get_children(&children);

    if (params.defaults && !children.empty())
      submenu.separator();

    for (unsigned i=0; i<children.size(); ++i)
      if (children[i])
        children[i]->get_popup(params, submenu);

    menu.add(type_name(), submenu);
  }
  else {
    // not nested.
    if (submenu.size() > 0) // do not add empty submenus.
      menu.include(submenu);

    if (params.recurse) {
      vcl_vector<vgui_tableau_sptr> children;
      get_children(&children);

      for (unsigned i=0; i<children.size(); ++i)
        if (children[i])
          children[i]->get_popup(params, menu);
    }
  }
}

//-----------------------------------------------------------------------------
//: Prints pretty name and address of tableau.
//  eg : pig.jpg[vgui_composite:0xeffff728]
vcl_ostream &operator<<(vcl_ostream &os, vgui_tableau_sptr const &t)
{
  if (t)
    return os << t->pretty_name() << '[' << t->type_name() << ':' << static_cast<const void*>(t.operator->()) << ']';
  else
    return os << "(empty vgui_tableau_sptr)" << vcl_flush;
}

//-----------------------------------------------------------------------------
//: Push all tableaux onto the given vector.
void vgui_tableau::get_all(vcl_vector<vgui_tableau_sptr> *v) {
  //v->insert(v->begin(), all->begin(), all->end());
  for (unsigned i=0; i<all->size(); ++i)
    v->push_back((*all)[i]);
}

//-----------------------------------------------------------------------------
//: Returns true if the given address points to a valid tableau.
bool vgui_tableau::exists(vgui_tableau_sptr const& ptr) {
  return vcl_find(all->begin(), all->end(), ptr.operator->()) != all->end();
}
