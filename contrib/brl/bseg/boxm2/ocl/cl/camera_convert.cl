//Camera Convert: A bunch of kernels to convert camera to ray images
// NEEDS to include backproject.cl

__kernel
void
persp_to_generic( __global    float16            * persp_camera,         // camera orign and SVD of inverse of camera matrix
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    uint4              * cam_dims)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uint i=get_global_id(0);
  uint j=get_global_id(1);
  uint imIndex = j*get_global_size(0)+i;

  //make sure the index exists in the image, although this can be thrown away if
  //it is guaranteed that the global_size is equal to the image size in each dim
  uint i_min = (*cam_dims).x;
  uint j_min = (*cam_dims).y;
  uint i_size = (*cam_dims).z;
  uint j_size = (*cam_dims).w;

  if( i < i_size && j < j_size )
  {
    float4 ray_o = convert_float4(persp_camera[2].s4567); ray_o.w = 1.0f;
    float4 ray_d = backproject(i+i_min, j+j_min, persp_camera[0], persp_camera[1], persp_camera[2], ray_o);
    ray_d.w = 0.0f;

    //also make sure to write cone half angle
    float4 K = persp_camera[2].s89ab; //calibration matrix [f, f, principle U, principle V];
    float4 ref_d = backproject_corner(K.z, K.w, persp_camera[0], persp_camera[1], persp_camera[2], ray_o);
    ref_d.w = 0.0f;

    //store half angle in ray_d.w
    float focal_length = K.x;
    float ca1 = dot(ray_d, ref_d);
    ray_d.w = (float) fabs(ca1*ca1/focal_length) * .7071;

    //store half angle in ray_d.w
    ray_origins[imIndex] = ray_o;
    ray_directions[imIndex] = ray_d;
  }
}
