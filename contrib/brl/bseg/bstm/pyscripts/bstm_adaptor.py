import brl_init
import bstm_batch as batch
dbvalue = brl_init.register_batch(batch)
import os
#############################################################################
# PROVIDES higher level python functions to make bstm_batch
# code more readable/refactored
#############################################################################


# small enum for type names
T_DOUBLE = "double"
T_FLOAT = "float"
T_INT = "int"
T_LONG = "long"
T_UNSIGNED = "unsigned"


def get_input_types(inputs, float_name=T_DOUBLE, int_name=T_INT):
    """Converts a list of values to a list of tuples (type_name, value).
    It is designed so that each value can be passed in to a process
    using batch.set_input_<type_name>(index, value).
    e.g. if typename is "double", batch.set_input_double() is called.

    NOTE: This function is not able to distinguish between float and
    double, or between types of integers. As such, float_name and
    int_name must be specified to choose what type_name to give to
    numeric variables. This also means that it may not always be
    possible to use this function for a process, for instance if it
    accepts some variables as doubles and some as ints.

    :returns: A list of tuples (type_name, value) that can be passed
    in to execute_process.

    """
    def get_type(var):
        if isinstance(var, str):
            return "string"
        elif isinstance(var, int):
            return int_name
        elif isinstance(var, float):
            return float_name
        elif isinstance(var, bool):
            return "bool"
        elif isinstance(var, bstm_register.dbvalue):
            return "from_db"
        elif isinstance(var, list):
            return get_type(var[0]) + "_array"
        else:
            raise ValueError(
                "Could not convert argument of type: " +
                type(var))
    return map(lambda v: (get_type(v), v), inputs)


def execute_process(name, n_outputs, inputs):
    """Convenience function for running a process with the given name and
    inputs.  Initializes the process, sets the inputs, runs the
    process, and returns the given number of outputs.
    Raises an exception of process fails to run.

    NOTE: Only returns dbvalue's. Caller might need to use
    batch.get_output_float(id) or similar afterwards.

    :param name: The name of the process to run
    :param n_outputs: The number of outputs to recover
    :param inputs: A list of tuples (type_name, value) representing
    inputs to the process. The type_name determines which "set_input"
    function is called: e.g. if the type name is "string",
    batch.set_input_string will be called. If the typename is
    "from_db", batch.set_input_from_db is called. It is important
    that the names exactly match what is required by the corresponding
    process's C++ code. To automatically generate this list when
    possible, see `get_input_types`.
    :returns: A list of the outputs, of length n_outputs. If there are no outputs, returns None.

    """
    batch.init_process(name)
    for i, (t, v) in enumerate(inputs):
        method_name = "set_input_" + t
        getattr(bstm_batch, method_name)(i, v)

    if not batch.run_process():
        raise Exception("Failed to run " + name)

    outputs = []
    for i in range(n_outputs):
        (out_id, out_type) = batch.commit_output(i)
        outputs.append(bstm_register.dbvalue(out_id, out_type))
    if len(outputs) > 0:
        return outputs
    return


# Print ocl info for all devices
def ocl_info():
    # print("Init Manager");
    batch.init_process("boclInitManagerProcess")
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mgr = dbvalue(id, type)

    print("Get OCL info")
    batch.init_process("bocl_info_process")
    batch.set_input_from_db(0, mgr)
    batch.run_process()


def load_scene(scene_str):
    # print("Loading a Scene from file: ", scene_str);
    batch.init_process("bstmLoadSceneProcess")
    batch.set_input_string(0, scene_str)
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)
    return scene


# does the opencl prep work on an input scene
def load_opencl(scene_str, device_string="gpu"):
    scene = load_scene(scene_str)

    ###############################################################
    # Create cache, opencl manager, device, and gpu cache
    ###############################################################
    # print("Create Main Cache");
    batch.init_process("bstmCreateCacheProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, "lru")
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cache = dbvalue(id, type)

    # print("Init Manager");
    batch.init_process("boclInitManagerProcess")
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mgr = dbvalue(id, type)

    # print("Get Gpu Device");
    batch.init_process("boclGetDeviceProcess")
    batch.set_input_string(0, device_string)
    batch.set_input_from_db(1, mgr)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    device = dbvalue(id, type)

    # print("Create Gpu Cache");
    batch.init_process("bstmOclCreateCacheProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    openclcache = dbvalue(id, type)

    return scene, cache, mgr, device, openclcache


# Just loads up CPP cache
def load_cpp(scene_str):
    scene = load_scene(scene_str)
    batch.init_process("bstmCreateCacheProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, "lru")
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cache = bstm_register.dbvalue(id, type)
    return scene, cache


def describe_scene(scene):
    batch.init_process("bstmDescribeSceneProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    dataPath = batch.get_output_string(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    appType = batch.get_output_string(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    voxel_size = batch.get_output_double(id)
    batch.remove_data(id)
    description = {'voxelLength': voxel_size,
                   'dataPath': dataPath,
                   'appType': appType}
    return description


#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################


def render(scene, device, cache, cam, time=0,
           ni=1624, nj=1224, render_label=False):
    if cache.type == "bstm_cache_sptr":
        print "bstm_batch CPU render grey and vis not yet implemented"
        return
    elif cache.type == "bstm_opencl_cache_sptr" and device:
        batch.init_process("bstmOclRenderExpectedImageProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_float(6, time)
        batch.set_input_bool(7, render_label)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        vis_image = dbvalue(id, type)
        return exp_image, vis_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def render_change(scene, device, cache, cam, time=0, ni=1624, nj=1224):
    if cache.type == "bstm_cache_sptr":
        print "bstm_batch CPU render grey and vis not yet implemented"
        return
    elif cache.type == "bstm_opencl_cache_sptr" and device:
        batch.init_process("bstmOclRenderExpectedChangeProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_float(6, time)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type


######################################################################
# cache methods
#####################################################################
# generic write cache to disk
def write_cache(cache, do_clear=0):
    if cache.type == "bstm_cache_sptr":
        batch.init_process("bstmWriteCacheProcess")
        batch.set_input_from_db(0, cache)
        batch.set_input_bool(1, do_clear)
        batch.run_process()
    else:
        print "ERROR: Cache type needs to be bstm_cache_sptr, not ", cache.type

# generic clear cache


def clear_cache(cache):
    if cache.type == "bstm_cache_sptr":
        batch.init_process("bstmClearCacheProcess")
        batch.set_input_from_db(0, cache)
        batch.run_process()
    elif cache.type == "bstm_opencl_cache_sptr":
        batch.init_process("bstmOclClearCacheProcess")
        batch.set_input_from_db(0, cache)
        batch.run_process()
    else:
        print "ERROR: Cache type needs to be bstm_cache_sptr, not ", cache.type


######################################################################
# trajectory methods
#####################################################################
def init_trajectory(scene, startInc, endInc, radius, ni=1280, nj=720):
    batch.init_process("bstmViewInitTrajectoryProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_double(1, float(startInc))  # incline0
    batch.set_input_double(2, float(endInc))  # incline1
    batch.set_input_double(3, float(radius))  # radius
    batch.set_input_unsigned(4, ni)  # ni
    batch.set_input_unsigned(5, nj)  # nj
    batch.run_process()
    (id, type) = batch.commit_output(0)
    trajectory = dbvalue(id, type)
    return trajectory


def trajectory_next(trajectory):
    batch.init_process("bstmViewTrajectoryNextProcess")
    batch.set_input_from_db(0, trajectory)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def trajectory_size(trajectory):
    batch.init_process("bstmViewTrajectorySizeProcess")
    batch.set_input_from_db(0, trajectory)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    size = batch.get_output_unsigned(id)
    batch.remove_data(id)
    return size


# detect change wrapper,
def change_detect(scene, device, cache, cam, img, time,
                  mask_img=None, raybelief="", max_mode=False):
    batch.init_process("bstmOclChangeDetectionProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_from_db(5, mask_img)
    batch.set_input_string(6, raybelief)
    batch.set_input_bool(7, max_mode)
    batch.set_input_float(8, time)

    batch.run_process()
    (id, type) = batch.commit_output(0)
    cd_img = dbvalue(id, type)
    return cd_img


def label_change(scene, device, cache, cam, change_img, change_t, label, time):
    batch.init_process("bstmOclLabelRayProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, change_img)
    batch.set_input_float(5, change_t)
    batch.set_input_float(6, time)
    batch.set_input_int(7, label)
    batch.run_process()

# detect change wrapper,


def update(scene, device, cache, cam, img, time, mog_var=-1,
           mask_img=None, update_alpha=True, update_changes_only=False):
    batch.init_process("bstmOclUpdateProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_float(5, time)
    batch.set_input_float(6, mog_var)
    batch.set_input_from_db(7, mask_img)
    batch.set_input_bool(8, update_alpha)
    batch.set_input_bool(9, update_changes_only)

    batch.run_process()

# detect change wrapper,


def update_color(scene, device, cache, cam, img, time, mog_var=-1,
                 mask_img=None, update_alpha=True, update_changes_only=False):
    batch.init_process("bstmOclUpdateColorProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_float(5, time)
    batch.set_input_float(6, mog_var)
    batch.set_input_from_db(7, mask_img)
    batch.set_input_bool(8, update_alpha)
    batch.set_input_bool(9, update_changes_only)
    batch.run_process()

# update change wrapper,


def update_change(scene, device, cache, cam, img, time, mask_img=None):
    batch.init_process("bstmOclUpdateChangeProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_from_db(5, mask_img)
    batch.set_input_float(6, time)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cd_img = dbvalue(id, type)
    return cd_img

    # return


def refine(scene, cpu_cache, p_threshold, time):
    batch.init_process("bstmCppRefineSpacetimeProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, p_threshold)
    batch.set_input_float(3, time)
    batch.run_process()


def refine_space(scene, cpu_cache, change_prob_t, time):
    batch.init_process("bstmCppRefineSpaceProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, change_prob_t)
    batch.set_input_float(3, time)
    batch.run_process()


def refine_time(scene, cpu_cache, change_prob_t, time):
    batch.init_process("bstmCppRefineTTProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, change_prob_t)
    batch.set_input_float(3, time)
    batch.run_process()


def merge(scene, cpu_cache, p_threshold, time):
    batch.init_process("bstmCppMergeTTProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, p_threshold)
    batch.set_input_float(3, time)
    batch.run_process()


def filter_changes(scene, cpu_cache, time):
    batch.init_process("bstmCppMajorityFilterProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, time)
    batch.run_process()


def copy_data_to_future(scene, cpu_cache, time):
    batch.init_process("bstmCppCopyDataToFutureProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, time)
    batch.run_process()


def change_btw_frames(scene, cpu_cache, time0, time1):
    batch.init_process("bstmCppChangeBtwFramesProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cpu_cache)
    batch.set_input_float(2, time0)
    batch.set_input_float(3, time1)
    batch.run_process()


def scene_bbox(scene):
    batch.init_process("bstmSceneBboxProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    out = []
    for outIdx in range(6):
        (id, type) = batch.commit_output(outIdx)
        pt = batch.get_output_double(id)
        batch.remove_data(id)
        out.append(pt)
    minPt = (out[0], out[1], out[2])
    maxPt = (out[3], out[4], out[5])
    return (minPt, maxPt)


def scene_lvcs(scene):
    outputs = execute_process("bstmSceneLVCSProcess", 1,
                              get_input_types([scene]))
    return outputs[0]


def scene_statistics(scene, cache):
    batch.init_process("bstmSceneStatisticsProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.run_process()
    (s1_id, s1_type) = batch.commit_output(0)
    (s2_id, s2_type) = batch.commit_output(1)
    (s3_id, s3_type) = batch.commit_output(2)
    s1 = batch.get_output_float(s1_id)
    s2 = batch.get_output_float(s2_id)
    s3 = batch.get_output_unsigned(s3_id)
    return [s1, s2, s3]


def label_tt_depth(scene, cache):
    batch.init_process("bstmCppLabelTTDepthProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.run_process()


def export_pt_cloud(scene, cache, output_filename, prob_t, time, output_aux=True):
    batch.init_process("bstmCppExtractPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, prob_t)
    batch.set_input_float(3, time)
    batch.run_process()

    batch.init_process("bstmCppExportPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, output_filename)
    batch.set_input_bool(3, output_aux)
    batch.set_input_float(4, time)

    batch.run_process()
    return


def bundle2scene(bundle_file, img_dir, app_model="bstm_mog3_grey",
                 isalign=True, out_dir="", timeSteps=32):
    if app_model == "bstm_mog3_grey":
        nobs_model = "bstm_num_obs"
    else:
        print "ERROR appearance model not recognized!!!", app_model
        return

    # run process
    batch.init_process("bstmBundleToSceneProcess")
    batch.set_input_string(0, bundle_file)
    batch.set_input_string(1, img_dir)
    batch.set_input_string(2, app_model)
    batch.set_input_string(3, nobs_model)
    batch.set_input_bool(4, isalign)
    batch.set_input_unsigned(5, timeSteps)
    batch.set_input_string(6, out_dir)
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    uscene = dbvalue(scene_id, scene_type)
    return uscene


def boxm22scene(boxm2_filename, bstm_datapath, timeSteps=32):
    batch.init_process("bstmBoxm2SceneToBstmProcess")
    batch.set_input_string(0, boxm2_filename)
    batch.set_input_string(1, bstm_datapath)
    batch.set_input_unsigned(2, timeSteps)
    batch.run_process()

def ingest_boxm2_scene(bstm_scene, bstm_cache, boxm2_scene,
                       boxm2_cache, time, p_threshold, app_threshold):
    execute_process("bstmCppIngestBoxm2SceneProcess", 0,
                    get_input_types([bstm_scene, bstm_cache, boxm2_scene,
                                     boxm2_cache, float(time), p_threshold, app_threshold], float_name=T_DOUBLE))


def analyze_coherency(bstm_scene, bstm_cpu_cache, center, lengths,
                      initial_time, end_time, p_threshold, output_filename):
    process_name = "bstmCppAnalyzeCoherencyProcess"
    inputs = [bstm_scene, bstm_cpu_cache]
    inputs += map(float, center + lengths +
                  [initial_time, end_time, p_threshold])
    inputs += [output_filename]
    inputs = get_input_types(inputs, float_name=T_FLOAT)
    execute_process(process_name, 0, inputs)
