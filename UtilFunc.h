//
// Created by alkaid on 2020/2/7.
//

#ifndef C___TEST_UTILFUNC_H
#define C___TEST_UTILFUNC_H
#include "Epoll.h"
#include <assert.h>

void addsig(int sig,void(handler)(int), bool restart=true){
    struct sigaction sa;
    memset(&sa,'\0', sizeof(sa));
    sa.sa_handler=handler;
    if(restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL)!=-1);
}
#endif //C___TEST_UTILFUNC_H