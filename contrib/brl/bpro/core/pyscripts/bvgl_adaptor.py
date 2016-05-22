# import the batch module and dbvalue from init 
# set the global variable, batch, on init before importing this file
import init
dbvalue = init.dbvalue;
batch = init.batch

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
