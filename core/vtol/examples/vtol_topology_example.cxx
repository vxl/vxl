// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1999 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

// Example use of the vtol_topology classes 
//
// Author: Peter Tu May 1 2000

#include <vcl/vcl_iostream.h>
#include <vtol/top_hold.h>

void main()
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


}
