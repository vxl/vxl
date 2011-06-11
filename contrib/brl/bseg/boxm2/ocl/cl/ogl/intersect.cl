//:
// \file
// \brief tolerance constants/functions
#define OGL_EPS 1.0e-8; // tolerance for intersections
#define FOUR_THIRDS_PI 4.188790204786391f
#define ONE_THIRD_PI 1.04719755119659775f

//: calculate the volume of intersection between these two spheres
// A sphere is represented by a float4, where (x,y,z) is its 3d center, and
// w is its radius
float sphere_intersection_volume(float4 A, float4 B)
{
  //distance between two spheres and radius
  float r0 = A.w;
  float r1 = B.w;
  A.w = 0.0f; B.w = 0.0f;
  float d = distance(A,B);
  float difR = fabs(r0-r1);

  //0. if one sphere is completely inside the other one
  if ( d <= difR )
  {
    float minR = min(r0, r1);
    return FOUR_THIRDS_PI * minR * minR * minR;
  }

  //1. The two spheres intersect...
  float sumR = r0 + r1;
  if (d > difR && d < sumR)
  {
    //calculate distance to the plane of intersection from the center of circle A
    float xInt = ( r0*r0 - r1*r1 + d*d ) / (2.0*d);

    //calculate height of sperhical caps
    float h0 = r0 - xInt;
    float h1 = r1 + xInt - d;

    // volume is sum of tops
    return ONE_THIRD_PI * ( h0*h0*(3*r0-h0) + h1*h1*(3*r1-h1) );
  }

  //otherwise the two spheres do not intersect
  return 0.0f;
}
