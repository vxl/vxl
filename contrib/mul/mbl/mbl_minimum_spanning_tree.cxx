#include "mbl_minimum_spanning_tree.h"
//:
// \file
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h> // for vnl_matrix<double>::get_row()
#include <vcl_algorithm.h>

//: Select the smallest pair s.t. first is in \param a, second in \param b
static vcl_pair<unsigned,unsigned> mbl_mst_next_pair(
                                           const vnl_matrix<double>& D,
                                           const vcl_vector<unsigned>& a,
                                           const vcl_vector<unsigned>& b)
{
  vcl_pair<unsigned,unsigned> p;
  double min_sim = 9.9e9;
  for (unsigned i=0; i<a.size(); ++i)
    for (unsigned j=0; j<b.size(); ++j)
    {
      double s = D(a[i],b[j]);
      if (s<min_sim)
      {
        min_sim=s;
        p.first=a[i];
        p.second=b[j];
      }
    }
  return p;
}

//: Compute the minimum spanning tree given a distance matrix
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vnl_matrix<double>& D,
                               vcl_vector<vcl_pair<int,int> >& pairs)
{
  unsigned n = D.rows();
  vcl_vector<unsigned> a(0),b(n);
  for (unsigned i=0;i<n;++i) b[i]=i;
  // Select element closest to all others on average
  double min_sum=9e9;
  unsigned best_i=0;
  for (unsigned i=0;i<n;++i)
  {
    double sum = D.get_row(i).sum();
    if (sum<min_sum) { min_sum=sum; best_i=i; }
  }
  b.erase(vcl_find(b.begin(),b.end(),best_i));
  a.push_back(best_i);

  for (unsigned i=1;i<n;++i)
  {
    vcl_pair<unsigned,unsigned> p = mbl_mst_next_pair(D,a,b);
    pairs.push_back(p);
    b.erase(vcl_find(b.begin(),b.end(),p.second));
    a.push_back(p.second);
  }
}

//: Compute the minimum spanning tree of given points
// \param  pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vcl_vector<vgl_point_2d<double> >& pts,
                               vcl_vector<vcl_pair<int,int> >& pairs)
{
  unsigned n=pts.size();
  vnl_matrix<double> D(n,n);
  for (unsigned i=0;i<n;++i) D(i,i)=0.0;
  for (unsigned i=1;i<n;++i)
    for (unsigned j=0;j<i;++j)
    {
      D(i,j) = (pts[i]-pts[j]).length();
      D(j,i) = D(i,j);
    }

  mbl_minimum_spanning_tree(D,pairs);
}

//: Compute the minimum spanning tree of given points
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vcl_vector<vgl_point_3d<double> >& pts,
                               vcl_vector<vcl_pair<int,int> >& pairs)
{
  unsigned n=pts.size();
  vnl_matrix<double> D(n,n);
  for (unsigned i=0;i<n;++i) D(i,i)=0.0;
  for (unsigned i=1;i<n;++i)
    for (unsigned j=0;j<i;++j)
    {
      D(i,j) = (pts[i]-pts[j]).length();
      D(j,i) = D(i,j);
    }

  mbl_minimum_spanning_tree(D,pairs);
}
