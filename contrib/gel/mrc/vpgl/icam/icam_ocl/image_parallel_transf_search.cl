/* extract a rotation matrix from the linear rodrigues vector array */

void rot_matrix(__global float4* rod, float4* r0, float4* r1, float4* r2)
{
  (*r0).w = 0.0f;  (*r1).w = 0.0f; (*r2).w = 0.0f;
  /* rod is a pointer to a rodrigues vector */
  float ang = length(*rod);
  if(ang < 1e-3f){
    (*r0).x = 1.0f;  (*r0).y = 0.0f; (*r0).z = 0.0f;
    (*r1).x = 0.0f;  (*r1).y = 1.0f; (*r1).z = 0.0f;
    (*r2).x = 0.0f;  (*r2).y = 0.0f; (*r2).z = 1.0f;
    return;
  }
  float ax = (*rod).x/ang, ay = (*rod).y/ang, az = (*rod).z/ang;
  float c = cos(ang);
  float s = sin(ang);
  float t = 1.0f - c;

  (*r0).x = c + ax*ax*t;
  (*r1).y = c + ay*ay*t;
  (*r2).z = c + az*az*t;
  float tmp1 = ax*ay*t;
  float tmp2 = az*s;
  (*r1).x = tmp1 + tmp2;
  (*r0).y = tmp1 - tmp2;
  tmp1 = ax*az*t;
  tmp2 = ay*s;
  (*r2).x = tmp1 - tmp2;
  (*r0).z = tmp1 + tmp2;    
  tmp1 = ay*az*t;
  tmp2 = ax*s;
  (*r2).y = tmp1 + tmp2;
  (*r1).z = tmp1 - tmp2;
}
/* Transform an input image location (udest, vdest)
 * using the perspective depth transform
 */
void dtrans(float4* tr, float4* r0, float4* r1, float4* r2,
            __global float* inv_depth,
            uint dni, uint dnj, float4* Kdi, 
            uint sni, uint snj, float4* Ks,
            uint i, uint j, float* u, float* v, float* msk)
{

  float k00=(*Kdi).x, k02 = (*Kdi).y, k11 = (*Kdi).z, k12 = (*Kdi).w;
  float to_fl = (*Ks).x, to_pu = (*Ks).y, to_pv = (*Ks).w;

  int linvd  = i + dni*j;
  float Zinv = inv_depth[linvd];

  float fi = i, fj = j, fw0, fw1;
  fw0 = k00*fi+ k02;
  fw1 = k11*fj+ k12;
  // trans/Z (note Zinv is reciprocal depth)
  float t0=Zinv*(*tr).x, t1 = Zinv*(*tr).y, t2 = Zinv*(*tr).z;

  float den = (*r2).x*fw0 + (*r2).y*fw1 + (*r2).z+ t2;
  *msk = den>1.0e-6f ? 1.0f : 0.0f;
  den = 1.0f/den;
  float ut = ((*r0).x*fw0 + (*r0).y*fw1 +(*r0).z + t0)*den;
  float vt = ((*r1).x*fw0 + (*r1).y*fw1 +(*r1).z + t1)*den;

  *u = (ut*to_fl + to_pu);
  *v = (vt*to_fl + to_pv);

}

/* Interpolate bilinearly at position (u, v) */
float bilin(__global float* source, uint sni, uint snj, float u, float v)
{
  /* integer and fractional pixel positions */
  int p1x = floor(u);  float normu = u-p1x;
  int p1y = floor(v);  float normv = v-p1y;
  int org = p1x + sni*p1y;
  if(p1x<0||p1y<0||p1x>=(sni-1)||v>=(snj-1))
    return -1.0f;

  /*   normu
   * ------->             
   *  org         org+1   | 
   *                      | normv  
   *        x <-(u,v)     V
   *  org+sni     org+1+sni
   */
  float i1 = (source[org]+(source[org+sni]-source[org])*normv);
  float i2 = source[org+1]+(source[org+1+sni]-source[org+1])*normv;
  float val = i1+(i2-i1)*normu;
  return val;
}

void mapper(__global float* source, __global float* inv_depth, 
            uint dni, uint dnj, float4* Kdi, uint sni, uint snj, 
            float4* Ks, float4* tr, float4* r0, float4* r1, float4* r2,
            __global float* result_img, __global float* mask_img)
{

  uint i = get_global_id(0), j = get_global_id(1);
  uint gni = get_global_size(0);
  int glin = i + gni*j;
  if(i>=dni||j>=dnj) 
    return;
  float u=0.0f, v=0.0f, msk=1.0f;
  dtrans(tr, r0, r1, r2,inv_depth, dni, dnj, 
         Kdi, sni, snj, Ks, i, j, &u, &v, &msk);
  int iu = floor(u), iv = floor(v);
  bool in_src = iu>=0 && iv>=0 && iu<sni && iv<snj;
  float val = -1.0f;
  val = in_src ? bilin(source, sni, snj, u, v) : -1.0f;
  mask_img[glin] = val>=0.0f ? msk : 0.0f;
  result_img[glin] = val;
}

__kernel void 
image_parallel_transf_search(/* source img */
                             __global uint* sni, __global uint* snj, 
                             __global float4* Ks,       /* to_fl, to_p..*/
                             __global float* source, 
                             /* dest img */
                             __global uint* dni, __global uint* dnj, 
                             __global float4* Kdi,     /* inverse Kd*/
                             __global float* dest, 
                             /* inv depth img */
                             __global float* inv_depth,
                             /* transformation */
                             __global float4* rotr,
                             __global float4* trans,
                             __global float4* cost,  /* output cost array */
                             __global int4* cost_flag, /* status flag */
                             /* depth mapped source */
                             __global float* result_img,
                             __global float* mask_img   /* map mask */
                             )
{
  int gsizei = get_global_size(0);
  int gsizej = get_global_size(1);
  int lsizei = get_local_size(0);
  int lsizej = get_local_size(1);

  int i = get_global_id(0), j = get_global_id(1);
  int glin = i + gsizei*j;
#if 0
  if(i>=(*dni)||j>=(*dnj)){
    result_img[glin] = 0.0f;
    return;
  }
#endif
  (*cost_flag) = (int4)(gsizei, gsizej, lsizei, lsizej);

  float4 Kdi_reg = *Kdi, Ks_reg = *Ks;

  float4 r0, r1, r2;
  rot_matrix(rotr, &r0, &r1, &r2);
  float4 tr = *trans;
  mapper(source, inv_depth, *dni, *dnj, &Kdi_reg, *sni, *snj, &Ks_reg, 
         &tr, &r0, &r1, &r2, result_img, mask_img);
  barrier(CLK_LOCAL_MEM_FENCE);
  if(result_img[glin]<0.0f)
    *cost = (float4)(1.0f, 2.0f, 3.0f, 4.0f);
}
        
  
