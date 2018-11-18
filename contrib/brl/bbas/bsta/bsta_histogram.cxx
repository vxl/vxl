#include <iostream>
#include "bsta_histogram.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <>
void bsta_histogram<char>::pretty_print(std::ostream& os) const
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
  for (char count : counts_)
    os << (int)count << ' ';

}
