#ifndef vgl_rtree_hxx_
#define vgl_rtree_hxx_
/*
  fsm
*/
#include <iostream>
#include "vgl_rtree.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifdef DEBUG
#define trace(str) { std::cerr << str << std::endl; }
#else
#define trace(str)
#endif

//--------------------------------------------------------------------------------

template <class V, class B, class C>
vgl_rtree_node<V, B, C>::vgl_rtree_node(node *parent_node, V const &v)
  : parent(parent_node)
  //
  , total_vts(1)
  , local_vts(1)
  //
  , total_chs(1)
  , local_chs(0)
{
  C::init(bounds, v);
  vts[0] = v;
}

template <class V, class B, class C>
vgl_rtree_node<V, B, C>::~vgl_rtree_node()
{
  parent = nullptr;
  for (unsigned int i=0; i<local_chs; ++i)
    delete chs[i];
}

template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::update_total_vts(int diff)
{
  for (node *p=this; p; p=p->parent)
    p->total_vts += diff;
}

template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::update_total_chs(int diff)
{
  for (node *p=this; p; p=p->parent)
    p->total_chs += diff;
}

template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::update_vertex_count(int diff)
{
  local_vts += diff;
  update_total_vts(diff);
}

template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::update_child_count(int diff)
{
  local_chs += diff;
  update_total_chs(diff);
}

//--------------------------------------------------------------------------------

// look for a vertex which compares equal to the given one.
// if found, return true and place the node and index in
// the given locations. else return false.
template <class V, class B, class C>
bool vgl_rtree_node<V, B, C>::find(V const &v, node **n, int *i) const
{
  B tmp;
  C::init(tmp, v);
  return find(tmp, v, n, i);
}

template <class V, class B, class C>
bool vgl_rtree_node<V, B, C>::find(B const &b, V const &v, node **n_out, int *i_out) const
{
  if (C::meet(b, bounds)) {
    // check if it is one of the vertices in this node.
    for (unsigned int i=0; i<local_vts; ++i) {
      if (vts[i] == v) {
        *n_out = const_cast<node*>(this);
        *i_out = i;
        return true;
      }
    }
    // if not, try the child nodes.
    for (unsigned int i=0; i<local_chs; ++i)
      if (chs[i]->find(b, v, n_out, i_out))
        return true;
    return false;
  }
  //
  else
    return false;
}

template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::print() const
{
  std::cout << "node bounds: ";
  bounds.print(std::cout);
  std::cout << "\n--------";
  for (unsigned int i=0; i<local_chs; ++i) {
    std::cout << "\n\t";
    chs[i]->print();
  }
  std::cout << "------------" << std::endl;
}

//--------------------------------------------------------------------------------

// add a vertex to the tree, returning the node into
// which it was placed.
template <class V, class B, class C>
vgl_rtree_node<V, B, C> *vgl_rtree_node<V, B, C>::add(V const &v)
{
  // if there is room on this node for another vertex, do that :
  if (local_vts < vgl_rtree_MAX_VERTICES) {
    vts[local_vts++] = v;
    update_total_vts(1);

    C::update(bounds, v);
    for (node *p=parent; p; p=p->parent)
      p->compute_bounds();

    return this;
  }

  // if there is room on this node for add another child, do that :
  if (local_chs < vgl_rtree_MAX_CHILDREN) {
    node *nn = new node(this, v);

    chs[local_chs++] = nn;
    update_total_chs(1);
    update_total_vts(1);

    C::update(bounds, v);
    for (node *p=parent; p; p=p->parent)
      p->compute_bounds();

    return nn;
  }

  // all full up, so add the vertex to a suitable child.
  node *child = nullptr;
#if 0
  // get the smallest subtree :
  child = chs[0];
  for (unsigned int i=0; i<local_chs; ++i)
    if (chs[i]->total_vts < child->total_vts)
      child = chs[i];
#else
  { // get the subtree which needs the least enlargement :
    float cost = 0;
    int best = -1;
    for (unsigned int i=0; i<local_chs; ++i) {
      B tmp(chs[i]->bounds);
      C::update(tmp, v);
      float dd = C::volume(tmp) - C::volume(chs[i]->bounds);
      if (best==-1 || dd<cost) {
        cost = dd;
        best = i;
      }
    }
    child = chs[best];
  }
#endif
  assert(child);
  return child->add(v);
}

// remove the ith element from the node.
template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::erase(unsigned int i)
{
  assert(i<local_vts);

  if (total_vts > 1) { // there are other vertices.

    // decrease vertex counts.
    --local_vts;
    update_total_vts(-1);

    // move top element down to position i.
    if (i != local_vts)
      vts[i] = vts[local_vts];

    for (node *p = this; p; p=p->parent)
      p->compute_bounds();
  }

  else { // it's the only vertex in this node and below.

    // decrease vertex counts.
    --local_vts;
    update_total_vts(-1);

    // this node is now empty, so attempt some pruning.
    if (parent) {
      trace("prune");

      // move upwards as far as we need to prune. we can only prune
      // a node if it has total_vts equal to zero and has a parent.
      node *n = this;
      while (n->parent && (n->parent->parent && n->parent->total_vts==0))
        n = n->parent;

      // get pointer to parent.
      node *p = n->parent;

      // find out what index n has in p :
      unsigned int j = n->find_index_in_parent();
      assert(n == p->chs[j]);

      // update the node counts in p :
      p->update_total_chs(- (int)n->total_chs);

      -- p->local_chs;

      // move top child down to position j.
      if (p->local_chs != j)
        p->chs[j] = p->chs[p->local_chs];

      // delete the node.
      delete n; n=nullptr;

      // recompute the bounding boxes all the way up to the root.
      for (node *t = p; t; t=t->parent)
        t->compute_bounds();
    }
  }
}

//--------------------------------------------------------------------------------

template <class V, class B, class C>
unsigned int vgl_rtree_node<V, B, C>::find_index_in_parent() const
{
  assert(parent);
  for (unsigned int i=0; i<parent->local_chs; ++i)
    if (parent->chs[i] == this)
      return i;
  assert(!"this not found in parent");
  return (unsigned int)(-1);
}

// recompute the bounds of this node, using the vertices on
// the node and the bounds of the children. non-recursive.
template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::compute_bounds()
{
  if (local_vts>0) {
    C::init(bounds, vts[0]);
    for (unsigned int i=1; i<local_vts; ++i)
      C::update(bounds, vts[i]);
    for (unsigned i=0; i<local_chs; ++i)
      C::update(bounds, chs[i]->bounds );
  }
  else if (local_chs>0) {
    bounds = chs[0]->bounds;
    for (unsigned int i=1; i<local_chs; ++i)
      C::update(bounds, chs[i]->bounds );
  }
  else {
    assert(!"This cannot happen. this node should be pruned.");
  }
}

//--------------------------------------------------------------------------------

// this is a special case of the probe version.
// calls only itself.
template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::get(B const &region, std::vector<V> &vs) const
{
#ifdef DEBUG
  std::cout << " In get: " << region << std::endl;
#endif // DEBUG
  // get vertices from this node :
  for (unsigned int i=0; i<local_vts; ++i)
    if (C::meet(region, vts[i] )) {
#ifdef DEBUG
      std::cout << " pushed " << vts[i] << std::endl;
#endif // DEBUG
      vs.push_back(vts[i]);
    }

  // get vertices from children :
  for (unsigned int i=0; i<local_chs; ++i)
    if (C::meet(region, chs[i]->bounds )) {
#ifdef DEBUG
      std::cout << "--- region of child: " << i << " :" << chs[i]->bounds << " met search region----\n";
#endif // DEBUG
      chs[i]->get(region, vs);
    }
}

// calls only itself.
template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::get(vgl_rtree_probe<V, B, C> const &region, std::vector<V> &vs) const
{
  // get vertices from this node :
  for (unsigned int i=0; i<local_vts; ++i)
    if (region.meets( vts[i] ))
      vs.push_back(vts[i]);

  // get vertices from children :
  for (unsigned int i=0; i<local_chs; ++i)
    if (region.meets( chs[i]->bounds ))
      chs[i]->get(region, vs);
}

template <class V, class B, class C>
void vgl_rtree_node<V, B, C>::get_all(std::vector<V> &vs) const
{
  vs.reserve(vs.size() + total_vts);

  for (unsigned int i=0; i<local_vts; ++i)
    vs.push_back(vts[i]);

  for (unsigned int i=0; i<local_chs; ++i)
    chs[i]->get_all(vs);
}

//--------------------------------------------------------------------------------
// ITERATORS

template <class V, class B, class C>
void vgl_rtree_iterator_base<V, B, C>::operator_pp()
{
  if (!current)
    return;

  ++i; // class member!
  if (i < current->local_vts)
    return;

  if (current->local_chs > 0) {
    // descend to first child.
    current = current->chs[0];
    i = 0;
    return;
  }

  // backtrack :
  unsigned int j;
  node *n;
  node *p;
 again:
  n = current;
  p = current->parent;

  if (!p) { // reached the end
    current = nullptr;
    return;
  }

  // find index j of n in p :
  j = n->find_index_in_parent();

  ++j;
  if (j<p->local_chs) {
    // go to next child of p
    current = p->chs[j];
    i = 0;
    return;
  }

  // no more children in p.
  current = p;
  goto again;
}

template <class V, class B, class C>
void vgl_rtree_iterator_base<V, B, C>::operator_mm()
{
  assert(!"not implemented");
}

template <class V, class B, class C>
bool operator==(vgl_rtree_iterator_base<V, B, C> const &a,
                vgl_rtree_iterator_base<V, B, C> const &b)
{
  if (a.current || b.current)
    return (a.current==b.current) && (a.i == b.i);
  else
    return true; // both "at end"
}

//--------------------------------------------------------------------------------

#define VGL_RTREE_INSTANTIATE_tagged(tag, V, B, C) \
template class vgl_rtree_probe<V, B, C >; \
template class vgl_rtree_node<V, B, C >; \
template class vgl_rtree_iterator_base<V, B, C >; \
typedef vgl_rtree_iterator_base<V, B, C > itVBC##tag; \
template bool operator==(itVBC##tag const &, itVBC##tag const &); \
/*template bool operator!=(itVBC##tag const &, itVBC##tag const &) ; */ \
template class vgl_rtree_iterator<V, B, C >; \
template class vgl_rtree_const_iterator<V, B, C >; \
template class vgl_rtree<V, B, C >

// the __LINE__ tag gets expanded here
#define VGL_RTREE_INSTANTIATE_expand(tag, V, B, C) \
VGL_RTREE_INSTANTIATE_tagged(tag, V, B, C)

#undef VGL_RTREE_INSTANTIATE
#define VGL_RTREE_INSTANTIATE(V, B, C) \
VGL_RTREE_INSTANTIATE_expand(__LINE__, V, B, C)

#endif
