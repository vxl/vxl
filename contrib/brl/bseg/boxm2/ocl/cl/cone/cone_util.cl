//cone ray include functions - factored out code that all cone stuff uses


/////////////////////////////////////////////////////////////////////////////
//Helper methods (that will hopefully, one day, become vector ops)
////////////////////////////////////////////////////////////////////////////////
inline float calc_tfar( float ray_ox, float ray_oy, float ray_oz,
                        float ray_dx, float ray_dy, float ray_dz,
                        float max_facex, float max_facey, float max_facez)
{
  return min(min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
}

inline float calc_tnear(float ray_ox, float ray_oy, float ray_oz,
                        float ray_dx, float ray_dy, float ray_dz,
                        float min_facex, float min_facey, float min_facez)
{
  return max(max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));
}

//requires cell_minx
inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

//requires cell_minx
inline int calc_blkInt(int cell_minx, int cell_miny, int cell_minz, int4 dims)
{
  return cell_minz + (cell_miny + cell_minx*dims.y)*dims.z;
}

//requires float position
inline void calc_cell_min( float* cell_minx, float* cell_miny, float* cell_minz,
                           float posx, float posy, float posz, int4 dims)
{
  (*cell_minx) = clamp(floor(posx), 0.0f, dims.x-1.0f);
  (*cell_miny) = clamp(floor(posy), 0.0f, dims.y-1.0f);
  (*cell_minz) = clamp(floor(posz), 0.0f, dims.z-1.0f);
}

//intersect scene - calc tnear and tfar
inline void intersect_block(float4 ray_o, float4 ray_d,
                            __constant RenderSceneInfo* linfo,
                            float* tFar, float* tblock)
{
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_d.x > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_d.y > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_d.z > 0.0f) ? (linfo->dims.z) : 0.0f;
  (*tFar) = calc_tfar(ray_o.x, ray_o.y, ray_o.z, ray_d.x, ray_d.y, ray_d.z, max_facex, max_facey, max_facez);
  float min_facex = (ray_d.x < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_d.y < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_d.z < 0.0f) ? (linfo->dims.z) : 0.0f;
  (*tblock) = calc_tnear(ray_o.x, ray_o.y, ray_o.z, ray_d.x, ray_d.y, ray_d.z, min_facex, min_facey, min_facez);
}


////////////////////////////////////////////////////////////
//find new split centers
//
//Given a current ball's radius (r), it's distance from camera center (t), current
//half angle (alpha) and returns the specified new location (either 0, 1, 2, 3)
//     ____ ____
//    | 0  |  1 |
//    |____|____|
//    | 2  | 3  |
//    |____|____|
////////////////////////////////////////////////////////////
void calc_cone_split(float r, float t, float alpha, float* new_r, float* tprime)
{
  float denom = (cos(alpha)+1.0f)/2.0f;
  float first = r*sin(alpha/2.0f) + t*cos(alpha/2.0f);
  float second = sqrt( r*(r+sin(alpha)) );
  (*tprime) =  (first + second) / denom;

  //calc new radius
  (*new_r) = (*tprime) * sin(alpha/2.0f);
}

//calculates the t and radius for a previous sphere,
void prev_sphere(float currT, float currR, float* prevT, float* prevR)
{
  (*prevR) = currR * (currT-currR) / (currT+currR);
  (*prevT) = currT-currR-(*prevR);
}

//calculates the t and radius value for next sphere
void next_sphere(float currT, float currR, float* nextT, float* nextR)
{
  (*nextR) = currR * (currR + currT) / (currT - currR);
  (*nextT) = currT + currR + (*nextR);
}

