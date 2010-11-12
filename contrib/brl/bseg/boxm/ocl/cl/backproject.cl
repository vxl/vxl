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
