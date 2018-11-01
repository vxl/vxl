// This is oxl/mvl/PairMatchMulti.cxx
//:
//  \file

#include <iostream>
#include <fstream>
#include <utility>
#include "PairMatchMulti.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_sparse_array_2d.h>
#include <vnl/vnl_matrix.h>
#include <mvl/PairMatchSet.h>

typedef vbl_sparse_array_2d<double> hack ;

vcl_multimap_uint_uint::iterator vcl_multimap_uint_uint::insert(unsigned key, unsigned value)
{
  return base::insert(std::pair<const unsigned, unsigned>(key, value));
}

void vcl_multimap_uint_uint::clear() { base::erase(begin(), end()); }

//: Default constructor
PairMatchMulti::PairMatchMulti()
{
  scores_ = nullptr;
}

// - Construct and load matches (via operator>>) from std::istream.
PairMatchMulti::PairMatchMulti(std::istream& f)
{
  scores_ = nullptr;
  f >> *this;
}

//: Copy ctor
PairMatchMulti::PairMatchMulti(const PairMatchMulti& that)
{
  scores_ = nullptr;
  operator=(that);
}

//: Assignment
PairMatchMulti& PairMatchMulti::operator=(const PairMatchMulti& that)
{
  matches12_ = that.matches12_;
  delete scores_; scores_ = nullptr;
  if (that.scores_)
    scores_ = new vbl_sparse_array_2d<double>(*that.scores_);
  return *this;
}

//: Destructor
PairMatchMulti::~PairMatchMulti()
{
  delete scores_; scores_ = nullptr;
}

void PairMatchMulti::add_match(int i1, int i2, double score)
{
  add_match(i1, i2);
  set_score(i1, i2, score);
}

void PairMatchMulti::set_score(int i1, int i2, double score)
{
  if (!scores_)
    scores_ = new vbl_sparse_array_2d<double>;

  scores_->put(i1, i2, score);
}

bool PairMatchMulti::contains(int i1, int i2) const
{
  for (auto p = matches12_.lower_bound(i1); p != matches12_.upper_bound(i1); ++p)
    if ((*p).second == (unsigned)i2)
      return true;
  return false;
}

double PairMatchMulti::get_score(int i1, int i2) const
{
  if (scores_ == nullptr)
    return -1.0;

  double* p = scores_->get_addr(i1, i2);
  if (p == nullptr)
    return -1.0;

  return *p;
}

std::ostream& operator << (std::ostream& s, const PairMatchMulti& pm)
{
  for (PairMatchMultiIterator p(pm); p; ++p) {
    int i1 = p.get_i1();
    int i2 = p.get_i2();
    double score = pm.get_score(i1, i2);

    s << i1 << ' ' << i2;
    if (score != -1)
      s << "   " << score;
    s << std::endl;
  }
  return s;
}

static int dbl2int(double d)
{
  int i = (int)d;
  if ((double)i != d)
    std::cerr << "PairMatchMulti: WARNING: saw double " << d << ", expected int.\n";
  return i;
}

std::istream& operator >> (std::istream& s, PairMatchMulti& pm)
{
  // why do we bother trying to do things properly...
  pm.read_ascii(s);
  //if (!pm.read_ascii(s)) s.setstate(ios::failbit);
  return s;
}

bool PairMatchMulti::read_ascii(std::istream& s)
{
  vnl_matrix<double> m;
  s >> m;
  if (!(s.good() || s.eof())) {
    std::cerr << "PairMatchMulti load failed\n";
    return false;
  }

  int cols = m.columns();
  if (cols != 2 && cols != 3) {
    std::cerr << "PairMatchMulti load failed: Saw " << cols << " data per line, expected 2 or 3\n";
    return false;
  }


  int n = m.rows();
  for (int i = 0; i < n; ++i) {
    int i1 = dbl2int(m(i,0));
    int i2 = dbl2int(m(i,1));
    if (cols == 3) {
      double score = m(i,2);
      add_match(i1, i2, score);
    } else
      add_match(i1, i2, 0.0);
  }
  assert(count() == n);

  // std::cerr << "PairMatchMulti load: read " << count() << " matches\n";

  return true;
}

bool PairMatchMulti::is_superset(PairMatchSet& matches)
{
  bool allok = true;
  for (PairMatchSet::iterator p = matches; p; ++p) {
    int i1 = p.get_i1();
    int i2 = p.get_i2();
    bool ok = false;
    for (PairMatchMultiIterator i = get_match_12(i1); i; ++i)
      if (i.get_i2() == i2) {
        ok = true;
        break;
      }
    if (!ok) {
      allok = false;
      break;
    }
  }

  if (allok)
    return true;
  else {
    std::cerr << "PairMatchMulti::is_superset() -- it ain't\n";
    for (PairMatchSet::iterator p = matches; p; ++p) {
      int i1 = p.get_i1();
      int i2 = p.get_i2();
      std::cerr << i1 << ": [" << i2 << "] ";
      bool ok = false;
      for (PairMatchMultiIterator i = get_match_12(i1); i; ++i) {
        std::cerr << i.get_i2() << " ";
        if (i.get_i2() == i2) ok = true;
      }
      if (!ok)
        std::cerr << "!!!!!";
      std::cerr << std::endl;
    }
    return false;
  }
}

//: load from ascii file
bool PairMatchMulti::load(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good()) {
    std::cerr << "PairMatchMulti: Error opening " << filename << std::endl;
    return false;
  }
  f >> *this;
  return true;
}

#ifdef TEST_PairMatchMulti

main()
{
  PairMatchMulti mm;
  mm.add_match(1,2);
  mm.add_match(2,2);
  mm.add_match(1,3);
  mm.add_match(7,2);

  mm.add_match(1,4);
  mm.add_match(3,5);
  mm.add_match(3,2);

  std::cerr << "All matches, sorted:\n";
  for (PairMatchMulti::match_iterator p(mm); !p.done(); p.next())
    std::cerr << p.get_i1() << " " << p.get_i2() << std::endl;

  for (int target = 1; target <= 7; ++target) {
    std::cerr << "Matches for " << target << std::endl;
    for (PairMatchMulti::match_iterator p = mm.get_match_12(target); !p.done(); p.next())
      std::cerr << p.get_i1() << " " << p.get_i2() << std::endl;
  }
}
#endif
