//:
// \file
// \brief Randomly select n from m integers without replacement
// \author Tim Cootes

#include <mbl/mbl_random_n_from_m.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

mbl_random_n_from_m::mbl_random_n_from_m()
{
}

void mbl_random_n_from_m::reseed(long new_seed)
{
  mz_random_.reseed(new_seed);
}

//: Select n integers from range [0,m-1], without replacement
void mbl_random_n_from_m::choose_n_from_m(vcl_vector<int>& choice, int n, int m)
{
  if ((n>m) | (n<0) | (m<0))
  {
    vcl_cerr<<"mbl_random_n_from_m::chooseNfromM : Can't choose "<<n;
    vcl_cerr<<" different integers from "<<m<<vcl_endl;
    vcl_abort();
  }

  if (choice.size()!=n) choice.resize(n);

  int i,j,k1,k2;

  if (used_.size()!=m) used_.resize(m);

  int* used_data = &used_[0];
  int* choice_data = &choice[0];

  for (i=0;i<m;i++) used_[i] = 0;

  for (i=0;i<n;i++)
  {
    j = mz_random_.lrand32(m-i-1);  // Select a random integer in
                    // a reducing range
    // Find the j'th un-used integer
    k1=0;
    k2=-1;
    while (k1<=j)
    {
      // Move k2 to next unused
      k2++;
      while (used_data[k2])
      {
        k2++;
      }

      k1++;
    }

    choice_data[i] = k2;
    used_data[k2] = 1;
  }
}
  
//: Select n integers from range [0,m-1], without replacement
void mbl_random_n_from_m::choose_n_from_m(vcl_vector<int>& chosen,
            vcl_vector<int>& not_chosen, int n, int m)
{
  choose_n_from_m(chosen,n,m);

  // The used array contains details of choice.
  int n1 = m-n;
  if (not_chosen.size()!=n1) not_chosen.resize(n1);

  int i;
  int j=0;

  for (i=0;i<m;i++)
  {
    if (!used_[i])
    {
      not_chosen[j]=i;
      j++;
    }
  }
}

