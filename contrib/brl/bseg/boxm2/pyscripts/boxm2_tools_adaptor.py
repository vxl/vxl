from boxm2_register import boxm2_batch, dbvalue;

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################

def get_info_along_ray(scene,cache,cam,u,v,prefix,identifier=""):			  
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
	  if(prefix != ""):
		  (id, type) = boxm2_batch.commit_output(4);
		  data_array_1d = boxm2_batch.get_bbas_1d_array_float(id);
		  (id, type) = boxm2_batch.commit_output(5);
		  nelems = boxm2_batch.get_output_int(id);
		  return len_array_1d, alpha_array_1d, vis_array_1d ,tabs_array_1d, data_array_1d, nelems;
	  else:
		  return len_array_1d, alpha_array_1d, vis_array_1d ,tabs_array_1d;