// This is oxl/mvl/PairMatchMulti.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "PairMatchMulti.h"

#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vbl/vbl_sparse_array_2d.h>
#include <vnl/vnl_matrix.h>
#include <mvl/PairMatchSet.h>

typedef vbl_sparse_array_2d<double> hack ;

vcl_multimap_uint_uint::iterator vcl_multimap_uint_uint::insert(unsigned key, unsigned value)
{
  return base::insert(vcl_pair<const unsigned, unsigned>(key, value));
}

void vcl_multimap_uint_uint::clear() { base::erase(begin(), end()); }

//: Default constructor
PairMatchMulti::PairMatchMulti()
{
  _scores = 0;
}

// - Construct and load matches (via operator>>) from vcl_istream.
PairMatchMulti::PairMatchMulti(vcl_istream& f)
{
  _scores = 0;
  f >> *this;
}

//: Copy ctor
PairMatchMulti::PairMatchMulti(const PairMatchMulti& that)
{
  _scores = 0;
  operator=(that);
}

//: Assignment
PairMatchMulti& PairMatchMulti::operator=(const PairMatchMulti& that)
{
  _matches12 = that._matches12;
  delete _scores; _scores = 0;
  if (that._scores)
    _scores = new vbl_sparse_array_2d<double>(*that._scores);
  return *this;
}

//: Destructor
PairMatchMulti::~PairMatchMulti()
{
  delete _scores; _scores = 0;
}

void PairMatchMulti::add_match(int i1, int i2, double score)
{
  add_match(i1, i2);
  set_score(i1, i2, score);
}

void PairMatchMulti::set_score(int i1, int i2, double score)
{
  if (!_scores)
    _scores = new vbl_sparse_array_2d<double>;

  _scores->put(i1, i2, score);
}

bool PairMatchMulti::contains(int i1, int i2) const
{
  for (vcl_multimap_uint_uint::const_iterator p = _matches12.lower_bound(i1); p != _matches12.upper_bound(i1); ++p)
    if ((*p).second == (unsigned)i2)
      return true;
  return false;
}

double PairMatchMulti::get_score(int i1, int i2) const
{
  if (_scores == 0)
    return -1.0;

  double* p = _scores->get_addr(i1, i2);
  if (p == 0)
    return -1.0;

  return *p;
}

vcl_ostream& operator << (vcl_ostream& s, const PairMatchMulti& pm)
{
  for (PairMatchMultiIterator p(pm); p; ++p) {
    int i1 = p.get_i1();
    int i2 = p.get_i2();
    double score = pm.get_score(i1, i2);

    s << i1 << ' ' << i2;
    if (score != -1)
      s << "   " << score;
    s << vcl_endl;
  }
  return s;
}

static int dbl2int(double d)
{
  int i = (int)d;
  if ((double)i != d)
    vcl_cerr << "PairMatchMulti: WARNING: saw double " << d << ", expected int.\n";
  return i;
}

vcl_istream& operator >> (vcl_istream& s, PairMatchMulti& pm)
{
  // why do we bother trying to do things properly...
  pm.read_ascii(s);
  //if (!pm.read_ascii(s)) s.setstate(ios::failbit);
  return s;
}

bool PairMatchMulti::read_ascii(vcl_istream& s)
{
  vnl_matrix<double> m;
  s >> m;
  if (!(s.good() || s.eof())) {
    vcl_cerr << "PairMatchMulti load failed\n";
    return false;
  }

  int cols = m.columns();
  if (cols != 2 && cols != 3) {
    vcl_cerr << "PairMatchMulti load failed: Saw " << cols << " data per line, expected 2 or 3\n";
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

  // vcl_cerr << "PairMatchMulti load: read " << count() << " matches\n";

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
    vcl_cerr << "PairMatchMulti::is_superset() -- it ain't\n";
    for (PairMatchSet::iterator p = matches; p; ++p) {
      int i1 = p.get_i1();
      int i2 = p.get_i2();
      vcl_cerr << i1 << ": [" << i2 << "] ";
      bool ok = false;
      for (PairMatchMultiIterator i = get_match_12(i1); i; ++i) {
        vcl_cerr << i.get_i2() << " ";
        if (i.get_i2() == i2) ok = true;
      }
      if (!ok)
        vcl_cerr << "!!!!!";
      vcl_cerr << vcl_endl;
    }
    return false;
  }
}

//: load from ascii file
bool PairMatchMulti::load(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good()) {
    vcl_cerr << "PairMatchMulti: Error opening " << filename << vcl_endl;
    return false;
  }
  f >> *this;
  return true;
}

#ifdef TEST_PairMatchMulti
#include <vcl_iostream.h>

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

  vcl_cerr << "All matches, sorted:\n";
  for (PairMatchMulti::match_iterator p(mm); !p.done(); p.next())
    vcl_cerr << p.get_i1() << " " << p.get_i2() << vcl_endl;

  for (int target = 1; target <= 7; ++target) {
    vcl_cerr << "Matches for " << target << vcl_endl;
    for (PairMatchMulti::match_iterator p = mm.get_match_12(target); !p.done(); p.next())
      vcl_cerr << p.get_i1() << " " << p.get_i2() << vcl_endl;
  }
}
#endif
