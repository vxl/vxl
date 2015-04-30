import bvxm_batch
bvxm_batch.not_verbose()
bvxm_batch.register_processes()
bvxm_batch.register_datatypes()

def box_2d_intersection(in_kml, out_kml=""):
  bvxm_batch.init_process("bvgl2DBoxIntersectionProcess")
  bvxm_batch.set_input_string(0, in_kml)
  bvxm_batch.set_input_string(1, out_kml)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    ll_lon = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(1)
    ll_lat = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(2)
    ur_lon = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(3)
    ur_lat = bvxm_batch.get_output_double(id)
    return ll_lon, ll_lat, ur_lon, ur_lat
  else:
    return 0.0, 0.0, 0.0, 0.0
