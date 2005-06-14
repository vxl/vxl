//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_k_medoid.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

//: Test bsta histograms
void test_k_medoid()
{
  unsigned nelements = 5;
  bsta_k_medoid km(nelements);
  //fill the distance array
  km.insert_distance(0,1, 1.0);
  km.insert_distance(0,2, 2.0);
  km.insert_distance(0,3, 2.0);
  km.insert_distance(0,4, 3.0);
  km.insert_distance(1,2, 2.0);
  km.insert_distance(1,3, 2.0);
  km.insert_distance(1,4, 3.0);
  km.insert_distance(2,3, 1.0);
  km.insert_distance(2,4, 4.0);
  km.insert_distance(3,4, 3.0);
  vcl_cout << "Five point Distance Array "; km.print_distance_array();
  //form clusters
  unsigned k = 2;
  km.do_clustering(k);
  //print the mediods
  for(unsigned i = 0; i<k; ++i)
    {
      vcl_cout << "Medoid[" << i << "] = " << km.medoid(i) << '\n';
      vcl_cout << "with cluster \n";
      const vcl_vector<unsigned>& elements = km.elements(i);
      for(unsigned j = 0; j<km.size(i); ++j)
        vcl_cout << elements[j] << ' ' ;
      vcl_cout << '\n';
    }
  TEST("Five Point Clusters", km.medoid(0)==3&&km.medoid(1)==1, true);
  //A larger problem
  // 0 1 2 3 4 <-5-> 5 6 7 8 9
  nelements = 10;
  bsta_k_medoid km2(nelements);
  for(unsigned i = 0; i<5; ++i)
    {
      double di = i;
      for(unsigned j =i+1; j<5;++j) 
        {
          double dj = j;
          km2.insert_distance(i,j, vcl_fabs(di-dj));
        }
      for(unsigned j = 5; j<10;++j)
        {
          double dj = j;
          km2.insert_distance(i,j, 5.0+ vcl_fabs(di-dj));
        }
    }

  for(unsigned i = 5; i<10; ++i)
    {
      double di = i;
      for(unsigned j =i+1; j<10;++j) 
        {
          double dj = j;
          km2.insert_distance(i,j, vcl_fabs(di-dj));
        }
    }
  vcl_cout << "Linear Sets Distance Array "; km2.print_distance_array();
  km2.do_clustering(k);
  //print the mediods
  for(unsigned i = 0; i<k; ++i)
    {
      vcl_cout << "Medoid[" << i << "] = " << km2.medoid(i) << '\n';
      vcl_cout << "with cluster \n";
      const vcl_vector<unsigned>& elements = km2.elements(i);
      for(unsigned j = 0; j<km2.size(i); ++j)
        vcl_cout << elements[j] << ' ' ;
      vcl_cout << '\n';
    }
  TEST("Linear Sets Clusters", km2.medoid(0)==7&&km2.medoid(1)==2, true);

// ----------------------------
//  A more structured example  
//      5                 7
//      *                 *
// 4 *  o                 o * 6
//      0                 1
//
//
//      3                 2
// 10*  o                 o * 8
//      *                 *
//     11                 9
// ---------------------------
  nelements = 12;
  bsta_k_medoid km3(nelements);
  
  km3.insert_distance(0,1, 1.0);  
  km3.insert_distance(0,2, 2.0);  
  km3.insert_distance(0,3, 1.0);  
  km3.insert_distance(0,4, 0.1);  
  km3.insert_distance(0,5, 0.1);  
  km3.insert_distance(0,6, 1.1);  
  km3.insert_distance(0,7, 1.1);  
  km3.insert_distance(0,8, 2.1);  
  km3.insert_distance(0,9, 2.1);  
  km3.insert_distance(0,10, 1.1);  
  km3.insert_distance(0,11, 1.1);  

  km3.insert_distance(1,2, 1.0);  
  km3.insert_distance(1,3, 2.0);  
  km3.insert_distance(1,4, 1.1);  
  km3.insert_distance(1,5, 1.1);  
  km3.insert_distance(1,6, 0.1);  
  km3.insert_distance(1,7, 0.1);  
  km3.insert_distance(1,8, 1.1);  
  km3.insert_distance(1,9, 1.1);  
  km3.insert_distance(1,10, 2.1);  
  km3.insert_distance(1,11, 2.1);  

  km3.insert_distance(2,3, 1.0);  
  km3.insert_distance(2,4, 2.1);  
  km3.insert_distance(2,5, 2.1);  
  km3.insert_distance(2,6, 1.1);  
  km3.insert_distance(2,7, 1.1);  
  km3.insert_distance(2,8, 0.1);  
  km3.insert_distance(2,9, 0.1);  
  km3.insert_distance(2,10, 1.1);  
  km3.insert_distance(2,11, 1.1);  

  km3.insert_distance(3,4, 1.1);  
  km3.insert_distance(3,5, 1.1);  
  km3.insert_distance(3,6, 2.1);  
  km3.insert_distance(3,7, 2.1);  
  km3.insert_distance(3,8, 1.1);  
  km3.insert_distance(3,9, 1.1);  
  km3.insert_distance(3,10, 0.1);  
  km3.insert_distance(3,11, 0.1);  

  km3.insert_distance(4,5, 0.2);  
  km3.insert_distance(4,6, 1.2);  
  km3.insert_distance(4,7, 1.2);  
  km3.insert_distance(4,8, 2.2);  
  km3.insert_distance(4,9, 2.2);  
  km3.insert_distance(4,10, 1.0);  
  km3.insert_distance(4,11, 1.2);  

  km3.insert_distance(5,6, 1.2);  
  km3.insert_distance(5,7, 1.0);  
  km3.insert_distance(5,8, 2.2);  
  km3.insert_distance(5,9, 2.2);  
  km3.insert_distance(5,10, 1.2);  
  km3.insert_distance(5,11, 1.2);  

  km3.insert_distance(6,7, 0.2);  
  km3.insert_distance(6,8, 1.0);  
  km3.insert_distance(6,9, 1.2);  
  km3.insert_distance(6,10, 2.2);  
  km3.insert_distance(6,11, 2.2);  

  km3.insert_distance(7,8, 1.2);  
  km3.insert_distance(7,9, 1.2);  
  km3.insert_distance(7,10, 2.2);  
  km3.insert_distance(7,11, 2.2);  

  km3.insert_distance(8,9, 0.2);  
  km3.insert_distance(8,10, 1.2);  
  km3.insert_distance(8,11, 1.2);  

  km3.insert_distance(9,10, 1.2);  
  km3.insert_distance(9,11, 1.0);  

  km3.insert_distance(10,11, 0.2);  

  vcl_cout << "Square Star Distance Array "; km3.print_distance_array();
  k = 4;
  km3.do_clustering(k);
  //print the mediods
  for(unsigned i = 0; i<k; ++i)
    {
      vcl_cout << "Medoid[" << i << "] = " << km3.medoid(i) << '\n';
      vcl_cout << "with cluster \n";
      const vcl_vector<unsigned>& elements = km3.elements(i);
      for(unsigned j = 0; j<km3.size(i); ++j)
        vcl_cout << elements[j] << ' ' ;
      vcl_cout << '\n';
    }
  TEST("Square Star Clusters", 
       km3.medoid(0)==4&&km3.medoid(1)==7&&km3.medoid(2)==8&&km3.medoid(3)==11,
       true);
}
  
TESTMAIN(test_k_medoid);
