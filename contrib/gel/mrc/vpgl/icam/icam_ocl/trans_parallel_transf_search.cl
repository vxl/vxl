
void clear_array(uint n_bins, __local uint* array)
{
  uint lid = get_local_id(0);
  uint wgsize = get_local_size(0);
  uint n = n_bins/wgsize;

  for(uint i = 0; i<n; ++i)
    array[lid + i*wgsize] = 0;

  uint nplus = n_bins%wgsize;
  if(lid<nplus)
    array[n*wgsize + lid]=0;
}

void update_array(__local uint* array, uint bin, uint lid)
{
  uint ltag = lid<<27;
  uint count;
  do{
    count = array[bin] & 0x07FFFFFFU;
    count = ltag | (count + 1);
    array[bin] = count;
  }while(array[bin] != count);
}
uint array_count(__local uint* array, uint bin)
{
  return array[bin] & 0x07FFFFFFU;
}
/* extract a rotation matrix from the linear rodrigues vector array */

void rot_matrix(__global float4* rod, __local float4* r0, __local float4* r1,
                __local float4* r2)
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
void dtrans(__local float4* tr, __local float4* r0, __local float4* r1,
            __local float4* r2,
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
  float sorg = source[org], sorg1 = source[org+1];
  float i1 = (sorg+(source[org+sni]-sorg)*normv);
  float i2 = sorg1+(source[org+1+sni]-sorg1)*normv;
  float val = i1+(i2-i1)*normu;
  return val;
}

void map_to_dest(uint j,uint sni, uint snj, float4* Ks,
                 __global float* source, uint dni, uint dnj,
                 float4* Kdi, __global float* dest,
                 __global float* inv_depth, 
                 __local float4* tr, 
                 __local float4* r0, __local float4* r1, __local float4* r2,
                 uint nbins,
                 float scl,
                 __local uint* mdhist,
                 __local uint* histogram,
                 __global float* mdest_img)
{

  uint wgsize = get_local_size(0);
  uint ngrps = dni/wgsize;
  uint lid = get_local_id(0);
  uint off = dni*j;
  for(uint ig = 0; ig<ngrps; ++ig)
    {
      uint i = ig*wgsize + lid;
      float u=0.0f, v=0.0f, msk=1.0f;
      dtrans(tr, r0, r1, r2,inv_depth, dni, dnj, 
             Kdi, sni, snj, Ks, i, j, &u, &v, &msk);
      int iu = floor(u), iv = floor(v);
      bool in_src = iu>=0 && iv>=0 && iu<sni && iv<snj;
      float val = -1;
      val = in_src ? bilin(source, sni, snj, u, v) : -1.0f;
      msk = val>=0.0f ? msk : 0.0f;
      uint uval = (uint)(val*scl);
      float dval = dest[i+off];
      uint udval = (uint)(dval*scl);
      uint hindx = uval + nbins*udval;
      if(msk>0){
        update_array(histogram, hindx, lid); 
        update_array(mdhist, uval, lid); 
      }
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  /* extra pixels to process */
  uint nloc = dni%wgsize;
  if(nloc>0){
    uint i = ngrps*wgsize + lid;
    if(i<(dni-1)){
      float u=0.0f, v=0.0f, msk=1.0f;
      dtrans(tr, r0, r1, r2,inv_depth, dni, dnj, 
             Kdi, sni, snj, Ks, i, j, &u, &v, &msk);
      int iu = floor(u), iv = floor(v);
      bool in_src = iu>=0 && iv>=0 && iu<sni && iv<snj;
      float val = -1;
      val = in_src ? bilin(source, sni, snj, u, v) : -1.0f;
      msk = val>=0.0f ? msk : 0.0f;
      uint uval = (uint)(val*scl);
      float dval = dest[i+off];
      uint udval = (uint)(dval*scl);
      uint hindx = uval + nbins*udval;
      if(msk>0){
        update_array(histogram, hindx, lid); 
        update_array(mdhist, uval, lid); 
      }
    }
  }
}    

/* the group size is greater than or equal to 1/2 the number of bins */
/* initialize the entropy sum and the bin counts  */
void H_init(uint n_bins, __local uint* hist, __local float* reduc_buf)
{
  uint half  = n_bins/2; /* assume divides evenly for now */
  uint wgsize = get_local_size(0);
  uint lid = get_local_id(0);
  if(lid>=half)
    return;
  /*add the entropies into the buffer using half the threads */
  float hi0 = (float)array_count(hist,lid); 
  float hi1 = (float)array_count(hist,(lid+half));
  float ent0 = hi0 > 0.0f ? hi0*log(hi0) : 0.0f;
  float ent1 = hi1 > 0.0f ? hi1*log(hi1) : 0.0f;
  reduc_buf[lid] = ent0 + ent1; /*entropy of counts */
  reduc_buf[lid+wgsize] = hi0 + hi1;/* just the sum of counts*/
 }

/* 
 *   if the work group size is < than 1/2 the number of bins      
 *        wg             wg              wg           partial wg
 * [---------------:---------------:---------------:------------] nbins
 *     \           /                /
 *      \         sum         /sum
 *       sum     /       /    
 *        \     sum entropy        sum counts
 *         [-----------------:------------------]  reduc_buf
 *          <---- wgsize ----><-----wgsize----->
 */
void H_sum(uint n_bins, __local uint* hist, __local float* reduc_buf)
{
  uint wgsize = get_local_size(0);
  uint lid = get_local_id(0);
  uint nsums = n_bins/wgsize;
  /*move the first wgrp interval into the buffer */
  float hi = (float)array_count(hist,lid);
  float ent = hi>0.0f ? hi*log(hi) : 0.0f;
  reduc_buf[lid] = ent; /*entropy of counts */
  reduc_buf[lid+wgsize] = hi;/* just the sum of counts*/
  barrier(CLK_LOCAL_MEM_FENCE);
  /* step across and sum using each thread to add in parallel*/
  uint off;
  for(uint i = 1; i<nsums; ++i)
    {
      off = lid+i*wgsize;
      hi = (float)array_count(hist,off);
      ent = hi>0.0f ? hi*log(hi) : 0.0f;
      reduc_buf[lid]+=ent;
      reduc_buf[lid+wgsize]+=hi;
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  uint n_plus = n_bins%wgsize;
  if(n_plus>0){
    off = lid + nsums*wgsize;
    if(off<n_bins){
      hi = (float)array_count(hist,off);
      ent = hi>0.0f ? hi*log(hi) : 0.0f;
      reduc_buf[lid]+=ent;
      reduc_buf[lid+wgsize]+=hi;
    }
  }
}

/* the work group size is greater than or equal to 1/2 the number of bins */
/* sum using reduction - assume the reduc buf holds the entropies already*/
void H_reduce(uint n_bins,  __local float* reduc_buf)
{
  uint half  = n_bins/2; /* assume divides evenly for now */
  uint wgsize = get_local_size(0);
  uint lid = get_local_id(0);
  for(unsigned int s = half; s > 0; s >>= 1) 
    {
        if(lid < s) 
        {
            reduc_buf[lid] += reduc_buf[lid + s];
            reduc_buf[lid+wgsize] += reduc_buf[lid+wgsize+s];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
  /* at this point the sums are available */
  /* store in location 0 of the reduc_buf */
  if(lid == 0){
    float nsamp = reduc_buf[wgsize];
    float entsum = reduc_buf[0];
    float H = -(entsum/nsamp - log(nsamp));
    reduc_buf[0]=H;
  }
}

float mutual_info(uint n_bins, __local uint* mdhist, __local uint* histogram,
                  __local float* reduc_buf)
{
  float H_mdest, H_joint;
  uint wgsize = get_local_size(0);
  uint lid = get_local_id(0);
  /* note that the size of reduc_buf is 2*wgsize */
  /* entropy of mdhist */
  if(wgsize >= n_bins/2){
    H_init(n_bins, mdhist, reduc_buf);
    barrier(CLK_LOCAL_MEM_FENCE);
    H_reduce(n_bins/2, reduc_buf);
  }else{
    H_sum(n_bins, mdhist, reduc_buf);
    barrier(CLK_LOCAL_MEM_FENCE);
    H_reduce(wgsize, reduc_buf);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  H_mdest = reduc_buf[0];
  clear_array(2*wgsize, reduc_buf);
  barrier(CLK_LOCAL_MEM_FENCE);
  /*  joint entropy  */
  if(wgsize >= n_bins*n_bins/2){
    H_init(n_bins, histogram, reduc_buf);
    barrier(CLK_LOCAL_MEM_FENCE);
    H_reduce(n_bins*n_bins, reduc_buf);
  }else{
    H_sum(n_bins*n_bins, histogram, reduc_buf);
    barrier(CLK_LOCAL_MEM_FENCE);
    H_reduce(wgsize, reduc_buf);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  H_joint = reduc_buf[0];
  float minf = H_mdest - H_joint;
  return minf/log(2.0f);/* convert to bits */
}

__kernel void 
trans_parallel_transf_search(__global uint* n_bins, /* histogram bins */
                             /* source img */
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
                             __global float* minfo,/*output minfo array */
                             __global int4* debug_flag, /* debug flag */
                             /* cached translation */
                             __local float4* tr,
                             /* cached rotation matrix */
                             __local float4* r0,
                             __local float4* r1,
                             __local float4* r2,
                             /* local data to compute mutual info */
                             __local uint* mdhist,
                             __local uint* histogram,
                             __local float* reduc_buf
                             )
{
  /* 1-d workgroup - process image by rows */
  uint wgid  = get_group_id(0);
  uint wgsize = get_local_size(0);/* 1-d workspace */
  uint nb = *n_bins;
  float4 Ksr = *Ks, Kdir = *Kdi;/* put in local registers*/
  uint snir = *sni, snjr = *snj, dnir = *dni, dnjr = *dnj;
  float scl = nb/255.0f;
  /* Thread 0 transfers transformation data to local memory */
  uint lid = get_local_id(0);
  if(lid == 0){
    *tr = *trans;
    rot_matrix(rotr+wgid, r0, r1, r2);
  }
  /* Synchronize the treads */
  barrier(CLK_LOCAL_MEM_FENCE);
  /* Clear the histogram */
  clear_array(nb*nb, histogram);
  barrier(CLK_LOCAL_MEM_FENCE);
  clear_array(nb, mdhist);
  barrier(CLK_LOCAL_MEM_FENCE);
  clear_array(2*wgsize, reduc_buf);
  barrier(CLK_LOCAL_MEM_FENCE);
  /* Map the destination image */
  /* each thread processes the same row */
  for(uint j = 0; j<(dnjr-1); ++j){

    /* Map to dest and update histogram */
    map_to_dest(j, snir, snjr, &Ksr, source, 
                dnir, dnjr, &Kdir, dest, inv_depth,
                tr, r0, r1, r2, nb, scl, mdhist, histogram, minfo);
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  /* Compute mutual information and output locally*/
  barrier(CLK_LOCAL_MEM_FENCE);
  float inf = mutual_info(nb, mdhist, histogram,
                          reduc_buf);
 
  barrier(CLK_LOCAL_MEM_FENCE);
  if(lid == 0){
    minfo[wgid] = inf;
    *debug_flag = (int4)(1000.0f*inf,0,0,0);
  }
}
