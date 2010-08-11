__kernel 
void test_project(__global float16* cam,
                  __global float4* point3d,
                  __global float2* point2d)
{

   float2 p2d;
   float16 cam_=(*cam);
   float4 point3d_=(*point3d);
   bool flag= project(cam_,point3d_,&p2d);
   (*point2d)=p2d;
}
__kernel 
void test_backproject(__global float16* cam_inv, 
                      __global float2* point2d,
                      __global float4* point3d)
{

}
