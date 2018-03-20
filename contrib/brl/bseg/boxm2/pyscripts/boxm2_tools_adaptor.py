import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################


def get_info_along_ray(scene, cache, cam, u, v, prefix, identifier=""):
    print("Ray  Probe")
    batch.init_process("boxm2CppRayProbeProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_from_db(2, cam)
    batch.set_input_unsigned(3, u)
    batch.set_input_unsigned(4, v)
    batch.set_input_string(5, prefix)
    batch.set_input_string(6, identifier)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    len_array_1d = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    alpha_array_1d = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    vis_array_1d = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(3)
    tabs_array_1d = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(4)
    res_array_1d = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    if (prefix != ""):
        (id, type) = batch.commit_output(5)
        data_array_1d = batch.get_bbas_1d_array_float(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(6)
        nelems = batch.get_output_int(id)
        batch.remove_data(id)
        return len_array_1d, alpha_array_1d, vis_array_1d, tabs_array_1d, res_array_1d, data_array_1d, nelems
    else:
        return len_array_1d, alpha_array_1d, vis_array_1d, tabs_array_1d, res_array_1d


def query_cell_brdf(scene, cache, point, model_type):
    batch.init_process("boxm2CppQueryCellBrdfProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, point[0])
    batch.set_input_float(3, point[1])
    batch.set_input_float(4, point[2])
    batch.set_input_string(5, model_type)
    batch.run_process()


def query_cell_data(scene, cache, point):
    batch.init_process("boxm2CppQueryCellDataProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, point[0])
    batch.set_input_float(3, point[1])
    batch.set_input_float(4, point[2])
    batch.run_process()


def get_index_from_3d_point(scene, cache, point):
    batch.init_process("boxm2CppGetIndexFrom3dPointProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, point[0])
    batch.set_input_float(3, point[1])
    batch.set_input_float(4, point[2])
    batch.run_process()

    (id, type) = batch.commit_output(0)
    blk_i = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    blk_j = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    blk_k = batch.get_output_int(id)
    batch.remove_data(id)

    (id, type) = batch.commit_output(3)
    index = batch.get_output_int(id)
    batch.remove_data(id)

    return ((blk_i, blk_j, blk_k), index)


def get_3d_point_from_index(scene, cache, block_index, index):
    # Warning, you probably shouldn't be doing this!
    batch.init_process("boxm2CppGet3dPointFromIndexProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_int(2, block_index[0])
    batch.set_input_int(3, block_index[1])
    batch.set_input_int(4, block_index[2])
    batch.set_input_int(5, index)
    batch.run_process()

    (id, type) = batch.commit_output(0)
    x = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_float(id)
    batch.remove_data(id)

    (id, type) = batch.commit_output(3)
    xs = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(4)
    ys = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(5)
    zs = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(6)
    leaf = batch.get_output_int(id)
    batch.remove_data(id)

    return ((x, y, z), (xs, ys, zs), leaf)


def query_cell(scene, cache, point, model_name, model_type):
    # Point should be 3 len, for a x, y, z coordinate OR
    #4 in lenght, blk_i, blk_j, blk_k, index

    if len(point) == 3:
        (blk, index) = get_index_from_3d_point(scene, cache, point)
        point = blk + (index,)

    batch.init_process("boxm2CppQueryCellProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_int(2, point[0])
    batch.set_input_int(3, point[1])
    batch.set_input_int(4, point[2])
    batch.set_input_int(5, point[3])
    batch.set_input_string(6, model_name)
    batch.set_input_string(7, model_type)
    batch.run_process()

    (id, type) = batch.commit_output(0)
    data = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)

    return data


def probe_intensities(scene, cpu_cache, str_cache, point):
    batch.init_process("boxm2CppBatchProbeIntensitiesProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_from_db(2, str_cache)
    batch.set_input_float(3, point[0])
    batch.set_input_float(4, point[1])
    batch.set_input_float(5, point[2])
    batch.run_process()
    (id, type) = batch.commit_output(0)
    intensities = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    visibilities = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    return intensities, visibilities


def extract_cell_centers(scene, cache, prob_thresh=0.0):
    if cache.type == "boxm2_cache_sptr":
        batch.init_process("boxm2ExtractPointCloudProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_float(2, prob_thresh)  # prob t
        return batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def export_points_and_normals(scene, cache, file_out, save_aux=True, prob_thresh=0.0, vis_thresh=0.0, nmag_thresh=0.0, exp_thresh=0.0, bbox_file=""):
    if cache.type == "boxm2_cache_sptr":
        print("Exporting to oriented point cloud")
        batch.init_process("boxm2ExportOrientedPointCloudProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_string(2, file_out)
        # output aux (prob, vis, normal magnitdue)
        batch.set_input_bool(3, save_aux)
        batch.set_input_float(4, vis_thresh)  # vis threshold
        batch.set_input_float(5, nmag_thresh)  # nmag threshold
        batch.set_input_float(6, prob_thresh)  # prob threshold
        # batch.set_input_float(7,exp_thresh); #exp threshold
        # batch.set_input_string(8, bbox_file); #bb filename
        batch.set_input_string(7, bbox_file)  # bb filename
        return batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def add_aux_info_to_ply(scene, cache, file_in, file_out):
    if cache.type == "boxm2_cache_sptr":
        print("Adding aux info to ply points")
        batch.init_process("boxm2_add_aux_info_to_ply_process")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_string(2, file_in)
        # output aux (prob, vis, normal magnitdue)
        batch.set_input_string(3, file_out)
        return batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False
