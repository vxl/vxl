//
// vbl_test_basic_relation
// Author: Rupert W. Curwen
// Created: 22 Jan 98
//
//-----------------------------------------------------------------------------

#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vbl/vbl_basic_relation.h>

class TestType
{
public:
  void ref() { count++; vcl_cout << "ref " << this << " = " << count << vcl_endl; }
  void unref() { count--; vcl_cout << "unref " << this << " = " << count << vcl_endl; if (!count) delete this; }

public:
  int i,count;
  static int ninstances;

public:
  TestType(int i_)
    : i(i_), count(0)
  {
    ninstances++;
  }

  ~TestType()
  {
    vcl_cout << "Destructor " << this << vcl_endl;
    ninstances--;
  }
};

struct vcl_hash<TestType*>
{
  vcl_size_t operator()(TestType* t) const { return (int)t; }
};

#include <vbl/vbl_protection_traits.txx>
#include <vbl/vbl_basic_relation.txx>
#include <vbl/vbl_br_default.txx>
#include <vbl/vbl_br_impl.txx>

VBL_PROTECTION_TRAITS_ref(TestType*);
template class vbl_br_impl<TestType*, int, vcl_string>;
template class vbl_br_iter_impl<TestType*, int, vcl_string>;
template class vbl_basic_relation<TestType*, int, vcl_string>;
template class vbl_basic_relation_where<TestType*, int, vcl_string>;
template class vbl_basic_relation_iterator<TestType*, int, vcl_string>;
template class vbl_basic_relation_factory<TestType*, int, vcl_string BR_DEFAULT2>;
template class vbl_br_default_factory<TestType*, int, vcl_string>;
template class vbl_br_default<TestType*, int, vcl_string BR_DEFAULT2>;
template class vbl_br_default_iter<TestType*, int, vcl_string>;
template class vbl_br_default_wild_iter<TestType*, int, vcl_string>;

int TestType::ninstances = 0;

vcl_string NewString()
{
  static int i = 0;
  char buf[20];
  vcl_sprintf(buf,"%d",++i);
  vcl_string s(buf);
  return s;
}

const int ntest = 1000;

bool CheckRelation(vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2 >& r)
{
  // Check each inserted tuple.
  bool passed = true;
  for (int i=0; i<ntest; i++)
  {
    int len = r.where_second(i).size();
    if (len != 3)
    {
      vcl_cout << "Failed: where_second(" << i << ").size() returns " << len
               << ", should be 3\n";
      passed = false;
    }
    vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::iterator j;
    int count = 0;
    vcl_string s1("");
    vcl_string s2("");
    for (j = r.where_second(i).begin(); j!= r.end(); j++, count++)
    {
      if ((*j).GetSecond() != i)
      {
        vcl_cout << "Failed: where_second(" << i << ") returns tuple " << *j << vcl_endl;
        passed = false;
      }
      if (! ((*j).GetFirst() == (*j).GetThird()) )
      {
        s1 = (*j).GetFirst();
        s2 = (*j).GetThird();
      }
    }
    if (count != 3)
    {
      vcl_cout << "Failed: where_second(" << i << ") returns " << count
               << " tuples, should be 3.\n";
      passed = false;
    }
    bool case1found = false;
    bool case2found = false;
    bool case3found = false;
    for (j = r.where_second(i).begin(); j!= r.end(); j++)
    {
      if ((*j).GetFirst() == (*j).GetThird())
      {
        if ((*j).GetFirst() == s1)
          case1found = true;
        else if ((*j).GetFirst() == s2)
          case2found = true;
      }
      else if (((*j).GetFirst() == s1) && ((*j).GetThird() == s2))
        case3found = true;
    }
    if (!case1found || !case2found || !case3found)
    {
      vcl_cout << "Failed: where_second(" << i << ") returns wrong three tuples.\n";
      passed = false;
    }

    // Now check for these s1 and s2.
    int l1 = r.where_first(s1).size();
    if (l1 != 2)
    {
      vcl_cout << "Failed: where_first(" << s1 << ") has size " << l1
               << ", expected 2\n";
      passed = false;
    }
    int l2 = r.where_third(s2).size();
    if (l2 != 2)
    {
      vcl_cout << "Failed: where_third(" << s2 << ") has size " << l2
               << ", expected 2\n";
      passed = false;
    }

    // And check consistency.
    int l12 = r.where_first(s1).where_third(s2).size();
    if (l12 != 1)
    {
      vcl_cout << "Failed: where_first(" << s1 << ").where_third(" << s2 << ") has size " << l12
               << ", expected 1\n";
      passed = false;
    }
    int l11 = r.where_first(s1).where_third(s1).size();
    if (l11 != 1)
    {
      vcl_cout << "Failed: where_first(" << s1 << ").where_third(" << s1 << ") has size " << l11
               << ", expected 1\n";
      passed = false;
    }
    int l22 = r.where_first(s2).where_third(s2).size();
    if (l22 != 1)
    {
      vcl_cout << "Failed: where_first(" << s2 << ").where_third(" << s2 << ") has size " << l22
               << ", expected 1\n";
      passed = false;
    }
    int li12 = r.where_first(s1).where_second(i).where_third(s2).size();
    if (li12 != 1)
    {
      vcl_cout << "Failed: where_first(" << s1 << ").where_second(" << i << ").where_third("
               << s2 << ") has size " << li12 << ", expected 1\n";
      passed = false;
    }
    int li11 = r.where_first(s1).where_second(i).where_third(s1).size();
    if (li11 != 1)
    {
      vcl_cout << "Failed: where_first(" << s1 << ").where_second(" << i << ").where_third("
               << s1 << ") has size " << li11 << ", expected 1\n";
      passed = false;
    }
    int li22 = r.where_first(s2).where_second(i).where_third(s2).size();
    if (li22 != 1)
    {
      vcl_cout << "Failed: where_first(" << s2 << ").where_second(" << i << ").where_third("
               << s2 << ") has size " << li22 << ", expected 1\n";
      passed = false;
    }
  }
  return passed;
}

void testStringIntString()
{
  vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2> r("name_age_eyes");
  r.insert("Rupert",30,"blue");
  r.insert("Bill",31,"brown");
  r.insert("Joe",120,"brown");
  vcl_cout << r << vcl_endl;

  // Do a stress test.
  for (int i=0; i<ntest; i++)
  {
    vcl_string s1 = NewString();
    vcl_string s2 = NewString();
    r.insert(s1,i,s1);
    r.insert(s1,i,s2);
      r.insert(s2,i,s2);
  }

  // Test length;
  if (r.size() == 3 + 3*ntest)
    vcl_cout << "insert test passed\n";
  else
    vcl_cout << "insert test failed\n";

  // Delete those three tuples.
  r.remove("Rupert",30,"blue");
  r.remove("Bill",31,"brown");
  r.remove("Joe",120,"brown");

  if (r.size() == 3*ntest)
    vcl_cout << "remove test passed\n";
  else
    vcl_cout << "remove test failed\n";

  bool passed = CheckRelation(r);
  if (passed)
    vcl_cout << "Where test passed\n";
  else
    vcl_cout << "Where test failed\n";

  // Test random insert/delete.
  vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2> temp_rel("holding_relation");
  passed = true;
  for (int i=0; i<ntest; i++)
  {
    int n = (rand()/256) % ntest;
    int m = (rand()/256) % 3;
    int count = 0;
    if ((rand()/1024)%2 == 0)
    {
      vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::iterator j;
      for (j = r.where_second(n).begin(); j != r.end(); j++, count++)
      {
        if (count == m)
        {
          temp_rel.insert(*j);
          vcl_cout << "Removing " << *j << vcl_endl;
          r.remove(*j);
          break;
        }
      }
    }
    else
    {
      vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::iterator j = temp_rel.begin();
      if (j != temp_rel.end())
      {
        r.insert(*j);
        vcl_cout << "inserting " << *j << vcl_endl;
        temp_rel.remove(*j);
      }
    }
  }

  // insert all remaining removed.
  for (vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::iterator j = temp_rel.begin();
       j != temp_rel.end(); ++j)
  {
    r.insert(*j);
    vcl_cout << "inserting " << *j << vcl_endl;
  }

  temp_rel.clear();

  passed  = passed && CheckRelation(r);
  if (passed)
    vcl_cout << "Stress test passed\n";
  else
    vcl_cout << "Stress test failed\n";

  r.clear();
}

void testDowncasting()
{
  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2> r1("test_relation");
  vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2> r2("vcl_string_relation");

  vbl_basic_relation_type* r = &r1;
  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2>* r1p =
    vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r1p != &r1)
    vcl_cout << "Failed downcast test 1\n";
  else
    vcl_cout << "Passed downcast test 1\n";

  r = &r2;
  r1p = vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r1p)
    vcl_cout << "Failed downcast test 2\n";
  else
    vcl_cout << "Passed downcast test 2\n";

  r = &r1;
  vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>* r2p =
    vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r2p)
    vcl_cout << "Failed downcast test 3\n";
  else
    vcl_cout << "Passed downcast test 3\n";

  r = &r2;
  r2p = vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r2p != &r2)
    vcl_cout << "Failed downcast test 4\n";
  else
    vcl_cout << "Passed downcast test 4\n";
}

static void vbl_test_basic_relation()
{
  TestType* e1 = new TestType(2);
  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2> rel("my_relation");
  vcl_cout << rel << vcl_endl;
  rel.insert(e1,20,"what is it?");
  rel.insert(e1,40,"what is it?");
  rel.insert(e1,10,"who is it?");
  rel.insert(e1,10,"why is it?");
  // This insert should fail.
  TEST("no duplicate tuple inserted", rel.insert(e1,10,"who is it?"), false);

  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2>::iterator i;
  int count = 0;
  for (i = rel.where_second(10).begin();
       i != rel.end();
       i++, count++)
    vcl_cout << "Found tuple where second = 10 : " << *i << vcl_endl;
  TEST("where_second() should return 2 tuples", count, 2)

  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2> selected = rel.where_third("what is it?");
  vcl_cout << "Where third = \"what is it?\" :" << selected << vcl_endl;

  count = 0;
  for (i = selected.begin();
       i != selected.end();
       i++, count++)
    vcl_cout << "Found tuple " << *i << vcl_endl;
  TEST("where_third() should return 2 tuples", count, 2)

  TEST("size", rel.size(), 4)

  vcl_cout << rel << vcl_endl;
  rel.where_second(10).clear();

  TEST("clear where_second(10)", rel.size(), 2)

  vcl_cout << rel << vcl_endl;
  rel.clear();

  TEST("no leaks ?", TestType::ninstances, 0)

  rel.clear();

  // Test the downcasting methods.
  testDowncasting();

  // Now do some serious tests on the <vcl_string,int,vcl_string BR_DEFAULT2> relation.
  testStringIntString();
}

TESTMAIN(vbl_test_basic_relation);
