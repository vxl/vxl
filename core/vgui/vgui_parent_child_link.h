#ifndef vgui_parent_child_link_h_
#define vgui_parent_child_link_h_
//:
// \file
// \brief  Describes the relationship between a parent and child tableau.
// \author fsm
//
//  Contains class  vgui_parent_child_link
//
// \verbatim
//  Modifications
//   17-Sep-2002 K.Y.McGaul - Added doxygen style comments.
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class  vgui_event;
class  vgui_tableau;
struct vgui_tableau_sptr;
struct vgui_parent_child_link_impl;   // implementation class.

//: Describes the relationship between a parent and child tableau.
//
// Q: what is a vgui_parent_child_link?
// A: These are essentially specialized smart pointers. A parent_child_link
// refers to an edge in the tableau [di]graph. A tableau acquires
// a child by creating a parent_child_link with itself as parent and the child
// as, er, child. The parent_child_links are responsible for storing this
// relation behind the scenes so that it can be queried and used for, say,
// propagating redraw requests.
//
// Semantics:
// Assigning a parent_child_link to a parent_child_link
// \code
//   lhs = rhs;
// \endcode
// doesn't change the graph, only the lhs which no longer refers
// to the edge it used to refer to.
// Calling set_child(t) on a parent_child_link changes the graph (the given
// tableau *t becomes the new child of the parent_child_link).
// E.g.:
// \code
//   vgui_parent_child_link left (this,child);
//   vgui_parent_child_link right(this,child);
//   bool v = (left == right); // this is false
// \endcode
// or :
// \code
//   vgui_parent_child_link e(this,child);
//   vgui_parent_child_link f=e;
//   bool v = (e == f); // this is true
// \endcode
//
// You can use a parent_child_link much like a pointer to tableaux. It
// will behave like the child of the parent_child_link :
// \code
//   left->handle(e);   // same as left.child()->handle(e);
//   right->method();   // same as right.child()->method();
// \endcode
// In particular, you may put your parent_child_links into vectors, trees,
// stacks etc if that is useful for your purposes. Repeat :
// copying a parent_child_link does not create a new edge in the graph, only
// another handle to the same edge.
//
// Attempting to create a non-empty parent_child_link whose parent and child
// are the same tableau causes an abort().
struct vgui_parent_child_link
{
 private:
  //: Helper types for safe boolean conversion.
  struct safe_bool_dummy { void dummy() {} };
  typedef void (safe_bool_dummy::* safe_bool)();
 public:
  //: Constructor - creates a default vgui_parent_child_link.
  vgui_parent_child_link();

  //: Constructor - creates a vgui_parent_child_link same as the given one.
  vgui_parent_child_link(vgui_parent_child_link const &);

  //: Constructor - takes the parent tableau.
  //  The 'parent' parameter is the self pointer ('this') of the tableau
  //  which intends to hold the parent_child_link. It may *not* be a null
  //  pointer.
  //  To make an uninitialized parent_child_link, use the default constructor.
  vgui_parent_child_link(vgui_tableau * parent /* child is zero */);

  //: Constructor - takes the parent and child tableaux.
  //  The 'parent' parameter is the self pointer ('this') of the tableau
  //  which intends to hold the parent_child_link. It may *not* be a null
  //  pointer.
  //  To make an uninitialized parent_child_link, use the default constructor.
  vgui_parent_child_link(vgui_tableau * parent, vgui_tableau_sptr const &child);

  //: Destructor - delete this parent_child_link.
  ~vgui_parent_child_link();

  //: Make this parent_child_link equal to the given one.
  vgui_parent_child_link &operator=(vgui_parent_child_link const &);

  //: Returns true if this parent_child_link is the same as the given link.
  //  Links are equal if they have the same implementation. Merely having
  //  the same parent and child does not imply equality.
  bool operator==(vgui_parent_child_link const &s) const { return pimpl == s.pimpl; }

  //: Comparing a parent_child_link with a tableau compares the child.
  bool operator==(vgui_tableau_sptr const &t) const;

  //: Returns the parent tableau for this parent_child_link.
  vgui_tableau_sptr parent() const;

  //: Returns the child tableau for this parent_child_link.
  vgui_tableau_sptr child () const;

  //: Return true if both parent and child tableaux exist.
  operator safe_bool () const;

  //: Return false if both parent and child tableaux exist.
  bool operator!() const;

  //: Return a pointer to the child tableau.
  //  A parent_child_link behaves more like its child than its parent.
  operator vgui_tableau_sptr () const;

  //: Return a pointer to the child tableau.
  //  A parent_child_link behaves more like its child than its parent.
  vgui_tableau *operator -> () const;

  //: Let the child tableau handle the event.
  //  A parent_child_link behaves more like its child than its parent.
  bool handle(vgui_event const &e);

  //: Make the given tableau the child tableau in this relationship.
  //  A parent_child_link's parent pointer cannot be changed because there is no
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

  //: In all parent_child_links, replace old_child with new_child.
  static void replace_child_everywhere (vgui_tableau_sptr const &old_child,
                                        vgui_tableau_sptr const &new_child);

 private:
  friend class vgui_tableau;
  // Pointer to implementation
  vgui_parent_child_link_impl *pimpl;
};

// Work-around for Borland and safe_bool.
#ifdef VCL_BORLAND
inline
bool operator&&(const vgui_parent_child_link& link, bool b)
{
  return b && (link?true:false);
}
inline
bool operator&&(bool b, const vgui_parent_child_link& link)
{
  return b && (link?true:false);
}
inline
bool operator||(const vgui_parent_child_link& link, bool b)
{
  return b || (link?true:false);
}
inline
bool operator||(bool b, const vgui_parent_child_link& link)
{
  return b || (link?true:false);
}
#endif

vcl_ostream & operator<<(vcl_ostream &, vgui_parent_child_link const &);

#endif // vgui_parent_child_link_h_
