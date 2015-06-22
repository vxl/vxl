#define LERP(w1,w2,p,p1,p2) (w1 * (p2 - p) + w2 * (p-p1))/(p2 - p1)
#define LERP24(w1,w2,p,p1,p2,out) for(unsigned i=0;i<24; out[i] = LERP(w1[i],w2[i],p,p1,p2),i++)

float interp_generic_float(float4* neighbors, float* probs, float4 p ){
  float dx00 = LERP(probs[0],probs[2],  p.x,neighbors[0].x,neighbors[2].x); // interp   between (x0,y0,z0) and (x1,y0,z0)
  float dx10 = LERP(probs[1],probs[3],  p.x,neighbors[1].x,neighbors[3].x); // interp   between (x0,y1,z0) and (x1,y1,z0)
  float dx01 = LERP(probs[4],probs[6],  p.x,neighbors[4].x,neighbors[6].x); // interp   between (x0,y0,z1) and (x0,y0,z1)
  float dx11 = LERP(probs[5],probs[7],  p.x,neighbors[5].x,neighbors[7].x); // interp x between x-1 and x+1 z = 1 y =1


  float dxy0 = LERP( dx00, dx10,p.y,neighbors[0].y,neighbors[1].y);
  float dxy1 = LERP( dx01, dx11,p.y,neighbors[0].y,neighbors[1].y);
  float dxyz = LERP( dxy0, dxy1,p.z,neighbors[0].z,neighbors[4].z);

  return dxyz;

}
uchar16 interp_mog_view_dep(float4* neighbors, uchar16* params,float4 p){

  float8 means[8],vars[8];
  for(unsigned i= 0;i<8;i++){
	means[i] = convert_float8(params[i].even)/255.0f;
	vars[i]  = convert_float8(params[i].odd)/255.0f;
}

  float8 dx00 = LERP(means[0],means[2],  p.x,neighbors[0].x,neighbors[2].x); // interp   between (x0,y0,z0) and (x1,y0,z0)
  float8 dx10 = LERP(means[1],means[3],  p.x,neighbors[1].x,neighbors[3].x); // interp   between (x0,y1,z0) and (x1,y1,z0)
  float8 dx01 = LERP(means[4],means[6],  p.x,neighbors[4].x,neighbors[6].x); // interp   between (x0,y0,z1) and (x0,y0,z1)
  float8 dx11 = LERP(means[5],means[7],  p.x,neighbors[5].x,neighbors[7].x); // interp x between x-1 and x+1 z = 1 y =1


  float8 dxy0 = LERP( dx00, dx10,p.y,neighbors[0].y,neighbors[1].y);
  float8 dxy1 = LERP( dx01, dx11,p.y,neighbors[0].y,neighbors[1].y);
  float8 dxyz = LERP( dxy0, dxy1,p.z,neighbors[0].z,neighbors[4].z);

  uchar16 final_value;
  final_value.even=convert_uchar8(dxyz*255.0f);
  final_value.odd =convert_uchar8(vars[0]);
  return final_value;

}

int16 interp_mog_view_dep_color(float4* neighbors, int16* params_compact,float4 p){
  float params[8][24];    float vars[8][24];
  int16 output = (int16)(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

  for (short j = 0;j < 8; j++)
    unpack_rgb_mean(&params_compact[j],params[j],vars[j]);


  float dx00[24],dx10[24],dx01[24],dx11[24],dxy0[24],dxy1[24],dxy2[24],dxyz[24];
  LERP24(params[0],params[2],  p.x,neighbors[0].x,neighbors[2].x,dx00); // interp   between (x0,y0,z0) and (x1,y0,z0)
  LERP24(params[1],params[3],  p.x,neighbors[1].x,neighbors[3].x,dx10); // interp   between (x0,y1,z0) and (x1,y1,z0)
  LERP24(params[4],params[6],  p.x,neighbors[4].x,neighbors[6].x,dx01); // interp   between (x0,y0,z1) and (x0,y0,z1)
  LERP24(params[5],params[7],  p.x,neighbors[5].x,neighbors[7].x,dx11); // interp x between x-1 and x+1 z = 1 y =1

  LERP24( dx00, dx10,p.y,neighbors[0].y,neighbors[1].y,dxy0);
  LERP24( dx01, dx11,p.y,neighbors[0].y,neighbors[1].y,dxy1);
  LERP24( dxy0, dxy1,p.z,neighbors[0].z,neighbors[4].z,dxyz);


  pack_rgb_mean(&output,dxyz,vars[0]);
  return output;

}

float4 interp_flow(float4* neighbors, float4* flow,float4 p){
  float4 dx00 = LERP(flow[0],flow[2],  p.x,neighbors[0].x,neighbors[2].x); // interp   between (x0,y0,z0) and (x1,y0,z0)
  float4 dx10 = LERP(flow[1],flow[3],  p.x,neighbors[1].x,neighbors[3].x); // interp   between (x0,y1,z0) and (x1,y1,z0)
  float4 dx01 = LERP(flow[4],flow[6],  p.x,neighbors[4].x,neighbors[6].x); // interp   between (x0,y0,z1) and (x0,y0,z1)
  float4 dx11 = LERP(flow[5],flow[7],  p.x,neighbors[5].x,neighbors[7].x); // interp x between x-1 and x+1 z = 1 y =1


  float4 dxy0 = LERP( dx00, dx10,p.y,neighbors[0].y,neighbors[1].y);
  float4 dxy1 = LERP( dx01, dx11,p.y,neighbors[0].y,neighbors[1].y);
  float4 dxyz = LERP( dxy0, dxy1,p.z,neighbors[0].z,neighbors[4].z);

  return dxyz;

}
