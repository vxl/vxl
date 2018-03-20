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
        (id, type) = batch.commit_output(1)
        ll_lat = batch.get_output_double(id)
        (id, type) = batch.commit_output(2)
        ur_lon = batch.get_output_double(id)
        (id, type) = batch.commit_output(3)
        ur_lat = batch.get_output_double(id)
        return True, ll_lon, ll_lat, ur_lon, ur_lat
    else:
        return False, 0.0, 0.0, 0.0, 0.0


# obtain 2d quad tree geological leaves that intersect with given region
def geo_index_region_resource(geo_index_txt, ll_lon, ll_lat, ur_lon, ur_lat, out_file):
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
        n_leaves = batch.get_output_unsigned(id)
        return n_leaves
    else:
        return 0


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
        return n_leaves
    else:
        return 0
