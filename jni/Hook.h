#ifndef HOOK_H
#define HOOK_H

#include "elf.h"
#include <sys/mman.h>

class Hook {
	private:
		ELF *elf;
		char *libc_path;
		size_t base; //加载基址
		void copy(const Hook & hook);
	public:
		Hook(const char *filename); //从文件加载
		Hook(char *buf,size_t len); //从缓冲区加载
                Hook(const Hook & hook);
                Hook & operator=(const Hook & hook);
		~Hook();
                void load(); //加载到内存中
		void set_base(size_t addr); //设置加载基址
		void set_libc(const char *name); //设置libc路径
		int set_hook(const char *name,void *func); //设置hook
		void set_memory(size_t offset,size_t len,void *value); //设置内存
		void* run(size_t offset,const void *a,const void *b,const void *c,const void *d); //运行指定位置的shellcode，最多允许4个参数
};
#endif
