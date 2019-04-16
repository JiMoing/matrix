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

    void CallStack::dumpBacktraceIndex(char* out, intptr_t* buffer, size_t count)
    {
        for (size_t idx = 0; idx < count; ++idx) {
            intptr_t addr = buffer[idx];
            const char* symbol = "      ";
            const char* dlfile="      ";
            Dl_info info;
            if (dladdr((void*)addr, &info)) {
                if(info.dli_sname){
                    symbol = info.dli_sname;
                }
                if(info.dli_fname){
                    dlfile = info.dli_fname;
                }            
            }else{
                strcat(out,"#                               \n");
                continue;
            }
            /*out.append("%zu",idx);
            out.append(" #: ");
            out.append("%zu",addr);
            out.append("    ");
            out.append(symbol);
            out.append("    ");
            out.append(dlfile);
            out.append("\n");*/
            char temp[50];
            memset(temp,0,sizeof(temp));
            sprintf(temp,"%zu",idx);
            strcat(out,"#");
            strcat(out,temp);
            strcat(out, ": ");
            memset(temp,0,sizeof(temp));
            sprintf(temp,"%zu",addr);
            strcat(out,temp);
            strcat(out, "  " );
            strcat(out, symbol);
            strcat(out, "      ");
            strcat(out, dlfile);
            strcat(out, "\n" );
        }
    }

    void CallStack::dumpCallStack(char* outBuf) {
        const size_t maxStackDeep = 20;
        intptr_t stackBuf[maxStackDeep];
        dumpBacktraceIndex(outBuf, stackBuf, captureBacktrace(stackBuf, maxStackDeep));
    }
}