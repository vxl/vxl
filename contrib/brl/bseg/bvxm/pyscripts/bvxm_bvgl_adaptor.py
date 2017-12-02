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
        return status, ll_lon, ll_lat, ur_lon, ur_lat
    else:
        return status, 0.0, 0.0, 0.0, 0.0


def geo_index_region_resource(geo_index_txt, ll_lon, ll_lat, ur_lon, ur_lat, out_file):
  bvxm_batch.init_process("bvglGeoIndexRegionResourceProcess")
  bvxm_batch.set_input_string(0, geo_index_txt)
  bvxm_batch.set_input_double(1, ll_lon)
  bvxm_batch.set_input_double(2, ll_lat)
  bvxm_batch.set_input_double(3, ur_lon)
  bvxm_batch.set_input_double(4, ur_lat)
  bvxm_batch.set_input_string(5, out_file)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    n_leaves = bvxm_batch.get_output_unsigned(id)
    return n_leaves
  else:
    return 0

# get leaf regions that intersect with given polygon region
def geo_index_region_poly_resource(geo_index_txt, poly_kml, out_file):
  bvxm_batch.init_process("bvglGeoIndexRegionPolyResourceProcess")
  bvxm_batch.set_input_string(0, geo_index_txt)
  bvxm_batch.set_input_string(1, poly_kml)
  bvxm_batch.set_input_string(2, out_file)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    n_leaves = bvxm_batch.get_output_unsigned(id)
    return n_leaves
  else:
    return 0