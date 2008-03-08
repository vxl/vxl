// This is brl/bbas/bsta/io/bsta_io_histogram.h
#ifndef bsta_io_histogram_h_
#define bsta_io_histogram_h_
//:
// \file
// \brief Binary I/O for bsta_histogram 
// \author J.L. Mundy
// \date February 29, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/bsta_histogram_sptr.h>
#include <bsta/bsta_joint_histogram_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/io/vbl_io_array_2d.h>
//: Binary save bsta_histogram to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream &os, const bsta_histogram<T>& h)
{
  int nbins = h.nbins();
  T min = h.min();
  T max = h.max();
  T min_prob = h.min_prob();
  vcl_vector<T> values = h.value_array();
  vcl_vector<T> counts = h.count_array();
  vsl_b_write(os, nbins);
  vsl_b_write(os, min);
  vsl_b_write(os, max);
  vsl_b_write(os, min_prob);
  vsl_b_write(os, values);
  vsl_b_write(os, counts);
}

//: Binary load bsta_histogram from stream.
template <class T>
void
vsl_b_read(vsl_b_istream &is, bsta_histogram<T>& h)
{
  int nbins;
  T min, max, min_prob;
  vcl_vector<T> values, counts;
  vsl_b_read(is, nbins);
  vsl_b_read(is, min);
  vsl_b_read(is, max);
  vsl_b_read(is, min_prob);
  vsl_b_read(is, values);
  vsl_b_read(is, counts);
  bsta_histogram<T> temp(min, max, nbins, min_prob);
  for(unsigned i = 0; i<static_cast<unsigned>(nbins); ++i)
    temp.upcount(values[i], counts[i]);
  h = temp;
}

//: Print summary
template <class T>
void
vsl_print_summary(vcl_ostream &os, const bsta_histogram<T>& h)
{
  os << "bsta_histogram\n";
  h.print(os);
}

//: Binary save bsta_joint_histogram to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream &os, const bsta_joint_histogram<T>& h)
{
  int nbins = h.nbins();
  T range = h.range();
  T min_prob = h.min_prob();
  vbl_array_2d<T> counts = h.counts();
  vsl_b_write(os, nbins);
  vsl_b_write(os, range);
  vsl_b_write(os, min_prob);
  vsl_b_write(os, counts);
}

//: Binary load bsta_joint_histogram from stream.
template <class T>
void
vsl_b_read(vsl_b_istream &is, bsta_joint_histogram<T>& h)
{
  int nbins;
  T range, min_prob;
  vbl_array_2d<T> counts;
  vsl_b_read(is, nbins);
  vsl_b_read(is, range);
  vsl_b_read(is, min_prob);
  vsl_b_read(is, counts);
  bsta_joint_histogram<T> temp(range, nbins, min_prob);
  unsigned nr = counts.rows(), nc = counts.cols();
  for(unsigned r = 0; r<nr; ++r)
    for(unsigned c = 0; c<nc; ++c)
      temp.set_count(r, c, counts[r][c]);
  h = temp;
}

//: Print summary
template <class T>
void
vsl_print_summary(vcl_ostream &os, const bsta_joint_histogram<T>& h)
{
  os << "bsta_joint_histogram\n";
  h.print(os);
}

void vsl_b_write(vsl_b_ostream &os, const bsta_histogram_sptr& hptr)
{
  //not implemented yet
}

void vsl_b_read(vsl_b_istream &is, bsta_histogram_sptr& hptr)
{
  //not implemented yet
}

void vsl_b_write(vsl_b_ostream &os, const bsta_joint_histogram_sptr& hptr)
{
  //not implemented yet
}

void vsl_b_read(vsl_b_istream &is, bsta_joint_histogram_sptr& hptr)
{
  //not implemented yet
}

#endif // bsta_io_histogram_h_
