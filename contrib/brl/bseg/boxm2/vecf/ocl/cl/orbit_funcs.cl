#define SPHERE 0
#define IRIS  1
#define PUPIL 2
#define EYELID 3
#define LOWER_LID 4
#define EYELID_CREASE 5

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

typedef struct{
    __global uchar* sphere;
    __global uchar* iris;
    __global uchar* pupil;
    __global uchar* eyelid;
    __global uchar* lower_lid;
    __global uchar* eyelid_crease;
}OrbitLabels;

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
bool is_anatomy(int anatomy_in,unsigned  data_index,
                __local OrbitLabels* orbit){

#define SPHERES        orbit->sphere        [data_index]
#define IRISES         orbit->iris          [data_index]
#define PUPILS         orbit->pupil         [data_index]
#define EYELIDS        orbit->eyelid        [data_index]
#define LOWER_LIDS     orbit->lower_lid     [data_index]
#define EYELID_CREASES orbit->eyelid_crease [data_index]
  if(anatomy_in == SPHERE)
    return   SPHERES && !IRISES && !PUPILS &&! EYELIDS;
  else if(anatomy_in == IRIS)
    return IRISES  &&  !PUPILS && !EYELIDS;
  else if(anatomy_in == PUPIL)
    return PUPILS && !EYELIDS;
  else if(anatomy_in == LOWER_LID)
    return LOWER_LIDS && !SPHERES && !EYELIDS;
  else if(anatomy_in == EYELID_CREASE)
    return EYELID_CREASES;
  else if (anatomy_in==EYELID)
    return EYELIDS && !SPHERES && !EYELID_CREASES && !LOWER_LIDS;
  else
    return 0;
#undef SPHERES
#undef IRISES
#undef PUPILS
#undef EYELIDS
#undef LOWER_LIDS
#undef EYELID_CREASES

}

int get_ranked_anatomy(unsigned  data_index,
                       __local OrbitLabels* orbit){

#define SPHERES        orbit->sphere        [data_index]
#define IRISES         orbit->iris          [data_index]
#define PUPILS         orbit->pupil         [data_index]
#define EYELIDS        orbit->eyelid        [data_index]
#define LOWER_LIDS     orbit->lower_lid     [data_index]
#define EYELID_CREASES orbit->eyelid_crease [data_index]

 /* if(SPHERES && !EYELIDS && !IRISES && !PUPILS) */
 /*    return  SPHERE; */
 /*  if(IRISES && !PUPILS && !EYELIDS) */
 /*    return IRIS; */
 /*   if(PUPILS && !EYELIDS) */
 /*    return PUPIL; */
if(LOWER_LIDS && !SPHERES && !EYELIDS)
    return LOWER_LID;
  if(SPHERES && !LOWER_LIDS && !EYELIDS)
    return SPHERE;
  if(LOWER_LIDS && !EYELIDS) // sphere and lower lid label
    return LOWER_LID;
  if(SPHERES && !EYELIDS) // sphere and lower lid label
    return SPHERE;
  if(EYELID_CREASES && !EYELIDS)
    return EYELID_CREASE;
  if (EYELIDS )
    return EYELID;
#undef SPHERES
#undef IRISES
#undef PUPILS
#undef EYELIDS
#undef LOWER_LIDS
#undef EYELID_CREASES
   return 6;
}



float8 weight_appearance(float vis_A, float vis_B, float8 int_A,float8 int_B,float8 mean_A){

  int the_dan_way = 1;
  float dominant_vis   = vis_A > vis_B ?  vis_A :  vis_B;

  float8 ret_val;
  if (the_dan_way){
    float s = 10.0f;
    float m = 1.1;
    float v = 0.6f;
    float inv_weight1 = 1.0/(1.0 + exp( -s * (vis_B - m * vis_A)));
    float inv_weight2 = 1.0/(1.0 + exp( -s * (v - vis_A)));
    float weight = 1.0f - inv_weight1 * inv_weight2;
    float8 observed_intensity =  int_A * weight + int_B *  (1.0f - weight);

    s = 16 ; v = 0.3;
    weight = 1.0/(1.0 + exp( -s * ( dominant_vis -v )));
    ret_val = observed_intensity * weight + mean_A * (1.0f - weight);

  }else{
    ret_val = vis_A * int_A + (1 - vis_A) * mean_A;
  }
  return ret_val;

}
