import glob
import os
import argparse
import math

from bstm_scene_adaptor import bstm_scene_adaptor
import bstm_adaptor as bstm
import bstm_batch
import bstm_register

import bstm_vil_adaptor as vil
import bstm_vpgl_adaptor as vpgl

# These functions are copies of those found in /boxm2/pyscripts/..., but
# they use bstm_batch instead, so that resulting dbvalues can be found by
# the BSTM batch DB.


def remove_from_db(dbvals):
    if not isinstance(dbvals, (list, tuple)):
        dbvals = [dbvals]
    for dbval in dbvals:
        bstm_batch.init_process("bbasRemoveFromDbProcess")
        bstm_batch.set_input_unsigned(0, dbval.id)
        bstm_batch.run_process()


def boxm2_load_scene(scene_str):
    # print("Loading a Scene from file: ", scene_str);
    bstm_batch.init_process("boxm2LoadSceneProcess")
    bstm_batch.set_input_string(0, scene_str)
    status = bstm_batch.run_process()
    if status:
        (scene_id, scene_type) = bstm_batch.commit_output(0)
        scene = bstm_register.dbvalue(scene_id, scene_type)
        return scene
    else:
        raise Exception('Could not load scene file')


# does the opencl prep work on an input scene
def boxm2_load_opencl(scene_str, device_string="gpu"):
    scene = boxm2_load_scene(scene_str)

    ###############################################################
    # Create cache, opencl manager, device, and gpu cache
    ###############################################################
    # print("Create Main Cache");
    bstm_batch.init_process("boxm2CreateCacheProcess")
    bstm_batch.set_input_from_db(0, scene)
    bstm_batch.set_input_string(1, "lru")
    result = bstm_batch.run_process()
    if not result:
        raise Exception('boxm2CreateCacheProcess returned false')
    (id, type) = bstm_batch.commit_output(0)
    cache = bstm_register.dbvalue(id, type)

    # print("Init Manager");
    bstm_batch.init_process("boclInitManagerProcess")
    result = bstm_batch.run_process()
    if not result:
        raise Exception('boxm2InitManagerProcess returned false')

    # print("Get Gpu Device");
    bstm_batch.init_process("boclGetDeviceProcess")
    bstm_batch.set_input_string(0, device_string)
    result = bstm_batch.run_process()
    if not result:
        raise Exception('boclGetDeviceProcess returned false')
    (id, type) = bstm_batch.commit_output(0)
    device = bstm_register.dbvalue(id, type)

    # print("Create Gpu Cache");
    bstm_batch.init_process("boxm2CreateOpenclCacheProcess")
    bstm_batch.set_input_from_db(0, device)
    result = bstm_batch.run_process()
    if not result:
        raise Exception('boxm2CreateOpenclCacheProcess returned false')
    (id, type) = bstm_batch.commit_output(0)
    openclcache = bstm_register.dbvalue(id, type)

    return scene, cache, device, openclcache


def boxm2_clear_cache(cache):
    if cache.type == "boxm2_cache_sptr":
        bstm_batch.init_process("boxm2ClearCacheProcess")
        bstm_batch.set_input_from_db(0, cache)
        bstm_batch.run_process()
    elif cache.type == "boxm2_opencl_cache_sptr":
        bstm_batch.init_process("boxm2ClearOpenclCacheProcess")
        bstm_batch.set_input_from_db(0, cache)
        bstm_batch.run_process()
    else:
        print "ERROR: Cache type needs to be boxm2_cache_sptr, not ", cache.type


def boxm2_render_grey_view_dep(scene, cache, cam, ni=1280, nj=720, device=None,
                               ident_string=""):
    if cache.type == "boxm2_opencl_cache_sptr" and device:
        bstm_batch.init_process("boxm2OclRenderViewDepExpectedImageProcess")
        bstm_batch.set_input_from_db(0, device)
        bstm_batch.set_input_from_db(1, scene)
        bstm_batch.set_input_from_db(2, cache)
        bstm_batch.set_input_from_db(3, cam)
        bstm_batch.set_input_unsigned(4, ni)
        bstm_batch.set_input_unsigned(5, nj)
        bstm_batch.set_input_string(6, ident_string)
        bstm_batch.run_process()
        (id, type) = bstm_batch.commit_output(0)
        exp_image = bstm_register.dbvalue(id, type)
        return exp_image
    else:
        print "ERROR: Cache type not recognized: ", cache.type


def parse_args():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        "bstm_model_dir",
        type=str,
        help="Directory containing BSTM data and scene.xml file")
    parser.add_argument(
        "boxm2_frames_dir",
        type=str,
        help="Directory containing BOXM2 models for each frame.")
    parser.add_argument(
        "--boxm2-scene-filename",
        type=str,
        default="scene.xml",
        help="Filename of BOXM2 scene XML files.")
    return parser.parse_args()


def get_boxm2_frames(boxm2_dir):
    """
Returns a sorted list of directories containing boxm2 frames. Assumes that `boxm2_dir` contains a set of folders, all named frame_<number>_boxm2
    """
    return sorted(glob.glob(os.path.join(boxm2_dir, "frame_*_boxm2")))


def main():
    args = parse_args()

    boxm2_frames = get_boxm2_frames(args.boxm2_frames_dir)
    if len(boxm2_frames) == 0:
        print "No BOXM2 frames found, doing nothing."
        return
    else:
        print "Found %d boxm2 frames..." % len(boxm2_frames)

    # create BSTM xml file if not present
    scene_file = os.path.join(args.bstm_model_dir, "scene.xml")
    if not os.path.exists(scene_file):
        print "BSTM scene.xml does not exist, trying to create from first BOXM2 frame's scene.xml...."
        first_frame_scene_file = os.path.join(
            boxm2_frames[0], args.boxm2_scene_filename)
        print "******", first_frame_scene_file
        bstm.boxm22scene(
            first_frame_scene_file,
            args.bstm_model_dir,
            timeSteps=len(boxm2_frames))
    if not os.path.exists(scene_file):
        print "Could not open BSTM scene file :-("
        return

    # create camera
    output_size = (960, 540)
    ppoint = [output_size[0] / 2, output_size[1] / 2]
    render_cam = vpgl.create_perspective_camera(scale=[2000, 2000],
                                                ppoint=ppoint,
                                                center=[0, 5, 0],
                                                look_pt=[0, 0, 0.5],
                                                up=[0, 0, 1]
                                                )

    bstm_scene = bstm_scene_adaptor(scene_file, "gpu0")
    for i, frame_dir in enumerate(boxm2_frames):
        theta = 6.28 * float(i) / len(boxm2_frames)
        render_cam = vpgl.create_perspective_camera(scale=[2000, 2000],
                                                    ppoint=ppoint,
                                                    center=[
                                                        6.5 * math.sin(theta), 6.5 * math.cos(theta), theta / 3],
                                                    look_pt=[0, 0, 1.0],
                                                    up=[0, 0, 1]
                                                    )
        print "***** %s" % ([
            5 * math.sin(theta), 5 * math.cos(theta), 0],)
        print "***** INGESTING FRAME %s" % frame_dir
        boxm2_scene_file = os.path.join(frame_dir, args.boxm2_scene_filename)
        boxm2_scene, boxm2_cpu_cache, boxm2_device, boxm2_opencl_cache = boxm2_load_opencl(
            boxm2_scene_file, "gpu0")
        # bstm_scene.ingest_boxm2_scene(boxm2_scene, boxm2_cpu_cache, i)

        # render BSTM scene
        exp_img, vis_img = bstm_scene.render(
            render_cam, i, output_size[0], output_size[1])
        exp_img_byte = vil.convert_image(exp_img)
        vil.save_image(
            exp_img_byte, "%s/render_bstm_%03d.png" %
            (args.bstm_model_dir, i))
        remove_from_db(exp_img)
        remove_from_db(vis_img)
        remove_from_db(exp_img_byte)

        # render BOXM2 scene for comparison
        exp_img = boxm2_render_grey_view_dep(
            boxm2_scene, boxm2_opencl_cache, render_cam, *output_size, device=boxm2_device)
        exp_img_byte = vil.convert_image(exp_img)
        vil.save_image(
            exp_img_byte, "%s/render_boxm2_%03d.png" %
            (args.bstm_model_dir, i))
        remove_from_db(exp_img)
        remove_from_db(vis_img)
        remove_from_db(exp_img_byte)

        # clean up BOXM2
        boxm2_clear_cache(boxm2_cpu_cache)
        boxm2_clear_cache(boxm2_opencl_cache)
        remove_from_db(boxm2_cpu_cache)
        remove_from_db(boxm2_opencl_cache)
        remove_from_db(boxm2_device)
        remove_from_db(boxm2_scene)

        # save BSTM
        bstm_scene.write_cache()

    remove_from_db(render_cam)
    bstm_scene.clear_cache()


if __name__ == "__main__":
    main()
