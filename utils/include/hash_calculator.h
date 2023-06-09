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

#ifndef AV_SESSION_HASH_CALCULATOR_H
#define AV_SESSION_HASH_CALCULATOR_H

#include <vector>
#include <openssl/sha.h>

#include "avsession_errors.h"
#include "avsession_log.h"

namespace OHOS::AVSession {
class HashCalculator {
public:
    HashCalculator() = default;

    ~HashCalculator();

    int32_t Init();

    int32_t Update(const std::vector<uint8_t> &value);

    int32_t GetResult(std::vector<uint8_t> &value);

private:
    SHA256_CTX *context_ = nullptr;
};
}
#endif // AV_SESSION_HASH_CALCULATOR_H