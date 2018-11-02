//:
// \file
// \brief A class which returns an N element subset of the integers [0..M-1]
// \author Tim Cootes

#include <iostream>
#include "mbl_select_n_from_m.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

mbl_select_n_from_m::mbl_select_n_from_m()
 : n_(0), m_(0), is_done_(false), use_random_(false)
{
  random_.reseed(123746);
}

mbl_select_n_from_m::mbl_select_n_from_m(unsigned int new_n, unsigned int new_m)
 : n_(new_n), m_(new_m), is_done_(false), use_random_(false)
{
  set_n_m(new_n,new_m);
  random_.reseed(123746);
}

//: Reseed randomiser
void mbl_select_n_from_m::reseed(long s)
{
  random_.reseed(s);
}

  // member functions

void mbl_select_n_from_m::set_n_m(unsigned int new_n, unsigned int new_m)
{
  if (new_n>new_m)
  {
    std::cerr<<"mbl_select_n_from_m::set_n_m(): Warning: N>M: nothing selected\n"
            <<"N="<<new_n<<"  M="<<new_m<<std::endl;
    is_done_ = true;
  }

  n_=new_n;
  m_=new_m;
  index_.resize(n_);
  reset();
}

void mbl_select_n_from_m::use_random(bool flag)
{
  use_random_=flag;

  if (use_random_)
    random_.choose_n_from_m(index_,not_index_,n_,m_);
  else
    reset();
}

bool mbl_select_n_from_m::reset()
{
  is_done_ = (n_>m_);

  if (is_done_)
    return false;

  if (use_random_)
  {
    random_.choose_n_from_m(index_,not_index_,n_,m_);
    return true;
  }

  for (unsigned int i=0;i<n_;++i)
    index_[i]=i;

  return true;
}

bool mbl_select_n_from_m::next()
{
  if (is_done_) return false;

  if (use_random_)
  {
    random_.choose_n_from_m(index_,not_index_,n_,m_);
    return true;
  }

  // quick return if possible; ensures n_-1 >= 0 further down (which is used!)
  if (n_==m_ || n_ == 0) { is_done_=true; return false; }

  int* index_data = &index_[0];
  index_data[n_-1]++;  // Increment counter

  // Increment previous digit if current one has tripped over limit
  unsigned int low_trip_digit = n_;  // Lowest digit which has tripped
  int start_index=0;                 // index to start digits when trip occurs
  for (unsigned int j=n_-1;j>0;--j)
  {
    // index_[j] has range j..m-n+j
    if (index_data[j]>int(m_-n_+j))
    {
      index_data[j-1]++;
      low_trip_digit=j;
      start_index=index_data[j-1]+1;
      // Start one above previous digit value
    }
  }

  if (index_data[0]>int(m_-n_))
  {
    reset(); // to ensure valid data in index_
    is_done_=true;
    return false;
  }
  else
  if (low_trip_digit<n_)
  {
    // Reset those above
    for (unsigned int i=low_trip_digit;i<n_;i++)
    {
      index_data[i]=start_index;
      start_index++;
    }
  }

  return true;
}

const std::vector<int>& mbl_select_n_from_m::subset() const
{
  assert (!is_done_);
  return index_;
}

const std::vector<int>& mbl_select_n_from_m::complement()
{
  if (use_random_)
    return not_index_;
    // not_index_ calculated during call to next();

  assert (!is_done_);
  if (not_index_.size()!=m_-n_) not_index_.resize(m_-n_);

  // Fill not_index_ with values in range 0..m-1 not in index_
  // Use fact that index_[i+1]>index_[i]
  unsigned int j=0;  //index in not_index_ array
  unsigned int k=0;
  for (unsigned int i=0;i<n_;i++)
  {
    unsigned int ind_i = index_[i];
    while (k<ind_i)
    {
      not_index_[j]=k;
      j++;
      k++;
    }
    ++k;
  }

  // Fill out end of array
  while (k<m_)
  {
    not_index_[j]=k;
    j++;
    k++;
  }

  return not_index_;
}
