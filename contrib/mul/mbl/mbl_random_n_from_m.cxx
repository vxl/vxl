//:
// \file
// \brief Randomly select n from m integers without replacement
// \author Tim Cootes

#include <iostream>
#include <cstdlib>
#include "mbl_random_n_from_m.h"
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

mbl_random_n_from_m::mbl_random_n_from_m() = default;

void mbl_random_n_from_m::reseed(long new_seed)
{
  mz_random_.reseed(new_seed);
}

//: Select n integers from range [0,m-1], without replacement
void mbl_random_n_from_m::choose_n_from_m(std::vector<unsigned>& choice,
                                          unsigned int n, unsigned int m)
{
  if (n>m)
  {
    std::cerr<<"mbl_random_n_from_m::choose_n_from_m() : Can't choose "<<n
            <<" different integers from "<<m<<std::endl;
    std::abort();
  }

  if (choice.size()!=n) choice.resize(n);

  if (used_.size()<m) used_.resize(m);

  for (unsigned int i=0;i<m;i++) used_[i] = false;

  for (unsigned int i=0;i<n;i++)
  {
    // Select a random integer in a reducing range
    int j = mz_random_.lrand32(m-i-1);
    // Find the j'th un-used integer
    int k2=-1;
    for (int k1=0; k1<=j; ++k1)
    {
      // Move k2 to next unused
      ++k2;
      while (used_[k2])
        ++k2;
    }

    choice[i] = k2;
    used_[k2] = true;
  }
}

//: Select n integers from range [0,m-1], without replacement
void mbl_random_n_from_m::choose_n_from_m(std::vector<unsigned>& chosen,
                                          std::vector<unsigned>& not_chosen,
                                          unsigned int n, unsigned int m)
{
  choose_n_from_m(chosen,n,m);

  // The used array contains details of choice.
  unsigned int n1 = m-n; // n is guaranteed <= m
  if (not_chosen.size()!=n1) not_chosen.resize(n1);

  for (unsigned int i=0,j=0;i<m;i++)
    if (!used_[i])
      not_chosen[j++]=i;
}

//: Select n integers from range [0,m-1], without replacement
void mbl_random_n_from_m::choose_n_from_m(std::vector<int>& choice,
                                          unsigned int n, unsigned int m)
{
  if (n>m)
  {
    std::cerr<<"mbl_random_n_from_m::choose_n_from_m() : Can't choose "<<n
            <<" different integers from "<<m<<std::endl;
    std::abort();
  }

  if (choice.size()!=n) choice.resize(n);

  if (used_.size()<m) used_.resize(m);

  for (unsigned int i=0;i<m;i++) used_[i] = false;

  for (unsigned int i=0;i<n;i++)
  {
    // Select a random integer in a reducing range
    int j = mz_random_.lrand32(m-i-1);
    // Find the j'th un-used integer
    int k2=-1;
    for (int k1=0; k1<=j; ++k1)
    {
      // Move k2 to next unused
      ++k2;
      while (used_[k2])
        ++k2;
    }

    choice[i] = k2;
    used_[k2] = true;
  }
}

//: Select n integers from range [0,m-1], without replacement
void mbl_random_n_from_m::choose_n_from_m(std::vector<int>& chosen,
                                          std::vector<int>& not_chosen,
                                          unsigned int n, unsigned int m)
{
  choose_n_from_m(chosen,n,m);

  // The used array contains details of choice.
  unsigned int n1 = m-n; // n is guaranteed <= m
  if (not_chosen.size()!=n1) not_chosen.resize(n1);

  for (unsigned int i=0,j=0;i<m;i++)
    if (!used_[i])
      not_chosen[j++]=i;
}

