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

#ifndef MATRIX_FD_CANARY_FD_DUMP_H
#define MATRIX_FD_CANARY_FD_DUMP_H

#include <string>
#include <list>

namespace fdcanary{
    struct FDI{
        int fd = 0;
        int type = 0;
        int error = 0;
        std::string path_or_name;
    };
    static bool get_fd_path(int fd, char szbuf[1024]);

    class QueryFD {
    public:
        static std::list<FDI> QueryFDInfo(int maxfd);
        static std::list<std::string> PrettyFDInfo(const std::list<FDI>& fdi);
    };

};
#endif //MATRIX_FD_CANARY_FD_DUMP_H
