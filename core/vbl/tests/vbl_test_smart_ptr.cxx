#include <testlib/testlib_test.h>
#include "vbl_test_classes.h"
#include <vcl_list.h>
#include <vcl_iostream.h>
#include <vbl/vbl_smart_ptr.h>

//----------------------------------------------------------------------

void printval (base_sptr const &p)
{
  vcl_cout << "base_impl val = " << p->n << vcl_endl;
}

void changeval (base_impl *p, int k)
{
  // To pass a base_sptr to this function, use the .as_pointer() method
  // to get the raw/dumb pointer out of the smart pointer.
  // We could have used 'base_sptr const &p', instead of 'base_impl *p'
  // here, in which case the call to as_pointer() is unnecessary.
  p->n = k;
}

base_sptr newbase_impl (int k)
{
  // This is just to prove that one can pass these things
  // around as function values
  base_sptr q(new base_impl);
  q->n = k;
  return q;
}

static void test_base_sptr()
{
  base_sptr p;
  TEST("initial value of base_sptr should be null", bool(p), false);
  if (!p) // This is actually unnecessary - just to demonstrate operator bool()
    p = new base_impl;
  if (p == (base_impl*)0) // identical result - just to demonstrate operator==()
    p = new base_impl;

  vcl_cout << "operator<< gives : " << p << vcl_endl;

  base_sptr a = new base_impl (5);
  TEST("p == a", p == a, false);
  TEST("p != a", p != a, true);
  TEST("!p", (!p), false);
  TEST("bool(p)", bool(p), true);

  // Get a smart pointer as the return value of a function :
  base_sptr q = newbase_impl(10);

  // smart pointer to derived class :
  derived_sptr qder = new  derived_impl();
  q = qder.as_pointer();

  // These two things are effectively the same now
  p = q;
  // They should be the same
  TEST("p == q", p, q);

  vcl_cout << "value of   p->n : " << p->n << vcl_endl
           << "value of (*p).n : " << (*p).n << vcl_endl;

  {
    // make a new base
    base_sptr r = new base_impl ();
    // assign p to r
    r = p;
    // r is really the same as p in this, so when r is changed, so is p
    changeval(r.as_pointer(), 27);
    printval (r);
    printval (p);
  }

  // Now test lists of base_impl
  {
    vcl_list<base_sptr> videos;
    for (int i=1; i<=10; i++)
    {
      base_sptr newvid = new base_impl (i);
      videos.push_back (newvid);
    }

    // Print out the list
    vcl_cout << "List of video sequences\n";
    for (vcl_list<base_sptr>::const_iterator i = videos.begin(); i != videos.end(); i++)
      vcl_cout << ' ' << *i << vcl_endl;

    // Clear the list
    videos.clear();

    vcl_cout << "Video list is clear : filling again\n";

    for (int i=11; i<=15; i++)
    {
      base_sptr newvid = new base_impl(i);
      videos.push_back (newvid);
    }

    // Print out the list
    vcl_cout << "List of video sequences\n";
    for (vcl_list<base_sptr>::const_iterator i = videos.begin(); i != videos.end(); i++)
      vcl_cout << ' ' << *i << vcl_endl;
  }
}

static void test_ref_unref()
{
  // Test protect and unref
  int good_count = base_impl::reftotal + 2;
  base_sptr p = new base_impl( 1 );
  base_sptr q = p;
  base_sptr r = new base_impl( 1 );
  TEST("reference counts", base_impl::checkcount(good_count) &&
                           p->get_references() == 2 &&
                           r->get_references() == 1,            true);
  q.unprotect();
  TEST("unprotect should unref", p->get_references(), 1 );
  q = r;
  TEST("object should not be unexpectedly destroyed", base_impl::checkcount(good_count), true);
  TEST("assignment should ref", r->get_references(), 2);
}

static void vbl_test_smart_ptr()
{
  test_base_sptr();
  test_ref_unref();
  TEST("checkcount(0)", base_impl::checkcount(0), true);
}

TESTMAIN(vbl_test_smart_ptr);
