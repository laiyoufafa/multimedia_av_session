/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "avcontroller_item.h"

#include "avsession_errors.h"
#include "avsession_log.h"
#include "avsession_item.h"

namespace OHOS::AVSession {
AVControllerItem::AVControllerItem(pid_t pid, sptr<AVSessionItem> &session)
    : pid_(pid), session_(session)
{
    SLOGD("construct");
}

AVControllerItem::~AVControllerItem()
{
    SLOGD("destroy");
}

int32_t AVControllerItem::RegisterCallbackInner(const sptr<IRemoteObject> &callback)
{
    callback_ = iface_cast<AVControllerCallbackProxy>(callback);
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::GetAVPlaybackState(AVPlaybackState &state)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    state = session_->GetPlaybackState();
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::GetAVMetaData(AVMetaData &data)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    data.CopyFrom(session_->GetMetaData());
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::sendMediaButtonEvent(MMI::KeyEvent& keyEvent)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    session_->HandleMediaButtonEvent(keyEvent);
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::GetLaunchAbility(AbilityRuntime::WantAgent::WantAgent &ability)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    ability = session_->GetLaunchAbility();
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::GetSupportedCommand(std::vector<int32_t> &cmds)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    cmds = session_->GetSupportCommand();
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::IsSessionActive(bool &isActive)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    isActive = session_->IsActive();
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::SendCommand(AVControlCommand &cmd)
{
    if (session_ == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    session_->ExecuteControllerCommand(cmd);
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::SetMetaFilter(const AVMetaData::MetaMaskType &filter)
{
    metaMask_ = filter;
    return AVSESSION_SUCCESS;
}

int32_t AVControllerItem::Release()
{
    if (callback_ != nullptr) {
        callback_.clear();
    }
    ClearSession();
    if (serviceCallback_) {
        serviceCallback_(*this);
    }
    return AVSESSION_SUCCESS;
}

void AVControllerItem::HandleSessionRelease(const AVSessionDescriptor &descriptor)
{
    if (callback_ != nullptr) {
        callback_->OnSessionRelease(descriptor);
    }
    ClearSession();
}

void AVControllerItem::HandlePlaybackStateChange(const AVPlaybackState &state)
{
    if (callback_ != nullptr) {
        callback_->OnPlaybackStateUpdate(state);
    }
}

void AVControllerItem::HandleMetaDataChange(const AVMetaData &data)
{
    if (callback_ == nullptr) {
        return;
    }
    AVMetaData metaOut;
    metaOut.SetMetaMask(metaMask_);
    if (data.CopyToByMask(metaOut)) {
        callback_->OnMetaDataUpdate(metaOut);
    }
}

void AVControllerItem::HandleActiveStateChange(bool isActive)
{
    if (callback_ != nullptr) {
        callback_->OnActiveStateChange(isActive);
    }
}

pid_t AVControllerItem::GetPid() const
{
    return pid_;
}

void AVControllerItem::ClearSession()
{
    if (session_ != nullptr) {
        session_->RemoveController(pid_);
        session_.clear();
    }
}

bool AVControllerItem::HasSession(int32_t sessionId)
{
    if (session_ != nullptr) {
        return session_->GetSessionId() == sessionId;
    }
    return false;
}

void AVControllerItem::SetServiceCallbackForRelease(const std::function<void(AVControllerItem &)> &callback)
{
    serviceCallback_ = callback;
}
} // OHOS::AVSession