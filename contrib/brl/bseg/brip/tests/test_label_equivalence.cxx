#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
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
  vcl_map<unsigned, vcl_set<unsigned> >& es = le.equivalence_sets();
  vcl_map<unsigned, vcl_set<unsigned> >::iterator mit;
  bool good = true;
  for (mit=es.begin(); mit!= es.end(); ++mit)
  {
    vcl_set<unsigned>& eqs = (*mit).second;
    vcl_cout << '(' ;
    for (vcl_set<unsigned>::iterator sit = eqs.begin();
         sit != eqs.end(); ++sit)
      vcl_cout << *sit << ' ';
    vcl_cout << ")\n";
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
  vcl_map<unsigned, vcl_set<unsigned> >& es1 = le1.equivalence_sets();

  for (mit=es1.begin(); mit!= es1.end(); ++mit)
  {
    vcl_set<unsigned>& eqs = (*mit).second;
    vcl_cout << '(' ;
    for (vcl_set<unsigned>::iterator sit = eqs.begin();
         sit != eqs.end(); ++sit)
      vcl_cout << *sit << ' ';
    vcl_cout << ")\n";
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
  vcl_map<unsigned, vcl_set<unsigned> >& es2 = le2.equivalence_sets();
  for (mit=es2.begin(); mit!= es2.end(); ++mit)
  {
    vcl_set<unsigned>& eqs = (*mit).second;
    n+= eqs.size();
    vcl_cout << '(' ;
    for (vcl_set<unsigned>::iterator sit = eqs.begin();
         sit != eqs.end(); ++sit)
      vcl_cout << *sit << ' ';
    vcl_cout << ")\n";
  }
  good = good && n ==7;
  TEST("Test equivalence classes", good, true);
}

TESTMAIN(test_label_equivalence);
