
#include <stdio.h>
#include <vcl/vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

class SmartPointerTest : public vbl_ref_count 
{
public :
  static int reftotal;
  int n;
 
  SmartPointerTest(int nn) : n(nn)
  {
  reftotal++;
  cout << "Creating SmartPointerTest : " <<  (void *)this
       << " = " << this << endl;
  }
 
  SmartPointerTest() : n(7)
  {
  reftotal++;
  cout << "Creating SmartPointerTest : " << (void *)this
       << " = " << this << endl;
  }
 
  ~SmartPointerTest()
  {
    reftotal--;
    cout << "Destroying SmartPointerTest : " << (void *)this
      << " = " << this << endl;
  }
 
  void Print (ostream &str)
  {
    str << "SmartPointerTest(" << n << ") ";
  }
 
  static void checkcount ()
  {
    if (reftotal == 0)
      cout << "SmartPointerTest : PASSED" << endl;
    else
      cout << "SmartPointerTest : FAILED : count = " << reftotal <<endl;
  }
};
 
int SmartPointerTest::reftotal = 0;

//-------------------------------------------------------
 
class SmartPointerSubclass : public SmartPointerTest
{
   public : int k;
};

//-------------------------------------------------------

#include <vcl/vcl_iterator.h>
#include <vcl/vcl_list.txx>
#include <vcl/vcl_algorithm.txx>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(SmartPointerTest);
VBL_SMART_PTR_INSTANTIATE(SmartPointerSubclass);

VCL_OPERATOR_NE_INSTANTIATE(vbl_smart_ptr<SmartPointerTest>);
VCL_CONTAINABLE_INSTANTIATE(vbl_smart_ptr<SmartPointerTest>);
VCL_LIST_INSTANTIATE(vbl_smart_ptr<SmartPointerTest>);

typedef vbl_smart_ptr<SmartPointerTest> SmartPointerTest_ref;
typedef vbl_smart_ptr<SmartPointerSubclass> SmartPointerSubclass_ref;

//======================================================================

void printval (const SmartPointerTest_ref &p)
{
  cout << "SmartPointerTest val = " <<  p->n << endl;
}

void changeval (SmartPointerTest *p, int k)
{
  // This is just to prove that one can pass them in as arguments of functions
  // We could just as well write SmartPointerTest p, instead of &p here.
  p->n = k;
}

SmartPointerTest_ref newSmartPointerTest (int k)
{
  // This is just to prove that one can pass these things
  // around as function values

  SmartPointerTest_ref q(new SmartPointerTest());
  q->n = k;
  return q;
}

int doit ()
{
  SmartPointerTest_ref p;
  if (p == 0) // This is actually unnecessary - just to demonstrate ==
    p = new SmartPointerTest();

  SmartPointerTest_ref a = new SmartPointerTest (5);
  if (p == a)
    cout << "p is the same as a (wrong)" << endl;
  if (p != a)
    cout << "p is not the same as a " << endl;
  
  if (!p)
    cout << "p is not defined" << endl;
  if (p)
    cout << "p is defined" << endl;
   
  // Get a thing back from a function
  SmartPointerTest_ref q = newSmartPointerTest(10);
  SmartPointerSubclass_ref qsub = new  SmartPointerSubclass();
  q = qsub;
 
  // These two things are effectively the same now
  p = q;

  // They should be the same
  if (p == q)
    cout << "P and Q are the same\n" << endl;

  cout << "value = " << p->n << endl;
  cout << "Again : value = " << (*p).n << endl;

  {
    // r is really the same as p in this, so when r is changed, so is p
    SmartPointerTest_ref r = new SmartPointerTest ();
    r = p;
    changeval(r, 27);
    printval (r);
  }

  cout << "value = " << p->n << endl;

  // Now test lists of SmartPointerTest
  {
    vcl_list<SmartPointerTest_ref> videos;
    for (int i=1; i<=10; i++)
    {
      SmartPointerTest_ref newvid = new SmartPointerTest (i);
      videos.push_back (newvid);
    }

    // Print out the list
    cout << "List of video sequences" << endl;
    for (vcl_list<SmartPointerTest_ref>::const_iterator i = videos.begin(); i != videos.end(); i++) 
      cout << " " << *i << endl;
    //cout << videos << endl;

    // Clear the list
    videos.clear();

    cout << "Video list is clear : filling again" << endl;

    for (int i=11; i<=15; i++)
    {
      SmartPointerTest_ref newvid = new SmartPointerTest (i);
      videos.push_back (newvid);
    }

    // Print out the list
    cout << "List of video sequences" << endl;
    for (vcl_list<SmartPointerTest_ref>::const_iterator i = videos.begin(); i != videos.end(); i++) 
      cout << " " << *i << endl;
    //cout << videos << endl;
  }

  return 0;
}

extern "C" void test_vbl_smart_ptr()
{
  doit();
  SmartPointerTest::checkcount();
}

main()
{
  cout << "Running" << endl;
  test_vbl_smart_ptr();
}
