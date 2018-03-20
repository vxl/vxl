"""
Wrapper around bstm_adaptor functions. Creates "Scene" class to manage scene, caches, etc.
"""
import brl_init
import bstm_batch as batch
dbvalue = brl_init.register_batch(batch)

import sys
from os.path import basename, splitext

import bstm_adaptor
import vil_adaptor_bstm_batch as vil
import vpgl_adaptor_bstm_batch as vpgl


class bstm_scene_adaptor(object):

    # scene adaptor init
    def __init__(self, scene_str, device_string="gpu",
                 opencl_multi_scene_cache=False):

        # init (list) self vars
        self.scene = None
        self.active_cache = None
        self.device_string = None
        self.cpu_cache = None
        self.device = None
        self.opencl_cache = None
        self.str_cache = None
        self.model_dir = None
        self.bbox = None
        self.lvcs = None

        # if device_string is gpu, load up opencl
        self.device_string = device_string
        if device_string[0:3] == "gpu" or device_string[0:3] == "cpu":
            self.scene, self.cpu_cache, self.device, self.opencl_cache = bstm_adaptor.load_opencl(
                scene_str, device_string)
            self.active_cache = self.opencl_cache
        elif device_string[0:3] == "cpp":
            self.scene, self.cpu_cache = bstm_adaptor.load_cpp(scene_str)
            self.active_cache = self.cpu_cache
        else:
            print "UNKNOWN device type: ", device_string
            print "exiting."
            sys.exit(-1)
        # store model directory for later use
        self.bbox = bstm_adaptor.scene_bbox(self.scene)
        self.description = bstm_adaptor.describe_scene(self.scene)
        self.model_dir = self.description['dataPath']
        # stores whether appearance model contains RGB - also includes view_dep
        self.rgb = self.description['appType'].startswith("bstm_gauss_rgb")
        self.view = ("view" in self.description['appType'])
        self.lvcs = bstm_adaptor.scene_lvcs(self.scene)

    def __del__(self):
        if self.scene is not None:
            batch.remove_data(self.scene.id)
        if self.cpu_cache is not None:
            batch.remove_data(self.cpu_cache.id)
        if self.device is not None:
            batch.remove_data(self.device.id)
        if self.opencl_cache is not None:
            batch.remove_data(self.opencl_cache.id)
        if self.lvcs is not None:
            batch.remove_data(self.lvcs.id)

    # describe scene (returns data path)
    def describe(self):
        return self.description

    # returns scene bounding box
    def bounding_box(self):
        return self.bbox

    def lvcs(self):
        return self.lvcs

    def transform_to_scene(self, to_scene, trans, rot, scale):
        if self.opencl_cache.type == "bstm_opencl_cache_sptr":
            print("transforming scene")
            batch.init_process("bstmVecfOclTransformSceneProcess")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, to_scene)
            batch.set_input_from_db(2, self.opencl_cache)
            batch.set_input_double(3, trans[0])
            batch.set_input_double(4, trans[1])
            batch.set_input_double(5, trans[2])
            batch.set_input_double(6, rot[0][0])
            batch.set_input_double(7, rot[0][1])
            batch.set_input_double(8, rot[0][2])
            batch.set_input_double(9, rot[1][0])
            batch.set_input_double(10, rot[1][1])
            batch.set_input_double(11, rot[1][2])
            batch.set_input_double(12, rot[2][0])
            batch.set_input_double(13, rot[2][1])
            batch.set_input_double(14, rot[2][2])
            batch.set_input_double(15, scale[0])
            batch.set_input_double(16, scale[1])
            batch.set_input_double(17, scale[2])

            return batch.run_process()
        else:
            print "ERROR: Cache type not recognized: ", self.opencl_cache.type
            return False

    # update wrapper, can pass in a Null device to use
    def update(self, cam, img, time=0, update_alpha=True, update_changes_only=False,
               mask=None, device_string="", var=-1.0):
        cache = self.active_cache
        dev = self.device

        # check if force gpu or cpu
        if device_string == "gpu":
            cache = self.opencl_cache
        elif device_string == "cpp":
            cache = self.cpu_cache
            dev = None

        # run update grey or RGB
        if self.rgb:
            return bstm_adaptor.update_color(
                self.scene, dev, cache, cam, img, time, var, mask, update_alpha, update_changes_only)
        else:
            return bstm_adaptor.update(self.scene, dev, cache, cam, img,
                                       time, var, mask, update_alpha, update_changes_only)

    # update wrapper, can pass in a Null device to use
    def update_app(self, cam, img, time, var, mask,
                   device_string="", force_grey=False):
        cache = self.active_cache
        dev = self.device

        # check if force gpu or cpu
        if device_string == "gpu":
            cache = self.opencl_cache
        elif device_string == "cpp":
            print " Not  implemented in C++ yet "
            return
        if self.rgb and not force_grey:
            bstm_adaptor.update_color(
                self.scene,
                dev,
                cache,
                cam,
                img,
                time,
                var,
                mask,
                update_alpha=False)
        else:
            bstm_adaptor.update(
                self.scene,
                dev,
                cache,
                cam,
                img,
                time,
                var,
                mask,
                update_alpha=False)

    def render(self, cam, time=0, ni=1280, nj=720, device_string="",
               ident_string="", tnear=1000000.0, tfar=1000000.0, render_label=False):
        cache = self.active_cache
        dev = self.device
        # check if force gpu or cpu
        if device_string == "gpu":
            cache = self.opencl_cache
        elif device_string == "cpp":
            cache = self.cpu_cache
            dev = None
        exp_img, vis_img = bstm_adaptor.render(
            self.scene, dev, cache, cam, time, ni, nj, render_label)
        return exp_img, vis_img

    # detect change wrapper,
    def change_detect(self, cam, img, exp_img, n=1, raybelief="",
                      max_mode=False, rgb=False, device_string="", ident=""):
        cache = self.active_cache
        dev = self.device
        if device_string == "gpu":
            cache = self.opencl_cache
        elif device_string == "cpp":
            cache = self.cpu_cache
            dev = None
        cd_img = bstm_adaptor.change_detect(self.scene, cache, cam, img,
                                            exp_img, dev, rgb, n, raybelief, max_mode, ident)
        return cd_img

    def refine(self, thresh=0.3, device_string=""):
        if device_string == "":
            nCells = bstm_adaptor.refine(
                self.scene, self.active_cache, thresh, self.device)
        elif device_string == "gpu":
            nCells = bstm_adaptor.refine(
                self.scene, self.opencl_cache, thresh, self.device)
        elif device_string == "cpp":
            nCells = bstm_adaptor.refine(
                self.scene, self.cpu_cache, thresh, None)
        return nCells

    def merge(self, thresh=0.3, device_string=""):
        if device_string == "":
            bstm_adaptor.merge(
                self.scene,
                self.active_cache,
                thresh,
                self.device)
        elif device_string == "gpu":
            bstm_adaptor.merge(
                self.scene,
                self.opencl_cache,
                thresh,
                self.device)
        elif device_string == "cpp":
            bstm_adaptor.merge(self.scene, self.cpu_cache, thresh, None)

    # only write the cpu_cache to disk
    def write_cache(self, do_clear=0):
        bstm_adaptor.write_cache(self.cpu_cache, do_clear)

    # clear cache (both caches if OPENCL scene)
    def clear_cache(self):
        bstm_adaptor.clear_cache(self.cpu_cache)
        if self.opencl_cache:
            bstm_adaptor.clear_cache(self.opencl_cache)

    ################################
    # get info functions
    # def get_info_along_ray(self, cam, u, v, prefix, identifier=""):
    #     return get_info_along_ray(
    #         self.scene, self.cpu_cache, cam, u, v, prefix, identifier)

    # def query_cell_brdf(self, point, model_type):
    #     return query_cell_brdf(self.scene, self.cpu_cache, point, model_type)

    #####################################################################
    ######### BATCH UPDATE METHODS ######################################
    #####################################################################
    def create_stream_cache(self, imgs, interval=1, types="", max_gb=6.0):

        # write image identifiers to file
        # imgRange = range(0, len(imgs), interval);
        # num_imgs = len(imgRange);
        image_id_fname = self.model_dir + "/image_list.txt"
        fd = open(image_id_fname, "w")
        print >> fd, len(imgs)
        # for i in imgRange:
        #  print >>fd, "img_%05d"%i
        for img in imgs:
            fname, fextension = splitext(img)
            bname = basename(fname)
            print >> fd, bname
        fd.close()

        # write type identifiers into file
        type_id_fname = self.model_dir + "/type_names_list.txt"
        fd2 = open(type_id_fname, "w")
        print >>fd2, 4
        print >>fd2, "aux0"
        print >>fd2, "aux1"
        print >>fd2, "aux2"
        print >>fd2, "aux3"
        fd2.close()

        # open the stream cache, this is a read-only cache
        batch.init_process("bstmCreateStreamCacheProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_string(1, type_id_fname)
        batch.set_input_string(2, image_id_fname)
        batch.set_input_float(3, max_gb)
        batch.run_process()
        (cache_id, cache_type) = batch.commit_output(0)
        self.str_cache = batch.dbvalue(cache_id, cache_type)

    # remove stream cache object from database
    def destroy_stream_cache(self):
        if self.str_cache:
            batch.remove_data(self.str_cache.id)
            self.str_cache = None

    # writes aux data for each image in imgs array
    def write_aux_data(self, imgs, cams):
        for idx in range(len(imgs)):
            print '--------------------------'
            print "processing image " + imgs[idx]

            # load cam/img
            img, ni, nj = vil.load_image(imgs[idx])
            pcam = vpgl.load_perspective_camera(cams[idx])
            gcam = vpgl.persp2gen(pcam, ni, nj)

            # update aux per view call
            fname, fextension = splitext(imgs[idx])
            imageID = basename(fname)
            self.update_aux(img, gcam, imageID)

    # takes already created aux buffers (for each image) and fits a Mixture of 3
    # Gaussians to each cell, saves the appearance
    def batch_paint(self, imgs, cams, device_string=""):
        # verify stream cache
        if (self.str_cache is None):
            self.create_stream_cache(imgs)

        # sigma norm table?
        under_estimation_probability = 0.2
        batch.init_process("bstaSigmaNormTableProcess")
        batch.set_input_float(0, under_estimation_probability)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        n_table = batch.dvalue(id, type)

        # call batch paint process
        if device_string == "":
            batch.init_process("bstmOclPaintBatchProcess")
            batch.set_input_from_db(0, self.device)
            batch.set_input_from_db(1, self.scene)
            batch.set_input_from_db(2, self.opencl_cache)
            batch.set_input_from_db(3, self.str_cache)
            batch.set_input_from_db(4, n_table)
            batch.run_process()
        elif device_string == "cpu":
            print "Can't use CPU for Paint Batch Process."

        # close the files so that they can be reloaded after the next iteration
        batch.init_process("bstmStreamCacheCloseFilesProcess")
        batch.set_input_from_db(0, self.str_cache)
        batch.run_process()

        # write out afterwards
        self.write_cache()

    def cpu_batch_paint(self, imgs, cams):
        if (self.str_cache is None):
            self.create_stream_cache(imgs)

        # sigma norm table?
        under_estimation_probability = 0.2
        batch.init_process("bstaSigmaNormTableProcess")
        batch.set_input_float(0, under_estimation_probability)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        n_table = batch.dvalue(id, type)

        # loop over images creating aux data
        for idx in range(0, len(imgs)):

            # load cam/img
            img, ni, nj = vil.load_image(imgs[idx])
            pcam = vpgl.load_perspective_camera(cams[idx])
            gcam = vpgl.persp2gen(pcam, ni, nj)

            # create norm intensity (num rays...)
            batch.init_process("bstmCppCreateNormIntensitiesProcess")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, self.cpu_cache)
            batch.set_input_from_db(2, gcam)
            batch.set_input_from_db(3, img)
            batch.set_input_string(4, "img_" + "%05d" % idx)
            batch.run_process()

            # create aux
            batch.init_process("bstmCppCreateAuxDataOPT2Process")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, self.cpu_cache)
            batch.set_input_from_db(2, gcam)
            batch.set_input_from_db(3, img)
            batch.set_input_string(4, "img_" + "%05d" % idx)
            batch.run_process()
            self.write_cache(True)

        batch.init_process("bstmCppBatchUpdateOPT2Process")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.cpu_cache)
        batch.set_input_from_db(2, self.str_cache)
        batch.set_input_from_db(3, n_table)
        batch.run_process()

        # close the files so that they can be reloaded after the next iteration
        batch.init_process("bstmStreamCacheCloseFilesProcess")
        batch.set_input_from_db(0, self.str_cache)
        batch.run_process()

        self.write_cache()

    def cpu_batch_compute_normal_albedo(
            self, metadata_filename_list, atmospheric_params_filename_list):
        batch.init_process("bstmCppBatchComputeNormalAlbedoProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.cpu_cache)
        batch.set_input_from_db(2, self.str_cache)
        batch.set_input_string(3, metadata_filename_list)
        batch.set_input_string(4, atmospheric_params_filename_list)
        batch.run_process()

        # close the files so that they can be reloaded after the next iteration
        batch.init_process("bstmStreamCacheCloseFilesProcess")
        batch.set_input_from_db(0, self.str_cache)
        batch.run_process()

    def ocl_batch_compute_normal_albedo(
            self, img_id_list, metadata_filename_list, atmospheric_params_filename_list):
        batch.init_process(
            "bstmOclBatchComputeNormalAlbedoArrayProcess")
        batch.set_input_from_db(0, self.device)
        batch.set_input_from_db(1, self.scene)
        batch.set_input_from_db(2, self.opencl_cache)
        batch.set_input_string(3, img_id_list)
        batch.set_input_string(4, metadata_filename_list)
        batch.set_input_string(5, atmospheric_params_filename_list)
        batch.run_process()

    def render_expected_image_naa(
            self, camera, ni, nj, metadata, atmospheric_params):
        batch.init_process("bstmOclRenderExpectedImageNAAProcess")
        batch.set_input_from_db(0, self.device)
        batch.set_input_from_db(1, self.scene)
        batch.set_input_from_db(2, self.opencl_cache)
        batch.set_input_from_db(3, camera)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.set_input_from_db(6, metadata)
        batch.set_input_from_db(7, atmospheric_params)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_image = batch.dvalue(id, type)
        (id, type) = batch.commit_output(1)
        mask_image = batch.dvalue(id, type)
        return(exp_image, mask_image)

    def update_alpha_naa(self, image, camera, metadata,
                         atmospheric_params, alt_prior, alt_density):
        batch.init_process("bstmOclUpdateAlphaNAAProcess")
        batch.set_input_from_db(0, self.device)
        batch.set_input_from_db(1, self.scene)
        batch.set_input_from_db(2, self.opencl_cache)
        batch.set_input_from_db(3, camera)
        batch.set_input_from_db(4, image)
        batch.set_input_from_db(5, metadata)
        batch.set_input_from_db(6, atmospheric_params)
        batch.set_input_from_db(7, alt_prior)
        batch.set_input_from_db(8, alt_density)
        if not (batch.run_process()):
            print("ERROR: run_process() returned False")
        return

    def render_expected_albedo_normal(self, camera, ni, nj):
        batch.init_process("bstmOclRenderExpectedAlbedoNormalProcess")
        batch.set_input_from_db(0, self.device)
        batch.set_input_from_db(1, self.scene)
        batch.set_input_from_db(2, self.opencl_cache)
        batch.set_input_from_db(3, camera)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_albedo = batch.dvalue(id, type)
        (id, type) = batch.commit_output(1)
        exp_normal = batch.dvalue(id, type)
        (id, type) = batch.commit_output(2)
        mask_image = batch.dvalue(id, type)
        return(exp_albedo, exp_normal, mask_image)

    def transform(self, tx, ty, tz, rx, ry, rz, scale):
        batch.init_process("bstmTransformModelProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_float(1, tx)
        batch.set_input_float(2, ty)
        batch.set_input_float(3, tz)
        batch.set_input_float(4, rx)
        batch.set_input_float(5, ry)
        batch.set_input_float(6, rz)
        batch.set_input_float(7, scale)
        batch.run_process()
        return

    def cache_neighbor_info(self):
        batch.init_process("bstmVecfOclCacheNeighborInfoProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.opencl_cache)
        return batch.run_process()

    def refine_scene_around_geometry(
            self, filter_v, n_times, p_thresh, use_gpu):
        if self.opencl_cache.type == "bstm_opencl_cache_sptr":
            print("Refining around surface geometry")
            batch.init_process(
                "bstm_ocl_refine_scene_around_geometry_process")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, self.opencl_cache)
            batch.set_input_from_db(2, self.device)
            batch.set_input_from_db(3, filter_v)
            batch.set_input_int(4, n_times)
            # use negative value to refine all
            batch.set_input_float(5, p_thresh)
            batch.set_input_bool(6, use_gpu)
            return batch.run_process()
        else:
            print "ERROR: Cache type not recognized: ", cache.type
            return False

    def scene_statistics(self):
        return bstm_adaptor.scene_statistics(self.scene, self.cpu_cache)

    def ingest_boxm2_scene(self, boxm2_scene, boxm2_cpu_cache, time,
                           p_threshold=0.1, app_threshold=0.1):
        """ Ingests a BOXM2 scene for a certain time step.
        @param boxm2_scene - a boxm2_scene_adaptor object
        @param time - the timestemp in which to insert the BOXM2 scene
        """
        bstm_adaptor.ingest_boxm2_scene(
            self.scene,
            self.cpu_cache,
            boxm2_scene,
            boxm2_cpu_cache,
            time,
            p_threshold,
            app_threshold)

    def analyze_coherency(self, center, lengths, initial_time,
                          end_time, p_threshold, output_filename):
        bstm_adaptor.analyze_coherency(
            self.scene,
            self.cpu_cache,
            center,
            lengths,
            initial_time,
            end_time,
            p_threshold,
            output_filename)
