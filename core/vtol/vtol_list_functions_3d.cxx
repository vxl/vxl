// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
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
#include <vtol/vtol_list_functions_3d.h>
#include <vsol/vsol_spatial_object_3d.h>


//: Note if you really want a tagged union...this is it...
// otherwise you can call remove_duplicates and get the
// same result....pav 

void tagged_union(vcl_vector<vsol_spatial_object_3d*>* topolist)
{
  vcl_vector<vsol_spatial_object_3d*> temp;
  
  // Clearing the tags before processing...
  vcl_vector<vsol_spatial_object_3d*>::iterator i;

  for (i=topolist->begin();i!=topolist->end(); i++)
    ((vsol_spatial_object_3d*)(*i))->unset_tagged_union_flag();

  // Performing the Union by looping over
  // the elements and removing duplicates.
  
  for (i=topolist->begin(); i!=topolist->end(); i++)
    if (((vsol_spatial_object_3d*)(*i))->get_tagged_union_flag() == false)  
      {
	((vsol_spatial_object_3d*)(*i))->set_tagged_union_flag();
	temp.push_back(*i);
      }
  (*topolist)=temp;

}


