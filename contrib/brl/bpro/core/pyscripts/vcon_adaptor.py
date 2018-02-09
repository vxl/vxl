import brl_init
batch = brl_init.DummyBatch()
dbvalue = brl_init.DummyBatch()

##################
#  Take a OSM road points and some geotiff classified image that shows trafficability
#  compute the trafficability index of the given road segment.  Road with is in meter unit
def trafficability_analysis(pt_lon, pt_lat, class_img, class_cam, road_width = 1):
    batch.init_process("vconTrafficabilityProcess")
    batch.set_input_from_db(0, class_img)
    batch.set_input_from_db(1, class_cam)
    batch.set_input_float_array(2, pt_lon)
    batch.set_input_float_array(3, pt_lat)
    batch.set_input_float(4, road_width)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        roughness = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        n_pixels = batch.get_output_unsigned(id)
        return roughness, n_pixels
    else:
        return -1.0, -1
