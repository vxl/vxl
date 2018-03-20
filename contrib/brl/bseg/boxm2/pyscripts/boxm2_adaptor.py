import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

import os
from xml.etree.ElementTree import ElementTree
#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################

# Print ocl info for all devices


def ocl_info():
    # print("Init Manager");
    batch.init_process("boclInitManagerProcess")
    init_result = batch.run_process()

    print("Get OCL info")
    batch.init_process("bocl_info_process")
    info_result = batch.run_process()

    return init_result and info_result


def load_scene(scene_str):
    # print("Loading a Scene from file: ", scene_str);
    batch.init_process("boxm2LoadSceneProcess")
    batch.set_input_string(0, scene_str)
    status = batch.run_process()
    if status:
        (scene_id, scene_type) = batch.commit_output(0)
        scene = dbvalue(scene_id, scene_type)
        return scene
    else:
        raise Exception('Could not load scene file')


# does the opencl prep work on an input scene
def load_opencl(scene_str, device_string="gpu"):
    scene = load_scene(scene_str)

    ###############################################################
    # Create cache, opencl manager, device, and gpu cache
    ###############################################################
    # print("Create Main Cache");
    batch.init_process("boxm2CreateCacheProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, "lru")
    result = batch.run_process()
    if not result:
        raise Exception('boxm2CreateCacheProcess returned false')
    (id, type) = batch.commit_output(0)
    cache = dbvalue(id, type)

    # print("Init Manager");
    batch.init_process("boclInitManagerProcess")
    result = batch.run_process()
    if not result:
        raise Exception('boxm2InitManagerProcess returned false')

    # print("Get Gpu Device");
    batch.init_process("boclGetDeviceProcess")
    batch.set_input_string(0, device_string)
    result = batch.run_process()
    if not result:
        raise Exception('boclGetDeviceProcess returned false')
    (id, type) = batch.commit_output(0)
    device = dbvalue(id, type)

    # print("Create Gpu Cache");
    batch.init_process("boxm2CreateOpenclCacheProcess")
    batch.set_input_from_db(0, device)
    result = batch.run_process()
    if not result:
        raise Exception('boxm2CreateOpenclCacheProcess returned false')
    (id, type) = batch.commit_output(0)
    openclcache = dbvalue(id, type)

    return scene, cache, device, openclcache


# Just loads up CPP cache
def load_cpp(scene_str):
    scene = load_scene(scene_str)

    ###############################################################
    # Create cache, opencl manager, device, and gpu cache
    ###############################################################
    # print("Create Main Cache");
    batch.init_process("boxm2CreateCacheProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, "lru")
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cache = dbvalue(id, type)
    return scene, cache

# Just loads up CPP cache2


def load_cpp2(scene_str):
        # boxm2_cache has been replaced by boxm2_cache, so this function is
        # just a passthrough now.
    return load_cpp(scene_str)


# does the opencl prep work on an input scene
def load_opencl_2(scene_str, device_string="gpu"):
    # boxm2_opencl_cache has been replaced by boxm2_opencl_cache2, so this
    # function is just a passthrough now.
    return load_opencl(scene_str, device_string)

# describe scene process, returns the path containing scene data


def describe_scene(scene):
    batch.init_process("boxm2DescribeSceneProcess")
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

# modifies scene appearance, use case build the model in grey scale and
# then paint it with color images.


def modify_scene_appearance(scene, app1, app2):
    batch.init_process("boxm2ModifySceneAppearanceProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, app1)
    batch.set_input_string(2, app2)
    status = batch.run_process()
    return status

# returns bounding box as two tuple points (minpt, maxpt)


def scene_bbox(scene):
    batch.init_process("boxm2SceneBboxProcess")
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
    batch.init_process("boxm2SceneLVCSProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    (lvcs_id, lvcs_type) = batch.commit_output(0)
    lvcs = dbvalue(lvcs_id, lvcs_type)
    return lvcs


def write_scene_to_kml(scene, kml_filename):
    batch.init_process("boxm2SceneKmlProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, kml_filename)
    batch.run_process()


def write_scene_to_vrml(scene, vrml_filename):
    batch.init_process("boxm2SceneVrmlProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, vrml_filename)
    batch.run_process()


def init_alpha(scene, cache, device, pinit=0.01, thresh=1.0):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Initialize Alpha")
        batch.init_process("boxm2OclInitAlphaProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_float(3, pinit)
        batch.set_input_float(4, thresh)
        return batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False

###############################################
# Model building stuff
###############################################

# Generic update - will use GPU if device/openclcache are passed in


def update_grey(scene, cache, cam, img, device=None, ident="", mask=None,
                update_alpha=True, var=-1.0, update_app=True, tnear=100000.0, tfar=100000.0):
    # If no device is passed in, do cpu update
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU update"
        batch.init_process("boxm2CppUpdateImageProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_from_db(2, cam)
        batch.set_input_from_db(3, img)
        return batch.run_process()
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print("boxm2_batch GPU update")
        batch.init_process("boxm2OclUpdateProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, img)
        batch.set_input_string(5, ident)
        if mask:
            batch.set_input_from_db(6, mask)
        batch.set_input_bool(7, update_alpha)
        batch.set_input_float(8, var)
        batch.set_input_bool(9, update_app)
        batch.set_input_float(10, tnear)
        batch.set_input_float(11, tfar)
        return batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


# Updates with view-dependent grayscale appearance model.
def update_grey_view_dep(scene, cache, cam, img, device=None, ident="",
                         mask=None, update_alpha=True, var=-1.0):
    print "boxm2_batch GPU update (greyscale, view dependent)"
    batch.init_process("boxm2OclUpdateViewDepAppProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_string(5, ident)
    if mask:
        batch.set_input_from_db(6, mask)
    batch.set_input_bool(7, update_alpha)
    batch.set_input_float(8, var)
    return batch.run_process()


# Update with alternate possible pixel explanation - uses GPU
def update_grey_with_alt(scene, cache, cam, img, device=None, ident="",
                         mask=None, update_alpha=True, var=-1.0, alt_prior=None, alt_density=None):
    # If no device is passed in, do cpu update
    if cache.type == "boxm2_cache_sptr":
        print "ERROR: CPU update not implemented for update_with_alt"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print("boxm2_batch GPU update with alt")
        batch.init_process("boxm2OclUpdateWithAltProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, img)
        batch.set_input_string(5, ident)
        if mask:
            batch.set_input_from_db(6, mask)
        batch.set_input_bool(7, update_alpha)
        batch.set_input_float(8, var)
        batch.set_input_from_db(9, alt_prior)
        batch.set_input_from_db(10, alt_density)
        batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def update_app_grey(scene, cache, cam, img, device=None):
    # If no device is passed in, do cpu update
    print("boxm2 GPU App online update")
    batch.init_process("boxm2OclPaintOnlineProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, img)
    batch.set_input_from_db(4, cam)
    batch.run_process()


def update_hist_app_grey(scene, cache, cam, img, device=None):
    # If no device is passed in, do cpu update
    print("boxm2 GPU App online update")
    batch.init_process("boxm2OclUpdateHistogramAppProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, img)
    batch.set_input_from_db(4, cam)
    batch.run_process()
####################################################################
# Generic update - will use GPU if device/openclcache are passed in
####################################################################


def update_rgb(scene, cache, cam, img, device=None, mask="", updateAlpha=True):
    # If no device is passed in, do cpu update
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch rgb CPU update not implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print("boxm2_batch GPU update")
        batch.init_process("boxm2OclUpdateColorProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, img)
        batch.set_input_string(5, "")  # identifier
        batch.set_input_string(6, mask)  # mask file
        batch.set_input_bool(7, updateAlpha)
        batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type


# Updates with view-dependent rgb appearance model.
def update_rgb_view_dep(scene, cache, cam, img, device=None, ident="",
                        mask=None, update_alpha=False, var=-1.0):
    print "boxm2_batch GPU update (rgb, view dependent)"
    batch.init_process("boxm2OclUpdateViewDepAppColorProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_string(5, ident)
    if mask:
        batch.set_input_from_db(6, mask)
    batch.set_input_bool(7, update_alpha)
    batch.set_input_float(8, var)
    return batch.run_process()


def update_with_quality(scene, cache, cam, img, q_img, identifier=""):
    batch.init_process("boxm2CppUpdateUsingQualityProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_from_db(2, cam)
    batch.set_input_from_db(3, img)
    batch.set_input_from_db(4, q_img)
    batch.set_input_string(5, identifier)
    batch.run_process()

# Generic update - will use GPU if device/openclcache are passed in


def update_cpp(scene, cache, cam, img, ident=""):
    batch.init_process("boxm2CppUpdateImageProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_from_db(2, cam)
    batch.set_input_from_db(3, img)
    batch.set_input_string(4, ident)
    batch.run_process()
# Generic update - will use GPU if device/openclcache are passed in


def update_sky(scene, cache, cam, img, device):
    batch.init_process("boxm2OclUpdateSkyProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.run_process()


def update_sky2(scene, cache, cam, img, step, device):
    batch.init_process("boxm2OclUpdateSky2Process")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    if(cam is not None):
        batch.set_input_from_db(3, cam)
    if(img is not None):
        batch.set_input_from_db(4, img)
    batch.set_input_int(5, step)
    batch.run_process()

    # Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache


def render_height_map(scene, cache, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, render height map cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedHeightMapProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.run_process()

        # z_img
        (id, type) = batch.commit_output(0)
        z_image = dbvalue(id, type)

        # var image
        (id, type) = batch.commit_output(1)
        var_image = dbvalue(id, type)

        # x_img
        (id, type) = batch.commit_output(2)
        x_image = dbvalue(id, type)

        # y_img
        (id, type) = batch.commit_output(3)
        y_image = dbvalue(id, type)

        # prob_img
        (id, type) = batch.commit_output(4)
        prob_image = dbvalue(id, type)

        # appearance_img
        #(id,type) = batch.commit_output(5);
        #app_image = dbvalue(id,type);

        return z_image, var_image, x_image, y_image, prob_image  # , app_image;
    else:
        print "ERROR: Cache type not recognized: ", cache.type
    # Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache


def ingest_height_map(scene, cache, x_img, y_img, z_img,
                      zero_out_alpha=True, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, render height map cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestDemProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, z_img)
        batch.set_input_from_db(4, x_img)
        batch.set_input_from_db(5, y_img)
        batch.set_input_bool(6, zero_out_alpha)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def ingest_label_map(scene, cache, x_img, y_img, z_img,
                     label_img, ident, device=None):
    # def ingest_label_map(scene, cache, x_img, y_img, z_img, label_img,
    # device=None) :
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, ingest label map cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestLabelProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, x_img)
        batch.set_input_from_db(4, y_img)
        batch.set_input_from_db(5, z_img)
        batch.set_input_from_db(6, label_img)
        batch.set_input_string(7, ident)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type

# use this process to ingest e.g. a satellite classification map. pass the
# satellite cam to compute ray origins and directions


def ingest_label_map_with_cam(
        scene, cache, cam, label_img, ident, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, ingest label map cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestLabelwithCAMProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, label_img)
        batch.set_input_string(5, ident)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def ingest_osm_label_map(scene, cache, x_img, y_img,
                         z_img, label_img, ident, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, ingest osm label map cpp process not implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestOsmLabelProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, x_img)
        batch.set_input_from_db(4, y_img)
        batch.set_input_from_db(5, z_img)
        batch.set_input_from_db(6, label_img)
        batch.set_input_string(7, ident)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def ingest_mesh(scene, cache, plyfile, label_id, category):
    batch.init_process("boxm2IngestConvexMeshProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, plyfile)
    batch.set_input_int(3, label_id)
    batch.set_input_string(4, category)
    batch.run_process()


def ingest_height_map_space(
        scene, cache, x_img, y_img, z_img, crust_thickness, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, render height map cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestDemSpaceProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, z_img)
        batch.set_input_from_db(4, x_img)
        batch.set_input_from_db(5, y_img)
        batch.set_input_double(6, crust_thickness)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type

# selectively zero out the alphas along the passed rays


def ingest_to_zero_out_alpha(scene, cache, x_img, y_img, z_img, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, render height map cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestZeroProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, z_img)
        batch.set_input_from_db(4, x_img)
        batch.set_input_from_db(5, y_img)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type


# refine count should not exceed 3 for scenes with max_octree_level=4
def initialize_surface_with_height_img(
        scene, x_img, y_img, z_img, crust_thickness=20.0, refine_cnt=2, ingest_space=1, zero_out_alpha=True):
    if zero_out_alpha:
        scene.ingest_height_map(x_img, y_img, z_img)
    else:
        scene.ingest_to_zero_out_alpha(x_img, y_img, z_img)
        scene.ingest_height_map(x_img, y_img, z_img, False)

    # to save space by not refining empty voxels below surface voxels
    if ingest_space:
        print "crust_thickness = ", crust_thickness
        scene.ingest_height_map_space(x_img, y_img, z_img, crust_thickness)
    # scene.write_cache()

    for i in range(0, refine_cnt, 1):
        scene.refine()
        # scene.write_cache();

        if zero_out_alpha:
            scene.ingest_height_map(x_img, y_img, z_img)
        else:
            scene.ingest_to_zero_out_alpha(x_img, y_img, z_img)
            scene.ingest_height_map(x_img, y_img, z_img, False)

        if ingest_space:
            scene.ingest_height_map_space(x_img, y_img, z_img, crust_thickness)
        # scene.write_cache();

    # ingest one more time to fill up the empty voxels below the surface (They
    # are not refined but they still need to be occupied)
    if zero_out_alpha:
        scene.ingest_height_map(x_img, y_img, z_img)
    else:
        scene.ingest_to_zero_out_alpha(x_img, y_img, z_img)
        scene.ingest_height_map(x_img, y_img, z_img, False)
    scene.write_cache()


def initialize_ground(scene, global_ground_z, refine_cnt=2):
    (x_ground, y_ground, z_ground) = generate_xyz_for_ground_initialization(
        scene.scene, global_ground_z)

    scene.ingest_height_map(x_ground, y_ground, z_ground, False)
    for i in range(0, refine_cnt, 1):
        scene.refine()
        scene.ingest_height_map(x_ground, y_ground, z_ground, False)
    scene.write_cache()


def refine_and_ingest_with_height_img(
        scene, x_img, y_img, z_img, crust_thickness=20.0, refine_cnt=1):
    for i in range(0, refine_cnt, 1):
        scene.refine()
        scene.ingest_height_map(x_img, y_img, z_img)
        scene.ingest_height_map_space(x_img, y_img, z_img, crust_thickness)

    # ingest one more time to fill up the empty voxels below the surface (They
    # are not refined but they still need to be occupied)
    scene.ingest_height_map(x_img, y_img, z_img)
    scene.write_cache()

# Ingest a Buckeye-Style DEM, i.e. first return and last return image pair


def ingest_buckeye_dem(scene, cache, first_return_fname,
                       last_return_fname, geoid_height, geocam, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_adaptor, ingest_buckeye cpp process not implemented"

    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclIngestBuckeyeDemProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_string(3, first_return_fname)
        batch.set_input_string(4, last_return_fname)
        batch.set_input_float(5, geoid_height)
        batch.set_input_from_db(6, geocam)
        batch.run_process()
        return
    else:
        print "ERROR: Cache type not recognized: ", cache.type
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################


def render_grey(scene, cache, cam, ni=1280, nj=720, device=None,
                ident_string="", tnear=100000.0, tfar=100000.0):
    if cache.type == "boxm2_cache_sptr":
        batch.init_process("boxm2CppRenderExpectedImageProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_from_db(2, cam)
        batch.set_input_unsigned(3, ni)
        batch.set_input_unsigned(4, nj)
        batch.set_input_string(5, ident_string)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedImageProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_string(6, ident_string)
        batch.set_input_float(7, tnear)
        batch.set_input_float(8, tfar)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type


# boxm2_ocl_render_view_dep_expected_image_process.cxx
def render_grey_view_dep(scene, cache, cam, ni=1280, nj=720, device=None,
                         ident_string=""):
    if cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderViewDepExpectedImageProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_string(6, ident_string)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def render_scene_uncertainty(
        scene, cache, ni=1280, nj=720, device=None, ident_string=""):
    if cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderSceneUncertaintyMapProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_unsigned(3, ni)
        batch.set_input_unsigned(4, nj)
        batch.set_input_string(5, ident_string)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################


def render_grey_and_vis(scene, cache, cam, ni=1280,
                        nj=720, device=None, ident=""):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU render grey and vis not yet implemented"
        return
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedImageProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_string(6, ident)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        vis_image = dbvalue(id, type)
        return exp_image, vis_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type

#####################################################################
# Generic render, returns a dbvalue expected image
#####################################################################


def render_rgb(scene, cache, cam, ni=1280, nj=720,
               device=None, tnear=100000.0, tfar=100000.0):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU render rgb not yet implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedColorProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_float(6, tnear)
        batch.set_input_float(7, tfar)
        status = batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        vis_image = dbvalue(id, type)
        return exp_image, vis_image, status
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def render_rgb_view_dep(scene, cache, cam, ni=1280, nj=720, device=None,
                        ident_string=""):
    if cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderViewDepExpectedColorProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_string(6, ident_string)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type

#####################################################################
# render depth map
#####################################################################


def render_depth(scene, cache, cam, ni=1280, nj=720, device=None):
    if cache.type == "boxm2_cache_sptr":
    # print "boxm2_batch CPU render depth not yet implemented";
        batch.init_process("boxm2CppRenderExpectedDepthProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_from_db(2, cam)
        batch.set_input_unsigned(3, ni)
        batch.set_input_unsigned(4, nj)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        return exp_image, 0, 0
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedDepthProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        var_image = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        vis_image = dbvalue(id, type)
        return exp_image, var_image, vis_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type

# render the depth of the surface that has the max prob of being the first
# visible and occupied surface along the ray


def render_depth_of_max_prob_surface(
        scene, cache, cam, ni=1280, nj=720, device=None):
    if cache.type == "boxm2_cache_sptr":
        # print "boxm2_batch CPU render depth not yet implemented";
        batch.init_process("boxm2CppRenderDepthofMaxProbProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_from_db(2, cam)
        batch.set_input_unsigned(3, ni)
        batch.set_input_unsigned(4, nj)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        prob_image = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        vis_image = dbvalue(id, type)
        return exp_image, prob_image, vis_image
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print "boxm2_batch CPU render depth of max prob surface not yet implemented for GPU"
        return 0, 0, 0
    else:
        print "ERROR: Cache type not recognized: ", cache.type


#####################################################################
# render depth map by loading block inside certain region
#####################################################################
def render_depth_region(scene, cache, cam, lat, lon,
                        elev, radius, ni=1280, nj=720, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU render depth not yet implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedDepthRegionProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_double(4, lat)
        batch.set_input_double(5, lon)
        batch.set_input_double(6, elev)
        batch.set_input_double(7, radius)
        batch.set_input_unsigned(8, ni)
        batch.set_input_unsigned(9, nj)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        var_image = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        vis_image = dbvalue(id, type)
        return exp_image, var_image, vis_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type

#####################################################################
# render image of expected z values
#####################################################################


def render_z_image(scene, cache, cam, ni=1280, nj=720,
                   normalize=False, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU render depth not yet implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclRenderExpectedZImageProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_bool(6, normalize)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        z_exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        z_var_image = dbvalue(id, type)
        return z_exp_image, z_var_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type
#####################################################################
# change detection wrapper
#####################################################################


def change_detect(scene, cache, cam, img, exp_img, device=None,
                  rgb=False, n=1, raybelief="", max_mode=False, ident=""):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU change detection"
        batch.init_process("boxm2CppChangeDetectionProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_from_db(2, cam)
        batch.set_input_from_db(3, img)
        batch.set_input_from_db(4, exp_img)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        cd_img = dbvalue(id, type)
        return cd_img
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print "boxm2_batch GPU change detection"
        batch.init_process("boxm2OclChangeDetectionProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, img)
        batch.set_input_from_db(5, exp_img)
        batch.set_input_int(6, n)
        batch.set_input_string(7, raybelief)
        batch.set_input_bool(8, max_mode)
        batch.set_input_string(9, ident)
        batch.run_process()
        if not rgb:
            (id, type) = batch.commit_output(0)
            cd_img = dbvalue(id, type)
        else:
            (id, type) = batch.commit_output(1)
            cd_img = dbvalue(id, type)
        return cd_img
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def change_detect2(scene, cache, cam, img, identifier="",
                   maxmode=False, tnear=100000, tfar=0.00001, device=None):
    print "boxm2_batch GPU change detection"
    batch.init_process("boxm2OclChangeDetectionProcess2")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_string(5, identifier)
    batch.set_input_bool(6, maxmode)
    batch.set_input_float(7, tnear)
    batch.set_input_float(8, tfar)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cd_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    vis_img = dbvalue(id, type)
    return cd_img, vis_img

####################################################################
# Visualize Change Wrapper
####################################################################


def visualize_change(change_img, in_img, thresh=.5, low_is_change=False):
    batch.init_process("boxm2OclVisualizeChangeProcess")
    batch.set_input_from_db(0, change_img)
    batch.set_input_from_db(1, in_img)
    batch.set_input_float(2, thresh)
    batch.set_input_bool(3, low_is_change)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    vis_img = dbvalue(id, type)
    return vis_img

#####################################################################
# generic refine (will work on color and grey scenes)
#####################################################################


def refine(scene, cache, thresh=0.3, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU refine"
        batch.init_process("boxm2CppRefineProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_float(2, thresh)
        batch.run_process()
        return 0
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print "boxm2_batch GPU refine"
        batch.init_process("boxm2OclRefineProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_float(3, thresh)
        batch.run_process()

        # get and report cells output
        (id, type) = batch.commit_output(0)
        nCells = batch.get_output_int(id)
        batch.remove_data(id)
        return nCells
    else:
        print "ERROR: Cache type unrecognized: ", cache.type

#####################################################################
# generic merge method
#####################################################################


def merge(scene, cache, thresh=0.01, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU merge"
        batch.init_process("boxm2CppMergeProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.set_input_float(2, thresh)
        batch.run_process()
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print "boxm2_batch GPU refine"
        batch.init_process("boxm2OclMergeProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_float(3, thresh)
        batch.run_process()
    else:
        print "ERROR: Cache type unrecognized: ", cache.type

#####################################################################
# generic filter scene, should work with color and grey scenes
#####################################################################


def median_filter(scene, cache, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU median filter"
        batch.init_process("boxm2CppFilterProcess")
        batch.set_input_from_db(0, scene)
        batch.set_input_from_db(1, cache)
        batch.run_process()
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        print "boxm2_batch GPU median filter"
        batch.init_process("boxm2OclFilterProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.run_process()
    else:
        print "ERROR: Cache type unrecognized: ", cache.type


######################################################################
# cache methods
#####################################################################
# generic write cache to disk
def write_cache(cache, do_clear=0):
    if cache.type == "boxm2_cache_sptr":
        batch.init_process("boxm2WriteCacheProcess")
        batch.set_input_from_db(0, cache)
        batch.set_input_bool(1, do_clear)
        batch.run_process()
    else:
        print "ERROR: Cache type needs to be boxm2_cache_sptr, not ", cache.type

# generic clear cache


def clear_cache(cache):
    if cache.type == "boxm2_cache_sptr":
        batch.init_process("boxm2ClearCacheProcess")
        batch.set_input_from_db(0, cache)
        batch.run_process()
    elif cache.type == "boxm2_opencl_cache_sptr":
        batch.init_process("boxm2ClearOpenclCacheProcess")
        batch.set_input_from_db(0, cache)
        batch.run_process()
    else:
        print "ERROR: Cache type needs to be boxm2_cache_sptr, not ", cache.type


######################################################################
# trajectory methods
#####################################################################
def init_trajectory(scene, startInc, endInc, radius, ni=1280, nj=720):
    batch.init_process("boxm2ViewInitTrajectoryProcess")
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
    batch.init_process("boxm2ViewTrajectoryNextProcess")
    batch.set_input_from_db(0, trajectory)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def trajectory_size(trajectory):
    batch.init_process("boxm2ViewTrajectorySizeProcess")
    batch.set_input_from_db(0, trajectory)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    size = batch.get_output_unsigned(id)
    batch.remove_data(id)
    return size


def trajectory_direct(trajectory, index):
    batch.init_process("boxm2ViewTrajectoryDirectProcess")
    batch.set_input_from_db(0, trajectory)
    batch.set_input_unsigned(1, index)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def init_trajectory_regular(ni, nj, right_fov, top_fov, altitude, heading,
                            tilt, roll, x_start, y_start, x_end, y_end, x_inc, y_inc, heading_inc):
    batch.init_process("boxm2ViewInitRegularTrajectoryProcess")
    batch.set_input_unsigned(0, ni)
    batch.set_input_unsigned(1, nj)
    batch.set_input_double(2, right_fov)
    batch.set_input_double(3, top_fov)
    batch.set_input_double(4, altitude)
    batch.set_input_double(5, heading)
    batch.set_input_double(6, tilt)
    batch.set_input_double(7, roll)
    batch.set_input_double(8, x_start)
    batch.set_input_double(9, y_start)
    batch.set_input_double(10, x_end)
    batch.set_input_double(11, y_end)
    batch.set_input_double(12, x_inc)
    batch.set_input_double(13, y_inc)
    batch.set_input_double(14, heading_inc)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    trajectory = dbvalue(id, type)
    return trajectory

# heading is incremented from 0 to 360 with heading_increment


def init_trajectory_height_map(scene, x_img, y_img, z_img, ni, nj, right_fov, top_fov,
                               altitude, tilt, roll, margin, i_start, j_start, i_inc, j_inc, heading_start, heading_inc):
    batch.init_process("boxm2ViewInitHeightMapTrajectoryProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, x_img)
    batch.set_input_from_db(2, y_img)
    batch.set_input_from_db(3, z_img)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    batch.set_input_double(6, right_fov)
    batch.set_input_double(7, top_fov)
    batch.set_input_double(8, altitude)
    batch.set_input_double(9, tilt)
    batch.set_input_double(10, roll)
    batch.set_input_unsigned(11, margin)
    batch.set_input_unsigned(12, i_start)
    batch.set_input_unsigned(13, j_start)
    batch.set_input_unsigned(14, i_inc)
    batch.set_input_unsigned(15, j_inc)
    batch.set_input_double(16, heading_start)
    batch.set_input_double(17, heading_inc)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    trajectory = dbvalue(id, type)
    return trajectory

######################################################################
# camera/scene methods
#####################################################################


def bundle2scene(bundle_file, img_dir, app_model="boxm2_mog3_grey", nblks=8,
                 isalign=True, out_dir="nvm_out", ply_file="", scenecenter=[0, 0, 0]):
    if app_model == "boxm2_mog3_grey" or app_model == "boxm2_mog3_grey_16":
        nobs_model = "boxm2_num_obs"
    elif app_model == "boxm2_gauss_rgb":
        nobs_model = "boxm2_num_obs_single"
    else:
        print "ERROR appearance model not recognized!!!", app_model
        return

    # run process
    batch.init_process("boxm2BundleToSceneProcess")
    batch.set_input_string(0, bundle_file)
    batch.set_input_string(1, img_dir)
    batch.set_input_string(2, app_model)
    batch.set_input_string(3, nobs_model)
    batch.set_input_int(4, nblks)
    batch.set_input_bool(5, isalign)
    batch.set_input_string(6, out_dir)
    batch.set_input_string(7, ply_file)
    batch.set_input_float(8, scenecenter[0])
    batch.set_input_float(9, scenecenter[1])
    batch.set_input_float(10, scenecenter[2])
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    uscene = dbvalue(scene_id, scene_type)
    return uscene


def save_scene(scene, fname):
    batch.init_process("boxm2WriteSceneXMLProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, fname)
    batch.run_process()


def scale_scene(scene, scale):
    batch.init_process("boxm2ScaleSceneProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_float(1, scale)
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)
    return scene

# Create a scene from specified (lat,lon) corners and size of each voxel
# (in meters) at the finest scale, elev values are also in meters


def create_scene_and_blocks(scene_dir, app_model, obs_model, origin_lon, origin_lat, origin_elev, lon1, lat1,
                            elev1, lon2, lat2, elev2, vox_size, block_len_xy, block_len_z, local_cs_name, num_bins=0, xml_name="scene"):
    batch.init_process("boxm2CreateSceneAndBlocksProcess")
    batch.set_input_string(0, scene_dir)
    batch.set_input_string(1, app_model)
    batch.set_input_string(2, obs_model)
    batch.set_input_float(3, lon1)
    batch.set_input_float(4, lat1)
    batch.set_input_float(5, elev1)
    batch.set_input_float(6, lon2)
    batch.set_input_float(7, lat2)
    batch.set_input_float(8, elev2)
    batch.set_input_float(9, origin_lon)
    batch.set_input_float(10, origin_lat)
    batch.set_input_float(11, origin_elev)
    batch.set_input_float(12, vox_size)
    batch.set_input_float(13, block_len_xy)
    batch.set_input_float(14, block_len_z)
    batch.set_input_int(15, num_bins)
    batch.set_input_string(16, local_cs_name)
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)

    print("Write Scene")
    batch.init_process("boxm2WriteSceneXMLProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, xml_name)
    batch.run_process()

# Create a scene from a given polygon structure in kml file


def create_scene_poly_and_blocks(scene_dir, app_model, obs_model, poly_kml, origin_lon, origin_lat, origin_elev,
                                 scene_height, vox_size, block_len_xy, block_len_z, local_cs_name, num_bins=0, xml_name="scene"):
    # get the inpus
    batch.init_process("boxm2CreatePolySceneAndBlocksProcess")
    batch.set_input_string(0, scene_dir)
    batch.set_input_string(1, app_model)
    batch.set_input_string(2, obs_model)
    batch.set_input_string(3, poly_kml)
    batch.set_input_float(4, origin_lon)
    batch.set_input_float(5, origin_lat)
    batch.set_input_float(6, origin_elev)
    batch.set_input_float(7, scene_height)
    batch.set_input_float(8, vox_size)
    batch.set_input_float(9, block_len_xy)
    batch.set_input_float(10, block_len_z)
    batch.set_input_int(11, num_bins)
    batch.set_input_string(12, local_cs_name)

    # create scene
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)

    # write scene
    batch.init_process("boxm2WriteSceneXMLProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, xml_name)
    batch.run_process()

# Distribute a larger scene region and its blocks to smaller square scenes
# with a given dimension


def distribute_scene_blocks(scene, small_scene_dim,
                            xml_output_path, xml_name_prefix):
    batch.init_process("boxm2DistributeSceneBlocksProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_double(1, small_scene_dim)
    batch.set_input_string(2, xml_output_path)
    batch.set_input_string(3, xml_name_prefix)
    batch.run_process()


def prune_scene_blocks(scene, cache, xml_output_path, xml_name_prefix):
    batch.init_process("boxm2PruneSceneBlocksProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, xml_output_path)
    batch.set_input_string(3, xml_name_prefix)
    batch.run_process()

# prune the scene blocks by dem image


def prune_scene_blocks_by_dem(scene, dem_root, elev_cutoff):
    batch.init_process("boxm2PruneSceneBlocksByDemProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, dem_root)
    batch.set_input_float(2, elev_cutoff)
    batch.run_process()
    # return scene
    (scene_id, scene_type) = batch.commit_output(0)
    prune_scene = dbvalue(scene_id, scene_type)
    return prune_scene

# change the scene resolution by geo cover


def change_scene_res_by_geo_cover(scene, img_fname, refine_coefficient=1):
    batch.init_process("boxm2ChangeSceneResByGeoCover")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, img_fname)
    batch.set_input_int(2, refine_coefficient)
    result = batch.run_process()
    # return scene
    if result:
        (scene_id, scene_type) = batch.commit_output(0)
        changed_scene = dbvalue(scene_id, scene_type)
    else:
        changed_scene = 0
    return changed_scene

#######################################################
# Create prescribed numbers of camera positions on the ground plane along
# given kml path, under scene local coordinates systems


def create_camera_from_kml_path(kml_path_dir, lvcs, num_cam, txt_out_file):
        # get the inputs
    batch.init_process("boxm2CreateCameraFromKmlPath")
    batch.set_input_string(0, kml_path_dir)
    batch.set_input_from_db(1, lvcs)
    batch.set_input_unsigned(2, num_cam)
    batch.set_input_string(3, txt_out_file)
    # create camera position
    batch.run_process()


# Create multi block scene - params is a hash of scene parameters
def save_multi_block_scene(params):

    # load params
    scene_dir = params['scene_dir'] if 'scene_dir' in params else os.getcwd()
    app_model = params[
        'app_model'] if 'app_model' in params else "boxm2_mog3_grey"
    obs_model = params[
        'obs_model'] if 'obs_model' in params else "boxm2_num_obs"
    orig_x = params['orig_x'] if 'orig_x' in params else 0.0
    orig_y = params['orig_y'] if 'orig_y' in params else 0.0
    orig_z = params['orig_z'] if 'orig_z' in params else 0.0
    n_x = params['num_block_x'] if 'num_block_x' in params else 8
    n_y = params['num_block_y'] if 'num_block_y' in params else 8
    n_z = params['num_block_z'] if 'num_block_z' in params else 1
    num_vox_x = params['num_vox_x'] if 'num_vox_x' in params else 1536
    num_vox_y = params['num_vox_y'] if 'num_vox_y' in params else 1536
    num_vox_z = params['num_vox_z'] if 'num_vox_z' in params else 512

    # max mb per block, init level, and init prob
    max_data_mb = params[
        'max_block_mb'] if 'max_block_mb' in params else 1000.0
    p_init = params['p_init'] if 'p_init' in params else .01
    max_level = params['max_tree_level'] if 'max_tree_level' in params else 4
    init_level = params[
        'init_tree_level'] if 'init_tree_level' in params else 1
    vox_length = params['vox_length'] if 'vox_length' in params else 1.0
    sb_length = params[
        'sub_block_length'] if 'sub_block_length' in params else .125
    fname = params['filename'] if 'filename' in params else "scene"

    # reconcile sub block length vs voxel length
    if 'sub_block_length' in params:
        vox_length = sb_length / 8.0
    elif 'vox_length' in params:
        sb_length = vox_length * 8

    # set up tuples
    if 'origin' in params:
        orig_x, orig_y, orig_z = params['origin']
    if 'num_vox' in params:
        num_vox_x, num_vox_y, num_vox_z = params['num_vox']
    if 'num_blocks' in params:
        n_x, n_y, n_z = params['num_blocks']

    # run create scene process
    batch.init_process("boxm2CreateSceneProcess")
    batch.set_input_string(0, scene_dir)
    batch.set_input_string(1, app_model)
    batch.set_input_string(2, obs_model)
    batch.set_input_float(3, orig_x)
    batch.set_input_float(4, orig_y)
    batch.set_input_float(5, orig_z)
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)

    # calc number of sub blocks in each block
    num_sb_x = num_vox_x / 8
    num_sb_y = num_vox_y / 8
    num_sb_z = num_vox_z / 8
    num_x = num_sb_x / n_x
    num_y = num_sb_y / n_y
    num_z = num_sb_z / n_z

    for k in range(0, n_z):
        for i in range(0, n_x):
            for j in range(0, n_y):
                local_origin_z = k * num_z * sb_length + orig_z
                local_origin_y = j * num_y * sb_length + orig_y
                local_origin_x = i * num_x * sb_length + orig_x

                print("Adding block: ", i, " ", j, " ", k)
                batch.init_process("boxm2AddBlockProcess")
                batch.set_input_from_db(0, scene)
                batch.set_input_int(1, i)
                batch.set_input_int(2, j)
                batch.set_input_int(3, k)
                batch.set_input_unsigned(4, num_x)
                batch.set_input_unsigned(5, num_y)
                batch.set_input_unsigned(6, num_z)
                batch.set_input_unsigned(7, max_level)
                batch.set_input_float(8, local_origin_x)
                batch.set_input_float(9, local_origin_y)
                batch.set_input_float(10, local_origin_z)
                batch.set_input_float(11, sb_length)
                batch.set_input_float(12, max_data_mb)
                batch.set_input_float(13, p_init)
                batch.set_input_unsigned(14, init_level)
                batch.run_process()

    print("Write Scene")
    batch.init_process("boxm2WriteSceneXMLProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, fname)
    batch.run_process()


def roi_init(NITF_path, camera, scene, convert_to_8bit,
             params_fname, margin=0, clip_width=-1, clip_height=-1):
    def fail():
        local_cam = 0
        cropped_image = 0
        uncertainty = 0
        return result, local_cam, cropped_image, uncertainty

    batch.init_process("boxm2RoiInitProcess")
    result = batch.set_params_process(params_fname)
    if not result:
        return fail()
    batch.set_input_string(0, NITF_path)
    batch.set_input_from_db(1, camera)
    batch.set_input_from_db(2, scene)
    batch.set_input_bool(3, convert_to_8bit)
    batch.set_input_int(4, margin)
    batch.set_input_int(5, clip_width)
    batch.set_input_int(6, clip_height)
    result = batch.run_process()
    if result:
        (id, type) = batch.commit_output(0)
        local_cam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        cropped_image = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        uncertainty = batch.get_output_float(id)
        batch.remove_data(id)
        return result, local_cam, cropped_image, uncertainty
    else:
        return fail()


def create_mask_image(scene, camera, ni, nj, ground_plane_only=False):
    batch.init_process("boxm2CreateSceneMaskProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, camera)
    batch.set_input_unsigned(2, ni)
    batch.set_input_unsigned(3, nj)
    batch.set_input_bool(4, ground_plane_only)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mask = dbvalue(id, type)
    return mask


######################################################################
# blob detection methods
#####################################################################
# runs blob change detection process
def blob_change_detection(change_img, thresh, depth1=None, depth2=None):
    batch.init_process("boxm2BlobChangeDetectionProcess")
    batch.set_input_from_db(0, change_img)
    batch.set_input_float(1, thresh)
    if (depth1 and depth2):
        batch.set_input_from_db(2, depth1)
        batch.set_input_from_db(3, depth2)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    blobImg = dbvalue(id, type)
    return blobImg

# pixel wise roc process for change detection images


def blob_precision_recall(cd_img, gt_img, mask_img=None):
    batch.init_process("boxm2BlobPrecisionRecallProcess")
    batch.set_input_from_db(0, cd_img)
    batch.set_input_from_db(1, gt_img)
    if mask_img:
        batch.set_input_from_db(2, mask_img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    precision = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    recall = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)

    # return tuple of true positives, true negatives, false positives, etc..
    return (precision, recall)


#########################################################################
# Batch update process
#########################################################################
def update_aux_per_view(scene, cache, img, cam,
                        imgString, device=None, mask=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU update aux per view not yet implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclUpdateAuxPerViewProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, img)
        batch.set_input_string(5, imgString)
        if mask:
            batch.set_input_from_db(6, mask)
        batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type

# Update Aux for normal-albedo-array appearance model


def update_aux_per_view_naa(scene, cache, img, cam, metadata,
                            atm_params, imgString, alt_prior, alt_density, device=None):
    if cache.type == "boxm2_cache_sptr":
        print "boxm2_batch CPU update aux per view_naa not yet implemented"
    elif cache.type == "boxm2_opencl_cache_sptr" and device:
        batch.init_process("boxm2OclUpdateAuxPerViewNAAProcess")
        batch.set_input_from_db(0, device)
        batch.set_input_from_db(1, scene)
        batch.set_input_from_db(2, cache)
        batch.set_input_from_db(3, cam)
        batch.set_input_from_db(4, img)
        batch.set_input_from_db(5, metadata)
        batch.set_input_from_db(6, atm_params)
        batch.set_input_string(7, imgString)
        batch.set_input_from_db(8, alt_prior)
        batch.set_input_from_db(9, alt_density)
        batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type

###########################################################
# create sun camera
# astro_coords=True indicates az,el in degrees north of east, degrees
# above horizon


def compute_sun_affine_camera(scene, sun_az, sun_el, astro_coords=True):
    batch.init_process("boxm2ComputeSunAffineCameraProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_float(1, sun_el)
    batch.set_input_float(2, sun_az)
    batch.set_input_bool(3, astro_coords)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sun_cam = dbvalue(id, type)
    (ni_id, type) = batch.commit_output(1)
    (nj_id, type) = batch.commit_output(2)
    ni = batch.get_output_unsigned(ni_id)
    nj = batch.get_output_unsigned(nj_id)
    batch.remove_data(ni_id)
    batch.remove_data(nj_id)
    return sun_cam, ni, nj


#######################################################
# update sun visibility probabilities
def update_sun_visibilities(
        scene, device, ocl_cache, cache, sun_camera, ni, nj, prefix_name):
    batch.init_process("boxm2OclUpdateSunVisibilitiesProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, ocl_cache)
    batch.set_input_from_db(3, cache)
    batch.set_input_from_db(4, sun_camera)
    batch.set_input_unsigned(5, ni)
    batch.set_input_unsigned(6, nj)
    batch.set_input_string(7, prefix_name)
    batch.run_process()

#######################################################
# render shadow map


def render_shadow_map(scene, device, ocl_cache,
                      camera, ni, nj, prefix_name=''):
    batch.init_process("boxm2OclRenderExpectedShadowMapProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, ocl_cache)
    batch.set_input_from_db(3, camera)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    batch.set_input_string(6, prefix_name)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    shadow_map = dbvalue(id, type)
    return shadow_map


def scene_illumination_info(scene):
    batch.init_process("boxm2SceneIlluminationInfoProcess")
    batch.set_input_from_db(0, scene)
    status = batch.run_process()
    longitude = latitude = nbins = None
    if status:
        (lon_id, lon_type) = batch.commit_output(0)
        longitude = batch.get_output_float(lon_id)
        (lat_id, lat_type) = batch.commit_output(1)
        latitude = batch.get_output_float(lat_id)
        (nb_id, nb_type) = batch.commit_output(2)
        nbins = batch.get_output_int(nb_id)
        batch.remove_data(lon_id)
        batch.remove_data(lat_id)
        batch.remove_data(nb_id)
    return longitude, latitude, nbins

# create stream cache


def create_stream_cache(scene, type_id_fname, image_id_fname, mem=2.0):
    batch.init_process("boxm2CreateStreamCacheProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, type_id_fname)
    batch.set_input_string(2, image_id_fname)
    # number of gigabytes available for stream cache
    batch.set_input_float(3, mem)
    batch.run_process()
    (cache_id, cache_type) = batch.commit_output(0)
    strcache = dbvalue(cache_id, cache_type)
    return strcache


def perspective_camera_from_scene(scene, cent_x, cent_y, cent_z, ni, nj):
    batch.init_process("vpglPerspCameraFromSceneProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_float(1, cent_x)
    batch.set_input_float(2, cent_y)
    batch.set_input_float(3, cent_z)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam

# create orthogonal camera from a boxm2 scene


def ortho_geo_cam_from_scene(scene):
    batch.init_process("boxm2OrthoGeoCamFromSceneProcess")
    batch.set_input_from_db(0, scene)
    result = batch.run_process()
    if result:
        (id, type) = batch.commit_output(0)
        cam = dbvalue(id, type)
        (ni_id, type) = batch.commit_output(1)
        ni = batch.get_output_unsigned(ni_id)
        (nj_id, type) = batch.commit_output(2)
        nj = batch.get_output_unsigned(nj_id)
        batch.remove_data(ni_id)
        batch.remove_data(nj_id)
    else:
        cam = 0
        ni = 0
        nj = 0
    return cam, ni, nj

# Create x y z images from a DEM at the resolution of the scene


def generate_xyz_from_dem(scene, geotiff_dem, geoid_height,
                          geocam=None, fill_in_value=-1.0):
    batch.init_process("boxm2DemToXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, geotiff_dem)
    batch.set_input_double(2, geoid_height)
    if geocam:
        batch.set_input_from_db(3, geocam)
    batch.set_input_float(4, fill_in_value)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img

# Create x y z images from a DEM at the resolution of the input DEM images


def generate_xyz_from_dem2(
        scene, geotiff_dem, geoid_height, geocam=None, fill_in_value=-1.0):
    batch.init_process("boxm2DemToXYZProcess2")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, geotiff_dem)
    batch.set_input_double(2, geoid_height)
    if geocam:
        batch.set_input_from_db(3, geocam)
    batch.set_input_float(4, fill_in_value)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img

# generate x,y,z images to ingest to the world such that ground plane of
# the scene will be initialized as occupied


def generate_xyz_for_ground_initialization(scene, global_ground_z):
    batch.init_process("boxm2InitializeGroundXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_int(1, global_ground_z)
    batch.run_process()
    (xi_id, xi_type) = batch.commit_output(0)
    x_img = dbvalue(xi_id, xi_type)
    (yi_id, yi_type) = batch.commit_output(1)
    y_img = dbvalue(yi_id, yi_type)
    (zi_id, zi_type) = batch.commit_output(2)
    z_img = dbvalue(zi_id, zi_type)
    return x_img, y_img, z_img


def generate_xyz_from_shadow(scene, height_img, generic_cam, dem_fname, scale):
    batch.init_process("boxm2ShadowHeightsToXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, height_img)
    batch.set_input_from_db(2, generic_cam)
    batch.set_input_string(3, dem_fname)
    batch.set_input_double(4, scale)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img

# Create x y z images from a DEM at the resolution of the scene


def generate_xyz_from_lidar(scene, tiff_lidar):
    batch.init_process("boxm2LidarToXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, tiff_lidar)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img

# Create x y z images from geo cover images and open street map objects


def genearate_xyz_from_osm(scene, tiff_geo_cover, osm_bin):
    batch.init_process("boxm2OSMToXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, tiff_geo_cover)
    batch.set_input_string(2, osm_bin)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
        (label_img_id, label_img_id_type) = batch.commit_output(3)
        label_img = dbvalue(label_img_id, label_img_id_type)
        (label_color_id, label_color_type) = batch.commit_output(4)
        label_color_img = dbvalue(label_color_id, label_color_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
        label_img = 0
        label_color_img = 0
    return x_img, y_img, z_img, label_img, label_color_img

# Create x y z images from a class image at the resolution of the scene
# If camera is not given, reads image geo cam from the image file name or
# from file header


def generate_xyz_from_label_img(scene, label_tiff, geocam=0):
    batch.init_process("boxm2LabelImgToXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, label_tiff)
    batch.set_input_from_db(2, geocam)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
        (li_id, li_type) = batch.commit_output(3)
        l_img = dbvalue(li_id, li_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
        l_img = 0
    return x_img, y_img, z_img, l_img

# Create x y z images from a class image at the resolution of the given class image
# If the camera is not given, reads image geo cam from the image filename
# or from geotiff header


def generate_xyz_from_label_img2(scene, label_tiff, geocam=0):
    batch.init_process("boxm2LabelImgToXYZProcess2")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, label_tiff)
    batch.set_input_from_db(2, geocam)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
        (li_id, li_type) = batch.commit_output(3)
        l_img = dbvalue(li_id, li_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
        l_img = 0
    return x_img, y_img, z_img, l_img

# Create x y z images from an orhographic height map image,
# assumes that the height map has the pixel GSD the same as the finest voxel resolution of the scene
# assumes that the image is aligned with the world at its upper left corner,
# generate the local x,y,z images using this kind of an ortho height map


def generate_xyz_from_ortho_height_map(
        scene, height_map, height_map_mask, thres):
    batch.init_process("boxm2HeightMapToXYZProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, height_map)
    batch.set_input_from_db(2, height_map_mask)
    batch.set_input_float(3, thres)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img


def roi_init_geotiff(scene, geocam, geotiff_img_name, level=0):
    batch.init_process("boxm2RoiInitGeotiffProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, geocam)
    batch.set_input_string(2, geotiff_img_name)
    batch.set_input_unsigned(3, level)
    result = batch.run_process()
    if result:
        (id, type) = batch.commit_output(0)
        gcam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        cropped_image = dbvalue(id, type)
    else:
        gcam = 0
        cropped_image = 0
    return result, gcam, cropped_image


def extract_color_features(scene, cache, data_type, index):
    batch.init_process("boxm2ExtractColorFeaturesProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, data_type)
    batch.set_input_unsigned(3, index)
    batch.run_process()


def extract_surface_features(scene, cache, type, index):
    batch.init_process("boxm2ExtractSurfaceFeaturesProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, type)
    batch.set_input_unsigned(3, index)
    batch.run_process()


def block_similarity(scene, cache, i, j, k, vrml_filename,
                     feature_sim_variance, entropy_range_min, entropy_range_max):
    batch.init_process("boxm2BlockSimilarityProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_int(2, i)
    batch.set_input_int(3, j)
    batch.set_input_int(4, k)
    batch.set_input_string(5, vrml_filename)
    batch.set_input_float(6, feature_sim_variance)
    batch.set_input_float(7, entropy_range_min)  # for visualization
    batch.set_input_float(8, entropy_range_max)
    batch.run_process()


def compute_derivatives_process(scene, cache, prob_threshold, normal_threshold,
                                kernel_x_file_name, kernel_y_file_name, kernel_z_file_name, i=-1, j=-1, k=-1):
    batch.init_process("boxm2CppComputeDerivativeProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, prob_threshold)  # prob threshold
    batch.set_input_float(3, normal_threshold)  # normal t
    batch.set_input_string(
        4, "C:/projects/vxl/vxl/contrib/brl/bseg/bvpl/doc/taylor2_5_5_5/Ix.txt")
    batch.set_input_string(
        5, "C:/projects/vxl/vxl/contrib/brl/bseg/bvpl/doc/taylor2_5_5_5/Iy.txt")
    batch.set_input_string(
        6, "C:/projects/vxl/vxl/contrib/brl/bseg/bvpl/doc/taylor2_5_5_5/Iz.txt")
    batch.set_input_int(7, i)
    batch.set_input_int(8, j)
    batch.set_input_int(9, k)
    batch.run_process()


def compute_probability_of_image(device, scene, cache, cam, img):
    batch.init_process("boxm2OclProbabilityOfImageProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    result = batch.run_process()
    outimg = -1
    if result:
        (id, type) = batch.commit_output(0)
        outimg = dbvalue(id, type)
    return outimg


def cubic_compute_probabiltiy_of_image(
        device, scene, cache, cam, img, model_ident, img_ident):
    batch.init_process("boxm2OclProbabilityOfImageWcubicProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_string(5, model_ident)
    batch.set_input_string(6, img_ident)
    result = batch.run_process()
    outimg = -1
    if result:
        (id, type) = batch.commit_output(0)
        outimg = dbvalue(id, type)
    return outimg


def compute_visibility(device, scene, cache, camsfile,
                       depthdir, x, y, z, outputdir, scale):

    batch.init_process("boxm2OclComputeVisibilityProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_string(3, camsfile)
    batch.set_input_string(4, depthdir)
    batch.set_input_float(5, x)
    batch.set_input_float(6, y)
    batch.set_input_float(7, z)
    batch.set_input_string(8, outputdir)
    batch.set_input_int(9, scale)
    result = batch.run_process()
    return result


def compute_los_visibility(scene, cache, x0, y0, z0, x1, y1, z1, t=5):

    batch.init_process("boxm2CppLosVisibilityProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, x0)
    batch.set_input_float(3, y0)
    batch.set_input_float(4, z0)
    batch.set_input_float(5, x1)
    batch.set_input_float(6, y1)
    batch.set_input_float(7, z1)
    batch.set_input_float(8, t)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    vis = batch.get_output_float(id)
    batch.remove_data(id)
    return vis


def get_scene_from_box_cams(camsdir, x0, y0, z0, x1, y1, z1, modeldir,
                            lvcs_origin_lat=0.0, lvcs_origin_lon=0.0,
                            lvcs_origin_elev=0.0):

    batch.init_process("boxm2SceneFromBoxCamsProcess")
    batch.set_input_string(0, camsdir)
    batch.set_input_float(1, x0)
    batch.set_input_float(2, y0)
    batch.set_input_float(3, z0)
    batch.set_input_float(4, x1)
    batch.set_input_float(5, y1)
    batch.set_input_float(6, z1)
    batch.set_input_string(7, modeldir)
    batch.set_input_double(8, lvcs_origin_lat)
    batch.set_input_double(9, lvcs_origin_lon)
    batch.set_input_double(10, lvcs_origin_elev)

    result = batch.run_process()

    return result

####################################
# visibility index processes
####################################


def index_hypotheses(device, scene, opencl_cache, hyp_file, start_hyp_id, skip_hyp_id, elev_dif,
                     vmin, dmax, solid_angle, ray_file, out_name, visibility_threshold, index_buffer_capacity):
    batch.init_process("boxm2IndexHypothesesProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_string(3, hyp_file)
    batch.set_input_unsigned(4, start_hyp_id)
    batch.set_input_unsigned(5, skip_hyp_id)
    batch.set_input_float(6, elev_dif)
    batch.set_input_float(7, vmin)
    batch.set_input_float(8, dmax)
    batch.set_input_float(9, solid_angle)
    batch.set_input_string(10, ray_file)
    batch.set_input_string(11, out_name)
    batch.set_input_float(12, visibility_threshold)
    batch.set_input_float(13, index_buffer_capacity)
    batch.run_process()

# pass leaf_id = -1 to index all the leaves in the given tile


def index_hypotheses2(device, scene, opencl_cache, geo_hyp_file, tile_id, elev_dif, vmin, dmax,
                      solid_angle, ray_file, out_name, visibility_threshold, index_buffer_capacity, leaf_id=-1):
    batch.init_process("boxm2IndexHypothesesProcess2")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_string(3, geo_hyp_file)
    batch.set_input_unsigned(4, tile_id)
    batch.set_input_float(5, elev_dif)
    batch.set_input_float(6, vmin)
    batch.set_input_float(7, dmax)
    batch.set_input_float(8, solid_angle)
    batch.set_input_string(9, ray_file)
    batch.set_input_string(10, out_name)
    batch.set_input_float(11, visibility_threshold)
    batch.set_input_float(12, index_buffer_capacity)
    batch.set_input_int(13, leaf_id)
    batch.run_process()

# pass leaf_id = -1 to index all the leaves in the given tile


def index_label_data(device, scene, opencl_cache, geo_hyp_file, tile_id, elev_dif, vmin, dmax, solid_angle,
                     ray_file, out_name, visibility_threshold, index_buffer_capacity, identifier, leaf_id=-1):
    batch.init_process("boxm2IndexLabelDataProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_string(3, geo_hyp_file)
    batch.set_input_unsigned(4, tile_id)
    batch.set_input_float(5, elev_dif)
    batch.set_input_float(6, vmin)
    batch.set_input_float(7, dmax)
    batch.set_input_float(8, solid_angle)
    batch.set_input_string(9, ray_file)
    batch.set_input_string(10, out_name)
    batch.set_input_float(11, visibility_threshold)
    batch.set_input_float(12, index_buffer_capacity)
    batch.set_input_int(13, leaf_id)
    batch.set_input_string(14, identifier)
    batch.run_process()

# pass leaf_id = -1 to index all the leaves in the given tile


def index_label_data_combined(device, scene, opencl_cache, geo_hyp_file, tile_id, elev_dif, vmin, dmax,
                              solid_angle, ray_file, out_name, visibility_threshold, index_buffer_capacity, identifier, leaf_id=-1):
    batch.init_process("boxm2IndexLabelCombinedDataProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_string(3, geo_hyp_file)
    batch.set_input_unsigned(4, tile_id)
    batch.set_input_float(5, elev_dif)
    batch.set_input_float(6, vmin)
    batch.set_input_float(7, dmax)
    batch.set_input_float(8, solid_angle)
    batch.set_input_string(9, ray_file)
    batch.set_input_string(10, out_name)
    batch.set_input_float(11, visibility_threshold)
    batch.set_input_float(12, index_buffer_capacity)
    batch.set_input_int(13, leaf_id)
    batch.set_input_string(14, identifier)
    batch.run_process()


def visualize_indices(index_file, buffer_capacity,
                      start_i, end_i, out_prefix, ray_file):
    batch.init_process("boxm2VisualizeIndicesProcess")
    batch.set_input_string(0, index_file)
    batch.set_input_float(1, buffer_capacity)
    batch.set_input_unsigned(2, start_i)
    batch.set_input_unsigned(3, end_i)
    batch.set_input_string(4, out_prefix)
    batch.set_input_string(5, ray_file)
    batch.run_process()

# index_type = 0 --> visualize depth interval index
# = 1 --> visualize land class label index
# = 2 --> visualize orientation label index


def visualize_hyp_index(geo_hyp_file, geo_index_file,
                        ray_file, tile_id, lat, lon, index_type):
    batch.init_process("boxm2VisualizeHypIndexProcess")
    batch.set_input_string(0, geo_hyp_file)
    batch.set_input_string(1, geo_index_file)
    batch.set_input_string(2, ray_file)
    batch.set_input_unsigned(3, tile_id)
    batch.set_input_float(4, lat)
    batch.set_input_float(5, lon)
    batch.set_input_int(6, index_type)
    batch.run_process()

# load the score binary file generated by matcher


def load_score_binary(geo_hypo_folder, score_file,
                      out_text, tile_id, candidate_list=""):
    batch.init_process("boxm2LoadScoreBinary")
    batch.set_input_string(0, geo_hypo_folder)
    batch.set_input_string(1, candidate_list)
    batch.set_input_string(2, score_file)
    batch.set_input_string(3, out_text)
    batch.set_input_unsigned(4, tile_id)
    batch.run_process()

# return the location rank in the pin-point region


def location_rank_in_pin_point(lon, lat, pin_point_kml):
    batch.init_process("boxm2LocationPinPointRanking")
    batch.set_input_double(0, lon)
    batch.set_input_double(1, lat)
    batch.set_input_string(2, pin_point_kml)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        rank = batch.get_output_unsigned(id)
        batch.remove_data(id)
        return rank
    else:
        return 0


def cast_3d_point(scene, cache, perspective_camera, generic_camera,
                  depth_image, variance_image, appearance_model_name):
    batch.init_process("boxm2CppCast3dPointHypothesisProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_from_db(2, perspective_camera)
    batch.set_input_from_db(3, generic_camera)
    batch.set_input_from_db(4, depth_image)
    batch.set_input_from_db(5, variance_image)
    batch.set_input_string(6, appearance_model_name)
    batch.run_process()


def cast_3d_point_pass2(scene, cache, generic_camera, appearance_model_name,
                        cov_c_path, cov_v_path):
    batch.init_process("boxm2CppCast3dPointHypothesisProcess2")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_from_db(2, generic_camera)
    batch.set_input_string(3, appearance_model_name)
    batch.set_input_string(4, cov_c_path)
    batch.set_input_string(5, cov_v_path)
    batch.run_process()



def accumulate_3d_point_and_cov(scene, cache, appearance_model_name):
    batch.init_process("boxm2CppCompute3dPointsAndCovsProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, appearance_model_name)
    batch.run_process()



def normalize_3d_point_and_cov(scene, cache):
    batch.init_process("boxm2CppCompute3dPointsAndCovsNormalizeProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.run_process()

# process that find the minimum and maximum elevation from height map, for
# a give 2-d rectangluar region


def find_min_max_elev(ll_lon, ll_lat, ur_lon, ur_lat, dem_folder):
    batch.init_process("volmFindMinMaxHeightPorcess")
    batch.set_input_double(0, ll_lon)
    batch.set_input_double(1, ll_lat)
    batch.set_input_double(2, ur_lon)
    batch.set_input_double(3, ur_lat)
    batch.set_input_string(4, dem_folder)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        min_elev = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        max_elev = batch.get_output_double(id)
        return min_elev, max_elev
    else:
        return 0.0, 0.0


def boxm2_compute_pre_post(scene, device, cache, cam,
                           img, view_identifier, tnear=100000.0, tfar=100000.0):
    # If no device is passed in, do cpu update
    print("boxm2_batch GPU update")
    batch.init_process("boxm2OclComputePrePostProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_string(5, view_identifier)
    batch.set_input_float(6, tnear)
    batch.set_input_float(7, tfar)
    return batch.run_process()


def update_image_factor(scene, device, cache, sum, view_identifier):
    # If no device is passed in, do cpu update
    print("boxm2_batch GPU update")
    batch.init_process("boxm2OclUpdateImageFactorProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_bool(3, sum)
    batch.set_input_string(4, view_identifier)
    return batch.run_process()


def boxm2_fuse_factors(scene, device, cache, view_idents=[], weights=[]):
    # If no device is passed in, do cpu update
    print("boxm2_batch GPU update")
    batch.init_process("boxm2OclFuseFactorsProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_string_array(3, view_idents)
    batch.set_input_float_array(4, weights)
    return batch.run_process()


def compute_hmap_factor(scene, device, cache, zimg, zvar, ximg, yimg, sradius):
    # If no device is passed in, do cpu update
    print("boxm2_batch GPU compute_hmap_factor")
    batch.init_process("boxm2OclComputeHeightFactorProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_from_db(3, zimg)
    batch.set_input_from_db(4, zvar)
    batch.set_input_from_db(5, ximg)
    batch.set_input_from_db(6, yimg)
    batch.set_input_int(7, sradius)
    return batch.run_process()


def update_hmap_factor(scene, device, cache, add):
    # If no device is passed in, do cpu update
    print("boxm2_batch GPU update")
    batch.init_process("boxm2OclUpdateHeightMapFactorProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_bool(3, add)
    return batch.run_process()


def boxm2_init_uniform_prob(scene, device, cache):
    # If no device is passed in, do cpu update
    print("boxm2_batch GPU update")
    batch.init_process("boxm2OclInitProbUniformProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    return batch.run_process()


def boxm2_remove_low_nobs(scene, device, cache, nobs_thresh_multiplier):
    print("boxm2_batch GPU process")
    batch.init_process("boxm2OclRemoveLowNobsProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cache)
    batch.set_input_float(3, nobs_thresh_multiplier)
    return batch.run_process()


def compactify_mog6_view(scene, cache):
    """Converts a MOG6_VIEW scene to a compactified representation, in
    which each gaussian's mean and variance are stored with one byte
    of precision.

    """
    batch.init_process("boxm2CompactifyMog6ViewProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.run_process()
