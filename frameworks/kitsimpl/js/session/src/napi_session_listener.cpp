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

#include "napi_session_listener.h"
#include <memory>
#include "avsession_log.h"
#include "avsession_info.h"

namespace OHOS::AVSession {
NapiSessionListener::NapiSessionListener()
{
    SLOGI("construct");
}

NapiSessionListener::~NapiSessionListener()
{
    SLOGI("destroy");
}

void NapiSessionListener::HandleEvent(int32_t event, const AVSessionDescriptor &descriptor)
{
    if (callbacks_[event] == nullptr) {
        SLOGE("not register callback event=%{public}d", event);
        return;
    }

    asyncCallback_->Call(callbacks_[event], [descriptor](napi_env env, int &argc, napi_value *argv) {
        argc = 1;
        NapiUtils::SetValue(env, descriptor, *argv);
    });
}

void NapiSessionListener::OnSessionCreate(const AVSessionDescriptor& descriptor)
{
    SLOGI("enter");
    HandleEvent(EVENT_SESSION_CREATED, descriptor);
}

void NapiSessionListener::OnSessionRelease(const AVSessionDescriptor& descriptor)
{
    SLOGI("enter");
    HandleEvent(EVENT_SESSION_DESTROYED, descriptor);
}

void NapiSessionListener::OnTopSessionChanged(const AVSessionDescriptor& descriptor)
{
    SLOGI("enter");
    HandleEvent(EVENT_TOP_SESSION_CHANGED, descriptor);
}

napi_status NapiSessionListener::AddCallback(napi_env env, int32_t event, napi_value callback)
{
    std::lock_guard<std::mutex> lockGuard(lock_);
    napi_ref ref = nullptr;
    napi_status status = napi_create_reference(env, callback, 1, &ref);
    if (status != napi_ok) {
        SLOGE("napi_create_reference failed");
        return status;
    }
    if (asyncCallback_ == nullptr) {
        asyncCallback_ = std::make_shared<NapiAsyncCallback>(env);
    }
    callbacks_[event] = ref;
    return napi_ok;
}

napi_status NapiSessionListener::RemoveCallback(napi_env env, int32_t event)
{
    std::lock_guard<std::mutex> lockGuard(lock_);
    auto ref = callbacks_[event];
    callbacks_[event] = nullptr;
    return napi_delete_reference(env, ref);
}
}