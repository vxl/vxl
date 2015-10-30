#define SPHERE 0
#define IRIS  1
#define PUPIL 2
#define EYELID 3
#define LOWER_LID 4
#define EYELID_CREASE 5


typedef struct{
    __global uchar* sphere;
    __global uchar* iris;
    __global uchar* pupil;
    __global uchar* eyelid;
    __global uchar* lower_lid;
    __global uchar* eyelid_crease;
}OrbitLabels;

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
    return EYELIDS;
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

  /* if(LOWER_LIDS && !SPHERES && !EYELIDS) */
  /*   return LOWER_LID; */
  /* if(SPHERES && !LOWER_LIDS && !EYELIDS) */
  /*   return SPHERE; */
  /* if(LOWER_LIDS && !EYELIDS) // sphere and lower lid label */
  /*   return LOWER_LID; */
  /* if(SPHERES && !EYELIDS) // sphere and lower lid label */
  /*   return SPHERE; */
  /* if(EYELID_CREASES && !EYELIDS && !SPHERES) */
  /*   return EYELID_CREASE; */
  /* if (EYELIDS ) */
  /*   return EYELID; */

  if(SPHERES )
    return SPHERE;
  if(IRISES )
    return IRIS;
  if(PUPILS )
    return PUPIL;
  if(EYELIDS)
    return EYELID;
  if(LOWER_LIDS) // sphere and lower lid label
    return LOWER_LID;
  if(EYELID_CREASES) // sphere and lower lid label
    return EYELID_CREASE;
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
