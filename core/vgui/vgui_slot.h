#ifndef vgui_slot_h_
#define vgui_slot_h_
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Describes the relationship between a parent and child tableau.
// 
//  Contains classes:  vgui_slot
//  
// \verbatim
//  Modifications:
//    17-Sep-2002 K.Y.McGaul - Added doxygen style comments.
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class  vgui_event;
class  vgui_tableau;
struct vgui_tableau_sptr;
struct vgui_slot_impl;   // implementation class.

//: Describes the relationship between a parent and child tableau.
//
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
// \code
//   lhs = rhs;
// \endcode
// doesn't change the graph, only the lhs which no longer refers
// to the edge it used to refer to.
// Calling set_child(t) on a slot changes the graph (the given
// tableau *t becomes the new child of the slot).
// E.g.:
// \code
//   slot left (this,child);
//   slot right(this,child);
//   bool v = (left == right); // this is false
// \endcode
// or :
// \code
//   slot e(this,child);
//   slot f=e;
//   bool v = (e == f); // this is true
// \endcode
//
// You can use a slot much like a pointer to tableaux. It
// will behave like the child of the slot :
// \code
//   left->handle(e);   // same as left.child()->handle(e);
//   right->method();   // same as right.child()->method();
// \endcode
// In particular, you may put your slots into vectors, trees,
// stacks etc if that is useful for your purposes. Repeat :
// copying a slot does not create a new edge in the graph, only
// another handle to the same edge.
//
// Attempting to create a non-empty slot whose parent and child
// are the same tableau causes an abort().
struct vgui_slot
{
  //: Constructor - creates a default vgui_slot.
  vgui_slot();

  //: Constructor - creates a vgui_slot same as the given one.
  vgui_slot(vgui_slot const &);

  //: Constructor - takes the parent tableau.
  //  The 'parent' parameter is the self pointer ('this') of the tableau
  //  which intends to hold the slot. It may *not* be a null pointer.
  //  To make an uninitialized slot, use the default constructor.
  vgui_slot(vgui_tableau * parent /* child is zero */);

  //: Constructor - takes the parent and child tableaux.
  //  The 'parent' parameter is the self pointer ('this') of the tableau
  //  which intends to hold the slot. It may *not* be a null pointer.
  //  To make an uninitialized slot, use the default constructor.
  vgui_slot(vgui_tableau * parent, vgui_tableau_sptr const &child);

  //: Destructor - delete this slot.
  ~vgui_slot();

  //: Make this slot equal to the given one.
  vgui_slot &operator=(vgui_slot const &);

  //: Returns true if the this slot is the same as the given slot.
  //  Slots are equal if they have the same implementation. Merely having
  //  the same parent and child does not imply equality.
  bool operator==(vgui_slot const &s) const { return pimpl == s.pimpl; }

  // Comparing a slot with a tableau compares the child.
  bool operator==(vgui_tableau_sptr const &t) const;

  //: Returns the parent tableau for this slot.
  vgui_tableau_sptr parent() const;

  //: Returns the child tableau for this slot.
  vgui_tableau_sptr child () const;

  //: Return true if both parent and child tableaux exist.
  operator bool () const;
 
  //: Return a pointer to the child tableau. 
  //  A slot behaves more like its child than its parent.
  operator vgui_tableau_sptr () const;

  //: Return a pointer to the child tableau.
  //  A slot behaves more like its child than its parent.
  vgui_tableau *operator -> () const;

  //: Let the child tableau handle the event.
  //  A slot behaves more like its child than its parent.
  bool handle(vgui_event const &e);

  //: Make the given tableau the child tableau in this relationship.
  //  A slot's parent pointer cannot be changed because there is no
  //  legitimate use for that.
  //  Attempting to set the child to be the same tableau as the parent will 
  //  cause an abort().
  void assign(vgui_tableau_sptr const &); // sets child only

  // ---------- statics ----------

  //: Push all children of 'tab' onto the vector.
  static void get_children_of(vgui_tableau_sptr const &tab, 
    vcl_vector<vgui_tableau_sptr> *);

  //: Push all parents of 'tab' onto the vector.
  static void get_parents_of (vgui_tableau_sptr const &tab, 
    vcl_vector<vgui_tableau_sptr> *);

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
