#include <stdio.h>
#include <stdlib.h>
#include "Hook.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using namespace std;
FILE *myfopen(char *name,char *mode) {
        if (strstr(name,"su")) {
                cout << "banned:" << name << endl;
                return NULL;
        }
        cout << "name=" << name << endl;
        return fopen(name,mode);
}

int myatoi(char *str) {
        cout << "atoi bypass TracerPID check=" << endl;
        return 0;
}

void print_key(char *str) {
        cout << "test inline hook" << endl;
	cout << "key is:";
        for (int i=0;i<0x20;i++) {
                printf("\\x%02x",str[i]);
        }
        printf("\n");
        kill(getpid(),9);
}


void hello() {
        cout << "hello" << endl;
}


int main() {
        try {
                Hook *hook = new Hook("libnative-lib.so");
                hook->set_libc("/system/lib64/libc.so");
                hook->load();
                hook->set_hook("fopen",(void *)myfopen);
                hook->set_hook("atoi",(void *)myatoi);
                hook->set_hook("0x10E50",(void *)print_key); //hook text段内部，将key打印出来
		//patch
		char *nop = "\x1f\x20\x03\xd5\x1f\x20\x03\xd5\x1f\x20\x03\xd5";
		hook->set_memory(0x10F24,12,nop);
                getchar();
                size_t ret = (size_t)(hook->run(0x10F00,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",NULL,NULL,NULL));
                cout << "ret=" << ret << endl;
        } catch (char const *msg) {
                cout << msg << endl;
        }
}

