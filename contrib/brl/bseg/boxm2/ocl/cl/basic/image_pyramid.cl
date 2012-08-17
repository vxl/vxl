//Image pyramid, requires that you define IMG_TYPE
// It is assumed that:
//    level 0 is a 1x1
//    level 1 is a 2x2
//    level 2 is a 4x4
//    level 3 is a 8x8
#ifdef IMG_TYPE
typedef struct
{
  //pointer to pyramid memory
  __local IMG_TYPE** pyramid;

  //length of pyramid buffer
  int num_levels;

  //max side length of biggest element of pyramid
  int max_side_len;
} image_pyramid;

// Accesses a level and i,j offset within an image pyramid.
// Assumes that i and j are legal (not greater than level's size), eg
// level[0] only has one value, (0,0)
// level[1] has 4 values...
IMG_TYPE image_pyramid_access(image_pyramid* pyramid, int level, int i, int j)
{
  int sideLen = 1<<level;
  int fullID = i + sideLen*j;
  return pyramid->pyramid[level][fullID];
}

//safely accesses by level based on this thread's id
IMG_TYPE image_pyramid_access_safe(image_pyramid* pyramid, int level)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //offset (the amount to divide localI and localJ to get correctly indexed pyramid i,j)
  uchar offset = 1<<(3-level);
  return image_pyramid_access(pyramid, level, localI/offset, localJ/offset);
}

// sets the image based on finest i and j, much like safe, except localI and localJ are passed in
IMG_TYPE image_pyramid_access_level(image_pyramid* pyramid, int level, int localI, int localJ)
{
  //offset (the amount to divide localI and localJ to get correctly indexed pyramid i,j)
  uchar offset = 1<<(3-level);
  return image_pyramid_access(pyramid, level, localI/offset, localJ/offset);
}

//image pyramid set
void image_pyramid_set(image_pyramid* pyramid, int level, int i, int j, IMG_TYPE val)
{
  int sideLen = 1<<level;
  int fullID = i + sideLen*j;
  pyramid->pyramid[level][fullID] = val;
}

//image pyramid set safe
void image_pyramid_set_safe(image_pyramid* pyramid, int level, IMG_TYPE val)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //offset (the amount to divide localI and localJ to get correctly indexed pyramid i,j)
  uchar offset = 1<<(3-level);
  image_pyramid_set(pyramid, level, localI/offset, localJ/offset, val);
}

// sets the image based on finest i and j, much like safe, except localI and localJ are passed in
void image_pyramid_set_level(image_pyramid* pyramid, int level, int localI, int localJ, IMG_TYPE val)
{
  //offset (the amount to divide localI and localJ to get correctly indexed pyramid i,j)
  uchar offset = 1<<(3-level);
  image_pyramid_set(pyramid, level, localI/offset, localJ/offset, val);
}

//Increment value at level,i,j by val
void image_pyramid_incr(image_pyramid* pyramid, int level, int i, int j, IMG_TYPE val)
{
  int sideLen = 1<<level;
  int fullID = i + sideLen*j;
  pyramid->pyramid[level][fullID] += val;
}

//safe pyramid increment (automatically calucaltes based on local id
void image_pyramid_incr_safe(image_pyramid* pyramid, int level, IMG_TYPE val)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //offset (the amount to divide localI and localJ to get correctly indexed pyramid i,j)
  uchar offset = 1<<(3-level);
  image_pyramid_incr(pyramid, level, localI/offset, localJ/offset, val);
}

// "Constructor for image_pyramid"
// Method assumes that pyramid[num_levels-1] has been filled out (with
// the highest resolution pixels available)
// Also assumes that highest resolution availalable is local work group size
image_pyramid new_image_pyramid(__local IMG_TYPE** mem_start, int num_levels, int max_side_len)
{
  image_pyramid newPyramid;
  newPyramid.pyramid = mem_start;
  newPyramid.num_levels = num_levels;
  newPyramid.max_side_len = max_side_len;

  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //calculate the next highest resolution
  //(typically 4x4) ... requires that you map localI and localJ to 4x4 range
  //do 2x2 image now
  //going to trigger threads [0, 4] on each side - need to map this to
  //[0,2] to get appropriate cells from 4x4 iamge
  int curr_side_len = max_side_len/2; //current length of the pyramid being created - start out 4x4
  int offset = 2; //distance between active threads in workspace (2 in the case of 4x4, 4 in the case of 2x2)
  for (int curr_level=num_levels-2; curr_level >= 0; curr_level--)
  {
    //active threads are every 2, every 4, or every 8
    if (localI%offset==0 && localJ%offset==0)
    {
      //scale back thread IDs
      int prevOffset = offset/2;
      int prevI = localI / prevOffset;
      int prevJ = localJ / prevOffset;

      //grab the next level up to calculate average over the patch
      __local IMG_TYPE* prevImg = newPyramid.pyramid[curr_level+1];
      IMG_TYPE meanPix = (IMG_TYPE) 0;
      for (int j=prevJ; j<prevJ+2; ++j) {
        for (int i=prevI; i<prevI+2; ++i) {
          uchar pixID = (uchar) (i + 2*curr_side_len*j);  //previous side length was twice this side length
          meanPix += prevImg[pixID];
        }
      }
      meanPix /= 4;

      //make sure every other (even) thread
      __local IMG_TYPE* currImg = newPyramid.pyramid[curr_level];
      uchar halfI = localI/offset;
      uchar halfJ = localJ/offset;
      uchar halfID = (uchar)(halfI + curr_side_len*halfJ);
      currImg[halfID] = meanPix;
      barrier(CLK_LOCAL_MEM_FENCE);
    }
    offset *= 2;
    curr_side_len /= 2;
  }
  return newPyramid;
}


/////////////////////////////////////////////////////////////////////////////
//TESTS ABOVE CONSTRUCTION ALGO
__kernel
void
test_image_pyramid(__global    IMG_TYPE           * in_img,
                   __global    uint4              * imgdims,            // dimensions of the input image (8x8)
                   __global    IMG_TYPE           * out_image2,         // second level pyramid image (4x4)
                   __global    IMG_TYPE           * out_image1,         // third level pyramid image (2x2)
                   __global    IMG_TYPE           * out_image0)         // fourth level pyramid image (1x1)
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
  __local IMG_TYPE* img_pyramid_mem[4];
  __local IMG_TYPE img0[1];
  __local IMG_TYPE img1[4];
  __local IMG_TYPE img2[16];
  __local IMG_TYPE img3[64];
  img_pyramid_mem[0] = img0;
  img_pyramid_mem[1] = img1;
  img_pyramid_mem[2] = img2;
  img_pyramid_mem[3] = img3;
  img3[llid] = in_img[imIndex];
  barrier(CLK_LOCAL_MEM_FENCE);
  image_pyramid pyramid = new_image_pyramid(img_pyramid_mem, 4, 8);

  //ray 2 image
  if ( localJ<4 && localI<4 ) {
    uchar halfID = (uchar)(localI + 4*localJ);
    out_image2[halfID] = img2[halfID];
  }

  //ray 1 image
  if ( localJ<2 && localI<2 ) {
    uchar quarterID = (uchar)(localI+2*localJ);
    out_image1[quarterID] = img1[quarterID];
  }

  //copy fully reduced image into global mem
  if ( llid==0 )
    out_image0[0] = img0[0];
}

#endif

