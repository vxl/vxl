//////////////////////////////////////////////
// Local Float4 Pyramid
// It is assumed that:
//    level 0 is a 1x1
//    level 1 is a 2x2
//    level 2 is a 4x4
// 
typedef struct
{
    //pointer to head of list
    __local float4** pyramid;

    //buffer length
    int num_levels;
} ray_pyramid;


// "Constructor for ray_pyramid"
// Calculates cone ray pyramid
// Method assumes that pyramid[num_levels-1] has been filled out (with 
// the highest resolution rays available) 
// Also assumes that highest resolution availalable is local work group size
ray_pyramid new_ray_pyramid(__local float4** mem_start, int num_levels)
{
  ray_pyramid newPyramid;
  newPyramid.pyramid = mem_start;
  newPyramid.num_levels = num_levels;
 
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0); 
  uchar localJ = (uchar)get_local_id(1); 
  
  //calculate the next highest resolution (typically 4x4)
  //int curr_level = num_levels-2; 
  int curr_side_len = 4; 
  int offset = 2; 
  for(int curr_level=num_levels-2; curr_level >= 0; curr_level--) 
  {
    //active threads are every 2, every 4, or every 8
    if(localI%offset==0 && localJ%offset==0)
    {
      //grab the next level up to calculate average over the patch
      __local float4* prevImg = newPyramid.pyramid[curr_level+1]; 
      float4 meanDir = (float4) 0.0f; 
      for(int j=localJ; j<localJ+2; ++j) {
        for(int i=localI; i<localI+2; ++i) {
          uchar pixID = (uchar) (i + 2*curr_side_len*j); 
          meanDir += prevImg[pixID];  
        }
      }
      meanDir.w = 0.0f; 
      meanDir = normalize(meanDir); 
      uchar corner0 = (uchar)(localI + get_local_size(0)*localJ); 
      uchar corner1 = (uchar)(localI+1 + get_local_size(0)*(localJ+1)); 
      meanDir.w = prevImg[corner0].w + prevImg[corner1].w; 
      
      //make sure every other (even) thread 
      __local float4* currImg = newPyramid.pyramid[curr_level]; 
      uchar halfI = localI/offset; 
      uchar halfJ = localJ/offset; 
      uchar halfID = (uchar)(halfI + curr_side_len*halfJ); 
      currImg[halfID] = meanDir; 
      barrier(CLK_LOCAL_MEM_FENCE);
    }
    offset *= 2; 
    curr_side_len /= 2; 
  }
  return newPyramid;
}


//TESTS ABOVE CONSTRUCTION ALGO
__kernel
void
test_ray_pyramid(__global    float4             * ray_origins,
                 __global    float4             * ray_directions,
                 __global    uint4              * imgdims,            // dimensions of the input image (8x8)
                 __global    float4             * out_image1,         // second level pyramid image (4x4)
                 __global    float4             * out_image2,         // third level pyramid image (2x2)
                 __global    float4             * out_image3)         // fourth level pyramid image (1x1)
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1); 
  
  // get image coordinates and camera, check for validity before proceeding
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  int imIndex = j*get_global_size(0) + i;
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;

  //INITIALIZE RAY PYRAMID
  __local float4* ray_pyramid_mem[4]; 
  __local float4 ray0[1]; 
  __local float4 ray1[4]; 
  __local float4 ray2[16]; 
  __local float4 ray3[64]; 
  ray_pyramid_mem[0] = ray0; 
  ray_pyramid_mem[1] = ray1; 
  ray_pyramid_mem[2] = ray2; 
  ray_pyramid_mem[3] = ray3; 
  ray3[llid] = ray_directions[imIndex]; 
  barrier(CLK_LOCAL_MEM_FENCE);
  ray_pyramid pyramid = new_ray_pyramid(ray_pyramid_mem, 4); 

  //ray 2 image
  if( localJ<4 && localI<4 ) {
    uchar halfID = (uchar)(localI + 4*localJ); 
    out_image1[halfID] = ray2[halfID]; 
  }
  
  //ray 1 image
  if( localJ<2 && localI<2 ) {
    uchar quarterID = (uchar)(localI+2*localJ); 
    out_image2[quarterID] = ray1[quarterID]; 
  }
  
  //copy fully reduced image into global mem
  if( llid==0 ) 
    out_image3[0] = ray0[0]; 


}
