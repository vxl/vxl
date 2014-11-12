//:
// \file

#ifdef DODECAHEDRON
//: Interpolate normal from the responses to kernels in the directions of dodecahedron vertices
__kernel
void
aggregate(__constant  RenderSceneInfo    * linfo,
          __constant  float4             * directions,
          __global    float4             * normals,
          __global    float              * response0,
          __global    float              * response1,
          __global    float              * response2,
          __global    float              * response3,
          __global    float              * response4,
          __global    float              * response5 )
{
    int gid=get_global_id(0);
    int datasize = linfo->data_len ;//* info->num_buffer;

    if (gid<datasize) {
        normals[ gid ] = (float4)(0,0,0,0);

        float responses[6];
        responses[0] = response0[gid];
        responses[1] = response1[gid];
        responses[2] = response2[gid];
        responses[3] = response3[gid];
        responses[4] = response4[gid];
        responses[5] = response5[gid];

        float normal_x = 0, normal_y = 0, normal_z = 0;
        float sum_weights_x = 0, sum_weights_y = 0, sum_weights_z = 0;
        for (unsigned response_id = 0; response_id < 6; response_id++)
        {
            normal_x += responses[response_id] * directions[response_id].x;
            normal_y += responses[response_id] * directions[response_id].y;
            normal_z += responses[response_id] * directions[response_id].z;
            sum_weights_x += fabs( directions[response_id].x);
            sum_weights_y += fabs( directions[response_id].y);
            sum_weights_z += fabs( directions[response_id].z);
        }
        normal_x /= sum_weights_x;  normal_y /= sum_weights_y; normal_z /= sum_weights_z;

        float norm = length( (float4)(normal_x,normal_y,normal_z,0.0));
        if (norm > 0.0) {
            normal_x /= norm; normal_y /= norm; normal_z /= norm;
            normals[gid] = (float4)(normal_x,normal_y,normal_z,norm);
        }
    }
}

#endif //DODECAHEDRON

#ifdef XYZ
//: Interpolate normal from the responses to kernels in the directions of X, Y, Z axis
__kernel
void
aggregate(__constant  RenderSceneInfo      * linfo,
          __constant  float4               * directions,
          __global    float4               * normals,
          __global    const float          * response0,
          __global    const float          * response1,
          __global    const float          * response2)
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len ;//* info->num_buffer;

  if (gid<datasize) {
    normals[ gid ] = (float4)(0,0,0,0);

    float responses[3];
    responses[0] = response0[gid];
    responses[1] = response1[gid];
    responses[2] = response2[gid];


    float normal_x = 0, normal_y = 0, normal_z = 0;
    float sum_weights_x = 0, sum_weights_y = 0, sum_weights_z = 0;

    normal_x = responses[0];
    normal_y = responses[1];
    normal_z = responses[2];

    for (unsigned response_id = 0; response_id < 3; response_id++)
    {
      normal_x += responses[response_id] * directions[response_id].x;
      normal_y += responses[response_id] * directions[response_id].y;
      normal_z += responses[response_id] * directions[response_id].z;
      sum_weights_x += fabs( directions[response_id].x);
      sum_weights_y += fabs( directions[response_id].y);
      sum_weights_z += fabs( directions[response_id].z);
    }

    normal_x /= sum_weights_x;  normal_y /= sum_weights_y; normal_z /= sum_weights_z;
    float norm = length( (float4)(normal_x,normal_y,normal_z,0.0));

    if (norm > 0.0) {
      normal_x /= norm; normal_y /= norm; normal_z /= norm;
      normals[gid] = (float4)(normal_x,normal_y,normal_z,norm);
    }
  }
}

#endif //XYZ

