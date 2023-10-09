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
#include <iostream>
#include <fstream>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/sendfile.h>
#include <stdlib.h>

#include <fcntl.h>
#include <netinet/in.h>

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

extern "C" {

    JNIEXPORT jint JNICALL Java_com_example_ffi_1path_1helper_FfiPathHelperPlugin_assetToFilepath(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {
        const char *path = env->GetStringUTFChars(jAssetPath, 0);

        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

        AAsset *asset = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
        if(asset == NULL) {
            __android_log_print(ANDROID_LOG_VERBOSE, "mmap_stream_android", "Couldn't locate asset [ %s ]", path);
            return NULL;
        }
        off_t start;
        off_t outLength;
        int asset_fd = AAsset_openFileDescriptor(asset, &start, &outLength);
        __android_log_print(ANDROID_LOG_VERBOSE, "mmap_stream_android", "Opened file descriptor [ %d ] for [ %s ] with start %lld and length %lld", asset_fd, path, (long long)start,(long long) outLength);

        // char* wbuffer = new char[1];
        // wbuffer[0] = 'q';

        int pipefd[2];
        
        if (pipe(pipefd) == -1) {
            __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","pipe erro : %d", errno);    
            return NULL;
        }
        ssize_t written = sendfile(pipefd[1], asset_fd, &start, outLength);
        // ssize_t written = write(pipefd[1], wbuffer, 1);

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","wrote %d", written);

        close(pipefd[1]);

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","closed write pipe", written);

        // int fd2 

        //  write(fd2, buffer, 1);

        // int flags = fcntl(pipefd[0], F_GETFL);
        // fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

        // ssize_t written = sendfile(fd2, fd, &start, outLength);
        // if(written < 0) {
        //      __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Couldn't write with sendfile, error was %d", errno);    
        //      return NULL;
        //  }
        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","sendfile %d", written);

        // FILE* f = fdopen(fd2, "r");
        // fseek(f, start,SEEK_SET);
        // int num_read = fread(buffer, 1, 1, f);
        // fclose(f);

        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","numread  %d char %c", num_read, buffer[0]);

        // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Opening at %s", fdpath);

        char* fdpath = nullptr;
    	asprintf(&fdpath, "/proc/self/fd/%d", pipefd[0]);

        char* rbuffer = new char[1];

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Opening at %s", fdpath);

            
        std::ifstream fs(fdpath, std::ifstream::in);

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Created ifstream");

        if(!fs.good()) {
            __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Couldnt open");
            return 0;
        }

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Good");

        // fs.seekg(0, std::ios::beg);



        fs.read(rbuffer, 1);

        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","read");

        if(!fs.good()) {
            __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Failed to read");
            return 0;
        }



        // FILE* f_r = fopen(fdpath, "r");
        // if(!f_r) {
        //      __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read failed");    
        //      return 0;
        // }
        // // fseek(f_r,0,SEEK_SET);
        
        // int numread = fread(rbuffer, 1,1, f_r);
        __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","numread  %d char %c", fs.gcount(), rbuffer[0]);
        fs.close();
        env->ReleaseStringUTFChars(jAssetPath, path);
        return 0;

    }
}

//     int make_pipe(const char* path) {
//                 // if(mkfifo(path, S_IRWXU) < 0) { 
//                 //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","couldn't mkfifo\n", errno);            
//                 // }

//         // if(mkdir("/dev/shm/foo", S_IRWXU) < 0) {
//         //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","couldn't mkdir\n", errno);
//         // }
//         // return 0;
//         struct sockaddr_un sockAddr;
//         socklen_t sockLen;

//         // // if (makeAddr("/storage/emulated/0/Android/data/com.example.ffi_path_helper_example/files/foopipe4", &sockAddr, &sockLen) < 0)
//         // if (makeAddr("foo", &sockAddr, &sockLen) < 0)
//         //     return 1;
//         // int fd = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX);
//         int fd = open("/storage/emulated/0/Android/data/com.example.ffi_path_helper_example/files/foopipe4", O_CREAT| O_RDWR , S_IRWXU  );
//         if (fd < 0) {
//             __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","socket error %d\n", errno);
//             return 1;
//         }
//         __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Got fd %d\n", fd);
//         // // int fc = fchmod(fd, S_IWUSR | S_IRUSR);
//         // // if(fc < 0) {
//         // //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","chmod error %d\n", errno);
//         // //     return 1;
//         // // }

//         // // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Got fd %d, SERVER %s\n", fd, sockAddr.sun_path);

//         // if (bind(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
//         //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","error, couldn't bind %d\n", errno);
//         // }
        
//         char * buffer = new char [1];       

//         char blah = 'w';

//         int fd2 = dup(fd);

//         FILE* f = fdopen(fd, "r+");
//         if(!f) { 
//             __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","fdopen for writingi failed");
//             return 0;
//         }
// //        FILE* f_r = fdopen(fd2, "r+");

//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open");

//         fwrite(&blah, 1, 1, f);
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","write");
//         fclose(f);
//         __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","close");
//         // f = fdopen(fd, "r");
//         // if(!f_r) {
//         //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read failed");    
//         //     return 0;
//         // }
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read");
//         // int numread = fread(buffer, 1, 1, f);
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","numread");
//         // fclose(f);
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","close read");
        
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Read %d bytes : %c", numread, buffer[0]);

//         // return 0;
//         // std::ofstream fso("foo", std::ios_base::binary);

//         // if(fso) {
//         //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Couldn't open foo for writing");
//         //     return 0;
//         // }
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Opened foo for writing");

//         // // std::ofstream fso("/storage/emulated/0/Android/data/com.example.ffi_path_helper_example/files/foopipe3", std::ios_base::binary);

//         // fso.put(blah);
//         // fso.flush();
//         // if(fso.fail())  {
//         //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Write failed");
//         //     return 0;
//         // }

//         // fso.close();
//         char* fdpath = nullptr;
//     	asprintf(&fdpath, "/proc/self/fd/%d", fd2);

//         __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Opening at %s", fdpath);

//         // FILE* f_r = fopen(fdpath, "r+");
//         // if(!f_r) {
//         //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","open read failed");    
//         //     return 0;
//         // }
//         // fseek(f_r,0,SEEK_SET);
//         // int numread = fread(buffer, 1,1, f_r);
//         // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","numread  %d char %c", numread, buffer[0]);
        
//         std::ifstream fs(fdpath, std::ios_base::binary);

//         if(!fs.good()) {
//             __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","Couldnt open");
//             return 0;
//         }

//         fs.seekg(0, std::ios::beg);

//         fs.read(buffer, 1);

//         __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper","read %d bytes from foo : %c", fs.gcount(), buffer[0]);

//         fs.close();


//         // // int fd = mkfifo("foop", S_IWUSR | S_IRUSR );
//         // // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper", "fd : %d", fd);
//         // // if(fd == -1) {
//         // //     __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper", "errno : %d", errno);
//         // // }
//         // // int result = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
//         // // __android_log_print(ANDROID_LOG_VERBOSE, "fifo_helper", "fcntl reuslt : %d", result);
//         return fd;
//     }
// }