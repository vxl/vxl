#ifndef PairMatchSet_h_
#define PairMatchSet_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	PairMatchSet - Set of pairs of integers
// .LIBRARY	MViewBasics
// .HEADER	MultiView Package
// .INCLUDE	mvl/PairMatchSet.h
// .FILE	PairMatchSet.cxx
//
// .SECTION Description
//    A PairMatchSet stores tuples of integers (i1, i2), stored as
//    an array of matches indexed by i1. Access characteristics are
//    therefore:
//
//    Indexing on i1 ("forward matches") is O(1)
//
//    Indexing on i2 ("backward matches") is O(n)
//
//    The first index must be unique (see PairMatchMulti to allow
//    multiple i2's for a single i1).  In general the i1's ought also
//    to be small, less than 1000, say.
//
//    A class PairMatchSet::iterator is provided to allow traversal of
//    the complete list of matches.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <mvl/MatchSet.h>

class PairMatchSet : public MatchSet {
  // Constants-----------------------------------------------------------------
public:
  enum { use_existing = true };
public:
  // Constructors/Destructors--------------------------------------------------

  PairMatchSet(unsigned size = 0);
  PairMatchSet(const PairMatchSet& that);
  PairMatchSet& operator=(const PairMatchSet& that);

  virtual ~PairMatchSet();

  // Data Control--------------------------------------------------------------
  void set_size(unsigned size);
  bool add_match(int i1, int i2);
  int  get_match_12(int i1) const;
  int  get_match_21(int i2) const;
  void clear_match_1(int i1);
  void set_identity();

  // Operations----------------------------------------------------------------
  int count() const { return _match_count; }

  void update_feature_match_data();
  void clear();

  void update(const vcl_vector<bool>& inliers);

  int  size() const;

  // Computations--------------------------------------------------------------
  int  compute_match_count();

  // Data Access---------------------------------------------------------------

  // ******* ITERATOR
  class iterator {
  public:
    iterator(bool full_only = true);
    iterator(const PairMatchSet& ccc, bool full_only = true);
    iterator& operator=(const PairMatchSet& ccc);
    int get_i1() const { return i1; }
    int get_i2() const { return i2; }
    bool next();
    iterator& operator ++ (/*prefix*/) { next(); return *this; }
    bool isfull() const;
    operator bool () const;

  private:
    iterator& operator ++ (int /*postfix*/) { abort(); return *this; }

  private:
    const PairMatchSet* _c;
    int _match_index;
    int i1, i2;
    bool _full_only;
  };
  // ******* END ITERATOR

  // IO------------------------------------------------------------------------

  // Input/Output--------------------------------------------------------------
  void print_brief(ostream& s) const;
  void print_brief() const;
  void write_ascii(ostream& s) const;
  bool read_ascii(istream& s);

  friend ostream& operator<<(ostream& s, const PairMatchSet& cc);
  friend istream& operator>>(istream& s, PairMatchSet& cc);

  bool get_match(int at, int* i1, int* i2) const;
protected:
  // Data Members--------------------------------------------------------------
  vcl_vector<int> _matches;
  int _match_count;
};

#endif // PairMatchSet_h_
