#include "bsta_histogram.h"
#include <vcl_iostream.h>

template <>
void bsta_histogram<char>::pretty_print(vcl_ostream& os) const
{
  os << "area valid: " << area_valid_ << "\n"
  << "area: " << (int)area_ << "\n"
  << "number of bins: " <<  nbins_ << "\n"
  << "range: " << (int)range_ << "\n"
  << "delta: " << (int)delta_ << "\n"
  << "min_prob: " << (int)min_prob_ << "\n"
  << "min: " << (int)min_ << "\n"
  << "max: " << (int)max_ << "\n" 
  << "counts: ";
  for (unsigned i = 0; i < counts_.size() ; i++)
    os << (int)counts_[i] << ' ';
  
}

