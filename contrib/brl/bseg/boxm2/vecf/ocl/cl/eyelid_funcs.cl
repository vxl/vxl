typedef struct{
   float scale_x;
   float scale_y;
   float dphi_rad;
   float8 t0;
   float8 t1;
   float t_min,t_max;
} Eyelid;

typedef struct{
   float scale_x;
   float scale_y;
   float crease_scale_y;
   float dphi_rad;
   float8 t0;
   float8 t1;
} EyelidCrease;
int is_valid_t(float t,__local Eyelid* eyelid){
  float tolerance = 0.1;
  return (t>=(eyelid->t_min - tolerance) && t <= (eyelid->t_max + tolerance));
}
void eyelid_from_float_arr(__local Eyelid* eyelid , __global float* data){
  eyelid->t0 = (float8)(0,0,0,0,0,0,0,0);
  eyelid->t1 = (float8)(0,0,0,0,0,0,0,0);
__local   float* coefs_ptr_0 = (__local float*)&eyelid->t0;
__local   float* coefs_ptr_1 = (__local float*)&eyelid->t1;
  for(int i = 0; i<5;i++){
    coefs_ptr_0[i]   = data[i];
    coefs_ptr_1[i] = data[i+5];
  }

  eyelid->scale_x  = data[10];
  eyelid->scale_y  = data[11];
  eyelid->dphi_rad = data[12];
  eyelid->t_min = data[13];
  eyelid->t_max = data[14];

}
float dot_product(float8 a, float8 b){
  return a.s0 * b.s0 + a.s1 * b.s1 + a.s2 * b.s2 + a.s3 * b.s3 + a.s4 * b.s4 +a.s5 * b.s5 ;
}
void blended_2nd_order_coefs(float t, float* a0, float* a1,float* a2,__local Eyelid* eyelid) {
  float8 c0 = eyelid->t0;
  float8 c1 = eyelid->t1;
  *a0 = (1-t)*c0.s0 + t*c1.s0;
  *a1 = (1-t)*c0.s1 + t*c1.s1;
  *a2 = (1-t)*c0.s2 + t*c1.s2;
}


float beta( float xp, float a0,  float a1,  float a2,__local Eyelid* eyelid){
  if(fabs(eyelid->dphi_rad)<0.001)
    return xp;
  float sy = eyelid->scale_y;
  float ss = sin(eyelid->dphi_rad);
  float cs = cos(eyelid->dphi_rad);
  float temp0 = a0*sy*ss, temp1 = a1*sy*ss, temp2 = a2*sy*ss;
  float temp3 = 1.0 -(temp1/cs);
  float temp4 = -(4.0*temp2*(xp + temp0)/(cs*cs))+temp3*temp3;
  if(temp4<0.0)
    temp4 = 0.0;
  float neu = temp1 + cs*(-1.0 + sqrt(temp4));
  float ret = -neu/(2.0*temp2);
  return ret;

}
float8 eyelid_monomials( float xp){
  float8 q;
    q.s0 = 1.0; q.s1 = xp; q.s2 = q.s1 * q.s1; q.s3 = q.s2 * q.s1; q.s4 = q.s3 * q.s1;
    return q;
}
float compute_t0( float xp, float y,__local Eyelid* eyelid) {
  float8 c0 = eyelid->t0;
  float8 c1 = eyelid->t1;
  float xs = xp/eyelid->scale_x;
  float8 m = eyelid_monomials(xs);
  float temp =  dot_product(m, c0) * eyelid->scale_y;
  float temp1 = dot_product(m, c1) * eyelid->scale_y;
  temp  = xs * sin(eyelid->dphi_rad) + temp  * cos(eyelid->dphi_rad);
  temp1 = xs * sin(eyelid->dphi_rad) + temp1 * cos(eyelid->dphi_rad);
  float ret = (-temp + y)/(temp1 -temp );
  return ret;
}

float compute_t(float xp, float y,__local Eyelid* eyelid){
  float ts = compute_t0(xp, y,eyelid);
  float a0, a1, a2;
  blended_2nd_order_coefs(ts, &a0, &a1, &a2,eyelid);
  float xb = beta(xp,a0,a1,a2,eyelid);
  float xs = xb/eyelid->scale_x;
  float8 m = eyelid_monomials(xs);
  float8 c0 = eyelid->t0;
  float8 c1 = eyelid->t1;
  float temp  = dot_product(m, c0) * eyelid->scale_y;
  float temp1 = dot_product(m, c1) * eyelid->scale_y;
  temp  = xs * sin(eyelid->dphi_rad) + temp  * cos(eyelid->dphi_rad);
  temp1 = xs * sin(eyelid->dphi_rad) + temp1 * cos(eyelid->dphi_rad);
  float ret = (-temp + y)/(temp1 -temp );
  return ret;
}
