//: Backproject and project functions, using float16 for cameras

float4 backproject(unsigned i,unsigned j,float16 Ut,float16 V,float16 w, float4 origin)
{
  float4 inputpoint=(float4)((float)i+0.5f,(float)j+0.5f,1.0f,0.0f);

  float4 X=(float4)(dot((float4)(Ut.s0123),inputpoint),
                    dot((float4)(Ut.s4567),inputpoint),
                    dot((float4)(Ut.s89ab),inputpoint),
                    dot((float4)(Ut.scdef),inputpoint));

  X=X*(float4)(w.s0123);

  X=(float4)(dot((float4)(V.s0123),X),
             dot((float4)(V.s4567),X),
             dot((float4)(V.s89ab),X),
             dot((float4)(V.scdef),X));


  X=normalize(X-(float4)(origin.xyzw)*X.w);

  return X;
}

float4 backproject_corner(float i, float j,float16 Ut,float16 V,float16 w, float4 origin)
{
  float4 inputpoint=(float4)(i,j,1.0f,0.0f);

  float4 X=(float4)(dot((float4)(Ut.s0123),inputpoint),
                    dot((float4)(Ut.s4567),inputpoint),
                    dot((float4)(Ut.s89ab),inputpoint),
                    dot((float4)(Ut.scdef),inputpoint));

  X=X*(float4)(w.s0123);

  X=(float4)(dot((float4)(V.s0123),X),
             dot((float4)(V.s4567),X),
             dot((float4)(V.s89ab),X),
             dot((float4)(V.scdef),X));

  X= X / X.w;

  X=normalize(X-(float4)(origin.xyzw));

  return X;
}


bool project(__global float16 * cam, float4 p3d, float2 * p2d)
{
    p3d.w=1;
    float u = dot((float4)((*cam).s0123),p3d);
    float v = dot((float4)((*cam).s4567),p3d);

    float denom=dot((float4)((*cam).s89ab),p3d);

    if (denom==0.0f)
        return false;
    (*p2d)=(float2)(u/denom,v/denom);
    return true;
}

//utility method that uses backproject to output a single ray (6 floats) from a global cam
//uses perspective camera
bool calc_scene_ray(__constant RenderSceneInfo * linfo,
                    __global float16 * camera,
                    int i, int j,
                    float* ray_ox, float* ray_oy, float* ray_oz,
                    float* ray_dx, float* ray_dy, float* ray_dz)
{

  float4 ray_o = (float4) camera[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, camera[0], camera[1], camera[2], ray_o);
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f);
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0f; ray_d = normalize(ray_d);

  //store float 3's
  *ray_ox = ray_o.x;     *ray_oy = ray_o.y;     *ray_oz = ray_o.z;
  *ray_dx = ray_d.x;     *ray_dy = ray_d.y;     *ray_dz = ray_d.z;

  return true;
}


bool calc_scene_ray_generic_cam(__constant RenderSceneInfo * linfo,
                                float4 ray_o, float4 ray_d,
                                float* ray_ox, float* ray_oy, float* ray_oz,
                                float* ray_dx, float* ray_dy, float* ray_dz)
{
  //make sure w is one
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f);
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0f; ray_d = normalize(ray_d);

  //store float 3's
  *ray_ox = ray_o.x;     *ray_oy = ray_o.y;     *ray_oz = ray_o.z;
  *ray_dx = ray_d.x;     *ray_dy = ray_d.y;     *ray_dz = ray_d.z;

  return true;
}
