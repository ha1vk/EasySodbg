# EasySodbg
无需运行apk程序，单独调试so库，将本项目与IDA结合起来调试，极大的简化了对so的调试。众所周知，安卓上的so调试比较麻烦，要一系列繁琐的操作，如果能像调试终端程序那么简单的话就好了，有时，我们只需要测试某一个片段代码，而不需要运行整个程序。本项目的出现，就是为了解决这个问题。目前支持armeabi、armeabi-v7a、arm64-v8a处理器上的安卓so文件。采用的是自己解析so文件然后加载到内存的方案。因此，不管这个so在apk里是否会崩溃，在这里都可以被加载到内存中执行。

#### 使用方法
运行程序时，请先加载对应的程序核心库
```
export LD_PRELOAD=/data/local/tmp/libEsodbg.so
```
 以WMCTF 2020里的easy_apk为例，在0x0000000000010F00处，是程序计算flag的主要逻辑，最开始的几次附件，运行apk直接崩溃，比较麻烦。假如我们使用本项目来解决，那么就可以轻松搞定。首先，将so加载到内存中
```
                Hook *hook = new Hook("libnative-lib.so");
                hook->set_libc("/system/lib64/libc.so");
                hook->load();

```
然后我们准备运行关键函数，为了运行关键函数，我们需要对0x10F24函数的参数进行patch，原本参数为JNIENV *，现在我们nop掉几条指令，使其变成了char *，这样我们就可以将参数传进去了。so默认加载基址为0x66400000，可以使用hook->set_base()来自定义基址。程序执行到getchar()时，会等待，此时，我们用IDA打开so文件。
```
                //patch
                char *nop = "\x1f\x20\x03\xd5\x1f\x20\x03\xd5\x1f\x20\x03\xd5";
                hook->set_memory(0x10F24,12,nop);
                getchar();
                size_t ret = (size_t)(hook->run(0x10F00,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",NULL,NULL,NULL));
                cout << "ret=" << ret << endl;

```
在IDA的菜单中，选择Edit->Segments->Rebase Program...，将IDA里基址改为自己设定的地址，如0x66400000，然后，我们在0x66400000+0x10F24处断点后，使用IDA的attach将soattach到本程序的进程上，然后，回到程序，按下回车键，getchar()获取了字符后继续向下执行，就可以执行到我们需要调试的地方，然后断点断下来。然后，经过IDA的调试，发现该so里有root检测，主要是fopen了一些文件，那么，我们可以直接hook一下fopen，后面又获取了TracerPID，利用的是atoi将其值转为数字，因此，atoi我们也hook一下，这样，就可以绕过反调试等检测了。
```
                hook->set_hook("fopen",(void *)myfopen);
                hook->set_hook("atoi",(void *)myatoi);
```
最后经过调试，就可以拿到KEY，我们也可以直接内联hook，拿到key。
```
                hook->set_hook("0x10E50",(void *)print_key); //hook text段内部，将key打印出来
```
完整的代码示例见main.cpp。注意目标so的架构，则编译对应的架构然后上传到android模拟器上利用IDA调试即可。变得跟调试linux下程序一样方便。

hook android so and debug it easily with no need to run the apk!!
