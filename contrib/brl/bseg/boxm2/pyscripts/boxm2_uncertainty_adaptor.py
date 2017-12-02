from boxm2_scene_adaptor import *
from bbas_adaptor import remove_from_db


class UncertainScene(boxm2_scene_adaptor):
    """ Class for an uncertain scene - can handle updating, painting, rendering """

    def __init__(self, imgList, camList, *args, **kw):
        """Initialize uncertain scene with imgs/cams,scene and device"""
        assert len(imgList) == len(camList)
        super(UncertainScene, self).__init__(*args, **kw)
        self.imgList = imgList
        self.camList = camList

    def render_uncertainty(self, cam, ni, nj):
        """ Render uncertainty - this requires a float8 to be present """
        img = render_grey(self.scene, self.opencl_cache,
                          cam, ni, nj, self.device, "cubic_model")
        return img

    # update batch with one call
    def update_batch(self):
        """ Run one batch update pass """
        # create aux data for each view
        # self.create_all_view_directions();
        # compute vis/seglen/appearance etc
        self.create_all_aux_data()
        # update synoptic function (per voxel cubic)
        # self.batch_synoptic_function();

        # update alphas
        # TODO incorporate alpha update (for now separate call)
        # self.update_all_alphas_with_cubic();

    def compute_uncertainty(self):
        """Store voxel uncertainty in cubic (float8) """
        self.store_all_uncertainty_aux()
        self.batch_uncertainty()

    # store all uncertainty aux data for each view
    def store_all_uncertainty_aux(self, offset=0):
        for i, img in enumerate(self.imgList):
            cam = load_perspective_camera(self.camList[i])
            img, ni, nj = load_image(self.imgList[i])
            gcam = persp2gen(cam, ni, nj)
            self.uncertainty_per_view(gcam, img, i + offset)
            remove_from_db([img, cam, gcam])
        self.write_cache()

    def uncertainty_per_view(self, cam, img, viewNum):
        """Store uncertainty aux data per image for this model """
        boxm2_batch.init_process("boxm2OclUncertaintyPerImageProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_from_db(3, cam)
        boxm2_batch.set_input_from_db(4, img)
        boxm2_batch.set_input_string(5, "uncertain_" + "%05d" % viewNum)
        boxm2_batch.run_process()

    def batch_uncertainty(self, image_id_fname):
        """Calculate voxel uncertainty"""
        # write image identifiers to file

        # open the stream cache, this is a read-only cache
        boxm2_batch.init_process("boxm2OclBatchUncertaintyProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_unsigned(3, len(self.imgList))
        boxm2_batch.set_input_string(4, image_id_fname)
        boxm2_batch.run_process()
        self.write_cache(True)

    # update step 1
    def create_all_view_directions(self):
        for i, img in enumerate(self.imgList):
            img, ni, nj = load_image(self.imgList[i])
            cam = load_perspective_camera(self.camList[i])
            self.create_view_directions(cam, ni, nj, i)
            remove_from_db([img, cam])
        self.write_cache(True)

    def create_view_directions(self, cam, ni, nj, viewNum):
        """ Create aux data for direction """
        boxm2_batch.init_process("boxm2OclAuxUpdateViewDirectionProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.cpu_cache)
        boxm2_batch.set_input_from_db(3, self.opencl_cache)
        boxm2_batch.set_input_from_db(4, cam)
        boxm2_batch.set_input_unsigned(5, ni)
        boxm2_batch.set_input_unsigned(6, nj)
        boxm2_batch.set_input_string(7, "viewdir_" + "%05d" % viewNum)
        boxm2_batch.set_input_string(8, "cartesian")
        boxm2_batch.run_process()

    # update step 2
    def create_all_aux_data(self):
        for i, img in enumerate(self.imgList):
            img, ni, nj = load_image(self.imgList[i])
            cam = load_perspective_camera(self.camList[i])
            self.create_aux_data(cam, img, i)
            remove_from_db([img, cam])
        self.write_cache(True)

    def create_aux_data(self, cam, img, viewNum):
        """ Computes aux for view num (visibility, seg len, app) """
        boxm2_batch.init_process("boxm2OclUpdateAuxPerViewProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_from_db(3, cam)
        boxm2_batch.set_input_from_db(4, img)
        boxm2_batch.set_input_string(5, "img_" + "%05d" % viewNum)
        boxm2_batch.run_process()

    # update step 4
    def update_all_alphas_with_cubic(self):
        for i, img in enumerate(self.imgList):
            cam = load_perspective_camera(self.camList[i])
            img, ni, nj = load_image(self.imgList[i])
            self.update_alpha_with_cubic(cam, img)
            remove_from_db([img, cam])
        self.write_cache()

    def update_alpha_with_cubic(self, pcam, img):
        """ Update occupancy (alpha) values using the cubic appearances """
        boxm2_batch.init_process("boxm2OclUpdateAlphaWcubicProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_from_db(3, pcam)
        boxm2_batch.set_input_from_db(4, img)
        boxm2_batch.set_input_string(5, "cubic_model")
        boxm2_batch.run_process()

    # update step 3
    def batch_synoptic_function(self):
        """ Create synoptic function (cubic function) at each voxel """
        # write image identifiers to file
        image_id_fname = self.model_dir + "/image_list.txt"
        fd = open(image_id_fname, "w")
        print >> fd, 2 * len(self.imgList)
        for i, img in enumerate(self.imgList):
            print >> fd, "img_%05d" % i
        for i, img in enumerate(self.imgList):
            print >> fd, "viewdir_%05d" % i
        fd.close()

        type_id_fname = self.model_dir + "/type_names_list.txt"
        fd2 = open(type_id_fname, "w")
        print >>fd2, 4
        print >>fd2, "aux0"
        print >>fd2, "aux1"
        print >>fd2, "aux2"
        print >>fd2, "aux3"
        fd2.close()

        # open the stream cache, this is a read-only cache
        boxm2_batch.init_process("boxm2OclBatchSynopticFunctionProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_unsigned(3, len(self.imgList))
        boxm2_batch.set_input_string(4, image_id_fname)
        boxm2_batch.set_input_float(5, 0.09	)
        boxm2_batch.run_process()

    def batch_synoptic_alpha_update(self):
        """ Create synoptic function (cubic function) at each voxel """
        image_id_fname = self.model_dir + "/image_list.txt"
        fd = open(image_id_fname, "w")
        print >> fd, len(self.imgList)
        for i, img in enumerate(self.imgList):
            print img
            print >> fd, "img_%05d" % i
        fd.close()

        boxm2_batch.init_process("boxm2OclSynopticUpdateAlphaProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_unsigned(3, len(self.imgList))
        boxm2_batch.set_input_string(4, image_id_fname)
        boxm2_batch.run_process()

        self.write_cache()

    def batch_synoptic_alpha_update_one_atatime(self):
        """ Create synoptic function (cubic function) at each voxel """
        image_id_fname = self.model_dir + "/image_list.txt"

        for i, img in enumerate(self.imgList):
            if i <= 5:
                fd = open(image_id_fname, "w")
                print >> fd, 1
                print >> fd, "img_%05d" % i
                fd.close()

                boxm2_batch.init_process("boxm2OclSynopticUpdateAlphaProcess")
                boxm2_batch.set_input_from_db(0, self.device)
                boxm2_batch.set_input_from_db(1, self.scene)
                boxm2_batch.set_input_from_db(2, self.opencl_cache)
                boxm2_batch.set_input_unsigned(3, 1)
                boxm2_batch.set_input_string(4, image_id_fname)
                boxm2_batch.run_process()

            self.write_cache()

    def render_uncertainty_map(self, ni, nj, ident="", cam_dir_1="", cam_dir_2=""):
        """ Render Hemispherical Uncertainty Map """
        # open the stream cache, this is a read-only cache
        boxm2_batch.init_process("boxm2OclRenderSceneUncertaintyMapProcess")
        boxm2_batch.set_input_from_db(0, self.device)
        boxm2_batch.set_input_from_db(1, self.scene)
        boxm2_batch.set_input_from_db(2, self.opencl_cache)
        boxm2_batch.set_input_unsigned(3, ni)
        boxm2_batch.set_input_unsigned(4, nj)
        boxm2_batch.set_input_string(5, ident)
        boxm2_batch.set_input_string(6, cam_dir_1)
        boxm2_batch.set_input_string(7, cam_dir_2)
        boxm2_batch.run_process()
        (id, type) = boxm2_batch.commit_output(0)
        exp_img = dbvalue(id, type)
        (id, type) = boxm2_batch.commit_output(1)
        vis_img = dbvalue(id, type)
        return exp_img, vis_img
