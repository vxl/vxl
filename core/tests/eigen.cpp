//
// Created by Johnson, Hans J on 11/26/22.
//

#include <Eigen/Core>

int main(int , char * [])
{
  int data[] = {1,2,3,4,5,6,7,8,9};
  Map<RowVectorXi> v(data,4);
  cout << "The mapped vector v is: " << v << "\n";
  new (&v) Map<RowVectorXi>(data+4,5);
  cout << "Now v is: " << v << "\n";
  return 0;
}