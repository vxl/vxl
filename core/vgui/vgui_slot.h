#ifndef vgui_slot_h_
#define vgui_slot_h_

// .NAME vgui_slot
// .INCLUDE vgui/vgui_slot.h
// .FILE vgui_slot.cxx
//
// .SECTION Description
// Q: what is a vgui_slot?
// A: These are essentially specialized smart pointers. A slot
// refers to an edge in the tableau [di]graph. A tableau acquires
// a child by creating a slot with itself as parent and the child
// as, er, child. The slots are responsible for storing this relation
// behind the scenes so that it can be queried and used for, say,
// propagating redraw requests.
//
// Semantics:
// Assigning a slot to a slot
//   lhs = rhs;
// doesn't change the graph, only the lhs which no longer refers
// to the edge it used to refer to.
// Calling set_child(t) on a slot changes the graph (the given
// tableau *t becomes the new child of the slot).
// eg:
//   slot left (this,child);
//   slot right(this,child);
//   bool v = (left == right); // this is false
// or :
//   slot e(this,child);
//   slot f=e;
//   bool v = (e == f); // this is true
//
// You can use a slot much like a pointer to tableaux. It
// will behave like the child of the slot :
//   left->handle(e);   // same as left.child()->handle(e);
//   right->method();   // same as right.child()->method();
// In particular, you may put your slots into vectors, trees,
// stacks etc if that is useful for your purposes. Repeat :
// copying a slot does not create a new edge in the graph, only
// another handle to the same edge.
//
// Attempting to create a non-empty slot whose parent and child
// are the same tableau causes an abort().
//
// .SECTION Author
//   fsm@robots.ox.ac.uk
//
//--------------------------------------------------------------------------------

#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class  vgui_event;
class  vgui_tableau;
struct vgui_tableau_sptr;
struct vgui_slot_impl;   // implementation class.
//#include <vgui/vgui_tableau.h>

struct vgui_slot
{
  vgui_slot();
  vgui_slot(vgui_slot const &);
  // the 'parent' parameter is the self pointer ('this') of the tableau
  // which intends to hold the slot. it may *not* be a null pointer.
  // to make an uninitialized slot, use the default constructor.
  vgui_slot(vgui_tableau * parent /* child is zero */);
  vgui_slot(vgui_tableau * parent, vgui_tableau_sptr const &child);
  ~vgui_slot();

  vgui_slot &operator=(vgui_slot const &);

  // slots are equal if they have the same implementation. merely having
  // the same parent and child does not imply equality.
  bool operator==(vgui_slot const &s) const { return pimpl == s.pimpl; }

  // comparing a slot with a tableau compares the child.
  bool operator==(vgui_tableau_sptr const &t) const;

  // these methods are rarely needed. why would you use them?
  vgui_tableau_sptr parent() const;
  vgui_tableau_sptr child () const;

  // a slot behaves more like its child than its parent :
  operator bool () const;
  operator vgui_tableau_sptr () const;
  vgui_tableau *operator -> () const;

  // extra methods
  bool handle(vgui_event const &e);
  void assign(vgui_tableau_sptr const &); // sets child only

  // ---------- statics ----------

  //: Push all children of 'tab' onto the vector.
  static void get_children_of(vgui_tableau_sptr const &tab, vcl_vector<vgui_tableau_sptr> *);
  //: Push all parents of 'tab' onto the vector.
  static void get_parents_of (vgui_tableau_sptr const &tab, vcl_vector<vgui_tableau_sptr> *);
  //: In all slots, replace old_child with new_child.
  static void replace_child_everywhere (vgui_tableau_sptr const &old_child,
					vgui_tableau_sptr const &new_child);

private:
  friend class vgui_tableau;
  // Pointer to implementation
  vgui_slot_impl *pimpl;
};

vcl_ostream & operator<<(vcl_ostream &, vgui_slot const &);

#endif // vgui_slot_h_
