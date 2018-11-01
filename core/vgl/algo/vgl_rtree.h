// This is core/vgl/algo/vgl_rtree.h
#ifndef vgl_rtree_h_
#define vgl_rtree_h_
//:
// \file
// \author fsm
// \brief Templated rtree class and associated classes and functions
//--------------------------------------------------------------------------------

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// forward declare all classes.
template <class V, class B, class C> class vgl_rtree_probe;
template <class V, class B, class C> class vgl_rtree_node;
template <class V, class B, class C> class vgl_rtree_iterator_base;
template <class V, class B, class C> class vgl_rtree_iterator;
template <class V, class B, class C> class vgl_rtree_const_iterator;
template <class V, class B, class C> class vgl_rtree;

//: Function predicate object for querying the tree.
template <class V, class B, class C>
class vgl_rtree_probe
{
 public:
  virtual ~vgl_rtree_probe() = default;
  //: return true if the probe "meets" the given object.
  virtual bool meets(V const &v) const { B b; C::init(b, v); return meets(b); }
  virtual bool meets(B const &b) const =0;
};

//: max. number of Vs stored in a node.
// should be a template argument?
#define vgl_rtree_MAX_VERTICES (8)

//: max. number of children of a given node.
// should be a template argument?
#define vgl_rtree_MAX_CHILDREN (8)

//: Represent a node in the rtree.
template <class V, class B, class C>
class vgl_rtree_node
{
 public:
  typedef vgl_rtree_node<V, B, C> node;

  // contains bound on all Vs in this node and below.
  B bounds;

  // pointer to parent. 0 if none.
  node *parent;


  // number of vertex elements in this node and its children.
  unsigned total_vts;

  // number of vertex elements in this node.
  unsigned local_vts;

  // the elements are stored in this array.
  V vts[vgl_rtree_MAX_VERTICES];


  // 1 + number of nodes below this one.
  unsigned total_chs;

  // number of children of this node.
  unsigned local_chs;

  // pointers to children are stored in this array.
  node *chs[vgl_rtree_MAX_CHILDREN];

  // -------------------- methods

  vgl_rtree_node(node *parent, V const &v);
  ~vgl_rtree_node();

  // get all Vs which meet the given region.
  void get(B const &region, std::vector<V> &) const;

  // get all Vs whose bounds meet the given probe.
  void get(vgl_rtree_probe<V, B, C> const &region, std::vector<V> &) const;

  // get all Vs.
  void get_all(std::vector<V> &vs) const;

  // find node containing a V equal to the given one.
  bool find(V const &v, node **n, int *i) const;
  bool find(B const &b, V const &v, node **n, int *i) const;

  // add another V to the tree. return node it was added to.
  node *add(V const &v);

  // remove ith vertex from node.
  void erase(unsigned int i);

  void print() const;

 private:
  friend class vgl_rtree_iterator_base<V, B, C>;

  // the following methods are not used by the vgl_rtree.
  unsigned int find_index_in_parent() const;
  void compute_bounds();
  void update_total_vts(int diff);
  void update_total_chs(int diff);
  void update_vertex_count(int diff);
  void update_child_count (int diff);
};

//--------------------------------------------------------------------------------

//: Base class for both rtree iterators.
template <class V, class B, class C>
class vgl_rtree_iterator_base
{
 public:
  typedef vgl_rtree_node<V, B, C> node;
  node *current;
  unsigned int i;

  vgl_rtree_iterator_base(node *root) : current(root), i(0) { }
  vgl_rtree_iterator_base() : current(nullptr), i(0) { }

  void operator_pp();
  void operator_mm();
};

template <class V, class B, class C>
bool operator==(vgl_rtree_iterator_base<V, B, C> const &a,
                vgl_rtree_iterator_base<V, B, C> const &b);

template <class V, class B, class C>
inline bool operator!=(vgl_rtree_iterator_base<V, B, C> const &a,
                       vgl_rtree_iterator_base<V, B, C> const &b)
{ return !( a == b ); }

//: Iterator for rtree.
template <class V, class B, class C>
class vgl_rtree_iterator : public vgl_rtree_iterator_base<V, B, C>
{
 public:
  typedef vgl_rtree_iterator_base<V, B, C> base;
  typedef vgl_rtree_iterator<V, B, C> self;
  typedef vgl_rtree_node<V, B, C> node;

  vgl_rtree_iterator(node *root) : base(root) { }
  vgl_rtree_iterator() = default;

  V &operator*() const { return base::current->vts[base::i]; }

  self &operator++() { base::operator_pp(); return *this; }
  self &operator--() { base::operator_mm(); return *this; }

  self operator++(int) { self tmp = *this; base::operator_pp(); return tmp; }
  self operator--(int) { self tmp = *this; base::operator_mm(); return tmp; }
};

//: const_iterator for rtree.
template <class V, class B, class C>
class vgl_rtree_const_iterator : public vgl_rtree_iterator_base<V, B, C>
{
 public:
  typedef vgl_rtree_iterator_base<V, B, C> base;
  typedef vgl_rtree_const_iterator<V, B, C> self;
  typedef vgl_rtree_node<V, B, C> node;

  vgl_rtree_const_iterator(node *root) : base(root) { }
  vgl_rtree_const_iterator(vgl_rtree_iterator<V, B, C> const &that) : base(that) { }
  vgl_rtree_const_iterator() = default;

  V const &operator*() const { return base::current->vts[base::i]; }

  self &operator++() { base::operator_pp(); return *this; }
  self &operator--() { base::operator_mm(); return *this; }

  self operator++(int) { self tmp = *this; base::operator_pp(); return tmp; }
  self operator--(int) { self tmp = *this; base::operator_mm(); return tmp; }
};

//--------------------------------------------------------------------------------

//: Templated rtree class.
// The rtree is templated over the element
// type V, the type B of the bounding region used (e.g.
// axis-aligned bounding boxes are common but there are other
// possibilities such as boxes which are axis-aligned ones or even
// bounding ellipsoids) and a type C which is used as a namespace
// for some functionality needed by the rtree.
//
// The rtree is a container of Vs which may contain multiple
// copies of the same element. The container for client use is
// called vgl_rtree<V, B, C> and is defined at the bottom of the
// file.
//
// Note that the iterators are bidirectional but only forward
// advancement has been implement so far (it's tedious work).
// Moreover, beware that changing an element through an iterator
// will not update the rtree, so may lead to the data structure
// being corrupted.
//
// -  V : element type
// -  B : bounds type
// -  C : mystery type
//
// The C-device makes it possible to have an rtree of Vs using Bs
// without having to modify the class definitions of V or B. It
// may be better to have C's signatures non-static and store a C
// on every tree node, but I don't know about this.
//
// It is assumed that the cost of copying (e.g. by assignment) Vs
// and Bs is not too high. In any case, I suggest you inline and
// optimize heavily when compiling this source file.
//
// V must have the following signatures:
// \code
//   V::V();
//   V::V(V const &);
//   V::operator==(V const &) or operator==(V const &, V const &);
// \endcode
//
// B must have the following signatures :
// \code
//   B::B();
//   B::B(const &);
//   B::operator==(V const &) or operator==(B const &, B const &);
// \endcode
//
// C must have the following (static method) signatures :
// \code
//   void  C::init  (B &, V const &);
//   void  C::update(B &, V const &);
//   void  C::update(B &, B const &);
//   bool  C::meet  (B const &, V const &);
//   bool  C::meet  (B const &, B const &);
//   float C::volume(B const &);
// \endcode
//
// The volume() method is used by the rtree to make decisions
// about where to put new elements.
template <class V, class B, class C>
class vgl_rtree
{
 public:
  vgl_rtree() : root(nullptr) { }
  ~vgl_rtree() {
    if (root)
      delete root;
    root = nullptr;
  }

  //
  typedef vgl_rtree_probe<V, B, C> probe;

  //: iterators
  typedef vgl_rtree_iterator      <V, B, C> iterator;
  typedef vgl_rtree_const_iterator<V, B, C> const_iterator;

  iterator       begin() { return root ? iterator(root) : iterator(); }
  const_iterator begin() const { return root ? const_iterator(root) : const_iterator(); }

  iterator       end() { return iterator(); }
  const_iterator end() const { return const_iterator(); }

  //: add an element to the rtree.
  void add(V const &v) {
    if (root)
      root->add(v);
    else
      root = new node(nullptr/*parent*/, v);
  }

  //: remove one element from the rtree.
  void remove(V const &v) {
    if (root) {
      B tmp;
      C::init(tmp, v);
      node *n;
      int i;
      if (root->find(tmp, v, &n, &i))
        n->erase(i);

      if (root->total_vts == 0) {
        delete root;
        root = nullptr;
      }
    }
  }

  //: return true iff the rtree contains an element equal to v.
  bool contains(V const &v) const {
    node *n;
    int i;
    return root ? root->find(v, &n, &i) : false;
  }
#if 0
  iterator       find(V const &v);
  const_iterator find(V const &v) const;
#endif

  //: erase the element pointed to by the iterator.
  // may invalidate \e all iterators into the rtree.
  void erase(iterator i) {
    i.current->erase(i.i);
    if (root->total_vts == 0) {
      delete root;
      root = nullptr;
    }
  }

  //: get elements in the given region.
  void get(B const &region, std::vector<V> &vs) const {
    if (root)
      root->get(region, vs);
  }

  //: get elements which meet the given probe.
  void get(vgl_rtree_probe<V, B, C> const &region, std::vector<V> &vs) const {
    if (root)
      root->get(region, vs);
  }

  //: get all elements in the tree.
  void get_all(std::vector<V> &vs) const {
    if (root)
      root->get_all(vs);
  }

  //: return true iff the tree has no elements.
  bool empty() const {
    return root ? root->total_vts==0 : true;
  }

  //: return number of elements stored in the tree.
  unsigned size() const {
    return root ? root->total_vts : 0;
  }

  //: return number of nodes used by the tree.
  unsigned nodes() const {
    return root ? root->total_chs : 0;
  }

  // for internal use only.
  typedef vgl_rtree_node<V, B, C> node;
  node *secret_get_root() const { return root; } // for debugging purposes.

  void print() const {
    root->print();
  }

 private:
  node *root;
  // disallow assignment
  vgl_rtree<V, B, C>& operator=(vgl_rtree<V, B, C> const &) { return *this; }
  vgl_rtree(vgl_rtree<V, B, C> const &) { }
};

#define VGL_RTREE_INSTANTIATE(V, B, C) extern "you must include vgl_rtree.hxx first"

#endif // vgl_rtree_h_
