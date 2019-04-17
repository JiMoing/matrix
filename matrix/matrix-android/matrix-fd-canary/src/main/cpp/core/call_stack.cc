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
#include "call_stack.h"
namespace fdcanary {

    static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
    {
        BacktraceState* state = static_cast<BacktraceState*>(arg);
        intptr_t ip = (intptr_t)_Unwind_GetIP(context);
        if (ip) {
            if (state->current == state->end) {
                return _URC_END_OF_STACK;
            } else {
                state->current[0] = ip;
                state->current++;
            }
        }
        return _URC_NO_REASON;


    }
 
    size_t CallStack::captureBacktrace(intptr_t* buffer, size_t maxStackDeep)
    {
        BacktraceState state = {buffer, buffer + maxStackDeep};
        _Unwind_Backtrace(unwindCallback, &state);
        return state.current - buffer;
    }

    void CallStack::dumpBacktraceIndex(std::string &out, intptr_t* buffer, size_t count) {
        char deep[100] = {0};
        sprintf(deep, "call stack deep is %zu", count);
        out.append(deep);
        for(size_t idx = 0; idx < count; idx++) {
            intptr_t addr = buffer[idx];
            Dl_info info;
            if (dladdr((void*)addr, &info)) {
                out.append("\n");
                char temp[50] = {0};
                sprintf(temp,"#%zu    %zu    ",idx, addr);
                out.append(temp);
                if (info.dli_sname) {
                    out.append(info.dli_sname);
                    out.append("    ");
                } 
                if(info.dli_fname) {
                    out.append(info.dli_fname);
                }
            }
        }
    }

    void CallStack::dumpCallStack(std::string &outBuf) {
        
        const size_t maxStackDeep = 20;
        intptr_t stackBuf[maxStackDeep];
        dumpBacktraceIndex(outBuf, stackBuf, captureBacktrace(stackBuf, maxStackDeep));
        
    }
}