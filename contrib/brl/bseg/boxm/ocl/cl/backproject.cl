#if 0
float3 backproject(unsigned i,unsigned j,float16 Ut,float16 V,float16 w, float3 origin)
{
  float3 inputpoint=(float3) {i+0.1f, j+0.1f, 1.0f};

  float4 X=(float4)(f4f3dot((float4)(Ut.s0123),inputpoint),
                    f4f3dot((float4)(Ut.s4567),inputpoint),
                    f4f3dot((float4)(Ut.s89ab),inputpoint),
                    f4f3dot((float4)(Ut.scdef),inputpoint));

  X *= (float4) (w.s0123);

  X  = (float4)(dot((float4)(V.s0123),X),
                dot((float4)(V.s4567),X),
                dot((float4)(V.s89ab),X),
                dot((float4)(V.scdef),X));

  X /= X.w;
  X  = normalize(X-(float4)(origin.x, origin.y, origin.z, 1.0));
  return (float3) {X.x, X.y, X.z};
}
#endif

float4 backproject(unsigned i,unsigned j,float16 Ut,float16 V,float16 w, float4 origin)
{
  float4 inputpoint=(float4)((float)i+0.1f,(float)j+0.1f,1.0f,0.0f);

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


bool project(float16 cam, float4 point3d, float2 * point2d)
{
    point3d.w=1;
    float u = dot((float4)(cam.s0123),point3d);
    float v = dot((float4)(cam.s4567),point3d);

    float denom=dot((float4)(cam.s89ab),point3d);

    if (denom==0.0f)
        return false;
    (*point2d)=(float2)(u/denom,v/denom);
    return true;
}
