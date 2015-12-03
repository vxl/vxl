typedef struct{
   float scale_x;
   float scale_y;
   float dphi_rad;
   float8 t0;
   float8 t1;
   float t_min,t_max;
   float mid_superior_margin_z, mid_inferior_margin_z;
   float medial_socket_radius, lateral_socket_radius;
   float eyelid_radius;
} Eyelid;

typedef struct{
   float scale_x;
   float scale_y;
   float crease_scale_y;
   float dphi_rad;
   float8 t0;
   float8 t1;
} EyelidCrease;
int is_valid_t(float t,__local Eyelid* eyelid,float tolerance){

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
  eyelid->mid_superior_margin_z = data[15];
  eyelid->mid_inferior_margin_z = data[16];
  eyelid->medial_socket_radius  = data[17];
  eyelid->lateral_socket_radius = data[18];
  eyelid->eyelid_radius         = data[19];
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
float gi(float xp, float t,__local Eyelid* eyelid) {
  float a0, a1, a2;
  blended_2nd_order_coefs(t, &a0, &a1, &a2,eyelid);
  float xb = beta(xp, a0, a1, a2, eyelid);
  float xs = xb/eyelid->scale_x;
  float8 m = eyelid_monomials(xs);
  float8 c0 = eyelid->t0;
  float8 c1 = eyelid->t1;
  float dp0 = dot_product(m, c0 ) * eyelid->scale_y, dp1 = dot_product(m, c1 ) * eyelid->scale_y;
  dp0 = xs * sin(eyelid->dphi_rad) + dp0 * cos(eyelid->dphi_rad);
  dp1 = xs * sin(eyelid->dphi_rad) + dp1 * cos(eyelid->dphi_rad);
  float t0 = dp0 * (1.0 - t);
  float t1 = dp1 * t;
  return (t0 + t1);
}

float zlim(float xp,__local Eyelid* eyelid){
  float xlim = eyelid->lateral_socket_radius;
  if(xp < 0.0)
    xlim = eyelid->medial_socket_radius;
  float er = eyelid->eyelid_radius; ////JLM
  float arg = er * er - xlim * xlim;
  if(arg<0.0) return 0.0;
  float zr = sqrt(arg);
  return zr;
}

void extrema(float sy, float a0, float a1, float a2, float* yext, float* xext,__local Eyelid* eyelid){
  float ss = sin(eyelid->dphi_rad);
  float cs = cos(eyelid->dphi_rad);
  float tan = ss/cs;
  float temp0 = (a1 * a1 -4.0 * a0 * a2) * sy * sy * cs;
  float temp1 = ss * (2.0 * a1 *sy + tan);
  *yext = -(temp0 + temp1)/(4.0 * a2 * sy);
  float beta_ext = -(a1 * sy + tan)/(2.0*a2*sy);
  float temp2    = a0 + a1 * beta_ext + a2 * beta_ext*beta_ext;
  *xext = cs * beta_ext -ss * sy * temp2;
}

float lin_interp_z(float xp, float mid_z, float t, float sy, float a0, float a1, float a2,__local Eyelid* eyelid){
  // find extrema in the y excursion and the x at which it happens
  // uses 2nd order polynomial coeficients
  float yext = 0.0, xext = 0.0;
  extrema(sy, a0, a1, a2, &yext, &xext,eyelid);
  float xlat = eyelid->lateral_socket_radius, xmed = -eyelid->medial_socket_radius;
  float ylat = gi(xlat, t,eyelid), ymed = gi(xmed, t,eyelid), yx =gi(xp,t,eyelid);
  float ret_z = 0.0, s = 0.0;

  float zl = zlim(xp-xext,eyelid);//switch zlim values at xp == xext, not xp == 0

  float zlp = zlim(1.0,eyelid);   // zlimit value when xp is plus
  float zlm = zlim(-1.0,eyelid);  // zlimit value when xp is minus

  bool less_p = mid_z<zlp;  // the mid z value is less than the plus z limit
  bool less_m = mid_z<zlm;  // the mid z value is less than the minus z limit

  bool not_less = !less_p && !less_m; // the mid z value is greater than either z limit
  bool plus = xp>=xext;               // x is greater than or equal to the extremum  x

  // compute the interpolating weight and interpolated z
  if(plus)
    s = (yx-yext)/(ylat-yext);
  else
    s = (yx-yext)/(ymed-yext);
   ret_z = mid_z*(1-s) + zl*s;

   // cases where the interpolated z value is clamped
   // Case I - zmid is greater than either limit
   if(not_less&&(ret_z<zl))
     ret_z = zl;

   // Case II - zmid is less than the plus z limit
   if(plus && less_p && (ret_z>zl))
     ret_z = zl;

   // Case III - zmid is less than the minus z limit
   if(!plus && less_m && (ret_z>zl))
     ret_z = zl;
  return ret_z;
}

float z(float xp, float t,__local Eyelid* eyelid){
  float a0, a1, a2, sy = eyelid->scale_y;
  blended_2nd_order_coefs(t, &a0, &a1, &a2, eyelid);
  float xlim= eyelid->lateral_socket_radius;
  if(xp<0.0)
    xlim = eyelid->medial_socket_radius;
  if((fabs(xp)<xlim))
    return lin_interp_z(xp, eyelid->mid_superior_margin_z, t, sy, a0, a1, a2,eyelid);
  else
    return zlim(xp,eyelid);
}
inline float r(float xp, float t,__local Eyelid* eyelid){
  float zv = z (xp, t, eyelid);
  float gv = gi(xp, t, eyelid);
  return sqrt(zv*zv + gv*gv + xp*xp);
}

inline float theta(float xp, float t,__local Eyelid* eyelid){
  float zv = z(xp,t,eyelid);
  float rv = r(xp,t,eyelid);
  return acos(zv/rv);
}
inline float phi(float xp, float t,__local Eyelid* eyelid){
  return atan2(gi(xp,t,eyelid), xp);
}

inline float lid_X(float xp,float t,__local Eyelid* eyelid){
  return (r(xp,t,eyelid) * sin(theta(xp,t,eyelid) ) * cos(phi(xp,t,eyelid)));
}

inline float lid_Y(float xp,float t,__local Eyelid* eyelid){
  return (r(xp,t,eyelid) * sin(theta(xp,t,eyelid)) * sin(phi(xp,t,eyelid)));
}

inline float lid_Z(float xp,float t,__local Eyelid* eyelid){
  return (r(xp,t,eyelid) * cos(theta(xp,t,eyelid)));
}

float4  lid_vf(float xp, float t, float dt,__local Eyelid* eyelid){
  float4 X0 = (float4) (lid_X(xp,t,eyelid),    lid_Y(xp,t,eyelid),    lid_Z(xp,t,eyelid)   ,0);
  float4 Xt = (float4) (lid_X(xp,t+dt,eyelid), lid_Y(xp,t+dt,eyelid), lid_Z(xp,t+dt,eyelid),0);
  return (Xt-X0);
}
