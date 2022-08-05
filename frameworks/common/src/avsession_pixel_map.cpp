/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "avsession_pixel_map.h"
#include "avsession_log.h"

namespace OHOS::AVSession {
bool AVSessionPixelMap::Marshalling(Parcel& parcel) const
{
    CHECK_AND_RETURN_RET_LOG(parcel.WriteUInt8Vector(imageInfo_), false, "Write image info bytes failed");
    CHECK_AND_RETURN_RET_LOG(parcel.WriteUInt8Vector(data_), false, "Write image data failed");
    return true;
}

AVSessionPixelMap *AVSessionPixelMap::Unmarshalling(Parcel& data)
{
    auto *result = new (std::nothrow) AVSessionPixelMap();
    CHECK_AND_RETURN_RET_LOG(result != nullptr, nullptr, "new AVSessionPixelMap failed");

    if (!data.ReadUInt8Vector(&result->imageInfo_)) {
        SLOGE("Read image info bytes failed");
        delete result;
        return nullptr;
    }

    if (!data.ReadUInt8Vector(&result->data_)) {
        SLOGE("Read image data failed");
        delete result;
        return nullptr;
    }
    return result;
}
} // namespace OHOS::AVSession