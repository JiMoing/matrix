//
//  fd_info.hpp
//  cdn
//
//  Created by perryzhou on 2019/1/4.
//

#ifndef fd_info_hpp
#define fd_info_hpp

#include <string>
#include <list>

struct FDI{
    int fd = 0;
    int type = 0;
    int error = 0;
    std::string path_or_name;
};

class QueryFD {
public:
    static std::list<FDI> QueryFDInfo(int maxfd);
    static std::list<std::string> PrettyFDInfo(const std::list<FDI>& fdi);
};


#endif /* fd_info_hpp */
