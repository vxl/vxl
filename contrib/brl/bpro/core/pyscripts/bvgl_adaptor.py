# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

############################################################
# obtain intersection of multiple 2d rectangular boxes
############################################################


def box_2d_intersection(in_kml, out_kml=""):
    batch.init_process("bvgl2DBoxIntersectionProcess")
    batch.set_input_string(0,  in_kml)
    batch.set_input_string(1, out_kml)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        ll_lon = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(1)
        ll_lat = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(2)
        ur_lon = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(3)
        ur_lat = batch.get_output_double(id)
        batch.remove_data(id)
        return True, ll_lon, ll_lat, ur_lon, ur_lat
    else:
        return False, 0.0, 0.0, 0.0, 0.0


# obtain 2d quad tree geological leaves that intersect with given region
def geo_index_region_resource(geo_index_txt, ll_lon, ll_lat, ur_lon, ur_lat, out_file=""):
    batch.init_process("bvglGeoIndexRegionResourceProcess")
    batch.set_input_string(0, geo_index_txt)
    batch.set_input_double(1, ll_lon)
    batch.set_input_double(2, ll_lat)
    batch.set_input_double(3, ur_lon)
    batch.set_input_double(4, ur_lat)
    batch.set_input_string(5, out_file)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        indices = batch.get_bbas_1d_array_unsigned(id)
        batch.remove_data(id)
        return indices
    else:
        return None


# obtain 2d quad tree geological leaves that intersect with given polygon
# region
def geo_index_region_poly_resource(geo_index_txt, poly_kml, out_file):
    batch.init_process("bvglGeoIndexRegionPolyResourceProcess")
    batch.set_input_string(0, geo_index_txt)
    batch.set_input_string(1, poly_kml)
    batch.set_input_string(2, out_file)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        n_leaves = batch.get_output_unsigned(id)
        batch.remove_data(id)
        return n_leaves
    else:
        return 0


# obtain 2d quad tree geological leaves % overlap with given region
def geo_index_region_overlap(geo_index_txt, ll_lon, ll_lat, ur_lon, ur_lat):
  batch.init_process("bvglGeoIndexRegionOverlapProcess")
  batch.set_input_string(0, geo_index_txt)
  batch.set_input_double(1, ll_lon)
  batch.set_input_double(2, ll_lat)
  batch.set_input_double(3, ur_lon)
  batch.set_input_double(4, ur_lat)
  status = batch.run_process()
  if status:
    (id, type) = batch.commit_output(0)
    overlap = batch.get_output_double_array(id)
    batch.remove_data(id)
    return overlap
  else:
    return None


# obtain 2d quad tree geological leaf distance to given location
# users may provide an optional "max_nodes" parameter to limit the number of
# distance measurements, relying on tree nodes higher in the hierachy.
# returns a list of distance measurements and a list of indices denoting
# the position of each tree leaf in the distance list
# (e.g., distance[indices] provides a distance for every leaf, possibly
# measured from a parent node centroid)
def geo_index_region_distance(geo_index_txt, lon, lat, max_nodes=None):
  if not max_nodes: max_nodes = 0

  batch.init_process("bvglGeoIndexRegionDistanceProcess")
  batch.set_input_string(0, geo_index_txt)
  batch.set_input_double(1, lon)
  batch.set_input_double(2, lat)
  batch.set_input_unsigned(3, max_nodes)
  status = batch.run_process()
  if status:
    (id, type) = batch.commit_output(0)
    distance = batch.get_output_double_array(id)
    batch.remove_data(id)

    (id, type) = batch.commit_output(1)
    indices = batch.get_bbas_1d_array_unsigned(id)
    batch.remove_data(id)

    return distance, indices
  else:
    return None


# obtain geospatial extents from a geo_index file
# returns list, where each element is [ll_lon,ll_lat,ur_lon,ur_lat]
def geo_index_extents(geo_index_txt):
  batch.init_process("bvglGeoIndexExtentProcess")
  batch.set_input_string(0, geo_index_txt)
  status = batch.run_process()
  if status:
    (id, type) = batch.commit_output(0)
    extents_flat = batch.get_output_double_array(id)
    batch.remove_data(id)

    n_scene = len(extents_flat)/4
    extents = [extents_flat[4*i : 4*i+4] for i in range(n_scene)]
    return extents
  else:
    return None
