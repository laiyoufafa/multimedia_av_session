/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "play_info_holder.h"
#include "avsession_log.h"

namespace OHOS::AVSession {
bool PlayInfoHolder::Marshalling(Parcel& parcel) const
{
    int32_t playInfosSize = playInfos_.size();
    CHECK_AND_RETURN_RET_LOG(parcel.WriteInt32(playInfosSize), false, "write playInfoSize failed");
    for (auto playInfo : playInfos_) {
        CHECK_AND_RETURN_RET_LOG(playInfo.Marshalling(parcel), false, "write playInfoSize failed");
    }
    return true;
}

PlayInfoHolder *PlayInfoHolder::Unmarshalling(Parcel& data)
{
    auto *result = new (std::nothrow) PlayInfoHolder();

    int32_t playInfosSize;
    CHECK_AND_RETURN_RET_LOG(data.ReadInt32(playInfosSize), nullptr, "write playInfoSize failed");
    for (int i = 0; i < playInfosSize; i++) {
        AVQueueItem* queueItem = AVQueueItem::Unmarshalling(data);
        result->playInfos_.emplace_back(*queueItem);
    }
    CHECK_AND_RETURN_RET_LOG(result != nullptr, nullptr, "new PlayInfoHolder failed");
    return result;
}

void PlayInfoHolder::SetPlayInfos(const std::vector<AVQueueItem>& playInfos)
{
    playInfos_ = playInfos;
}

const std::vector<AVQueueItem>& PlayInfoHolder::GetPlayInfos() const
{
    return playInfos_;
}

bool PlayInfoHolder::IsValid() const
{
    return playInfos_.size() > 0;
}

void PlayInfoHolder::Reset()
{
    playInfos_.clear();
}
} // namespace OHOS::AVSession
