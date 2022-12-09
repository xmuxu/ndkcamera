
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include <camera/Camera.h>
#include <binder/IMemory.h>
#include <log/log.h>
#include <getopt.h>

#include "NdkSurface.h"
#include "NdkCamera.h"

using namespace android;

#define INFO(...) do { printf(__VA_ARGS__);} while(0)
#define ERR(...)  do { fprintf(stderr, "ERR: " __VA_ARGS__); } while (0)

#define CLEAR(x) memset(&(x), 0, sizeof(x))

void help() {
    INFO("-i -> camera id\n"
        "-p -> camera preview\n"
        "-c -> capture pic \n"
        "-v -> video recording\n"
        "-h -> help");
}

typedef struct cameraInfo_s {
    int camId;
    int preview;
    int capture;
    int videRec;
} cameraInfo_t;

static cameraInfo_t camInfo;

void parseArgs(int argc, char **argv) {
   int c;

   while (true) {
       int index = 0;
       static struct option mOptions[] = {
           {"index",        required_argument  , 0, 'i' },
           {"preview",      required_argument  , 0, 'p' },
           {"capture pic",   required_argument , 0, 'c' },
           {"video recording",required_argument, 0, 'v' },
           {"help",           no_argument,       0, 'h' },
           {0,                0,                 0,  0  }
       };

       c = getopt_long(argc, argv, "i:p:c:v:h", mOptions, &index);
       if (c == -1)
           break;

       switch (c) {
           case 'i':
               camInfo.camId = atoi(optarg);
               break;
           case 'p':
               camInfo.preview = atoi(optarg);
               break;
           case 'c':
               camInfo.capture = atoi(optarg);
               break;
           case 'v':
               camInfo.videRec = atoi(optarg);
               break;
           case 'h':
           default:
                help();
       }
   }
}


int main(int argc, char** argv) {
    sp<Camera> camera;

    parseArgs(argc, argv);

    camera = Camera::connect(camInfo.camId, String16("junsxu"), Camera::USE_CALLING_UID, Camera::USE_CALLING_PID);
    // make sure camera hardware is alive
    if (camera->getStatus() != NO_ERROR) {
        return NO_INIT;
    }

    sp<NdkCamera> context = new NdkCamera(camera);
    context->incStrong((void*)context.get());

    camera->setListener(context);

    // Update default display orientation in case the sensor is reverse-landscape
    CameraInfo cameraInfo;

    status_t rc = Camera::getCameraInfo(camInfo.camId, &cameraInfo);
    if (rc != NO_ERROR) {
        return rc;
    }

    JUNS_LOGI("cameraInfo.facing %d, cameraInfo.orientation %d",
               cameraInfo.facing, cameraInfo.orientation);

    if (cameraInfo.facing == CAMERA_FACING_FRONT) {
        cameraInfo.orientation = 270;
    } else {
        cameraInfo.orientation = 90;
    }

    rc = camera->sendCommand(CAMERA_CMD_SET_DISPLAY_ORIENTATION,
            cameraInfo.orientation, 0);
    if (rc != NO_ERROR) {
        JUNS_LOGE("Unable to update default orientation: %s (%d)",
                strerror(-rc), rc);
        return rc;
    }

    // get native window surface
    sp<IGraphicBufferProducer> gbp;
    NdkSurface ndkSurface;
    sp<Surface> surface = ndkSurface.getSurface();
    if (surface != NULL) {
        gbp = surface->getIGraphicBufferProducer();
    }

    JUNS_LOGI("set preview surface");
    if (camera->setPreviewTarget(gbp) != NO_ERROR) {
        JUNS_LOGE("set preview surface failed");
        return -1;
    }

    if (camera->startPreview() != NO_ERROR) {
        JUNS_LOGE("startPreview failed");
        return -1;
    }

    while(true) usleep(1000*1000);

    return 0;
}

