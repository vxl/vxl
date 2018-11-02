// This is brl/bseg/brip/brip_label_equivalence.h
#ifndef brip_label_equivalence_h_
#define brip_label_equivalence_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Maintain label equivalences and determine transitive closure of equivalence classes.
//
// \verbatim
//  Modifications
//   Initial version May 5, 2008
// \endverbatim
//
#include <iostream>
#include <map>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class brip_label_equivalence
{
 public:
  brip_label_equivalence():max_label_(0){}
  ~brip_label_equivalence()= default;

  //:add a label pair equivalence
  void add_label_pair(unsigned la, unsigned lb);

  //:carry out full propagation of label equivalence
  void transitive_closure();

  //:the full set of labels
  std::set<unsigned> labels() const;

  //:the set of labels equivalent to a given label
  std::map<unsigned, std::set<unsigned> >& equivalence_sets()
    { return equivalence_sets_;}

 protected:
  unsigned max_label_;
  bool get_next_label(std::set<unsigned> const& labels, unsigned int& label);

  bool merge_equivalence(std::map<unsigned int, std::set<unsigned int> >& tab,
                         unsigned int cur_label,
                         unsigned int label);
  std::map<unsigned, std::set<unsigned> > forward_pairs_;
  std::map<unsigned, std::set<unsigned> > reverse_pairs_;
  std::map<unsigned, std::set<unsigned> > equivalence_sets_;
};

#endif // brip_label_equivalence_h_
