import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

import os
import re
import shutil
import sys
from xml.etree.ElementTree import ElementTree
from os.path import basename, splitext

import boxm2_adaptor
import boxm2_tools_adaptor
import boxm2_filtering_adaptor
import vil_adaptor_boxm2_batch as vil_adaptor
import vpgl_adaptor_boxm2_batch as vpgl_adaptor


#############################################################################
# boxm2_scene_adaptor class offers super simple model manipulation syntax
# you can always force the process to use CPP by just passing in "cpp" as the last
# arg to any function in this class
#############################################################################


class boxm2_scene_adaptor(object):

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
            self.scene, self.cpu_cache, self.device, self.opencl_cache = \
                boxm2_adaptor.load_opencl(scene_str, device_string)
            self.active_cache = self.opencl_cache
        elif device_string[0:3] == "cpp":
            self.scene, self.cpu_cache = boxm2_adaptor.load_cpp(scene_str)
            self.active_cache = self.cpu_cache
        else:
            print "UNKNOWN device type: ", device_string
            print "exiting."
            exit(-1)
        # store model directory for later use
        self.bbox = boxm2_adaptor.scene_bbox(self.scene)
        self.description = boxm2_adaptor.describe_scene(self.scene)
        self.model_dir = self.description['dataPath']
        # stores whether appearance model contains RGB - also includes view_dep
        self.rgb = self.description['appType'] == "boxm2_gauss_rgb"
        self.lvcs = boxm2_adaptor.scene_lvcs(self.scene)
        self.view = ("view" in self.description['appType'])

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

    def modify_appearance(self, app1, app2):
        status = boxm2_adaptor.modify_scene_appearance(self.scene, app1, app2)
        self.rgb = self.description['appType']
        return status
    # returns scene bounding box

    def bounding_box(self):
        return self.bbox

    def lvcs(self):
        return self.lvcs

    def cache():
        return self.cache
    def _get_device_cache(self, device_string):
        dev = self.device

        if device_string:
            # check if force gpu or cpu
            if device_string[0:3] == "gpu" or device_string[0:3] == "cpu":
                cache = self.opencl_cache
            elif device_string == "cpp":
                cache = self.cpu_cache
                dev = None
        else:
            cache = self.active_cache

        return (dev, cache)

    def transform_to_scene(self, to_scene, trans, rot, scale):
        if self.opencl_cache.type == "boxm2_opencl_cache_sptr":
            print("transforming scene")
            batch.init_process("boxm2VecfOclTransformSceneProcess")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, to_scene)
            batch.set_input_from_db(2, self.opencl_cache)
            batch.set_input_double(3, trans[0])
            batch.set_input_double(4, trans[1])
            batch.set_input_double(5, trans[2])
            batch.set_input_double(6,  rot[0][0])
            batch.set_input_double(7,  rot[0][1])
            batch.set_input_double(8,  rot[0][2])
            batch.set_input_double(9,  rot[1][0])
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

    def init_alpha(self, pinit=0.01, thresh=1.0):
        cache = self.opencl_cache
        dev = self.device
        boxm2_adaptor.init_alpha(self.scene, cache, dev, pinit, thresh)

    # update with alternate explaination prior and appearance density
    def update_with_alt(self, cam, img, update_alpha=True,
                        mask=None, var=-1.0, alt_prior=None, alt_density=None):
        cache = self.opencl_cache
        dev = self.device
        boxm2_adaptor.update_grey_with_alt(self.scene, cache, cam, img, dev,
                                           "", mask, update_alpha, var, alt_prior, alt_density)
    # update wrapper, can pass in a Null device to use

    def update(self, cam, img, update_alpha=True, update_app=True, mask=None,
               device_string="", var=-1.0, ident_string="", tnear=100000.0, tfar=100000.0):
        dev, cache = self._get_device_cache(device_string)

        # run update grey or RGB
        if self.rgb:
            if self.view:
                return boxm2_adaptor.update_rgb_view_dep(self.scene, cache, cam, img, dev,
                                                         ident_string, mask, update_alpha, var)
            else:
                return boxm2_adaptor.update_rgb(self.scene, cache, cam,
                                  img, dev, "", update_alpha)
        else:
            if self.view:
                return boxm2_adaptor.update_grey_view_dep(self.scene, cache, cam, img, dev,
                                            ident_string, mask, update_alpha, var)
            else:
                return boxm2_adaptor.update_grey(self.scene, cache, cam, img, dev, ident_string,
                                   mask, update_alpha, var, update_app, tnear, tfar)

    # update wrapper, can pass in a Null device to use
    def update_app(self, cam, img, device_string="", force_grey=False):
        if device_string and device_string[0:3] == "cpp":
            print " Not  implemented in C++ yet "
            return

        dev, cache = self._get_device_cache(device_string)

        if self.rgb and not force_grey:
            boxm2_adaptor.update_rgb(self.scene, cache, cam, img, dev, "", False)
        else:
            boxm2_adaptor.update_app_grey(self.scene, cache, cam, img, dev)

    # update skky wrapper, can pass in a Null device to use
    def update_sky(self, cam, img, device_string=""):
        if device_string and device_string[0:3] == "cpp":
            print " Not  implemented in C++ yet "
            return

        dev, cache = self._get_device_cache(device_string)

        boxm2_adaptor.update_sky(self.scene, cache, cam, img, dev)
    # update skky wrapper, can pass in a Null device to use

    def update_sky2(self, cam, img, step, device_string=""):
        if device_string and device_string[0:3] == "cpp":
            print " Not  implemented in C++ yet "
            return

        dev, cache = self._get_device_cache(device_string)

        boxm2_adaptor.update_sky2(self.scene, cache, cam, img, step, dev)

    # render wrapper, same as above
    def render(self, cam, ni=1280, nj=720, device_string="", ident_string="",
               tnear=1000000.0, tfar=1000000.0, ):
        dev, cache = self._get_device_cache(device_string)
        if self.rgb:
            if self.view:
                expimg = boxm2_adaptor.render_rgb_view_dep(self.scene, cache, cam,
                                             ni, nj, dev, ident_string)
            else:
                expimg, vis_image, status = boxm2_adaptor.render_rgb(
                    self.scene, cache, cam, ni, nj, dev, tnear, tfar)
                batch.remove_data(vis_image.id)
        else:
            if self.view:
                expimg = boxm2_adaptor.render_grey_view_dep(self.scene, cache, cam,
                                              ni, nj, dev, ident_string)
            else:
                expimg = boxm2_adaptor.render_grey(self.scene, cache, cam,
                                     ni, nj, dev, ident_string, tnear, tfar)
        return expimg

    # render wrapper, same as above
    def render_vis(self, cam, ni=1280, nj=720, device_string="", ident=""):
        dev, cache = self._get_device_cache(device_string)
        if self.rgb:
            expimg, vis_image, status = boxm2_adaptor.render_rgb(
                self.scene, cache, cam, ni, nj, dev)
        else:
            expimg, vis_image = boxm2_adaptor.render_grey_and_vis(
                self.scene, cache, cam, ni, nj, dev, ident)
        return expimg, vis_image

    # render depth image wrapper
    def render_depth(self, cam, ni=1280, nj=720, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        expimg, varimg, visimg = boxm2_adaptor.render_depth(
            self.scene, cache, cam, ni, nj, dev)
        return expimg, varimg, visimg

    # render the depth of the surfaces with max probability of being the the
    # first visible and occupied surface along the rays
    def render_depth_of_max_prob_surface(
          self, cam, ni=1280, nj=720, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        expimg, probimg, visimg = boxm2_adaptor.render_depth_of_max_prob_surface(
            self.scene, cache, cam, ni, nj, dev)
        return expimg, probimg, visimg

    # render depth image with loading given region wrapper
    def render_depth_region(self, cam, lat, lon, elev,
                            radius, ni=1280, nj=720, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        expimg, varimg, visimg = boxm2_adaptor.render_depth_region(
            self.scene, cache, cam, lat, lon, elev, radius, ni, nj, dev)
        return expimg, varimg, visimg

    # render z image wrapper
    def render_z_image(self, cam, ni=1280, nj=720,
                       normalize=False, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        z_exp_img, z_var_img = boxm2_adaptor.render_z_image(
            self.scene, cache, cam, ni, nj, normalize, dev)
        return z_exp_img, z_var_img

    # render heigh map render
    def render_height_map(self, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        z_image, var_image, x_image, y_image, prob_image = \
            boxm2_adaptor.render_height_map(self.scene, cache, dev)
        return z_image, var_image, x_image, y_image, prob_image

    # ingest heigh map
    def ingest_height_map(self, x_img, y_img, z_img,
                          zero_out_alpha=True, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.ingest_height_map(self.scene, cache, x_img, y_img,
                                        z_img, zero_out_alpha, dev)
        return

    # ingest heigh map
    def ingest_height_map_space(
            self, x_img, y_img, z_img, crust_thickness, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.ingest_height_map_space(self.scene, cache, x_img,
                                              y_img, z_img, crust_thickness, dev)
        return

    # ingest to zero out alphas along the rays given by the input images
    def ingest_to_zero_out_alpha(self, x_img, y_img, z_img, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.ingest_to_zero_out_alpha(self.scene, cache, x_img, y_img,
                                               z_img, dev)
        return

    # ingest label map
    # def ingest_label_map(self,x_img,y_img,z_img,label_img,device_string="") :
    def ingest_label_map(self, x_img, y_img, z_img,
                         label_img, ident, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        # ingest_label_map(self.scene, cache, x_img, y_img, z_img, label_img, dev);
        boxm2_adaptor.ingest_label_map(self.scene, cache, x_img, y_img,
                                       z_img, label_img, ident, dev)
        return

    # ingest label map
    def ingest_osm_label_map(self, x_img, y_img, z_img,
                             label_img, ident="land", device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.ingest_osm_label_map(self.scene, cache, x_img,
                                           y_img, z_img, label_img, ident, dev)
        return

    # ingest buckeye-style dem
    def ingest_buckeye_dem(self, first_ret_fname, last_ret_fname,
                           geoid_height, geocam, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.ingest_buckeye_dem(self.scene, cache, first_ret_fname,
                                         last_ret_fname, geoid_height, geocam,
                                         dev)
        return

    def probability_of(self, cam, image):
        cache = self.active_cache
        dev = self.device
        outimg = boxm2_adaptor.compute_probability_of_image(
            self.device, self.scene, self.opencl_cache, cam, image)
        return outimg

    def cubic_probability_of(self, cam, image, model_ident, img_ident):
        cache = self.active_cache
        dev = self.device
        outimg = boxm2_adaptor.cubic_compute_probability_of_image(
            self.device, self.scene, self.opencl_cache, cam, image, model_ident, img_ident)
        return outimg

    # detect change wrapper,
    def change_detect(self, cam, img, exp_img, n=1, raybelief="",
                      max_mode=False, rgb=False, device_string="", ident=""):
        dev, cache = self._get_device_cache(device_string)
        cd_img = boxm2_adaptor.change_detect(self.scene, cache, cam, img,
                                             exp_img, dev, rgb, n, raybelief, max_mode, ident)
        return cd_img

    # detect change wrapper,
    def change_detect2(self, cam, img, identifier="", max_mode=False,
                       tnear=10000000, tfar=0.00001, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        cd_img, vis_img = boxm2_adaptor.change_detect2(
            self.scene, cache, cam, img, identifier, max_mode, tnear, tfar, dev)
        return cd_img, vis_img

    def refine(self, thresh=0.3, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        return boxm2_adaptor.refine(self.scene, cache, thresh, dev)

    def merge(self, thresh=0.3, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.merge(self.scene, cache, thresh, dev)

    def median_filter(self, device_string=""):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.median_filter(self.scene, cache, dev)

    # given the scene, chip the NITF and setup the camera
    def roi_init(self, NITF_path, camera, convert_to_8bit,
                 params_fname, margin=0, clip_width=-1, clip_height=-1):
        return boxm2_adaptor.roi_init(NITF_path, camera, self.scene, convert_to_8bit,
                                      params_fname, margin, clip_width, clip_height)

    # Apply multiple filters to  scene
    def kernel_vector_filter(self, filters):
        return boxm2_filtering_adapto.apply_filters(self.scene, self.opencl_cache,
                                                    self.device, filters)

    # Interpolate normal from various responses
    def interpolate_normals(self, filters):
        return boxm2_filtering_adaptor.interpolate_normals(self.scene,
                                                           self.opencl_cache, self.device, filters)

    # Extract cell centers to XYZ for fast access
    def extract_cell_centers(self, prob_thresh=0.0):
        return boxm2_tools_adaptor.extract_cell_centers(self.scene,
                                                        self.cpu_cache, prob_thresh=0.0)

    #  Flip normals towards direction of maximum visibility
    def flip_normals(self, use_sum=False):
        return boxm2_filtering_adaptor.flip_normals(self.scene,
                                                    self.opencl_cache, self.device, use_sum)

    # Export points and normals to a .PLY file or XYZ. Points and normals need
    # to be extracted first
    def export_points_and_normals(self, file_out, save_aux=True,
                                  prob_thresh=0.0, vis_thresh=0.0,
                                  nmag_thresh=0.0, exp_thresh=0.0,
                                  bbox_file=""):
        return boxm2_tools_adaptor.export_points_and_normals(self.scene,
                                                             self.cpu_cache, file_out, save_aux, prob_thresh, vis_thresh,
                                                             nmag_thresh, exp_thresh, bbox_file)

    # Adds auxiliary data to vertices in a .PLY
    def add_aux_info_to_ply(self, file_in, file_out):
        boxm2_tools_adaptor.add_aux_info_to_ply(self.scene, self.cpu_cache,
                                                file_in, file_out)

    # only write the cpu_cache to disk
    def write_cache(self, do_clear=0):
        boxm2_adaptor.write_cache(self.cpu_cache, do_clear)

    # clear cache (both caches if OPENCL scene)
    def clear_cache(self):
        boxm2_adaptor.clear_cache(self.cpu_cache)
        if self.opencl_cache:
            boxm2_adaptor.clear_cache(self.opencl_cache)

    ################################
    # get info functions
    def get_info_along_ray(self, cam, u, v, prefix, identifier=""):
        return boxm2_tools_adaptor.get_info_along_ray(self.scene,
                                                      self.cpu_cache, cam, u, v, prefix, identifier)

    def query_cell_brdf(self, point, model_type):
        return boxm2_tools_adaptor.query_cell_brdf(self.scene, self.cpu_cache,
                                                   point, model_type)

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
            fname, fextension = os.path.splitext(img)
            bname = os.path.basename(fname)
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
        batch.init_process("boxm2CreateStreamCacheProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_string(1, type_id_fname)
        batch.set_input_string(2, image_id_fname)
        batch.set_input_float(3, max_gb)
        batch.run_process()
        (cache_id, cache_type) = batch.commit_output(0)
        self.str_cache = dbvalue(cache_id, cache_type)

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
            img, ni, nj = vil_adaptor.load_image(imgs[idx])
            pcam = vpgl_adaptor.load_perspective_camera(cams[idx])
            gcam = vpgl_adaptor.persp2gen(pcam, ni, nj)

            # update aux per view call
            fname, fextension = os.path.splitext(imgs[idx])
            imageID = os.path.basename(fname)
            self.update_aux(img, gcam, imageID)

    # create an imagewise aux buffer for cam/img
    def update_aux(self, img, cam, imgId, device_string="", mask=None):
        dev, cache = self._get_device_cache(device_string)
        boxm2_adaptor.update_aux_per_view(self.scene, cache, img, cam, imgId,
                                          dev, mask)

    # create an imagewise aux buffer for batch update of normal-albedo-array
    # appearance model
    def update_aux_naa(self, img, cam, metadata, atm_params, imgId, alt_prior,
                       alt_density):
        boxm2_adaptor.update_aux_per_view_naa(self.scene, self.opencl_cache,
                                              img, cam, metadata, atm_params, imgId, alt_prior, alt_density,
                                              self.device)

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
        n_table = dbvalue(id, type)

        # call batch paint process
        if device_string == "":
            batch.init_process("boxm2OclPaintBatchProcess")
            batch.set_input_from_db(0, self.device)
            batch.set_input_from_db(1, self.scene)
            batch.set_input_from_db(2, self.opencl_cache)
            batch.set_input_from_db(3, self.str_cache)
            batch.set_input_from_db(4, n_table)
            batch.run_process()
        elif device_string == "cpu":
            batch.init_process("boxm2CppBatchUpdateAppProcess")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, self.cpu_cache)
            batch.set_input_from_db(2, self.str_cache)
            batch.set_input_from_db(3, n_table)
            batch.run_process()

        # close the files so that they can be reloaded after the next iteration
        batch.init_process("boxm2StreamCacheCloseFilesProcess")
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
        n_table = dbvalue(id, type)

        # loop over images creating aux data
        for idx in range(0, len(imgs)):

            # load cam/img
            img, ni, nj = vil_adaptor.load_image(imgs[idx])
            pcam = vpgl_adaptor.load_perspective_camera(cams[idx])
            gcam = vpgl_adaptor.persp2gen(pcam, ni, nj)

            # create norm intensity (num rays...)
            batch.init_process("boxm2CppCreateNormIntensitiesProcess")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, self.cpu_cache)
            batch.set_input_from_db(2, gcam)
            batch.set_input_from_db(3, img)
            batch.set_input_string(4, "img_" + "%05d" % idx)
            batch.run_process()

            # create aux
            batch.init_process("boxm2CppCreateAuxDataOPT2Process")
            batch.set_input_from_db(0, self.scene)
            batch.set_input_from_db(1, self.cpu_cache)
            batch.set_input_from_db(2, gcam)
            batch.set_input_from_db(3, img)
            batch.set_input_string(4, "img_" + "%05d" % idx)
            batch.run_process()
            self.write_cache(True)

        batch.init_process("boxm2CppBatchUpdateOPT2Process")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.cpu_cache)
        batch.set_input_from_db(2, self.str_cache)
        batch.set_input_from_db(3, n_table)
        batch.run_process()

        # close the files so that they can be reloaded after the next iteration
        batch.init_process("boxm2StreamCacheCloseFilesProcess")
        batch.set_input_from_db(0, self.str_cache)
        batch.run_process()

        self.write_cache()

    def cpu_batch_compute_normal_albedo(
          self, metadata_filename_list, atmospheric_params_filename_list):
        batch.init_process("boxm2CppBatchComputeNormalAlbedoProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.cpu_cache)
        batch.set_input_from_db(2, self.str_cache)
        batch.set_input_string(3, metadata_filename_list)
        batch.set_input_string(4, atmospheric_params_filename_list)
        batch.run_process()

        # close the files so that they can be reloaded after the next iteration
        batch.init_process("boxm2StreamCacheCloseFilesProcess")
        batch.set_input_from_db(0, self.str_cache)
        batch.run_process()

    def ocl_batch_compute_normal_albedo(
          self, img_id_list, metadata_filename_list, atmospheric_params_filename_list):
        batch.init_process(
            "boxm2OclBatchComputeNormalAlbedoArrayProcess")
        batch.set_input_from_db(0, self.device)
        batch.set_input_from_db(1, self.scene)
        batch.set_input_from_db(2, self.opencl_cache)
        batch.set_input_string(3, img_id_list)
        batch.set_input_string(4, metadata_filename_list)
        batch.set_input_string(5, atmospheric_params_filename_list)
        batch.run_process()

    def render_expected_image_naa(
          self, camera, ni, nj, metadata, atmospheric_params):
        batch.init_process("boxm2OclRenderExpectedImageNAAProcess")
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
        exp_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        mask_image = dbvalue(id, type)
        return(exp_image, mask_image)

    def update_alpha_naa(self, image, camera, metadata,
                         atmospheric_params, alt_prior, alt_density):
        batch.init_process("boxm2OclUpdateAlphaNAAProcess")
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
        batch.init_process("boxm2OclRenderExpectedAlbedoNormalProcess")
        batch.set_input_from_db(0, self.device)
        batch.set_input_from_db(1, self.scene)
        batch.set_input_from_db(2, self.opencl_cache)
        batch.set_input_from_db(3, camera)
        batch.set_input_unsigned(4, ni)
        batch.set_input_unsigned(5, nj)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        exp_albedo = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        exp_normal = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        mask_image = dbvalue(id, type)
        return(exp_albedo, exp_normal, mask_image)

    def transform(self, tx, ty, tz, rx, ry, rz, scale):
        batch.init_process("boxm2TransformModelProcess")
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

    def compute_sun_affine_camera(self, sun_az, sun_el, astro_coords=True):
        (camera, ni, nj) = boxm2_adaptor.compute_sun_affine_camera(
            self.scene, sun_az, sun_el, astro_coords)
        return (camera, ni, nj)

    def update_sun_visibilities(self, sun_camera, ni, nj, prefix_name=""):
        boxm2_adaptor.update_sun_visibilities(self.scene, self.device,
                                              self.opencl_cache, self.cpu_cache, sun_camera, ni, nj, prefix_name)

    def render_shadow_map(self, camera, ni, nj, prefix_name=''):
        shadow_map = boxm2_adaptor.render_shadow_map(
            self.scene, self.device, self.opencl_cache, camera, ni, nj,
            prefix_name)
        return shadow_map

    def render_scene_mask(self, camera, ni, nj, ground_plane_only=False):
        mask = boxm2_adaptor.create_mask_image(self.scene, camera, ni, nj,
                                               ground_plane_only)
        return mask

    def normals_to_id(self):
        print("Normals to id ")
        batch.init_process("boxm2CppNormalsToIdProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.cpu_cache)
        return batch.run_process()

    def cache_neighbor_info(self):
        batch.init_process("boxm2VecfOclCacheNeighborInfoProcess")
        batch.set_input_from_db(0, self.scene)
        batch.set_input_from_db(1, self.opencl_cache)
        return batch.run_process()

    def refine_scene_around_geometry(
          self, filter_v, n_times, p_thresh, use_gpu):
        if self.opencl_cache.type == "boxm2_opencl_cache_sptr":
            print("Refining around surface geometry")
            batch.init_process(
                "boxm2_ocl_refine_scene_around_geometry_process")
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
            print "ERROR: Cache type not recognized: ", self.opencl_cache.type
            return False

    def compute_pre_post(self, cam, img, view_identifier="",
                         tnear=100000.0, tfar=100000.0):
        dev = self.device
        cache = self.opencl_cache
        return boxm2_adaptor.boxm2_compute_pre_post(
            self.scene, dev, cache, cam, img, view_identifier, tnear, tfar)

    def update_if(self, does_add=True, view_identifier=""):
        dev = self.device
        cache = self.opencl_cache
        return boxm2_adaptor.update_image_factor(
            self.scene, dev, cache, does_add, view_identifier)

    def fuse_factors(self, view_idents, weights):
        dev = self.device
        cache = self.opencl_cache
        return boxm2_adaptor.boxm2_fuse_factors(self.scene, dev, cache, view_idents, weights)

    def compute_hmapf(self, zimg, zvar, ximg, yimg, sradius=16):
        dev = self.device
        cache = self.opencl_cache
        return boxm2_adaptor.compute_hmap_factor(
            self.scene, dev, cache, zimg, zvar, ximg, yimg, sradius)

    def update_hf(self, does_add=True):
        dev = self.device
        cache = self.opencl_cache
        return boxm2_adaptor.update_hmap_factor(self.scene, dev, cache, does_add)

    def init_uniform_prob(self):
        return boxm2_adaptor.boxm2_init_uniform_prob(
            self.scene, self.device, self.opencl_cache)

    def remove_low_nobs(self, nobs_thresh_multiplier=3.0):
        dev = self.device
        cache = self.opencl_cache
        return boxm2_adaptor.boxm2_remove_low_nobs(
            self.scene, dev, cache, nobs_thresh_multiplier)


def compactify_mog6_view_scene(boxm2_dir, save_dir=None):
    if save_dir is None:
        save_dir = os.path.join(boxm2_dir, "sav")
    original_scene_file = os.path.join(boxm2_dir, 'scene.xml')
    compact_scene_file = os.path.join(boxm2_dir, 'compact_scene.xml')

    # load scene
    scene = boxm2_scene_adaptor(original_scene_file)
    boxm2_adaptor.compactify_mog6_view(scene.scene, scene.cpu_cache)
    scene.write_cache()

    # create new compact scene file with updated appearance model
    tree = ElementTree()
    tree.parse(original_scene_file)
    root = tree.getroot()
    for apm in root.iter('appearance'):
        if "mog6" in apm.get('apm'):
            apm.set('apm', 'boxm2_mog6_view_compact')
        else:
            root.remove(apm)
    tree.write(compact_scene_file)

    # remove un-compactified data files
    old_data_files_pattern = r"boxm2_(mog6_view_id|num_obs|vis)"
    for f in os.listdir(boxm2_dir):
        if re.search(old_data_files_pattern, f):
            os.remove(os.path.join(boxm2_dir, f))

    # copy files to save directory
    if not os.path.isdir(save_dir):
        os.mkdir(save_dir)
    else:
        for f in os.listdir(save_dir):
            if re.search(r".*\.bin", f):
                os.remove(os.path.join(save_dir, f))

    id_pat = [r'^id_', r'^boxm2_mog6_view_compact_id', r'^alpha_id_']
    for pat in id_pat:
        save_pattern(boxm2_dir, pat, save_dir)
    shutil.copy(os.path.join(boxm2_dir, 'compact_scene.xml'), save_dir)
    rewriteScenePath(save_dir, 'compact_scene.xml')


def save_pattern(search_dir, pattern, save_dir):
    """Copies all files in `search_dir` that match the given `pattern`
    into `save_dir`. If `save_dir` does not exist then nothing is
    done.

    :param search_dir: Directory from which to copy files
    :param pattern: Regex string to match files in `search_dir`
    :param save_dir: Absolute path in which to save copied files from
    `search_dir`
    """
    if not os.path.exists(save_dir):
        return

    for f in os.listdir(search_dir):
        if re.search(pattern, f):
            shutil.copy(os.path.join(search_dir, f), save_dir)


def rewriteScenePath(boxm2_dir, scene_file):
    """Rewrites a given scene XML file with a new scene_paths element
    that points to its containing directory.

    :param boxm2_dir: The directory containing the given scene file.
    :param scene_file: Relative path from `boxm2_dir` to the given
    XML file to modify.

    """
    abs_name = os.path.join(boxm2_dir, scene_file)
    if os.path.isfile(abs_name):
        tree = ElementTree()
        tree.parse(abs_name)
        root = tree.getroot()
        for path in root.iter('scene_paths'):
            path.set('path', boxm2_dir + '/')
        tree.write(abs_name)
