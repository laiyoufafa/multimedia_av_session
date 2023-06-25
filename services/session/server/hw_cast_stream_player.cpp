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

#include "avsession_log.h"
#include "avcast_player_state.h"
#include "avqueue_item.h"
#include "avmedia_description.h"
#include "avsession_errors.h"
#include "hw_cast_stream_player.h"

using namespace OHOS::CastEngine;

namespace OHOS::AVSession {
HwCastStreamPlayer::~HwCastStreamPlayer()
{
    SLOGI("destruct the HwCastStreamPlayer");
    Release();
}

void HwCastStreamPlayer::Init()
{
    SLOGI("Init the HwCastStreamPlayer");
    if (streamPlayer_) {
        SLOGI("register self in streamPlayer");
        streamPlayer_->RegisterListener(shared_from_this());
    }
}

void HwCastStreamPlayer::Release()
{
    SLOGI("Release the HwCastStreamPlayer");
    streamPlayerListenerList_.clear();
    if (streamPlayer_) {
        streamPlayer_->UnregisterListener();
        streamPlayer_->Release();
    }
}

void HwCastStreamPlayer::SendControlCommand(const AVCastControlCommand castControlCommand)
{
    SLOGI("send command to streamPlayer");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return;
    }
    switch (castControlCommand.GetCommand()) {
        case AVCastControlCommand::CAST_CONTROL_CMD_PLAY:
            streamPlayer_ ->Play();
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_PAUSE:
            streamPlayer_ ->Pause();
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_STOP:
            streamPlayer_ ->Stop();
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_PLAY_NEXT:
            streamPlayer_ ->Next();
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_PLAY_PREVIOUS:
            streamPlayer_ ->Previous();
            break;
        default:
            SendControlCommandWithParams(castControlCommand);
            break;
    }
}

void HwCastStreamPlayer::SendControlCommandWithParams(const AVCastControlCommand castControlCommand)
{
    int32_t currentPosition = 0;
    switch (castControlCommand.GetCommand()) {
        case AVCastControlCommand::CAST_CONTROL_CMD_FAST_FORWARD:
            streamPlayer_->GetPosition(currentPosition);
            int32_t forwardTime;
            castControlCommand.GetForwardTime(forwardTime);
            streamPlayer_ ->Seek(currentPosition + forwardTime);
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_REWIND:
            streamPlayer_->GetPosition(currentPosition);
            int32_t rewindTime;
            castControlCommand.GetRewindTime(rewindTime);
            streamPlayer_ ->Seek(rewindTime > currentPosition ? 0 : currentPosition - rewindTime);
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_SEEK:
            int32_t seekTime;
            castControlCommand.GetSeekTime(seekTime);
            streamPlayer_ ->Seek(seekTime);
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_SET_VOLUME:
            int32_t volume;
            castControlCommand.GetVolume(volume);
            streamPlayer_ ->SetVolume(volume);
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_SET_SPEED:
            int32_t speed;
            castControlCommand.GetSpeed(speed);
            streamPlayer_ ->SetSpeed(static_cast<CastEngine::PlaybackSpeed>(speed));
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_SET_LOOP_MODE:
            int32_t loopMode;
            castControlCommand.GetLoopMode(loopMode);
            streamPlayer_ ->SetLoopMode(static_cast<CastEngine::LoopMode>(loopMode));
            break;
        case AVCastControlCommand::CAST_CONTROL_CMD_TOGGLE_FAVORITE:
            break;
        default:
            SLOGE("invalid command");
            break;
    }
}

int32_t HwCastStreamPlayer::SetMediaList(const MediaInfoHolder &mediaInfoHolder)
{
    CastEngine::MediaInfoHolder info;
    info.currentIndex = mediaInfoHolder.GetCurrentIndex();
    std::vector<CastEngine::MediaInfo> mediaInfoList;
    for (AVQueueItem item: mediaInfoHolder.GetPlayInfos()) {
        std::shared_ptr<AVMediaDescription> mediaDescription = item.GetDescription();
        CastEngine::MediaInfo mediaInfo;
        mediaInfo.mediaId = mediaDescription->GetMediaId();
        mediaInfo.mediaName = mediaDescription->GetTitle();
        mediaInfo.mediaUrl = mediaDescription->GetMediaUri();
        mediaInfo.mediaType = mediaDescription->GetMediaType();
        mediaInfo.mediaSize = mediaDescription->GetMediaSize();
        mediaInfo.startPosition = static_cast<uint32_t>(mediaDescription->GetStartPosition());
        mediaInfo.duration = static_cast<uint32_t>(mediaDescription->GetDuration());
        mediaInfo.albumTitle = mediaDescription->GetAlbumTitle();
        mediaInfo.mediaArtist = mediaDescription->GetArtist();
        mediaInfo.lrcUrl = mediaDescription->GetLyricUri();
        mediaInfo.appIconUrl = mediaDescription->GetIconUri();
        mediaInfo.appName = mediaDescription->GetAppName();
        mediaInfoList.emplace_back(mediaInfo);
    }
    info.mediaInfoList = mediaInfoList;
    if (streamPlayer_ && streamPlayer_->Start(info)) {
        SLOGI("SetMediaList successed");
        return AVSESSION_SUCCESS;
    }
    SLOGE("SetMediaList failed");
    return AVSESSION_ERROR;
}

void HwCastStreamPlayer::UpdateMediaInfo(const MediaInfo &mediaInfo)
{
}

int32_t HwCastStreamPlayer::GetDuration(int32_t& duration)
{
    SLOGI("GetDuration begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    streamPlayer_->GetDuration(duration);
    SLOGI("GetDuration successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::GetPosition(int32_t &position)
{
    SLOGI("GetPosition begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    streamPlayer_->GetPosition(position);
    SLOGI("GetPosition successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::GetVolume(int32_t &volume)
{
    SLOGI("GetVolume begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    streamPlayer_->GetVolume(volume);
    SLOGI("GetVolume successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::GetLoopMode(int32_t &loopMode)
{
    SLOGI("GetLoopMode begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    CastEngine::LoopMode castLoopMode;
    streamPlayer_->GetLoopMode(castLoopMode);
    loopMode = static_cast<int32_t>(castLoopMode);
    SLOGI("GetLoopMode successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::GetPlaySpeed(int32_t &playSpeed)
{
    SLOGI("GetPlaySpeed begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    CastEngine::PlaybackSpeed castPlaybackSpeed;
    streamPlayer_->GetPlaySpeed(castPlaybackSpeed);
    playSpeed = static_cast<int32_t>(castPlaybackSpeed);
    SLOGI("GetPlaySpeed successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::GetPlayState(AVCastPlayerState &playerState)
{
    SLOGI("GetPlayState begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    CastEngine::PlayerStates castPlayerStates;
    streamPlayer_->GetPlayerStatus(castPlayerStates);
    playerState.castPlayerState_ = castPlusStateToString_[castPlayerStates];
    SLOGI("GetPlayState successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::SetDisplaySurface(std::string &surfaceId)
{
    SLOGI("SetDisplaySurface begin");
    if (!streamPlayer_) {
        SLOGE("streamPlayer is nullptr");
        return AVSESSION_ERROR;
    }
    streamPlayer_->SetSurface(surfaceId);
    SLOGI("SetDisplaySurface successed");
    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::RegisterControllerListener(std::shared_ptr<IAVCastControllerProxyListener> listener)
{
    SLOGI("RegisterControllerListener begin");
    if (listener == nullptr) {
        SLOGE("RegisterControllerListener failed for the listener is nullptr");
        return AVSESSION_ERROR;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (find(streamPlayerListenerList_.begin(), streamPlayerListenerList_.end(), listener)
        != streamPlayerListenerList_.end()) {
        SLOGE("listener is already in streamPlayerListenerList_");
        return AVSESSION_ERROR;
    }
    SLOGI("RegisterControllerListener successed, and add it to streamPlayerListenerList_");
    streamPlayerListenerList_.emplace_back(listener);

    return AVSESSION_SUCCESS;
}

int32_t HwCastStreamPlayer::UnRegisterControllerListener(std::shared_ptr<IAVCastControllerProxyListener> listener)
{
    if (listener == nullptr) {
        SLOGE("UnRegisterCastSessionStateListener failed for the listener is nullptr");
        return AVSESSION_ERROR;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = streamPlayerListenerList_.begin(); iter != streamPlayerListenerList_.end();) {
        if (*iter == listener) {
            iter = streamPlayerListenerList_.erase(iter);
            SLOGI("UnRegisterControllerListener successed, and erase it from streamPlayerListenerList_");
            return AVSESSION_SUCCESS;
        } else {
            ++iter;
        }
    }
    SLOGE("listener is not found in streamPlayerListenerList_, so UnRegisterControllerListener failed");

    return AVSESSION_ERROR;
}

void HwCastStreamPlayer::OnStateChanged(const CastEngine::PlayerStates playbackState, bool isPlayWhenReady)
{
    AVCastPlayerState avCastPlayerState;
    if (castPlusStateToString_.count(playbackState) == 0) {
        SLOGE("current playbackState status is not exist in castPlusStateToString_");
        avCastPlayerState.castPlayerState_ = "error";
    } else {
        avCastPlayerState.castPlayerState_ = castPlusStateToString_[playbackState];
    }
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnStateChanged for registered listeners");
            listener->OnStateChanged(avCastPlayerState);
        }
    }
}

void HwCastStreamPlayer::OnPositionChanged(int position, int bufferPosition, int duration)
{
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnPositionChanged for registered listeners");
            listener->OnPositionChanged(position, bufferPosition, duration);
        }
    }
}

void HwCastStreamPlayer::OnMediaItemChanged(const CastEngine::MediaInfo& mediaInfo)
{
    std::shared_ptr<AVMediaDescription> mediaDescription = std::make_shared<AVMediaDescription>();
    mediaDescription->SetMediaId(mediaInfo.mediaId);
    mediaDescription->SetTitle(mediaInfo.mediaName);
    mediaDescription->SetMediaUri(mediaInfo.mediaUrl);
    mediaDescription->SetMediaType(mediaInfo.mediaType);
    mediaDescription->SetMediaSize(mediaInfo.mediaSize);
    mediaDescription->SetStartPosition(static_cast<uint32_t>(mediaInfo.startPosition));
    mediaDescription->SetDuration(static_cast<uint32_t>(mediaInfo.duration));
    mediaDescription->SetAlbumTitle(mediaInfo.albumTitle);
    mediaDescription->SetArtist(mediaInfo.mediaArtist);
    mediaDescription->SetLyricUri(mediaInfo.lrcUrl);
    mediaDescription->SetIconUri(mediaInfo.appIconUrl);
    mediaDescription->SetAppName(mediaInfo.appName);
    AVQueueItem queueItem;
    queueItem.SetDescription(mediaDescription);
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnMediaItemChanged for registered listeners");
            listener->OnMediaItemChanged(queueItem);
        }
    }
}

void HwCastStreamPlayer::OnVolumeChanged(int volume)
{
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnVolumeChanged for registered listeners");
            listener->OnVolumeChanged(volume);
        }
    }
}

void HwCastStreamPlayer::OnLoopModeChanged(const CastEngine::LoopMode loopMode)
{
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnLoopModeChanged for registered listeners");
            listener->OnLoopModeChanged(static_cast<int>(loopMode));
        }
    }
}

void HwCastStreamPlayer::OnPlaySpeedChanged(const CastEngine::PlaybackSpeed speed)
{
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnPlaySpeedChanged for registered listeners");
            listener->OnPlaySpeedChanged(static_cast<double>(speed));
        }
    }
}

void HwCastStreamPlayer::OnPlayerError(int errorCode, const std::string &errorMsg)
{
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnPlayerError for registered listeners");
            listener->OnPlayerError(errorCode, errorMsg);
        }
    }
}

void HwCastStreamPlayer::OnVideoSizeChanged(int width, int height)
{
    for (auto listener : streamPlayerListenerList_) {
        if (listener != nullptr) {
            SLOGI("trigger the OnVideoSizeChanged for registered listeners");
            listener->OnVideoSizeChanged(width, height);
        }
    }
}
} // namespace OHOS::AVSession