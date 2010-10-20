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

float projectU(float16 cam, float4 p3d)
{
    p3d.w = 1.0f;
    float u = dot((float4)(cam.s0123),p3d);
    float div=dot((float4)(cam.s89ab),p3d);
    return (u/div);
}
float projectV(float16 cam, float4 p3d)
{
    p3d.w = 1.0f;
    float v = dot((float4)(cam.s4567),p3d);
    float div=dot((float4)(cam.s89ab),p3d);
    return (v/div);
}
