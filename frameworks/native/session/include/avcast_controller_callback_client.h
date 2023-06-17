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

#ifndef OHOS_AVCAST_CONTROLLER_CALLBACK_CLIENT_H
#define OHOS_AVCAST_CONTROLLER_CALLBACK_CLIENT_H

#include "avcast_controller_callback_stub.h"
#include "avsession_info.h"

namespace OHOS::AVSession {
class AVCastControllerCallbackClient : public AVCastControllerCallbackStub {
public:
    explicit AVCastControllerCallbackClient(const std::shared_ptr<AVCastControllerCallback>& callback);
    ~AVCastControllerCallbackClient();


    void OnStateChange(const AVCastPlayerState& state) override;

    void OnMediaItemChange(const AVQueueItem& avQueueItem) override;

    void OnVolumeChange(const int32_t volume) override;

    void OnLoopModeChange(const int32_t loopMode) override;

    void OnPlaySpeedChange(const int32_t playSpeed) override;

    void OnPositionChange(const int32_t position, const int32_t bufferPosition, const int32_t duration) override;

    void OnVideoSizeChange(const int32_t width, const int32_t height) override;
    
    void OnError(const int32_t errorCode, const std::string& errorMsg) override;

private:
    std::shared_ptr<AVCastControllerCallback> callback_;
    static constexpr const char* EVENT_NAME = "AVCastControllerCallback";
};
}
#endif // OHOS_AVCAST_CONTROLLER_CALLBACK_CLIENT_H
