//
// Class: test_vbl_basic_relation
// Author: Rupert W. Curwen
// Created: 22 Jan 98
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_string.h> // C++ specific includes first
#include <vcl/vcl_list.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstdio.h>
#include <vbl/vbl_basic_relation.h>


class TestType
{
public:
  void ref() { count++; cout << "ref " << this << " = " << count << endl; }
  void unref() { count--; cout << "unref " << this << " = " << count << endl; if (!count) delete this; }
 
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
    cout << "Destructor " << this << endl;
    ninstances--;
  }
};

struct vcl_hash<TestType*>
{
  size_t operator()(TestType* t) const { return (int)t; }
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
  sprintf(buf,"%d",++i);
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
	  cout << "Failed: where_second(" << i << ").size() returns " << len
	       << ", should be 3" << endl;
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
	      cout << "Failed: where_second(" << i << ") returns tuple " << *j << endl;
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
	  cout << "Failed: where_second(" << i << ") returns " << count 
	       << " tuples, should be 3." << endl;
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
	  cout << "Failed: where_second(" << i << ") returns wrong three tuples." << endl;
	  passed = false;
	}

      // Now check for these s1 and s2.
      int l1 = r.where_first(s1).size();
      if (l1 != 2)
	{
	  cout << "Failed: where_first(" << s1 << ") has size " << l1 
	       << ", expected 2" << endl;
	  passed = false;
	}
      int l2 = r.where_third(s2).size();
      if (l2 != 2)
	{
	  cout << "Failed: where_third(" << s2 << ") has size " << l2 
	       << ", expected 2" << endl;
	  passed = false;
	}

      // And check consistency.
      int l12 = r.where_first(s1).where_third(s2).size();
      if (l12 != 1)
	{
	  cout << "Failed: where_first(" << s1 << ").where_third(" << s2 << ") has size " << l12 
	       << ", expected 1" << endl;
	  passed = false;
	}
      int l11 = r.where_first(s1).where_third(s1).size();
      if (l11 != 1)
	{
	  cout << "Failed: where_first(" << s1 << ").where_third(" << s1 << ") has size " << l11 
	       << ", expected 1" << endl;
	  passed = false;
	}
      int l22 = r.where_first(s2).where_third(s2).size();
      if (l22 != 1)
	{
	  cout << "Failed: where_first(" << s2 << ").where_third(" << s2 << ") has size " << l22 
	       << ", expected 1" << endl;
	  passed = false;
	}
      int li12 = r.where_first(s1).where_second(i).where_third(s2).size();
      if (li12 != 1)
	{
	  cout << "Failed: where_first(" << s1 << ").where_second(" << i << ").where_third(" << s2 << ") has size " << li12 
	       << ", expected 1" << endl;
	  passed = false;
	}
      int li11 = r.where_first(s1).where_second(i).where_third(s1).size();
      if (li11 != 1)
	{
	  cout << "Failed: where_first(" << s1 << ").where_second(" << i << ").where_third(" << s1 << ") has size " << li11 
	       << ", expected 1" << endl;
	  passed = false;
	}
      int li22 = r.where_first(s2).where_second(i).where_third(s2).size();
      if (li22 != 1)
	{
	  cout << "Failed: where_first(" << s2 << ").where_second(" << i << ").where_third(" << s2 << ") has size " << li22 
	       << ", expected 1" << endl;
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
  cout << r << endl;

  // Do a stress test.
  int i;
  for (i=0; i<ntest; i++)
    {
      vcl_string s1 = NewString();
      vcl_string s2 = NewString();
      r.insert(s1,i,s1);
      r.insert(s1,i,s2);
      r.insert(s2,i,s2);
    }

  // Test length;
  if (r.size() == 3 + 3*ntest)
    cout << "insert test passed\n";
  else
    cout << "insert test failed\n";

  // Delete those three tuples.
  r.remove("Rupert",30,"blue");
  r.remove("Bill",31,"brown");
  r.remove("Joe",120,"brown");
  
  if (r.size() == 3*ntest)
    cout << "remove test passed\n";
  else
    cout << "remove test failed\n";

  bool passed = CheckRelation(r);
  if (passed)
    cout << "Where test passed" << endl;
  else
    cout << "Where test failed" << endl;

  // Test random insert/delete.
  vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2> temp_rel("holding_relation");
  passed = true;
  for (i=0; i<ntest; i++)
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
		  cout << "Removing " << *j << endl;
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
	      cout << "inserting " << *j << endl;
	      temp_rel.remove(*j);
	    }
	}
    }

  // insert all remaining removed.
  for (vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::iterator j = temp_rel.begin();
       j != temp_rel.end();
       j++)
    {
      r.insert(*j);
      cout << "inserting " << *j << endl;
    }

  temp_rel.clear();

  passed  = passed && CheckRelation(r);
  if (passed)
    cout << "Stress test passed" << endl;
  else
    cout << "Stress test failed" << endl;

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
    cout << "Failed downcast test 1" << endl;
  else
    cout << "Passed downcast test 1" << endl;

  r = &r2;
  r1p = vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r1p)
    cout << "Failed downcast test 2" << endl;
  else
    cout << "Passed downcast test 2" << endl;

  r = &r1;
  vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>* r2p = 
    vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r2p)
    cout << "Failed downcast test 3" << endl;
  else
    cout << "Passed downcast test 3" << endl;

  r = &r2;
  r2p = vbl_basic_relation<vcl_string,int,vcl_string BR_DEFAULT2>::cast(r);
  if (r2p != &r2)
    cout << "Failed downcast test 4" << endl;
  else
    cout << "Passed downcast test 4" << endl;
}

//extern "C" void test_vbl_basic_relation()
main()
{
  TestType* e1 = new TestType(2);
  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2> rel("my_relation");
  cout << rel << endl;
  rel.insert(e1,20,"what is it?");
  rel.insert(e1,40,"what is it?");
  rel.insert(e1,10,"who is it?");
  rel.insert(e1,10,"why is it?");
  // This insert should fail.
  if (rel.insert(e1,10,"who is it?"))
    cout << "Failed: duplicate tuple inserted." << endl;
  else
    cout << "Passed: duplicate tuple was not inserted." << endl;

  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2>::iterator i;
  int count = 0;
  for (i = rel.where_second(10).begin();
       i != rel.end();
       i++, count++)
    cout << "Found tuple where second = 10 : " << *i << endl;
  if (count == 2)
    cout << "Passed: where_second succeeded." << endl;
  else
    cout << "Failed: where_second returned " << count << " tuples, expected 2." << endl;

  vbl_basic_relation<TestType*,int,vcl_string BR_DEFAULT2> selected = rel.where_third("what is it?");
  cout << "Where third = \"what is it?\" :" << selected << endl;

  count = 0;
  for (i = selected.begin();
       i != selected.end();
       i++, count++)
    cout << "Found tuple " << *i << endl;
  if (count == 2)
    cout << "Passed: where_third succeeded." << endl;
  else
    cout << "Failed: where_third returned " << count << " tuples, expected 2." << endl;

  if (rel.size() == 4)
    cout << "Passed: size succeeded." << endl;
  else
    cout << "Failed: size returned " << rel.size() << " tuples, expected 4." << endl;

  cout << rel << endl;
  rel.where_second(10).clear();

  if (rel.size() == 2)
    cout << "Passed: clear where second = 10 succeeded." << endl;
  else
    cout << "Failed: clear where second = 10 failed, leaving " << rel.size() << " tuples, expected 2." << endl;

  cout << rel << endl;
  rel.clear();

  if (TestType::ninstances == 0)
    cout << "Passed: no leaks found." << endl;
  else
    cout << "Failed: " << TestType::ninstances << " instances of T leaked." << endl;

  rel.clear();

  // Test the downcasting methods.
  testDowncasting();

  // Now do some serious tests on the <vcl_string,int,vcl_string BR_DEFAULT2> relation.
  testStringIntString();
}

