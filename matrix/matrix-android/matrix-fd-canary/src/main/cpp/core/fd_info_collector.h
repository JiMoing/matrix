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

#ifndef MATRIX_FD_CANARY_FD_INFO_COLLECTOR_H
#define MATRIX_FD_CANARY_FD_INFO_COLLECTOR_H

#include <jni.h>
#include <unistd.h>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "comm/fd_canary_utils.h"

namespace fdcanary
{

typedef enum
{
    kInit = 0,
    kRead,
    kWrite
} FileOpType;

class JavaContext
{
  public:
    JavaContext(intmax_t thread_id, const std::string &thread_name, const std::string &stack)
        : thread_id_(thread_id), thread_name_(thread_name), stack_(stack)
    {
    }

    const intmax_t thread_id_;
    const std::string thread_name_;
    const std::string stack_;
};

//rw short for read/write operation
class FDInfo
{
  public:
    FDInfo() = default;
    FDInfo(const std::string path, const JavaContext java_context)
        : start_time_μs_(GetSysTimeMicros()), op_type_(kInit), path_(path), java_context_(java_context)
    {
    }

    const std::string path_;
    const JavaContext java_context_;

    int64_t start_time_μs_;
    FileOpType op_type_ = kInit;
    int op_cnt_ = 0;
    long buffer_size_ = 0;
    long op_size_ = 0;
    long rw_cost_μs_ = 0;
    long max_continual_rw_cost_time_μs_ = 0;
    long max_once_rw_cost_time_μs_ = 0;
    long current_continual_rw_time_μs_ = 0;
    int64_t last_rw_time_μs_ = 0;
    long file_size_ = 0;

    long total_cost_μs_ = 0;
};

// A singleton to collect and generate operation info
class FDInfoCollector
{
  public:
    void OnOpen(int fd, std::string &stack);
    void OnClose(int fd);



  private:
    constexpr static const int kContinualThreshold = 8 * 1000; //in μs， half of 16.6667

    std::unordered_map<int, std::string> io_map_;

    std::unordered_map<int, std::string> pipe_map_;

    std::unordered_map<int, std::string> socket_map_;

    std::unordered_map<int, std::string> dmabuf_map_;

    int GetType(int fd);

    void InsertTypeMap(int type, int fd, std::string &stack);
    void RemoveTypeMap(int type, int fd);
    void RemoveImpl(int fd, std::unordered_map<int, std::string> &map);

    void GetMapsInfo();
    
};
} // namespace fdcanary

#endif //MATRIX_FD_CANARY_FD_INFO_COLLECTOR_H
