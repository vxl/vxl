from boxm2_register import boxm2_batch, dbvalue

############################################################
# obtain intersection of multiple 2d rectangular boxes
############################################################
def box_2d_intersection(in_kml, out_kml=""):
  boxm2_batch.init_process("bvgl2DBoxIntersectionProcess")
  boxm2_batch.set_input_string(0,  in_kml)
  boxm2_batch.set_input_string(1, out_kml)
  status = boxm2_batch.run_process();
  if status;
    (id, type) = boxm2_batch.commit_output(0);
    ll_lon = boxm2_batch.get_output_double(id);
    (id, type) = boxm2_batch.commit_output(1);
    ll_lat = boxm2_batch.get_output_double(id);
    (id, type) = boxm2_batch.commit_output(2);
    ur_lon = boxm2_batch.get_output_double(id);
    (id, type) = boxm2_batch.commit_output(3);
    ur_lat = boxm2_batch.get_output_double(id);
    boxm2_batch.remove(id)
    return ll_lon, ll_lat, ur_lon, ur_lat
  else
    return 0.0, 0.0, 0.0, 0.0
