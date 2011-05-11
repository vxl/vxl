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
  int i=get_global_id(0); 
  int j=get_global_id(1);
  uint imIndex = j*get_global_size(0)+i;
  
  //make sure the index exists in the image, although this can be thrown away if 
  //it is guaranteed that the global_size is equal to the image size in each dim
  if( i < cam_dims->x && j < cam_dims->y )
  {
    float4 ray_o = (float4) persp_camera[2].s4567; ray_o.w = 1.0f;
    float4 ray_d = backproject(i, j, persp_camera[0], persp_camera[1], persp_camera[2], ray_o);
    ray_d.w = 0.0f; 
    
    //also make sure to write cone half angle
    float4 cone_d = backproject_corner(i, j, persp_camera[0], persp_camera[1], persp_camera[2], ray_o);
    cone_d.w = 0.0f; 
    
    //calculate hte half angle stuff here
    
    
    //float half_angle = dot_product(rij.direction(), corner.direction()); 
    float half_angle; 
    float ca = dot(ray_d, cone_d); 
    if (ca>=-1.0)
    {
      if (ca<=1.0)
        half_angle = acos(ca);
      else
        half_angle = 0;
    }
    else
      half_angle = acos(-1.0); // pi

    //store half angle in ray_d.w
    ray_d.w = half_angle; 
    ray_origins[imIndex] = ray_o;
    ray_directions[imIndex] = ray_d; 
  }
}
