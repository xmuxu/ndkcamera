
#include "NdkCamera.h"
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include <camera/Camera.h>
#include <binder/IMemory.h>
#include <log/log.h>

using namespace android;

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

