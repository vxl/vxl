#ifdef __GNUC__
#pragma implementation
#endif

#include "vdgl_edgel_chain.h"
#include <vgl/vgl_distance.h>
#include <vcl_cassert.h>

vdgl_edgel_chain::vdgl_edgel_chain()
{
}

vdgl_edgel_chain::vdgl_edgel_chain( const vcl_vector<vdgl_edgel> edgels)
  : es_( edgels)
{
}

vdgl_edgel_chain::~vdgl_edgel_chain()
{
}


bool vdgl_edgel_chain::add_edgel( const vdgl_edgel &e)
{
  es_.push_back( e);

  // let friends know that chain has changed
  notify_change();

  return true;
}

bool vdgl_edgel_chain::set_edgel( int index, const vdgl_edgel &e)
{
  assert(index>=0);
  if ( (unsigned int)index >= es_.size())
    return false;

  es_[index]= e;

  return true;
}

void vdgl_edgel_chain::notify_change()
{
  // let friends know that chain has changed
  vul_timestamp::touch();
}

bool vdgl_edgel_chain::add_edgels( const vcl_vector<vdgl_edgel> &es, int index)
{
  assert(index>=0);
  if ( (unsigned int)index> es_.size())
    return false;
  else if (es_.size()== 0)
    es_= es;
  else
    {
      vcl_vector<vdgl_edgel> temp;
      for (int i=0; i< index; i++)
        temp.push_back( es_[i]);

      for (unsigned int i=0; i< es.size(); i++)
        temp.push_back( es[i]);

      for (unsigned int i=index; i< es_.size(); i++)
        temp.push_back( es_[i]);

      es_= temp;
    }

  // let friends know that chain has changed
  notify_change();

  return true;
}

vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel_chain& p)
{
  s << "<vdgl_edgel_chain (";
  if ( p.es_.size() > 0)
    s << p.es_[0];
  for (unsigned int i=1; i< p.es_.size(); ++i)
    s << ", " << p.es_[i];

  return s << ")";
}

inline static double sq_dist(vdgl_edgel const& e, double x, double y)
{
  double dx = e.get_x() - x, dy = e.get_y() - y;
  return dx*dx+dy+dy;
}

vdgl_edgel_chain_sptr vdgl_edgel_chain::extract_subchain(int start, int end)
{
  vcl_vector<vdgl_edgel> e;
  if (start < size() && end >= 0)
    for (int i=start; i<= end; ++i)
      e.push_back(edgel(i));
  return new vdgl_edgel_chain(e); // could be empty
}

bool vdgl_edgel_chain::split( double x, double y,
                              vdgl_edgel_chain_sptr &ec1,
                              vdgl_edgel_chain_sptr &ec2)
{
  int split_index = -1;
  const int n = size();
  double d = 1e10;
  for (int i=0;i+1<n;i++) {
    double x1=edgel(i  ).get_x(), y1=edgel(i  ).get_y(),
           x2=edgel(i+1).get_x(), y2=edgel(i+1).get_y();
    double e = vgl_distance2_to_linesegment(x1,y1,x2,y2,x,y);
    if (e < d) { d=e; split_index = i+1;}
  }

  if (split_index < 0) return false; // only happens with empty edgel_chain

  if (split_index == 1 && d == sq_dist(edgel(0),x,y)) split_index = 0;
  if (split_index == n-1 && d == sq_dist(edgel(n-1),x,y)) split_index = n;

  if (split_index > 0) ec1 = this->extract_subchain(0, split_index-1);
  if (split_index < n) ec2 = this->extract_subchain(split_index, n-1);
  return split_index > 0 && split_index < n;
}
