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

bool doit ()
{
  bool result_okay = true;

  base_sptr p;
  if (p) {
    vcl_cout << "FAILED: p is initially not null (true)" << vcl_endl;
    result_okay = false;
  }
  if (!p) // This is actually unnecessary - just to demonstrate operator bool()
    p = new base_impl;
  if (p == (base_impl*)0) // identical result - just to demonstrate operator==()
    p = new base_impl;

  vcl_cout << "operator<< gives : " << p << vcl_endl;

  base_sptr a = new base_impl (5);
  if (p == a) {
    vcl_cout << "FAILED: p==a (wrong)" << vcl_endl;
    result_okay = false;
  }
  if (p != a) {
    vcl_cout << "p!=a (correct)" << vcl_endl;
  } else {
    vcl_cout << "FAILED: ! p!=a (wrong)" << vcl_endl;
    result_okay = false;
  }
  if (!p) {
    vcl_cout << "FAILED: !p (wrong)" << vcl_endl;
    result_okay = false;
  }
  if (p)
    vcl_cout << "p converts to true (correct)" << vcl_endl;

  // Get a smart pointer as the return value of a function :
  base_sptr q = newbase_impl(10);

  // smart pointer to derived class :
  derived_sptr qder = new  derived_impl();
  q = qder.as_pointer();

  // These two things are effectively the same now
  p = q;
  // They should be the same
  if (p == q)
    vcl_cout << "p and q are the same\n\n";
  else {
    vcl_cout << "FAILED: p == q" << vcl_endl;
    result_okay = false;
  }

  vcl_cout << "value of   p->n : " << p->n << vcl_endl;
  vcl_cout << "value of (*p).n : " << (*p).n << vcl_endl;

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
    vcl_cout << "List of video sequences" << vcl_endl;
    for (vcl_list<base_sptr>::const_iterator i = videos.begin(); i != videos.end(); i++)
      vcl_cout << " " << *i << vcl_endl;
    //vcl_cout << videos << vcl_endl;

    // Clear the list
    videos.clear();

    vcl_cout << "Video list is clear : filling again" << vcl_endl;

    for (int i=11; i<=15; i++)
    {
      base_sptr newvid = new base_impl (i);
      videos.push_back (newvid);
    }

    // Print out the list
    vcl_cout << "List of video sequences" << vcl_endl;
    for (vcl_list<base_sptr>::const_iterator i = videos.begin(); i != videos.end(); i++)
      vcl_cout << " " << *i << vcl_endl;
    //vcl_cout << videos << vcl_endl;
  }

  {
    // Test protect and unref
    int good_count = base_impl::reftotal + 2;
    base_sptr p = new base_impl( 1 );
    base_sptr q = p;
    base_sptr r = new base_impl( 1 );
    if ( !( base_impl::checkcount( good_count ) && p->get_references() == 2
           && r->get_references() == 1 ) ) {
      vcl_cout << "FAILED: reference counts are wrong." << vcl_endl;
      result_okay = false;
    }
    q.unprotect();
    if ( p->get_references() != 1 ) {
      vcl_cout << "FAILED: unprotected didn't unref!" << vcl_endl;
      result_okay = false;
    }
    q = r;
    if ( !base_impl::checkcount( good_count ) ) {
      vcl_cout << "    an object was unexpectedly destroyed" << vcl_endl;
      result_okay = false;
    }
    if ( r->get_references() != 2 ) {
      vcl_cout << "FAILED: assignment didn't ref" << vcl_endl;
      result_okay = false;
    }
  }

  return result_okay;
}

extern "C" int test_vbl_smart_ptr()
{
  vcl_cout << "Running" << vcl_endl;
  return !(doit() && base_impl::checkcount());  // 0 = good, non-zero = bad
}

int main(int /*argc*/, char ** /*argv*/)
{
  return test_vbl_smart_ptr();
}

//-------------------------------------------------------
