#define LOG_TAG "ndkcamera"

#include "NdkCamera.h"
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include <camera/Camera.h>
#include <binder/IMemory.h>
#include <log/log.h>

using namespace android;

#define JUNS_LOGI(fmt, msg...)   ALOGI("%s(%d): " fmt, __FUNCTION__, __LINE__, ##msg)
#define JUNS_LOGE(fmt, msg...)   ALOGE("%s(%d): " fmt, __FUNCTION__, __LINE__, ##msg)

NdkCamera::NdkCamera(const sp<Camera>& camera)
{
    mCamera = camera;
}

void NdkCamera::notify(int32_t msgType, int32_t ext1, int32_t ext2)
{
    ALOGI("notify msgType %d, ext1 %d, ext2 %d", msgType, ext1, ext2);
}

void NdkCamera::copyAndPost(const sp<IMemory>& dataPtr, int msgType)
{
    ssize_t offset;
    size_t size;
    sp<IMemoryHeap> heap = dataPtr->getMemory(&offset, &size);
    uint8_t *heapBase = (uint8_t*)heap->base();
    uint8_t* data = heapBase + offset;
    
    ALOGI("copyAndPost: off=%zd, size=%zu, data %p", offset, size, data);
}

void NdkCamera::postData(int32_t msgType, const sp<IMemory>& dataPtr,
                                camera_frame_metadata_t *metadata)
{
    int32_t dataMsgType = msgType & ~CAMERA_MSG_PREVIEW_METADATA;

    copyAndPost(dataPtr, dataMsgType);
    ALOGI("dataMsgType 0x%X", dataMsgType);
}

void NdkCamera::postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr)
{
    // TODO: plumb up to Java. For now, just drop the timestamp
    postData(msgType, dataPtr, NULL);
}

void NdkCamera::postRecordingFrameHandleTimestamp(nsecs_t, native_handle_t* handle) {
    // Video buffers are not needed at app layer so just return the video buffers here.
    // This may be called when stagefright just releases camera but there are still outstanding
    // video buffers.
    if (mCamera != nullptr) {
        mCamera->releaseRecordingFrameHandle(handle);
    } else {
        native_handle_close(handle);
        native_handle_delete(handle);
    }
}

void NdkCamera::postRecordingFrameHandleTimestampBatch(
        const std::vector<nsecs_t>&,
        const std::vector<native_handle_t*>& handles) {
    // Video buffers are not needed at app layer so just return the video buffers here.
    // This may be called when stagefright just releases camera but there are still outstanding
    // video buffers.
    if (mCamera != nullptr) {
        mCamera->releaseRecordingFrameHandleBatch(handles);
    } else {
        for (auto& handle : handles) {
            native_handle_close(handle);
            native_handle_delete(handle);
        }
    }
}

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

    int defaultOrientation = 0;
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

    if (defaultOrientation != 0) {
        JUNS_LOGI("Setting default display orientation to %d", defaultOrientation);
        rc = camera->sendCommand(CAMERA_CMD_SET_DISPLAY_ORIENTATION,
                defaultOrientation, 0);
        if (rc != NO_ERROR) {
            ALOGE("Unable to update default orientation: %s (%d)",
                    strerror(-rc), rc);
            return rc;
        }
    }

    while(true) usleep(1000*1000);

    return 0;
}


