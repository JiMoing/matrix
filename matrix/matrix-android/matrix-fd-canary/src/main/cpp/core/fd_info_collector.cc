/*
 * Tencent is pleased to support the open source community by making wechat-matrix available.
 * Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the BSD 3-Clause License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android/log.h>
#include <thread>
#include "fd_info_collector.h"
#include "comm/fd_canary_utils.h"


namespace fdcanary {

    void FDInfoCollector::OnOpen(const char *pathname, int flags, mode_t mode, int open_ret, const JavaContext& java_context) {

        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::OnOpen path is [%s], flag:[%d], mode:[%d],open_ret:[%d]",
            pathname, flags, mode, open_ret);

        
    }


    std::shared_ptr<FDInfo> FDInfoCollector::OnClose(int fd, int close_ret) {
        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::OnClose fd:[%d], close_ret:[%d]",fd, close_ret);    
        return nullptr;
    }
}
