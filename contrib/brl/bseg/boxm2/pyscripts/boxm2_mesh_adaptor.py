import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)


def load_mesh(filename):
    batch.init_process("boxm2LoadMeshProcess")
    batch.set_input_string(0, filename)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mesh = dbvalue(id, type)
    return mesh


def gen_mesh(z_img, x_img, y_img, out_dir):
    batch.init_process("boxm2ExportMeshProcess")
    batch.set_input_from_db(0, z_img)
    batch.set_input_from_db(1, x_img)
    batch.set_input_from_db(2, y_img)
    batch.set_input_string(3, out_dir)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mesh = dbvalue(id, type)
    return mesh


def gen_tex_mesh(mesh, in_img_dir, in_cam_dir, out_dir):
    batch.init_process("boxm2TextureMeshProcess")
    batch.set_input_from_db(0, mesh)
    batch.set_input_string(1, in_img_dir)
    batch.set_input_string(2, in_cam_dir)
    batch.set_input_string(3, out_dir)
    batch.run_process()


def gen_point_cloud(scene, cache, filename, thresh=0.3, vis_thresh=0.5, depth=3):
    batch.init_process("boxm2ExtractPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, thresh)  # prob threshold
    batch.set_input_unsigned(3, depth)  # prob threshold
    batch.run_process()

    batch.init_process("boxm2ExportOrientedPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, filename)
    batch.set_input_float(4, vis_thresh)
    batch.set_input_float(6, thresh)
    batch.run_process()


def gen_color_point_cloud(scene, cache, filename, thresh=0.3, ident="", depth=3):
    batch.init_process("boxm2ExtractPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, thresh)  # prob threshold
    batch.set_input_unsigned(3, depth)  # prob threshold
    batch.run_process()

    batch.init_process("boxm2ExportColorPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, filename)
    batch.set_input_float(3, thresh)
    batch.set_input_string(4, ident)
    batch.run_process()


def gen_oriented_point_cloud(scene, cache, prob_t, norm_mag_t, filename):
    batch.init_process("boxm2CppComputeDerivativeProcesses")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, prob_t)
    batch.set_input_float(3, norm_mag_t)

# return points which have prob > prob_t and LE < LE_thres and CE < CE_thres
# if LE_thres and/or CE_thres are passed as -1.0, then thresholding based on corresponding error value is not applied
def gen_error_point_cloud(scene, cache, filename, thresh=0.3, LE_thresh=-1.0, CE_thresh=-1.0, depth=3):
    batch.init_process("boxm2ExtractPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_float(2, thresh)  # prob threshold
    batch.set_input_unsigned(3, depth)  # prob threshold
    batch.run_process()

    batch.init_process("boxm2ExportErrorPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, filename)  # ply filename
    batch.set_input_float(3, thresh)  # prob threshold
    batch.set_input_float(4, LE_thresh)  # LE threshold
    batch.set_input_float(5, CE_thresh)  # CE threshold
    batch.run_process()


def export_stack(scene, cache, outdir, identifier):
    batch.init_process("boxm2ExportStackImagesProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, outdir)
    batch.set_input_string(3, identifier)
    batch.run_process()


def paint_mesh(scene, cache, in_file, out_file):
    batch.init_process("boxm2PaintMeshProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, in_file)
    batch.set_input_string(3, out_file)
    batch.run_process()


def import_point_cloud(scene, cache, in_file, min_octree_depth=2):
    batch.init_process("boxm2ImportPointCloudProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, in_file)
    batch.set_input_unsigned(3, min_octree_depth)
    batch.run_process()


def import_triangle_mesh(scene, cache, ply_filename, occupied_prob=0.99):
    batch.init_process("boxm2ImportTriangleMeshProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_string(2, ply_filename)
    batch.set_input_float(3, occupied_prob)
    batch.run_process()


def batch_compute_3d_points(scene, cache, stream_cache):
    batch.init_process("boxm2CppBatchCompute3dPoints")
    batch.set_input_from_db(0, scene)
    batch.set_input_from_db(1, cache)
    batch.set_input_from_db(2, stream_cache)
    batch.run_process()
