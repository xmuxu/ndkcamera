/*
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __NDK_SURFACE_H__
#define __NDK_SURFACE_H__

#include <gui/SurfaceControl.h>

namespace android {

/*
 * A window that covers the entire display surface.
 *
 * The window is destroyed when this object is destroyed, so don't try
 * to use the surface after that point.
 */
class NdkSurface {
public:
    // Creates the window.
    NdkSurface();

    // Retrieves a handle to the window.
    sp<Surface> getSurface() const;

private:
    NdkSurface(const NdkSurface&);
    NdkSurface& operator=(const NdkSurface&);

    sp<SurfaceControl> mSurfaceControl;
};

} // namespace android

#endif /* __NDK_SURFACE_H__ */
