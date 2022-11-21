//
// Created by tnakkach on 19/11/2022.
//

#ifndef AUDIOOPENSLES_NATIVEOPENSLES_H
#define AUDIOOPENSLES_NATIVEOPENSLES_H


#include <jni.h>
#include <string>
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

// for android log
#include <android/log.h>

// for OpenSLES lib
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


class NativeOpenSLES {
public:
    // constructor to create audio player
    NativeOpenSLES(bool debug);
    ~NativeOpenSLES();

    // setup audio player
    int setup(JNIEnv* env, jobject assetManager, jstring filename);
    // destroy audio player
    void destroy();

    // set/get audio player state
    void playSetState(int state);
    int playGetState();

    void playSeek(int position);
    // get audio track duration
    int getTrackDuration();
    // get audio track position
    int getTrackPosition();

private:

    bool mDebug;

    SLObjectItf mSlEngineObject{nullptr};
    SLEngineItf mSlEngineInterface{nullptr};
    SLObjectItf mSlOutputMixObject{nullptr};

    SLObjectItf fdPlayerObject{nullptr};
    SLPlayItf mFdPlayerPlay{nullptr};
    SLSeekItf mfdPlayerSeek;
};

#endif //AUDIOOPENSLES_NATIVEOPENSLES_H
