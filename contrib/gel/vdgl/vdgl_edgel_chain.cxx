#ifdef __GNUC__
#pragma implementation
#endif

#include "vdgl_edgel_chain.h"

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

bool vdgl_edgel_chain::set_edgel( const int index, const vdgl_edgel &e)
{
  if(( index< 0) && ( index>= es_.size()))
    return false;

  es_[index]= e;

  return true;
}

void vdgl_edgel_chain::notify_change()
{
  // let friends know that chain has changed
  vbl_timestamp::touch();
}

bool vdgl_edgel_chain::add_edgels( const vcl_vector<vdgl_edgel> &es, const int index)
{
  if(( index< 0) || ( index> es_.size()))
    return false;
  else if( es_.size()== 0)
    es_= es;
  else
    {
      vcl_vector<vdgl_edgel> temp;
      for( int i=0; i< index; i++)
        temp.push_back( es_[i]);

      for( int i=0; i< es.size(); i++)
        temp.push_back( es[i]);

      for( int i=index; i< es_.size(); i++)
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
  for( int i=0; i< p.es_.size(); i++)
    {
      s << p.es_[i];
      if( i!= (p.es_.size()-1))
        s << ", ";
    }

  return s << ")";
}

