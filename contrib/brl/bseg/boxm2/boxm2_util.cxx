#include "boxm2_util.h"

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_svd.h>

void boxm2_util::random_permutation(int* buffer, int size)
{
  vnl_random random(9667566);
  
  //shuffle the buffer
  for(int i=0; i<size; i++) 
  {
    //swap current value with random one after it
    int curr = buffer[i]; 
    int rInd = random.lrand32(i, size-1); 
    buffer[i] = buffer[rInd];
    buffer[rInd] = curr; 
  }

}
float boxm2_util::clamp(float x, float a, float b)
{
    return x < a ? a : (x > b ? b : x);
}
