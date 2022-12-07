
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include <camera/Camera.h>
#include <binder/IMemory.h>
#include <log/log.h>

#include "NdkSurface.h"
#include "NdkCamera.h"

using namespace android;

int main(int argc __unused, char** argv __unused) {
    sp<Camera> camera;
    int cameraId = 0;

    camera = Camera::connect(cameraId, String16("junsxu"), Camera::USE_CALLING_UID, Camera::USE_CALLING_PID);
    // make sure camera hardware is alive
    if (camera->getStatus() != NO_ERROR) {
        return NO_INIT;
    }

    sp<NdkCamera> context = new NdkCamera(camera);
    context->incStrong((void*)context.get());

    camera->setListener(context);

    // Update default display orientation in case the sensor is reverse-landscape
    CameraInfo cameraInfo;

    status_t rc = Camera::getCameraInfo(cameraId, &cameraInfo);
    if (rc != NO_ERROR) {
        return rc;
    }

    int defaultOrientation = cameraInfo.orientation;
    switch (cameraInfo.orientation) {
        case 0:
            break;
        case 90:
            if (cameraInfo.facing == CAMERA_FACING_FRONT) {
                defaultOrientation = 180;
            }
            break;
        case 180:
            defaultOrientation = 180;
            break;
        case 270:
            if (cameraInfo.facing != CAMERA_FACING_FRONT) {
                defaultOrientation = 180;
            }
            break;
        default:
            ALOGE("Unexpected camera orientation %d!", cameraInfo.orientation);
            break;
    }
    JUNS_LOGI("cameraInfo.orientation %d", cameraInfo.orientation);

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

