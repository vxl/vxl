import os
import sys
from PIL import Image
import getopt
import time
import re
import subprocess
import shlex
from xml.dom.minidom import parse, parseString
import random
import shutil

import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

import boxm2_scene_adaptor
import boxm2_adaptor
import vil_adaptor_boxm2_batch as vil
import vpgl_adaptor_boxm2_batch as vpgl

dryrun = False


def log(msg):

    timeStr = time.strftime("%m/%d/%y %H:%M:%S", time.localtime())
    if dryrun:
        print "[%s][DRYRUN] %s" % (timeStr, msg)
    else:
        print "[%s] %s" % (timeStr, msg)


def getAllFilesInSubDirs(root, makeRelative=True):
    fileList = []
    for path, subdirs, files in os.walk(root):
        for fileName in files:
            if makeRelative:
                fileList.append(os.path.relpath(
                    os.path.join(path, fileName), root))
            else:
                fileList.append(os.path.join(path, fileName))

    return fileList


def copyFile(src, dst):

    if (not os.path.exists(os.path.dirname(dst))):
        if not dryrun:
            os.makedirs(os.path.dirname(dst))

    if not dryrun:
        shutil.copy2(src, dst)
    log("\tCopied %s to %s" % (src, dst))
    return 1


def convertcamera(srcFileName, dstFileName):
    filehandle = open(srcFileName)
    K = []
    Rbae = []
    Cbae = []
    for line in filehandle:
        p = str.split(line)
        for i in range(0, len(p)):
            K.append(float(p[i]))
        if(len(K) == 9):
            break
    for line in filehandle:
        p = str.split(line)
        for i in range(0, len(p)):
            Rbae.append(float(p[i]))
        if(len(Rbae) == 9):
            break
    for line in filehandle:
        p = str.split(line)
        for i in range(0, len(p)):
            Cbae.append(float(p[i]))
        if(len(Cbae) == 3):
            break
    filehandle.close()

    filehandleout = open(dstFileName, 'w')

    for i in range(0, 3):
        print>>filehandleout, K[i * 3 + 0], K[i * 3 + 1], K[i * 3 + 2]

    print>>filehandleout
    for i in range(0, 3):
        print>>filehandleout, Rbae[i], Rbae[i + 3], Rbae[i + 6]
    print>>filehandleout
    T = [0, 0, 0]
    for i in range(0, 3):
        T[i] = Rbae[i] * Cbae[0] + Rbae[i + 3] * \
            Cbae[1] + Rbae[i + 6] * Cbae[2]
    print>>filehandleout, -T[0], -T[1], -T[2]
    filehandleout.close()


def removeFile(fileName):

    if not dryrun:
        os.remove(fileName)
    log("\tRemoved %s." % (fileName))
    return 1


class TailwindPipelineRunner():

    def __init__(self, config):

        self.srcJPGDimensions = None
        self.config = config

    def runPipeline(self):

        steps = [
            # Step 1. Find the PNGs in the data directory, convert them to JPGs
            # and write them out to a new folder.
            self.movePNGs,
            self.runVisualSFM,  # Step 2. Run VisualSFM to generate a NVM file.
            # Step 3. Create the scene using the .nvm file produced above.
            self.createScene,
            self.updateScene,  # Step 4. Update the scene
            self.exportScene  # Step 5. Export the scene
        ]

        for stepIndex in xrange(self.config.resumeAtStep, len(steps)):

            retVal = steps[stepIndex](stepIndex + 1)

            if 0 != retVal:
                log("Processing cannot continue.")
                return retVal

        return 0

    def runPipeline2(self):

        steps = [
            # Step 1. Find the PNGs in the data directory, convert them to JPGs
            # and write them out to a new folder.
            self.moveData,
            # Step 2. Create the scene using the .nvm file produced above.
            self.createSceneFromBox,
            self.updateScene,  # Step 3. Update the scene
            self.exportScene   # Step 4. Export the scene
        ]
        for stepIndex in xrange(self.config.resumeAtStep, len(steps)):
            retVal = steps[stepIndex](stepIndex + 1)
            print stepIndex, retVal
            if 0 != retVal:
                log("Processing cannot continue.")
                return retVal

        return 0

    def callSystem(self, cmd, args, force=False):

        fullCommand = ' '.join([cmd, args])

        log("Running system command: %s" % (fullCommand))
        try:
            if dryrun and not force:
                retVal = 0
            else:
                retVal = subprocess.call(shlex.split(fullCommand))
        except OSError, err:
            log("Unhandled OSError exception %s" % err)

        return retVal

    def movePNGs(self, stepNum):

        srcDataFiles = getAllFilesInSubDirs(os.path.relpath(
            self.config.SRCImageDirName), makeRelative=False)
        pngRegex = re.compile('.+\.png', re.IGNORECASE)

        # Create a directory called "jpeg" to store all the newly converted PNG
        # files.
        if (not os.path.isdir(self.config.JPEGDirName)):
            if not dryrun:
                os.makedirs(self.config.JPEGDirName)

        for srcFileName in srcDataFiles:

            if re.search(pngRegex, srcFileName) is None:
                continue

            # This file is a PNG. Let's load the image
            img = Image.open(srcFileName)

            if None == self.srcJPGDimensions:
                self.srcJPGDimensions = img.size

            fileName = os.path.basename(srcFileName)
            baseFileName, extension = os.path.splitext(fileName)
            newName = os.path.join(
                self.config.JPEGDirName, baseFileName + ".jpg")
            img.save(newName, "JPEG")

            log("Converted and moved %s to %s." % (srcFileName, newName))

    def moveData(self, stepNum):
        pngRegex = re.compile('.+\.png', re.IGNORECASE)
        camRegex = re.compile('.+\.txt', re.IGNORECASE)

        inputfile = self.config.inputListFile
        filehandle = open(inputfile, 'r')
        # Create a directory called "jpeg" to store all the newly converted PNG
        # files.
        if (not os.path.isdir(self.config.IMGDirName)):
            if not dryrun:
                os.makedirs(self.config.IMGDirName)

        for srcFileName in filehandle:
            srcFileName = srcFileName.rstrip()
            if re.search(pngRegex, srcFileName) is not None:
                fileName = os.path.basename(srcFileName)
                newName = os.path.join(self.config.IMGDirName, fileName)
                copyFile(srcFileName, newName)
                log("moved %s to %s." % (srcFileName, newName))
            if re.search(camRegex, srcFileName) is not None:
                fileName = os.path.basename(srcFileName)
                newName = os.path.join(self.config.CAMDirName, fileName)

                copyFile(srcFileName, newName)
                convertcamera(srcFileName, newName)
                log("moved %s to %s." % (srcFileName, newName))
        return 0

    def createSceneFromBox(self, stepNum):
        filehandle = open(self.config.bbFileName, 'r')
        pts = []
        for line in filehandle:
            p = str.split(line)
            for i in range(0, len(p)):
                pts.append(float(p[i]))
            print pts

        success = 0
        if(len(pts) == 6):
            success = boxm2_adaptor.get_scene_from_box_cams(self.config.CAMDirName,
                                                            pts[0], pts[1], pts[2], pts[
                                                                3] - pts[0], pts[4] - pts[1],
                                                            pts[5] - pts[2], ".")
        if success:
            return 0
        else:
            return None

    def getSourceImageFileDimension(self):

        srcDataFiles = getAllFilesInSubDirs(os.path.relpath(
            self.config.SRCImageDirName), makeRelative=False)
        if 0 == len(srcDataFiles):
            log("Unable to find any source image files %s" %
                os.path.relpath(self.config.SRCImageDirName))
            return None

        pngRegex = re.compile('.+\.png', re.IGNORECASE)

        for srcFileName in srcDataFiles:
            if re.search(pngRegex, srcFileName) is None:
                continue
            img = Image.open(srcFileName)

            return img.size

        return None

    def runVisualSFM(self, stepNum):

        if self.srcJPGDimensions == None:
            self.srcJPGDimensions = self.getSourceImageFileDimension()
            if None == self.srcJPGDimensions:
                log("Step %d - Unable to determine source image size dimensions for images here: %s." %
                    (stepNum, os.path.relpath(self.config.SRCImageDirName)))
                return -stepNum

        commandLineArgs = "sfm+k=%d,%d,%d,%d %s %s" % (self.config.focalLength, self.srcJPGDimensions[0] / 2,
                                                       self.config.focalLength, self.srcJPGDimensions[
            1] / 2,
            os.path.abspath(
            self.config.JPEGDirName),
            self.config.NVMFileName
        )

        retVal = self.callSystem(
            "/home/vsi/projects/vsfm/vsfm/bin/VisualSFM", commandLineArgs)
        if 0 != retVal:
            log("Step %d - VisualSFM failed with return code %d!" %
                (stepNum, retVal))
            log("Processing will not proceed.")
            return -stepNum

        return 0

    def exportScene(self, stepNum):

        # Extract the cell size from a file called ./model/uscene.xml
        retVal, finestCellsize = self.extractCellSize(stepNum)

        if False == retVal:
            log("Step %d - extractCellSize failed." % (stepNum))
            return -stepNum

        commandLineArgs = "-scene ./model/uscene.xml -dir %s -depth 1 -ni 1024 -nj 768 -num_in 5 -init_incline 45 -end_incline 5 -gsd %s " % (
            self.config.finalExportDirName, finestCellsize)
        retVal = self.callSystem("boxm2_export_scene",
                                 commandLineArgs, force=True)

        if 0 != retVal:
            log("Step %d - boxm2_export_scene failed with return code %d!" %
                (stepNum, retVal))
            return -stepNum

        return 0

    def updateScene(self, stepNum):

        # Should initialize a GPU
        scenePath = os.path.abspath(self.config.uSceneFileName)
        scene = boxm2_scene_adaptor.boxm2_scene_adaptor(scenePath,  "gpu0")

        # Get list of imgs and cams
        imgsDir = os.path.abspath(os.path.join(
            self.config.NVMOutputDirName, "imgs"))
        camsDir = os.path.abspath(os.path.join(
            self.config.NVMOutputDirName, "cams"))

        if False == os.path.isdir(imgsDir):
            log("The folder %s does not exist." % (imgsDir))
            return -stepNum

        if False == os.path.isdir(camsDir):
            log("The folder %s does not exist." % (camsDir))
            return -stepNum

        imgFiles = getAllFilesInSubDirs(imgsDir)
        camFiles = getAllFilesInSubDirs(camsDir)

        if len(imgFiles) != len(camFiles):
            log("The number of files in %s is %d." % (imgsDir, len(imgFiles)))
            log("The number of fiels in %s is %d." % (camsDir, len(camFiles)))
            log("These numbers should match and they do not.")
            return -stepNum

        if len(imgFiles) == 0:
            log("No image files were found in %s." % (imgsDir))
            return -stepNum

        imgFiles.sort()
        camFiles.sort()

        # Make two passes over the image set
        for p in xrange(0, self.config.updateScenePassCount):
            frames = range(0, len(imgFiles), 1)
            if self.config.randomizeUpdateOrder:
                random.shuffle(frames)

            for idx, i in enumerate(frames):
                pcam = vpgl.load_perspective_camera(
                    os.path.join(camsDir, camFiles[i]))
                img, ni, nj = vil.load_image(os.path.join(imgsDir, imgFiles[i]))

                scene.update(pcam, img, 1, "", "gpu0")

                if 0 == (idx % 15):  # No idea what is special about the magic number 15
                    scene.refine()

        scene.write_cache()

        return 0

    def extractCellSize(self, stepNum):

        if False == os.path.isfile(self.config.uSceneFileName):
            log("Step %d - Cell size extraction failed because the uScene file can't be found at %s." %
                (stepNum, self.config.uSceneFileName))
            return False, None

        dom = parse(self.config.uSceneFileName)
        xmlTag = dom.getElementsByTagName("block")
        if 0 == len(xmlTag):
            log("Step %d - No \"block\" elements could be found in the XML document %s." %
                (stepNum, self.config.uSceneFileName))
            return False, None

        if False == xmlTag[0].hasAttribute("dim_x"):
            log("Step %d - The XML document %s Has a \"block\" element has no \"dim_x\" attribute." %
                (stepNum, self.config.uSceneFileName))
            return False, None

        dimXString = xmlTag[0].attributes["dim_x"].value

        try:
            dimXValue = float(dimXString) * 0.125
        except ValueError:
            log("Step %d - The parsed value of dimX %s appears to be invalid." %
                (stepNum, dimXValue))
            return False, None

        log("Finest cell size of %f found." % dimXValue)
        return True, dimXValue

    def createScene(self, stepNum):

        bundleFile = os.path.abspath(self.config.NVMFileName)
        imgDir = os.path.abspath(self.config.SRCImageDirName)
        outDir = os.path.abspath(self.config.NVMOutputDirName)

        if False == os.path.isfile(bundleFile):
            log("Step %d - SceneCreation cannot begin because the NVM file %s is missing." %
                (stepNum, bundleFile))
            log("Processing will not proceed.")
            return -stepNum

        if False == os.path.isdir(imgDir):
            log("Step %d - SceneCreation cannot beging because the image directory %s cannot be found." %
                (stepNum, imgDir))
            log("Processing will not proceed.")
            return -stepNum

        log("Creating scene with bundleFile %s." % (bundleFile))
        log("Output will be written to %s." % (outDir))

        # run process
        batch.init_process("boxm2BundleToSceneProcess")
        batch.set_input_string(0, bundleFile)
        batch.set_input_string(1, imgDir)
        batch.set_input_string(2, self.config.NVMAppModel)
        batch.set_input_string(3, self.config.NVMNobsModel)
        batch.set_input_int(4, 8)
        batch.set_input_string(5, outDir)
        batch.run_process()
        (scene_id, scene_type) = batch.commit_output(0)
        uscene = dbvalue(scene_id, scene_type)
        (scene_id, scene_type) = batch.commit_output(1)
        rscene = dbvalue(scene_id, scene_type)

        return 0


class PipelineConfig():

    def __init__(self):
        self.JPEGDirName = 'jpeg'
        self.CAMDirName = 'nvm_out/cams'
        self.IMGDirName = 'nvm_out/imgs'
        self.inputListFile = None
        self.bbFileName = None
        self.modelDir = 'model'
        self.SRCImageDirName = 'imgs'
        self.NVMFileName = './output.nvm'
        self.NVMOutputDirName = './nvm_out'
        self.NVMAppModel = "boxm2_mog3_grey"
        self.NVMNobsModel = "boxm2_num_obs"
        self.uSceneFileName = './model/uscene.xml'
        self.finalExportDirName = './export'
        self.randomizeUpdateOrder = False
        self.updateScenePassCount = 1
        self.resumeAtStep = 0

    def setFocalLength(self, fl):

        try:
            self.focalLength = int(fl)
        except ValueError:
            log("The value %s is not an acceptable focalLength." % fl)

    def setUpdatePassCount(self, cnt):
        try:
            self.updateScenePassCount = int(cnt)
        except ValueError:
            log("The value %s is not an acceptable update pass count." % cnt)

    def setResumeAtStep(self, step):
        try:
            self.resumeAtStep = int(step)
        except ValueError:
            log("The value %s is not an acceptable step to resume at." % step)

    def isConfigComplete(self):

        isOK = True
        if self.inputListFile == None or self.bbFileName == None:
            log("The input list file is missing ")
            isOK = False
        if self.resumeAtStep < 0 or self.resumeAtStep > 3:
            log("The \"step\" value of %d is not valid. Valid values are in the range of 0-3 (inclusive)." %
                self.resumeAtStep)
            isOK = False

        if self.updateScenePassCount < 1:
            log("The \"updateScenePassCount\" value of %d is not valid. Valid values are 1 or greater." %
                self.updateScenePassCount)
            isOK = False

        return isOK


if __name__ == '__main__':

    config = PipelineConfig()

    try:
        opts, args = getopt.getopt(sys.argv[1:], "d:b:i:e:rs:u:", [
            "dryrun", "bbfile=", "inputlist=", "exportDir=", "randomize", "step=", "updatePassCount="])
        print opts
    except getopt.GetoptError, err:
        print str(err) + "\n"
        sys.exit(1)

    for o, a in opts:
        o = str.lower(o)
        if o in ("-d", "--dryrun"):
            dryrun = True
        elif o in ("-i", "--inputlist"):
            config.inputListFile = a
        elif o in ("-b", "--bbfile"):
            config.bbFileName = a
        elif o in ("-e", "--exportdir"):
            config.finalExportDirName = a
        elif o in ("-r", "--randomize"):
            config.randomizeUpdateOrder = True
        elif o in ("-u", "--updatePassCount"):
            config.setUpdatePassCount(a)
        elif o in ("-s", "--step"):
            print a
            config.setResumeAtStep(a)

    if False == config.isConfigComplete():
        log("%s cannot be run until the above errors are fixed." %
            (sys.argv[0]))
        sys.exit(-100)

    pipeline = TailwindPipelineRunner(config)
    retVal = pipeline.runPipeline2()

    if 0 != retVal:
        log("Pipeline processing has failed with return code %s." % retVal)
    else:
        log("Pipeline processing has completed succesfully.")

    sys.exit(retVal)
