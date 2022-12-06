#ifndef __NDK_CAMERA_JUNSXU__

#include <camera/Camera.h>

namespace android {

class NdkCamera: public CameraListener
{
public:
    NdkCamera(const sp<Camera>& camera);
    NdkCamera() { mCamera.clear(); }
    // implemention of CameraListener interface
    virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2);
    virtual void postData(int32_t msgType, const sp<IMemory>& dataPtr,
                          camera_frame_metadata_t *metadata);
    virtual void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);
    virtual void postRecordingFrameHandleTimestamp(nsecs_t timestamp, native_handle_t* handle);
    virtual void postRecordingFrameHandleTimestampBatch(
            const std::vector<nsecs_t>& timestamps,
            const std::vector<native_handle_t*>& handles);
    sp<Camera> getCamera() { Mutex::Autolock _l(mLock); return mCamera; }

private:
    void copyAndPost(const sp<IMemory>& dataPtr, int msgType);
    sp<Camera>  mCamera;

    Mutex       mLock;
};

}
#endif //__NDK_CAMERA_JUNSXU__
