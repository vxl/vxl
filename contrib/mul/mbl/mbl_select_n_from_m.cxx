//:
// \file
// \brief A class which returns an N element subset of the integers [0..M-1]
// \author Tim Cootes

#include <mbl/mbl_select_n_from_m.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

mbl_select_n_from_m::mbl_select_n_from_m()
{
  n_=0;
  m_=0;
  is_done_=0;
  use_random_ = 0;
  random_.reseed(123746);
}

mbl_select_n_from_m::mbl_select_n_from_m(int new_n, int new_m)
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
void mbl_select_n_from_m::set_n_m(int new_n, int new_m)
{
  if (new_n>new_m)
  {
    vcl_cerr<<"mbl_select_n_from_m::set_n_m : N cannot be >M"<<vcl_endl;
    vcl_cerr<<"N="<<new_n<<"  M="<<new_m<<vcl_endl;
    vcl_abort();
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

void mbl_select_n_from_m::reset()
{
  if ((n_==0) || (m_==0))
  {
    vcl_cerr<<"mbl_select_n_from_m::reset() : N or M is zero."<<vcl_endl;
    vcl_abort();
  }

  if (use_random_)
  {
    random_.choose_n_from_m(index_,not_index_,n_,m_);
    return;
  }

  int i;
  for (i=0;i<n_;i++)
    index_[i]=i;

  if (n_==m_)  is_done_=true;
  else     is_done_=false;
  
}

bool mbl_select_n_from_m::next()
{
  if (is_done_) return false;

  if (use_random_)
  {
    random_.choose_n_from_m(index_,not_index_,n_,m_);
    return true;
  }

  if (n_==m_) return true;
  int i,j;
  int* index_data = &index_[0];

  index_data[n_-1]++;  // Increment counter

  // Increment previous digit if current one has tripped over limit
  int low_trip_digit = n_;  // Lowest digit which has tripped
  int start_index;    // index_ to start digits when trip occurs
  for (j=n_-1;j>0;j--)
  {
      // index_(j) has range j..m-n+j
    if (index_data[j]>(m_-n_+j))
    {
      index_data[j-1]++;
      low_trip_digit=j;
      start_index=index_data[j-1]+1;
        // Start one above previous digit value
    }
  }

  if (index_data[0]>(m_-n_))
  {
    reset();
    is_done_=true;
    return false;
  }
  else
  if (low_trip_digit<n_)
  {
    // Reset those above
    for (i=low_trip_digit;i<n_;i++)
    {
      index_data[i]=start_index;
      start_index++;
    }
  }

  return true;
}

bool mbl_select_n_from_m::is_done()
{
  return is_done_;
}

const vcl_vector<int>& mbl_select_n_from_m::subset()
{
  return index_;
}
const vcl_vector<int>& mbl_select_n_from_m::complement()
{
  if (use_random_)
    return not_index_;
    // not_index_ calculated during call to next();


  if (not_index_.size()!=m_-n_) not_index_.resize(m_-n_);

  // Fill not_index_ with values in range 0..m-1 not in index_
  // Use fact that index_(i+1)>index_(i)
  int i,ind_i,j,k;
  j=0;  //index_ in not_index_ array
  k=0;
  for (i=0;i<n_;i++)
  {
    ind_i = index_[i];
    while (k<ind_i)
    {
      not_index_[j]=k;
      j++;
      k++;
    }
    k=ind_i+1;
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

