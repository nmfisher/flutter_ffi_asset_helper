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
#include <thread>
#include <map>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <android/sharedmem_jni.h>
#include <sys/mman.h>


#include <fcntl.h>
#include <netinet/in.h>

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

static const char* _TAG = "flutter_ffi_asset_helper";

using namespace std;

static map<string, AAsset*> assets;

extern "C" {

    struct finfo {
        int fd_out;
        int fd_in;
        off_t start;
        off_t len;
    };

    JNIEXPORT jlongArray  JNICALL Java_app_polyvox_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_getNativePointer(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {
        const char *path = env->GetStringUTFChars(jAssetPath, 0);

        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

        __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Loading asset [ %s ].", path);

        string spath(path);

        AAsset *asset;

        auto it = assets.find(spath);
        if (it == assets.end()) {
            asset = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
        } else {
            asset = it->second;
        }

        if(!asset) {
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Couldn't locate asset [ %s ]", path);
            return 0;
        }

        for (auto it=assets.begin(); it!=assets.end(); ++it)
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "asset: %s", it->first.c_str());

        assets[spath]=asset;


        jlongArray longArray = env->NewLongArray(2);

        jlong data = (jlong)AAsset_getBuffer(asset);
        jlong length = (jlong)AAsset_getLength(asset);
        
        env->SetLongArrayRegion(longArray, (jsize) 0, 1, &data);
        env->SetLongArrayRegion(longArray, (jsize) 1, 1, &length);

        env->ReleaseStringUTFChars(jAssetPath, path);

        for (auto it=assets.begin(); it!=assets.end(); ++it)    
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "asset: %s", it->first.c_str());

        ;
        if(!assets[spath]) {
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "WTF?");
        }
        return longArray;
    }

    JNIEXPORT void JNICALL Java_app_polyvox_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_free(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {

        for (auto it=assets.begin(); it!=assets.end(); ++it)
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "asset: %s", it->first.c_str());


        const char *path = env->GetStringUTFChars(jAssetPath, 0);

        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

        __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Freeing asset [ %s ]", path);

        string spath(path);
        auto it = assets.find(spath);
        if (it == assets.end()) {
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Asset [ %s ] has not been loaded, ignoring call to free.", path);
            return;
        } 
        AAsset_close(it->second);
        assets.erase(it);
        
        env->ReleaseStringUTFChars(jAssetPath, path);
        return;
    }

    JNIEXPORT jint JNICALL Java_app_polyvox_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_closeFile(JNIEnv *env, jobject obj,  jobject assetManager, jint fd) {
        close((int)fd);
        return 0;
    }

    // DO NOT USE THIS
    // preserved for posterity only
    // originally I tried to use /proc/self/fd/{fd} as a proxy after opening the asset (using the FD returned from the NDK API)
    // this only works on some versions of Android (or some device manufacturers) 
    // other things I tried:
    // - /proc/self/fd/{fd} with the fd returned by memfd_create - permissions exception
    // - /proc/self/fd/{fd} with the fd returned by ASharedMemory_create - permissions exception
    // - creating a file descriptor in the application tmp dir then calling dup on the above - no permissions exception, but empty
    // - /proc/self/fd/{fd} with a pipe and then using sendfile - this works, but is flaky because the pipe is not seekable so any consumers trying to reposition the file will error out.
    // 
    // However it looks like the security model around memfd_create will be changed so this is probably the way to go in future
    // Revisit when this is available
    JNIEXPORT jint JNICALL Java_app_polyvox_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_getFdFromAsset(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {
        const char *path = env->GetStringUTFChars(jAssetPath, 0);

        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

        AAsset *asset = AAssetManager_open(mgr, path, AASSET_MODE_BUFFER);
        if(asset == NULL) {
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Couldn't locate asset [ %s ]", path);
            return NULL;
        }
        off_t start;
        off_t outLength;
        int asset_fd = AAsset_openFileDescriptor(asset, &start, &outLength);
        
        __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Opened file descriptor [ %d ] for [ %s ] with start %lld and length %lld", asset_fd, path, (long long)start,(long long) outLength);

        env->ReleaseStringUTFChars(jAssetPath, path);
        return asset_fd;       
        
    }


}

