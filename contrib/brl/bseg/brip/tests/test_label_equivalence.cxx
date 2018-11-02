#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brip/brip_label_equivalence.h>

static void test_label_equivalence()
{
  unsigned a = 1, b =2, c = 3, d =4, e = 5, f = 6, g = 7;
  brip_label_equivalence le;
  le.add_label_pair(a,b);
  le.add_label_pair(a,e);
  le.add_label_pair(b,c);
  le.add_label_pair(b,d);
  le.add_label_pair(e,f);
  le.add_label_pair(e,g);
  le.transitive_closure();
  std::map<unsigned, std::set<unsigned> >& es = le.equivalence_sets();
  std::map<unsigned, std::set<unsigned> >::iterator mit;
  bool good = true;
  for (mit=es.begin(); mit!= es.end(); ++mit)
  {
    std::set<unsigned>& eqs = (*mit).second;
    std::cout << '(' ;
    for (const auto & eq : eqs)
      std::cout << eq << ' ';
    std::cout << ")\n";
    good = eqs.size() == 7;
  }

  brip_label_equivalence le1;
  le1.add_label_pair(a,b);
  le1.add_label_pair(b,c);
  le1.add_label_pair(c,d);
  le1.add_label_pair(d,e);
  le1.add_label_pair(e,f);
  le1.add_label_pair(f,g);
  le1.transitive_closure();
  std::map<unsigned, std::set<unsigned> >& es1 = le1.equivalence_sets();

  for (mit=es1.begin(); mit!= es1.end(); ++mit)
  {
    std::set<unsigned>& eqs = (*mit).second;
    std::cout << '(' ;
    for (const auto & eq : eqs)
      std::cout << eq << ' ';
    std::cout << ")\n";
    good = good&& eqs.size() == 7;
  }
  brip_label_equivalence le2;
  le2.add_label_pair(a,b);
  le2.add_label_pair(b,c);
  le2.add_label_pair(c,d);
  le2.add_label_pair(e,f);
  le2.add_label_pair(f,g);
  le2.transitive_closure();
  unsigned n = 0;
  std::map<unsigned, std::set<unsigned> >& es2 = le2.equivalence_sets();
  for (mit=es2.begin(); mit!= es2.end(); ++mit)
  {
    std::set<unsigned>& eqs = (*mit).second;
    n+= eqs.size();
    std::cout << '(' ;
    for (const auto & eq : eqs)
      std::cout << eq << ' ';
    std::cout << ")\n";
  }
  good = good && n ==7;
  TEST("Test equivalence classes", good, true);
}

TESTMAIN(test_label_equivalence);
