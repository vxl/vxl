from boxm2_register import boxm2_batch, dbvalue;

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################

def get_info_along_ray(scene,cache,cam,u,v,prefix,identifier="") :
  print("Ray  Probe");
  boxm2_batch.init_process("boxm2CppRayProbeProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_from_db(2,cam);
  boxm2_batch.set_input_unsigned(3,u);
  boxm2_batch.set_input_unsigned(4,v);
  boxm2_batch.set_input_string(5,prefix);
  boxm2_batch.set_input_string(6,identifier);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  len_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
  (id, type) = boxm2_batch.commit_output(1);
  alpha_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
  (id, type) = boxm2_batch.commit_output(2);
  vis_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
  (id, type) = boxm2_batch.commit_output(3);
  tabs_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
  (id, type) = boxm2_batch.commit_output(4);
  res_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
  if (prefix != "") :
    (id, type) = boxm2_batch.commit_output(5);
    data_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
    (id, type) = boxm2_batch.commit_output(6);
    nelems = boxm2_batch.get_output_int(id);
    return len_array_1d, alpha_array_1d, vis_array_1d ,tabs_array_1d,res_array_1d, data_array_1d, nelems;
  else :
    return len_array_1d, alpha_array_1d, vis_array_1d ,tabs_array_1d,res_array_1d;
    
def query_cell_brdf(scene,cache,point,model_type): 
  boxm2_batch.init_process("boxm2CppQueryCellBrdfProcess");
  boxm2_batch.set_input_from_db(0, scene);
  boxm2_batch.set_input_from_db(1, cache);
  boxm2_batch.set_input_float(2, point[0]);
  boxm2_batch.set_input_float(3, point[1]);
  boxm2_batch.set_input_float(4, point[2]);
  boxm2_batch.set_input_string(5, model_type);
  boxm2_batch.run_process();
  
def probe_intensities(scene, cpu_cache, str_cache, point):
  boxm2_batch.init_process("boxm2CppBatchProbeIntensitiesProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cpu_cache);
  boxm2_batch.set_input_from_db(2,str_cache);
  boxm2_batch.set_input_float(3,point[0]);
  boxm2_batch.set_input_float(4,point[1]);
  boxm2_batch.set_input_float(5,point[2]);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  intensities=boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(1);
  visibilities=boxm2_batch.get_bbas_1d_array_float(id);
  return intensities, visibilities
  
def extract_cell_centers(scene, cache, prob_thresh=0.0):
  if cache.type == "boxm2_cache_sptr" :
    boxm2_batch.init_process("boxm2ExtractPointCloudProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_float(2,prob_thresh); #prob t
    return boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    return False;
  
def export_points_and_normals(scene, cache, file_out, save_aux=True, prob_thresh=0.0, vis_thresh=0.0, nmag_thresh=0.0, bbox_file=""):
  if cache.type == "boxm2_cache_sptr" :
    print("Exporting to oriented point cloud");
    boxm2_batch.init_process("boxm2ExportOrientedPointCloudProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_string(2,file_out);
    boxm2_batch.set_input_bool(3,save_aux);  #output aux (prob, vis, normal magnitdue)
    boxm2_batch.set_input_float(4,vis_thresh); #vis threshold
    boxm2_batch.set_input_float(5,nmag_thresh); #nmag threshold
    boxm2_batch.set_input_float(6,prob_thresh); #prob threshold
    boxm2_batch.set_input_string(7, bbox_file); #bb filename
    return boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    return False;

def add_aux_info_to_ply(scene, cache, file_in, file_out):
  if cache.type == "boxm2_cache_sptr" :
    print("Adding aux info to ply points");
    boxm2_batch.init_process("boxm2_add_aux_info_to_ply_process");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_string(2,file_in);
    boxm2_batch.set_input_string(3,file_out);  #output aux (prob, vis, normal magnitdue)
    return boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    return False;

