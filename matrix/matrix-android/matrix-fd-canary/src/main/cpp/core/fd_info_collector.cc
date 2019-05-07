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
#include <fcntl.h>
#include <sys/stat.h>
#include "fd_info_collector.h"
#include "comm/fd_canary_utils.h"
#include "core/fd_dump.h"


namespace fdcanary {

    void FDInfoCollector::SetIssueDetector(IssueDetector &issue_detector) {
        issue_detector_ = issue_detector;
    }

    void FDInfoCollector::OnOpen(int fd, std::string &stack) {
        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::OnOpen fd:[%d]", fd);
        int type = GetType(fd);
        if (type != -1) {
            InsertTypeMap(type, fd, stack);
        }

        if(issue_detector_.CheckLimit(fd)) {
            //todo 
             __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::OnOpen Exceed the upper limit fd:[%d]", fd);
            issue_detector_.PublishIssue(io_map_);
        }
    }


    void FDInfoCollector::OnClose(int fd) {
        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::OnClose fd:[%d]", fd);
        int type = GetType(fd);
        if (type != -1) {
            RemoveTypeMap(type, fd);
        }

        GetMapsInfo();
    }

    //debug方法
    void FDInfoCollector::GetMapsInfo() {
        std::string str;
        char temp_size_1[50];
        sprintf(temp_size_1, "pipe_map_:size: %zu ", pipe_map_.size());
        str.append(temp_size_1);
        if (pipe_map_.size() > 0) {
            str.append("[");
            for(std::unordered_map<int, FDInfo>::iterator iter = pipe_map_.begin(); iter != pipe_map_.end(); iter++) {
                str.append(std::to_string(iter->first));
                str.append(",");
            }

            str.append("]");
        }

        
        char temp_size_2[50];
        sprintf(temp_size_2, "dmabuf_map_:size: %zu ", dmabuf_map_.size());
        str.append(temp_size_2);
        if (dmabuf_map_.size() > 0) {
            str.append("[");
            for(std::unordered_map<int, FDInfo>::iterator iter = dmabuf_map_.begin(); iter != dmabuf_map_.end(); iter++) {
                str.append(std::to_string(iter->first));
                str.append(",");
            }

            str.append("]");
        }

        char temp_size_3[50];
        sprintf(temp_size_3, "io_map_:size: %zu ", io_map_.size());
        str.append(temp_size_3);
        if (io_map_.size() > 0) {
            str.append("[");
            for(std::unordered_map<int, FDInfo>::iterator iter = io_map_.begin(); iter != io_map_.end(); iter++) {
                str.append(std::to_string(iter->first));
                str.append(",");
            }

            str.append("]");
        }

        char temp_size_4[50];
        sprintf(temp_size_4, "socket_map_:size: %zu ", socket_map_.size());
        str.append(temp_size_4);
        if (socket_map_.size() > 0) {
            str.append("[");
            for(std::unordered_map<int, FDInfo>::iterator iter = socket_map_.begin(); iter != socket_map_.end(); iter++) {
                str.append(std::to_string(iter->first));
                str.append(",");
            }

            str.append("]");
        }


        __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI","FDInfoCollector::GetMapsInfo: %s", str.c_str());
        
        
    }

    void FDInfoCollector::GetAllMapsIssues() {
        
    }

    int FDInfoCollector::GetType(int fd) {
        int type;
        int flags = fcntl(fd, F_GETFD, 0);
        if (flags != -1) {
            struct stat statbuf;
            if (fstat(fd, &statbuf) == 0) {
                type = (S_IFMT & statbuf.st_mode);
                return type;
            }
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI","FDInfoCollector::GetType flags == -1");
        }
        return -1;
    }

    void FDInfoCollector::InsertTypeMap(int type, int fd, std::string &stack) {
        switch (type) {
            case S_IFIFO: {
                //命名管道
                FDInfo fdinfo1(fd, FDIssueType::kFDPIPE, stack);
                pipe_map_.insert(std::make_pair(fd, fdinfo1));
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::InsertTypeMap (named pipe) | fd is [%d]]", fd);
                break;
            }
                
            case S_IFCHR: {
                // 字符设备（串行端口）
                FDInfo fdinfo2(fd, FDIssueType::kFDDmabuf, stack);
                dmabuf_map_.insert(std::make_pair(fd, fdinfo2));
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::InsertTypeMap (character special) | fd is [%d]]", fd);
                break;
            }
                
            case S_IFREG: {
                //普通文件
                FDInfo fdinfo3(fd, FDIssueType::kFDIO, stack);
                io_map_.insert(std::make_pair(fd, fdinfo3));
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::InsertTypeMap (regular) | fd is [%d]]", fd);    
                break;
            }
                
            case S_IFSOCK: {
                //socket 
                FDInfo fdinfo4(fd, FDIssueType::kFDSocket, stack);
                socket_map_.insert(std::make_pair(fd, fdinfo4));
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::InsertTypeMap (socket) | fd is [%d]]", fd);
                break;
            } 
            case S_IFDIR:
                //目录
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "directory");
                break;
            case S_IFBLK:
                //块设备（数据存储接口设备）
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "block special");
                break;
            case S_IFLNK:
                //符号链接文件（文件的软连接文件，类似于window的快捷方式）
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "symbolic link");
                break;
            
            default:
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "<unknown>");
                break;
        }
    }

    void FDInfoCollector::RemoveTypeMap(int type, int fd) {
        switch (type) {
            case S_IFIFO:
                //命名管道
                RemoveImpl(fd, pipe_map_);
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::RemoveTypeMap (named pipe) | fd is [%d]]", fd);
                break;
            case S_IFCHR:
                // 字符设备（串行端口）
                RemoveImpl(fd, dmabuf_map_);
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::RemoveTypeMap (character special) | fd is [%d]]", fd);
                break;
            case S_IFREG:
                //普通文件
                RemoveImpl(fd, io_map_);
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::RemoveTypeMap (regular) | fd is [%d]]", fd); 
                break;   
            case S_IFSOCK:
                //socket 
                RemoveImpl(fd, socket_map_);
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "FDInfoCollector::RemoveTypeMap (socket) | fd is [%d]]", fd);
                break;

                
            case S_IFDIR:
                //目录
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "directory");
                break;
            case S_IFBLK:
                //块设备（数据存储接口设备）
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "block special");
                break;
            case S_IFLNK:
                //符号链接文件（文件的软连接文件，类似于window的快捷方式）
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "symbolic link");
                break;
            
            default:
                __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI", "<unknown>");
                break;
        }
    }

    void FDInfoCollector::RemoveImpl(int fd, std::unordered_map<int, FDInfo> &map) {
        std::unordered_map<int, FDInfo>::iterator it;
        it = map.find(fd);
        if (it != map.end()) {
            map.erase(fd);
           __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI","FDInfoCollector::RemoveImpl erase success, fd is [%d], current size is [%zu]", fd, map.size());
        } else {
           __android_log_print(ANDROID_LOG_DEBUG, "FDCanary.JNI","FDInfoCollector::RemoveImpl erase fail, fd is [%d], current size is [%zu]", fd, map.size());
        }
    }
}
