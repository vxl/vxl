// bcal_euclidean_transformation.cpp: implementation of the bcal_euclidean_transformation class.
//
//////////////////////////////////////////////////////////////////////

#include "bcal_euclidean_transformation.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bcal_euclidean_transformation::bcal_euclidean_transformation()
{
}

bcal_euclidean_transformation::~bcal_euclidean_transformation()
{
}

void bcal_euclidean_transformation::set_beat(vcl_vector<double> const& new_beat)
{
  vcsl_spatial_transformation::set_beat(new_beat);

  // allocate new space for data
  this->remove();
  int nBeats = new_beat.size();
  trans_.resize(nBeats);
}

int bcal_euclidean_transformation::remove()
{
  trans_.clear();
  return 0;
}

bool bcal_euclidean_transformation::is_invertible(double time) const
{
  assert(valid_time(time));
  return true;
}

vnl_vector<double> bcal_euclidean_transformation::execute(const vnl_vector<double> &v, double /* tims */) const
{
  // require not completed yet
  assert(is_valid());
  assert(v.size()==3);
  assert(v[2] == 1);

  return v;
}


vnl_vector<double> bcal_euclidean_transformation::inverse(const vnl_vector<double> &v, double /* time */) const
{
  // not finished yet
  vcl_cerr<<"\n bcal_euclidean_transformation::inverse() ----- not finished yet\n";
  return v;
}

void bcal_euclidean_transformation::set_transformations(vcl_vector<vgl_h_matrix_3d<double> > &trans)
{
  trans_ = trans; 
}


void bcal_euclidean_transformation::print(vcl_ostream& os)
{
  int size = trans_.size();
  for (int i=0; i<size; i++){
    os << "\n the "<<i<<"-th transformation is: \n"<< trans_[i];
  }
}
