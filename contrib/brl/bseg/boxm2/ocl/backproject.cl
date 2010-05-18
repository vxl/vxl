float4 backproject(unsigned i,unsigned j,float16 Ut,float16 V,float16 w, float4 origin)
{
  float4 inputpoint=(float4)(i,j,1.0,0.0);

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