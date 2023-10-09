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

    JNIEXPORT jlongArray  JNICALL Java_com_example_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_getNativePointer(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {
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

    JNIEXPORT void JNICALL Java_com_example_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_free(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {

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

    JNIEXPORT jint JNICALL Java_com_example_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_closeFile(JNIEnv *env, jobject obj,  jobject assetManager, jint fd) {
        close((int)fd);
        return 0;
    }

    JNIEXPORT jint JNICALL Java_com_example_flutter_1ffi_1asset_1helper_FlutterFfiAssetHelperPlugin_assetToFilepath(JNIEnv *env, jobject obj,  jobject assetManager, jstring jAssetPath) {
        const char *path = env->GetStringUTFChars(jAssetPath, 0);

        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

        AAsset *asset = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
        if(asset == NULL) {
            __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Couldn't locate asset [ %s ]", path);
            return NULL;
        }
        off_t start;
        off_t outLength;
        int asset_fd = AAsset_openFileDescriptor(asset, &start, &outLength);
        __android_log_print(ANDROID_LOG_VERBOSE, _TAG, "Opened file descriptor [ %d ] for [ %s ] with start %lld and length %lld", asset_fd, path, (long long)start,(long long) outLength);

        int pipefd[2];
        
        if (pipe(pipefd) == -1) {
            __android_log_print(ANDROID_LOG_ERROR, _TAG,"Error opening pipe : %d", errno);    
            return NULL;
        }

         // start a thread for each pipe to avoid blocking 
         // TODO - threadpool
        auto f = [](finfo info) {
            ssize_t written = sendfile(info.fd_out, info.fd_in, &(info.start), info.len);
            close(info.fd_out);
            __android_log_print(ANDROID_LOG_ERROR, _TAG,"Sendfile complete, wrote %d from fd %d to fd %d.", written, info.fd_in, info.fd_out);    
            // AAsset_close(asset);
        };

        finfo info;
        info.fd_out = pipefd[1];
        info.fd_in = asset_fd;
        info.start = start;
        info.len = outLength;
  
        thread th3(f, info);
        th3.detach();

        env->ReleaseStringUTFChars(jAssetPath, path);
        return pipefd[0];
    }


}

