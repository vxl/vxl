#include <stdlib.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iterator.h>
#include <vcl/vcl_cmath.h>

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_math.h>

#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>

// Randomly select samples points from the screen size of row_size & col_size bucketed symmetrically
// with buckets partitions on the area.
// Note : that no two selected points can lie in the same bucketed partition.

// Watch this routine, if the given points are not dispersed
// uniformly enough across the given distribution then the routine will
// not exit, rather keep searching for the requested number of subsamples.

vcl_vector<int> Monte_Carlo(vcl_vector<HomgPoint2D> points, vcl_vector<int> index, int buckets, int samples)
{
  double row_size = 2.0;
  double col_size = 2.0;
  vcl_vector<int> out_points(samples);
  double row_div = row_size/buckets;
  double col_div = col_size/buckets;  
  int no_buckets = buckets*buckets;
  if(buckets < 1) {
    cout << "Warning Monte Carlo sampling will not work." << endl;
    cout << "Not enough buckets, need 1 " << " have " << buckets << "." << endl;
  }
  if(index.size() < samples) {
    cout << "Warning Monte Carlo sampling will not work." << endl;
    cout << "Not enough points to choose from, need " << samples << " have " << index.size() << "." << endl; 
  }
  int i = 0;
  while(i < samples) {
    int random;
    if(buckets > 1) {
      random  = (int)((float)(no_buckets - 1)*rand()/(RAND_MAX+1.0));
    } else {
      random  = 1;
    }

    int row_num;
    if(buckets > 1) {
      row_num = abs(random/buckets);
    } else {
      row_num = 0;
    }
    int col_num;
    if(buckets > 1) {
      col_num = random - row_num*buckets;;
    } else {
      col_num = 0;
    }




    double row_check_lower = row_num * row_div;
    double col_check_lower = col_num * col_div; 
    double row_check_upper = row_num * row_div + row_div;
    double col_check_upper = col_num * col_div + col_div;
    row_check_lower -= 1.0;
    col_check_lower -= 1.0;
    row_check_upper -= 1.0;
    col_check_upper -= 1.0;

    vcl_vector<int> list;

    // Select from the first list
    for(int j = 0; j < index.size(); j++) {
      if(points[j].get_y() >= row_check_lower && points[j].get_y() < row_check_upper && points[j].get_x() >= col_check_lower &&
points[j].get_x() < col_check_upper) {
        list.push_back(index[j]);
        
      }
    }
    
    int list_size = list.size();  
    bool not_picked = true;
    if(list_size != 0) {
      int counter = 0;
      bool fail;
      while(not_picked && counter < list_size*4) {  
        int pick = (int)((float)(list_size - 1)*rand()/(RAND_MAX+1.0));
        int picked = list[pick];
        fail = false; 
        for(int k = 0; k < i; k++) {
          if(picked == out_points[k])
            fail = true;
        }
        if(!fail) {
          out_points[i] = picked; 
          not_picked = false;
          i++;
        } else {
          counter++;
          //cout << "Failed" << endl;
        }
      }       
    }  
    list.clear();
  }   
 
  return out_points;
} 
/*
// Note : this hasn't been implemented properly yet
vcl_vector<HomgPoint2D> Taubins_MLE(HomgPoint2D x1, HomgPoint2D x2, FMatrix *F)
{
  vcl_vector<HomgPoint2D> actual_points;
  
  // Generate a Jacobian matrix
  vnl_matrix<double> J;
  J.put(1, 1, (F->get(1, 1) - x2*F->get(3, 1)));
  J.put(1, 2, (F->get(1, 2) - x2*F->get(3, 2)));
  J.put(1, 3, (-x*F->get(3, 1) - y*F->get(3, 2) - F->get(3, 3)));
  J.put(1, 4, 0.0);
  J.put(2, 1, (F->get(2, 1) - y_dash*F->get(3, 1)));
  J.put(2, 2, (F->get(2, 2) - y_dash*F->get(3, 2)));
  J.put(2, 3, 0.0);
  J.put(2, 4, (-x*F->get(3, 1) - y_dash*F->get(3, 2) - F->get(3, 3)));

  // Find the Moore-Penrose Psuedo Inverse for the Jacobian matrix
  vnl_svd<double> svd(J, 1e-8);  
  vnl_diag_matrix<double> diag = svd.W;
  for(int i = 0; i < diag.size(); i++) {
    if(diag(i, i) != 0.0)
      diag(i, i) = 1 / diag(i, i);
  }
  vnl_matrix<double> psuedo = svd.U * diag * svd.V.transpose();
  
  // Find the residuals
   
  return actual_points;
}
*/

double Sampsons_MLE(HomgPoint2D x1, HomgPoint2D x2, FMatrix *F)
{
  double rX, rY, rX_dash, rY_dash, GRADr, r, dist; 
  vnl_matrix<double> temp(3, 3);
  temp = (vnl_matrix<double>)F->get_matrix();
  cout << x2.get_x() << endl;
  rX = temp.get(0, 0)*x2.get_x() + temp.get(1, 0)*x2.get_y() + temp.get(2, 0);
  rY = F->get(0, 1)*x2.get_x() + F->get(1, 1)*x2.get_y() + F->get(2, 1);  
  rX_dash = F->get(0, 0)*x1.get_x() + F->get(0, 1)*x1.get_y() + F->get(0, 2);
  rY_dash = F->get(1, 0)*x1.get_x() + F->get(1, 1)*x1.get_y() + F->get(1, 2);
  cout << "Points : " << rX << " " << rY << " " << rX_dash << " " << rY_dash << endl;
  GRADr = vnl_math_sqr(rX*rX + rY*rY + rX_dash*rX_dash + rY_dash*rY_dash);
  cout << "1 : " << " " << GRADr << endl;
  // This is an annoying interface
  HomgPoint2D *x1p = new HomgPoint2D(x1.get_x(), x1.get_y(), 1.0);
  HomgPoint2D *x2p = new HomgPoint2D(x2.get_x(), x2.get_y(), 1.0);
  cout << "2" << endl;
  r = F->image1_epipolar_distance_squared(x1p, x2p);
  cout << "r " << r << endl;
  dist = r/GRADr;
  return dist;
}
