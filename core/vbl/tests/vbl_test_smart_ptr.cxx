#include "vbl_test_smart_ptr.h"
#include <vcl/vcl_list.h>
#include <vbl/vbl_smart_ptr.h>

int base_impl::reftotal = 0;

void printval (base_ref const &p)
{
  cout << "base_impl val = " << p->n << endl;
}

void changeval (base_impl *p, int k)
{
  // To pass a base_ref to this function, use the .as_pointer() method
  // to get the raw/dumb pointer out of the smart pointer.
  // We could have used 'base_ref const &p', instead of 'base_impl *p'
  // here, in which case the call to as_pointer() is unnecessary.
  p->n = k;
}

base_ref newbase_impl (int k)
{
  // This is just to prove that one can pass these things
  // around as function values
  base_ref q(new base_impl);
  q->n = k;
  return q;
}

int doit ()
{
  base_ref p;
  if (p == 0) // This is actually unnecessary - just to demonstrate operator==()
    p = new base_impl;

  base_ref a = new base_impl (5);
  if (p == a)
    cout << "FAILED: p==a (wrong)" << endl;
  if (p != a)
    cout << "p!=a (correct)" << endl;
  if (!p)
    cout << "FAILED: !p (wrong)" << endl;
  if (p)
    cout << "p converts to true (correct)" << endl;

  // Get a smart pointer as the return value of a function :
  base_ref q = newbase_impl(10);

  // smart pointer to derived class :
  derived_ref qder = new  derived_impl();
  q = qder.as_pointer();

  // These two things are effectively the same now
  p = q;
  // They should be the same
  if (p == q)
    cout << "p and q are the same\n" << endl;
  else
    cout << "FAILED: p == q" << endl;

  cout << "value of   p->n : " << p->n << endl;
  cout << "value of (*p).n : " << (*p).n << endl;

  {
    // make a new base
    base_ref r = new base_impl ();
    // assign p to r
    r = p;
    // r is really the same as p in this, so when r is changed, so is p
    changeval(r.as_pointer(), 27);
    printval (r);
    printval (p);
  }
  
  // Now test lists of base_impl
  {
    vcl_list<base_ref> videos;
    for (int i=1; i<=10; i++)
    {
      base_ref newvid = new base_impl (i);
      videos.push_back (newvid);
    }

    // Print out the list
    cout << "List of video sequences" << endl;
    for (vcl_list<base_ref>::const_iterator i = videos.begin(); i != videos.end(); i++) 
      cout << " " << *i << endl;
    //cout << videos << endl;

    // Clear the list
    videos.clear();

    cout << "Video list is clear : filling again" << endl;

    for (int i=11; i<=15; i++)
    {
      base_ref newvid = new base_impl (i);
      videos.push_back (newvid);
    }

    // Print out the list
    cout << "List of video sequences" << endl;
    for (vcl_list<base_ref>::const_iterator i = videos.begin(); i != videos.end(); i++) 
      cout << " " << *i << endl;
    //cout << videos << endl;
  }
  return 0;
}

extern "C" void test_vbl_smart_ptr()
{
  cout << "Running" << endl;
  doit();
  base_impl::checkcount();
}

main(int /*argc*/, char **/*argv*/)
{
  test_vbl_smart_ptr();
}

//-------------------------------------------------------

#include <vbl/vbl_smart_ptr.txx>
VBL_SMART_PTR_INSTANTIATE(base_impl);
VBL_SMART_PTR_INSTANTIATE(derived_impl);

#include <vcl/vcl_list.txx>
VCL_LIST_INSTANTIATE(base_ref);

#include <vcl/vcl_iterator.h>
#include <vcl/vcl_algorithm.txx>
VCL_OPERATOR_NE_INSTANTIATE(base_ref);
VCL_CONTAINABLE_INSTANTIATE(base_ref);
