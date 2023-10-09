#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <android/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

extern "C" {
  /*
 * Create a UNIX-domain socket address in the Linux "abstract namespace".
 *
 * The socket code doesn't require null termination on the filename, but
 * we do it anyway so string functions work.
 */
int makeAddr(const char* name, struct sockaddr_un* pAddr, socklen_t* pSockLen)
{
    int nameLen = strlen(name);
    if (nameLen >= (int) sizeof(pAddr->sun_path) -1)  /* too long? */
        return -1;
    pAddr->sun_path[0] = '\0';  /* abstract namespace */
    strcpy(pAddr->sun_path+1, name);
    pAddr->sun_family = AF_LOCAL;
    *pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
    return 0;
}



    int make_pipe(const char* path) {
                // if(mkfifo(path, S_IRWXU) < 0) { 
                //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","couldn't mkfifo\n", errno);            
                // }

        // if(mkdir("/dev/shm/foo", S_IRWXU) < 0) {
        //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","couldn't mkdir\n", errno);
        // }
        // return 0;
        struct sockaddr_un sockAddr;
        socklen_t sockLen;

        // // if (makeAddr("/storage/emulated/0/Android/data/com.example.ffi_path_helper_example/files/foopipe4", &sockAddr, &sockLen) < 0)
        // if (makeAddr("foo", &sockAddr, &sockLen) < 0)
        //     return 1;
        // int fd = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX);
        int fd = open("/storage/emulated/0/Android/data/com.example.ffi_path_helper_example/files/foopipe4", O_CREAT| O_RDWR , S_IRWXU  );
        if (fd < 0) {
            __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","socket error %d\n", errno);
            return 1;
        }
        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Got fd %d\n", fd);
        // // int fc = fchmod(fd, S_IWUSR | S_IRUSR);
        // // if(fc < 0) {
        // //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","chmod error %d\n", errno);
        // //     return 1;
        // // }

        // // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Got fd %d, SERVER %s\n", fd, sockAddr.sun_path);

        // if (bind(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
        //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","error, couldn't bind %d\n", errno);
        // }
        
        char * buffer = new char [1];       

        char blah = 'w';

        int fd2 = dup(fd);

        FILE* f = fdopen(fd, "r+");
        if(!f) { 
            __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","fdopen for writingi failed");
            return 0;
        }
//        FILE* f_r = fdopen(fd2, "r+");

        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open");

        fwrite(&blah, 1, 1, f);
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","write");
        fclose(f);
        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","close");
        // f = fdopen(fd, "r");
        // if(!f_r) {
        //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read failed");    
        //     return 0;
        // }
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read");
        // int numread = fread(buffer, 1, 1, f);
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","numread");
        // fclose(f);
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","close read");
        
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Read %d bytes : %c", numread, buffer[0]);

        // return 0;
        // std::ofstream fso("foo", std::ios_base::binary);

        // if(fso) {
        //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Couldn't open foo for writing");
        //     return 0;
        // }
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Opened foo for writing");

        // // std::ofstream fso("/storage/emulated/0/Android/data/com.example.ffi_path_helper_example/files/foopipe3", std::ios_base::binary);

        // fso.put(blah);
        // fso.flush();
        // if(fso.fail())  {
        //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Write failed");
        //     return 0;
        // }

        // fso.close();
        char* fdpath = nullptr;
    	asprintf(&fdpath, "/proc/self/fd/%d", fd2);

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Opening at %s", fdpath);

        // FILE* f_r = fopen(fdpath, "r+");
        // if(!f_r) {
        //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read failed");    
        //     return 0;
        // }
        // fseek(f_r,0,SEEK_SET);
        // int numread = fread(buffer, 1,1, f_r);
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","numread  %d char %c", numread, buffer[0]);
        
        std::ifstream fs(fdpath, std::ios_base::binary);

        if(!fs.good()) {
            __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Couldnt open");
            return 0;
        }

        fs.seekg(0, std::ios::beg);

        fs.read(buffer, 1);

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","read %d bytes from foo : %c", fs.gcount(), buffer[0]);

        fs.close();


        // // int fd = mkfifo("foop", S_IWUSR | S_IRUSR );
        // // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper", "fd : %d", fd);
        // // if(fd == -1) {
        // //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper", "errno : %d", errno);
        // // }
        // // int result = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        // // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper", "fcntl reuslt : %d", result);
        return fd;
    }
}