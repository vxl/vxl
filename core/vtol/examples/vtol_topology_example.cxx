// Example use of the vtol_topology classes 
//
// Author: Peter Tu May 1 2000

#include <vcl/vcl_iostream.h>
#include <vtol/top_hold.h>

int main()
{

  top_hold *child1 = new top_hold();
  top_hold *child2 = new top_hold();
  top_hold *parent = new top_hold();
    
  cout << "link child1" << endl;
  parent->link_inferior(child1);

  cout << endl;
  parent->Print();
  child1->Print();
  child2->Print();

  cout << "link child2" << endl;

  parent->link_inferior(child2);

  cout << endl;
  parent->Print();
  child1->Print();
  child2->Print();

  
  cout << "unlink child1 " << endl;

  parent->unlink_inferior(child1);

  cout << endl;

  parent->Print();
  child1->Print();
  child2->Print();

  cout << "unlink child2 " << endl;
  parent->unlink_inferior(child2);

  cout << endl;
  parent->Print();
  child1->Print();
  child2->Print();

  return 0;
}
