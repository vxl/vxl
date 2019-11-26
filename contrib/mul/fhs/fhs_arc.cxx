#include <iostream>
#include <algorithm>
#include "fhs_arc.h"
//:
// \file
// \author Tim Cootes
// \brief Link between one node and another

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

    //: Write to binary stream
void fhs_arc::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,i_);
  vsl_b_write(bfs,j_);
  vsl_b_write(bfs,dx_);
  vsl_b_write(bfs,dy_);
  vsl_b_write(bfs,var_x_);
  vsl_b_write(bfs,var_y_);
}

//: Read from binary stream
void fhs_arc::b_read(vsl_b_istream& bfs)
{
  vsl_b_read(bfs,i_);
  vsl_b_read(bfs,j_);
  vsl_b_read(bfs,dx_);
  vsl_b_read(bfs,dy_);
  vsl_b_read(bfs,var_x_);
  vsl_b_read(bfs,var_y_);
}

//: Print
std::ostream& operator<<(std::ostream& os, const fhs_arc& a)
{
  os<<'('<<a.i()<<"->"<<a.j()<<" Offset: ("<<a.dx()<<','<<a.dy()
    <<") var: ("<<a.var_x()<<','<<a.var_y()<<')';
  return os;
}

//: Print set
std::ostream& operator<<(std::ostream& os, const std::vector<fhs_arc>& arc)
{
  os<<arc.size()<<" arcs:"<<'\n';
  for (unsigned i=0;i<arc.size();++i)
    os<<i<<") "<<arc[i]<<'\n';
  return os;
}

//: Print
void vsl_print_summary(std::ostream& os, const fhs_arc& a)
{
  os<<a;
}

//: Find children of node p_node
//  Add relevant arcs to new_arc, and fill children list
static void fhs_find_children(const std::vector<fhs_arc>& arc0,
                         std::vector<bool>& used,
                         std::vector<fhs_arc>& new_arc,
                         std::vector<unsigned>& children,
                         unsigned p_node)
{
  children.resize(0);
  for (unsigned i=0;i<arc0.size();++i)
  {
    if (used[i]) continue;
    if (arc0[i].i()==p_node)
    {
      new_arc.push_back(arc0[i]);
      used[i]=true;
      children.push_back(arc0[i].j());
    }
    else
    if (arc0[i].j()==p_node)
    {
      new_arc.push_back(arc0[i].flipped());
      used[i]=true;
      children.push_back(arc0[i].i());
    }
  }
}

//: Re-order list of arcs so that parents precede their children
//  Assumes that there are n nodes (indexed 0..n-1),
//  thus n-1 arcs defining a tree.
//  On exit children[i] gives list of children of node i
bool fhs_order_tree_from_root(const std::vector<fhs_arc>& arc0,
                         std::vector<fhs_arc>& new_arc,
                         std::vector<std::vector<unsigned> >& children,
                         unsigned new_root)
{
  // Number of nodes is one more than number of arcs for a tree
  unsigned n=arc0.size()+1;
  // Check that all nodes are in range [0,n-1]
  for (const auto & i : arc0)
    if (i.i()>=n || i.j()>=n)
    {
      std::cerr<<"Arc index outside range [0,"<<n-1<<']'<<'\n'
              <<"Arc = "<<i<<'\n';
      return false;
    }

  children.resize(n);
  for (unsigned i=0;i<n;++i) children[i].resize(0);

  std::vector<bool> used(n);
  std::fill(used.begin(),used.end(),false);

  new_arc.resize(0);

  // Find children of root node
  fhs_find_children(arc0,used,new_arc,children[new_root],new_root);

  // Now find children of each child node recursively
  unsigned p=0;
  while (p<new_arc.size() && new_arc.size()!=(n-1))
  {
    unsigned p_node = new_arc[p].j();
    fhs_find_children(arc0,used,new_arc,children[p_node],p_node);
    p++;
  }

  return new_arc.size()==arc0.size();
}
