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

#include "issue_detector.h"
#include <android/log.h>

namespace fdcanary {
    //todo
    static int kLimit = 90;

    IssueDetector::IssueDetector() {

    }

    IssueDetector::~IssueDetector() {

    }

    void IssueDetector::SetIssuedCallback(OnPublishIssueCallback issued_callback) {
        issued_callback_ = issued_callback;

        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "IssueDetector::SetIssuedCallback");
    }

    void IssueDetector::PublishIssue(std::unordered_map<int, FDInfo> all_map_) {
        // todo
        if (has_publish_issue && all_map_.size() == 0) {
            return;
        }
        
        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "IssueDetector::PublishIssue,size:[%zu]", all_map_.size());
        for(std::unordered_map<int, FDInfo>::iterator iter = all_map_.begin(); iter != all_map_.end(); iter++) {   
            FDIssue issue(iter->second);
            issues.push_back(issue);
        }
        if (issued_callback_) {
            issued_callback_(issues);
        }
        has_publish_issue = true;
        issues.clear();
    }

    bool IssueDetector::CheckLimit(int fd) {
        return fd > kLimit; 
    }


}

