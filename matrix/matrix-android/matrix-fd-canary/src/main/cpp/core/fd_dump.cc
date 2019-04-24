//
//  fd_info.cpp
//  cdn
//
//  Created by perryzhou on 2019/1/4.
//

#include "fd_dump.h"
#include <android/log.h>
#ifndef WIN32
#include <errno.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace fdcanary {

    static bool get_fd_path(int fd, char szbuf[1024]){

    #ifdef __APPLE__

        return -1 != fcntl(fd , F_GETPATH, szbuf);
    #endif

    #ifdef __ANDROID__

        char path[64];
        //获取fd的path
        snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
        
        ssize_t length = ::readlink(path, szbuf, 1023);
        if (length > 0){
            szbuf[length] = '\0';
            return true;
        }
    #endif

        return false;
    }

    static const char* type2name(int type){
        switch (type) {
            case S_IFIFO:
                return "named pipe"; //命名管道
            case S_IFCHR:
                return "character special"; // 字符设备（串行端口）
            case S_IFDIR:
                return "directory"; //目录
            case S_IFBLK:
                return "block special"; //块设备（数据存储接口设备）
            case S_IFREG:
                return "regular"; //普通文件
            case S_IFLNK:
                return "symbolic link"; //符号链接文件（文件的软连接文件，类似于window的快捷方式）
            case S_IFSOCK:
                return "socket"; //socket 
            default:
                return "<unknown>";
        }
    }

    std::list<FDI> QueryFD::QueryFDInfo(int maxfd){
        std::list<FDI> result;
    #ifndef WIN32
        char szbuf[1024] = {0};
        for (int fd = 0; fd < maxfd; fd++) {
            FDI item;
            item.fd = fd;
            
            int flags = fcntl(fd, F_GETFD, 0);
            if (-1 == flags){
                item.error = errno;
                item.path_or_name = "<F_GETFD failed>";
            }else{
                struct stat statbuf;
                if (fstat(fd, &statbuf) == 0) {
                    item.type = (S_IFMT & statbuf.st_mode);
                    item.path_or_name = "<unknown>";
                    if (get_fd_path(fd, szbuf)){
                        item.path_or_name = szbuf;
                    }
                }
            }
            
            result.push_back(item);
        }
    #endif
        
        return result;
    }

    std::list<std::string> QueryFD::PrettyFDInfo(const std::list<FDI>& fdi){
        std::list<std::string> result;
    #ifndef WIN32
        char szline[1024];
        std::string part;
        for (auto item : fdi) {
            part.clear();
            if (item.error != 0) {
                continue;
            }
            
            size_t rv = snprintf(szline, 1024, "\r%d|%d|%d(%s)|%s", item.fd, item.error, item.type, type2name(item.type), item.path_or_name.c_str());
            part.append(szline, rv);
            result.push_back(part);
        }
    #endif
        
        return result;
    }
}
