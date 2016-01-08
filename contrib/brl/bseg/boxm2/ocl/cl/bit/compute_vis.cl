#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef COMPVIS

typedef struct
{
   __global float* alpha;
            float* visibility;
            bool* start;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//create 4 rays (u_i) around given ray d such that (u_i).d=cos(5)
void create_aux_rays(float4 d, float4* u)
{
    d = normalize(d);

    float4 m1;
    if(d.z != 0.0f)
        m1 = (float4)(1, 1, -(d.x+d.y)/d.z, 0);
    else if(d.y != 0.0f)
        m1 = (float4)(1, -(d.x+d.z)/d.y, 1, 0);
    else
        m1 = (float4)(-(d.y+d.z)/d.x , 1, 1, 0);

    m1 = normalize(m1);
    float4 m2 = cross(d,m1);

    float c = tan(radians(5.0f));
    u[0] = d + m1 * c;
    u[1] = d - m1 * c;
    u[2] = d + m2 * c;
    u[3] = d - m2 * c;
}


//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);
__kernel
void
compute_vis(__constant  uint               * datasize_points,
            __constant  RenderSceneInfo    * linfo,
            __constant  float4             * directions,
            __global    int4               * tree_array,       // tree structure for each block
            __constant  uchar              * bit_lookup,       // used to get data_index
            __global    float              * alpha_array,
            __global    float4             * points,
            __global    float4             * normals,
            __global    float16            * vis_sphere,
            __constant   bool              * contain_point,
            __local     uchar16            * local_tree,       // cache current tree into local memory
            __local     uchar              * cumsum)
{
    int gid=get_global_id(0);
    if (gid<datasize_points[0]) {
        //get normal and point from global mem
        float4 ray_o = points[ gid ];
        float4 ray_d = normals[ gid ];

        //check if there is a normal here
        if ( (ray_d.x == 0 && ray_d.y == 0 && ray_d.z ==0) || (ray_o.x == 0 && ray_o.y == 0 && ray_o.z == 0)) {
           vis_sphere[gid].sf = -1.0f; //flag to indicate there is no normal in this location.
           return;
        }
        else {

          //declare ray
          float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, normal_x,normal_y,normal_z;
          calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
          normal_x = ray_dx; normal_y = ray_dy; normal_z = ray_dz;


          AuxArgs aux_args;
          aux_args.linfo    = linfo;
          aux_args.alpha   = alpha_array;

          //get visibilities from global mem to private mem
          float private_vis[12];
          private_vis[0] = vis_sphere[gid].s0;
          private_vis[1] = vis_sphere[gid].s1;
          private_vis[2] = vis_sphere[gid].s2;
          private_vis[3] = vis_sphere[gid].s3;
          private_vis[4] = vis_sphere[gid].s4;
          private_vis[5] = vis_sphere[gid].s5;
          private_vis[6] = vis_sphere[gid].s6;
          private_vis[7] = vis_sphere[gid].s7;
          private_vis[8] = vis_sphere[gid].s8;
          private_vis[9] = vis_sphere[gid].s9;
          private_vis[10] = vis_sphere[gid].sa;
          private_vis[11] = vis_sphere[gid].sb;

          float4 aux_rays[4];
          float vis_of_aux_rays[5];

          //loop thru directions
          bool start;
          float vis;
          for (unsigned int i = 0; i < 12; i++)
          {

            //setup ray
            start = !contain_point[0];


            vis = private_vis[i];
            aux_args.visibility = &(private_vis[i]);
            aux_args.start = &start;

            calc_scene_ray_generic_cam(linfo, ray_o, directions[i], &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

            //shoot ray
            cast_ray( 1, 1,
                      ray_ox, ray_oy, ray_oz,
                      ray_dx, ray_dy, ray_dz,
                      linfo, tree_array,                               //scene info
                      local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);   //utility info


            //zip thru aux rays
            create_aux_rays(directions[i],aux_rays);
            for(unsigned  j = 0; j < 4; j++)
            {
                start = !contain_point[0];

                vis_of_aux_rays[j] = vis;
                aux_args.visibility = &(vis_of_aux_rays[j]);
                aux_args.start = &start;

                calc_scene_ray_generic_cam(linfo, ray_o, aux_rays[j], &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

                //shoot ray
                cast_ray( 1, 1,
                      ray_ox, ray_oy, ray_oz,
                      ray_dx, ray_dy, ray_dz,
                      linfo, tree_array,                                 //scene info
                      local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);   //utility info
            }
            //pick the median of sorted rays
            vis_of_aux_rays[4] = private_vis[i];
            sort_vector( vis_of_aux_rays, 5);
            private_vis[i] = vis_of_aux_rays[2];

          }

          //transfer from private mem to global mem
          vis_sphere[gid].s0 = private_vis[0];
          vis_sphere[gid].s1 = private_vis[1];
          vis_sphere[gid].s2 = private_vis[2];
          vis_sphere[gid].s3 = private_vis[3];
          vis_sphere[gid].s4 = private_vis[4];
          vis_sphere[gid].s5 = private_vis[5];
          vis_sphere[gid].s6 = private_vis[6];
          vis_sphere[gid].s7 = private_vis[7];
          vis_sphere[gid].s8 = private_vis[8];
          vis_sphere[gid].s9 = private_vis[9];
          vis_sphere[gid].sa = private_vis[10];
          vis_sphere[gid].sb = private_vis[11];
        }
    }
}

//compute_vis step cell functor
void step_cell_computevis(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //start computing visibility after ray has left the original cell
    if ((*aux_args.start)) {
        float  alpha  = aux_args.alpha[data_ptr];
        float  seg_len = d * aux_args.linfo->block_len;
        (*aux_args.visibility) *= exp(-alpha * seg_len);
    }
    else
        (*aux_args.start) = true;
}





__kernel
void
decide_normal_dir(     __constant  RenderSceneInfo    * linfo,
                       __constant  float4             * directions,
                       __global    float4             * normals,
                       __global    float              * vis,
                       __global    float16            * vis_sphere )
{
    int gid=get_global_id(0);
    int datasize = linfo->data_len ;//* info->num_buffer;
    if (gid<datasize) {
        vis[gid] = -1.0f;

      //check if there is meaningful data here
      if (vis_sphere[gid].sf != -1.0f) {
          //declare ray
          float4 dummy = (float4) (1.0,1.0,1.0,1.0);
          float4 ray_d = normals[ gid ];

          float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, normal_x,normal_y,normal_z;
          calc_scene_ray_generic_cam(linfo, dummy, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
          normal_x = ray_dx; normal_y = ray_dy; normal_z = ray_dz;

          float max_vis = 0.0f;
          float max_vis_flipped = 0.0f;
          float sum_vis = 0.0f;
          float sum_vis_flipped = 0.0f;
          float private_vis[12];
          private_vis[0] = vis_sphere[gid].s0;
          private_vis[1] = vis_sphere[gid].s1;
          private_vis[2] = vis_sphere[gid].s2;
          private_vis[3] = vis_sphere[gid].s3;
          private_vis[4] = vis_sphere[gid].s4;
          private_vis[5] = vis_sphere[gid].s5;
          private_vis[6] = vis_sphere[gid].s6;
          private_vis[7] = vis_sphere[gid].s7;
          private_vis[8] = vis_sphere[gid].s8;
          private_vis[9] = vis_sphere[gid].s9;
          private_vis[10] = vis_sphere[gid].sa;
          private_vis[11] = vis_sphere[gid].sb;

          //compute max visibility in normal and opposite hemisphere
          for (unsigned int i = 0; i < 12; i++)
          {
              calc_scene_ray_generic_cam(linfo, dummy, directions[i], &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
              if (dot((float4)(ray_dx,ray_dy,ray_dz,0), (float4)(normal_x,normal_y,normal_z,0)) > 0.0){
                  max_vis = (max_vis < private_vis[i]) ? private_vis[i] : max_vis;
                  sum_vis+=private_vis[i];
              }
              else {
                  max_vis_flipped = (max_vis_flipped < private_vis[i]) ? private_vis[i] : max_vis_flipped;
                  sum_vis_flipped+=private_vis[i];
              }

          }
  #ifdef USESUM //use the sum of visibilities for the given hemisphere
            //flip if necessary
          if(sum_vis_flipped > sum_vis){
            normals[ gid ] = (float4)(-normal_x,-normal_y,-normal_z,normals[gid].w);
            vis[gid] = sum_vis_flipped;
          }
          else {
            normals[ gid ] = (float4)(normal_x,normal_y,normal_z,normals[gid].w);
            vis[gid] = sum_vis;
          }

  #else //use the max  visibility for the given hemisphere
          //flip if necessary
          if(max_vis_flipped > max_vis)
              normals[ gid ] = (float4)(-normal_x,-normal_y,-normal_z,normals[gid].w);
          else
              normals[ gid ] = (float4)(normal_x,normal_y,normal_z,normals[gid].w);
          //store max visibility
          vis[gid] = (max_vis_flipped > max_vis) ? max_vis_flipped : max_vis;
  #endif //USESUM

      }
    }
}

__kernel
void
decide_inside_cell(     __constant  RenderSceneInfo    * linfo,
                       __global    float             * alpha,
                       __global    float              * vis,
                       __global    float16            * vis_sphere )
{
    int gid=get_global_id(0);
    int datasize = linfo->data_len ;
    if (gid<datasize) {
        vis[gid] = -1.0f;

      //check if there is meaningful data here
      if (vis_sphere[gid].sf != -1.0f) {
          //declare ray


          float max_vis = 0.0f;
          float sum_vis = 0.0f;
          float private_vis[12];
          private_vis[0] = vis_sphere[gid].s0;
          private_vis[1] = vis_sphere[gid].s1;
          private_vis[2] = vis_sphere[gid].s2;
          private_vis[3] = vis_sphere[gid].s3;
          private_vis[4] = vis_sphere[gid].s4;
          private_vis[5] = vis_sphere[gid].s5;
          private_vis[6] = vis_sphere[gid].s6;
          private_vis[7] = vis_sphere[gid].s7;
          private_vis[8] = vis_sphere[gid].s8;
          private_vis[9] = vis_sphere[gid].s9;
          private_vis[10] = vis_sphere[gid].sa;
          private_vis[11] = vis_sphere[gid].sb;

          //compute max visibility in normal and opposite hemisphere
          for (unsigned int i = 0; i < 12; i++)
          {
                  max_vis = (max_vis < private_vis[i]) ? private_vis[i] : max_vis;
                  sum_vis+=private_vis[i];
          }
  #ifdef USESUM //use the sum of visibilities for the given hemisphere

            vis[gid] = sum_vis;

  #else //use the max  visibility for the given hemisphere
          vis[gid] =  max_vis;
  #endif //USESUM

          if(vis[gid] < 0.75 )
              alpha[gid] =0.0;
      }
    }
}
#endif //COMPVIS

