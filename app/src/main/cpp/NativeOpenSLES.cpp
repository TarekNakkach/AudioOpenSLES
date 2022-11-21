//
// Created by tnakkach on 19/11/2022.
//

#include "NativeOpenSLES.h"

NativeOpenSLES::NativeOpenSLES(bool isDebug) {
    mDebug = isDebug;
    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "CUSTOM_LIB", "Audio Player Create");

    SLresult result = slCreateEngine(&mSlEngineObject,
            /*numOptions=*/0, /*options=*/nullptr,
            /*numWantedInterfaces=*/0, /*wantedInterfaces=*/nullptr, /*wantedInterfacesRequired=*/nullptr);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineObject)->Realize(mSlEngineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlEngineObject)->GetInterface(mSlEngineObject, SL_IID_ENGINE, &mSlEngineInterface);
    assert(SL_RESULT_SUCCESS == result);

    SLuint32 const numWantedInterfaces = 0;
    result = (*mSlEngineInterface)->CreateOutputMix(mSlEngineInterface, &mSlOutputMixObject, numWantedInterfaces, nullptr, nullptr);
    assert(SL_RESULT_SUCCESS == result);

    result = (*mSlOutputMixObject)->Realize(mSlOutputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
}

NativeOpenSLES::~NativeOpenSLES()
{
    destroy();
}

void NativeOpenSLES::destroy()
{
    SLresult result;
    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "Destroy Audio Player");

    result = (*mFdPlayerPlay)->SetPlayState(mFdPlayerPlay, SL_PLAYSTATE_STOPPED);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    if (fdPlayerObject != nullptr) (*fdPlayerObject)->Destroy(fdPlayerObject);
    if (mSlOutputMixObject != nullptr) { (*mSlOutputMixObject)->Destroy(mSlOutputMixObject); mSlOutputMixObject = nullptr; }
    if (mSlEngineObject != nullptr) { (*mSlEngineObject)->Destroy(mSlEngineObject); mSlEngineObject = nullptr; }
}


int NativeOpenSLES::setup(JNIEnv* env, jobject assetManager, jstring filename)
{
    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "Audio Player Setup");
    SLresult result;
    int fd;
    off_t start, length;

    // convert Java string to UTF-8
    const char *utf8 = env->GetStringUTFChars(filename, nullptr);
    assert(nullptr != utf8);

    // use asset manager to open asset by filename
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    assert(nullptr != mgr);
    AAsset *asset = AAssetManager_open(mgr, utf8, AASSET_MODE_UNKNOWN);

    // release the Java string and UTF-8
    env->ReleaseStringUTFChars(filename, utf8);

    // the asset might not be found
    if (nullptr == asset) {
        if (mDebug) __android_log_print(ANDROID_LOG_ERROR, "NativeOpenSLES", "Audio Player Setup Failure !");
        return JNI_FALSE;
    }

    // open asset as file descriptor
    fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mSlOutputMixObject};
    SLDataSink audioSnk = {&loc_outmix, nullptr};

    //SLObjectItf fdPlayerObject = nullptr;
    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*mSlEngineInterface)->CreateAudioPlayer(mSlEngineInterface, &fdPlayerObject, &audioSrc, &audioSnk,
                                                      3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &mFdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the seek interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &mfdPlayerSeek);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    SLMuteSoloItf fdPlayerMuteSolo;
    // get the mute/solo interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_MUTESOLO, &fdPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    SLVolumeItf fdPlayerVolume;
    // get the volume interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // enable whole file looping
    result = (*mfdPlayerSeek)->SetLoop(mfdPlayerSeek, SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    return JNI_TRUE;
}

void NativeOpenSLES::playSetState(int state)
{
    SLresult result;
    SLuint32 pState;

    result = (*mFdPlayerPlay)->GetPlayState(mFdPlayerPlay, &pState);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    if (pState != state) {
        if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "Change Player State form %d to %d", pState, state);
        result = (*mFdPlayerPlay)->SetPlayState(mFdPlayerPlay, state);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    } else {
        if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "Player State already : %d ", state);
    }
}

int NativeOpenSLES::playGetState()
{
    SLresult result;
    SLuint32 pState;

    result = (*mFdPlayerPlay)->GetPlayState(mFdPlayerPlay, &pState);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "Player current state : %d ", pState);
    return pState;
}

void NativeOpenSLES::playSeek(int position)
{
    SLresult result;
    position *= 1000;
    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "Player seek to position : %d ", position);
    result = (*mfdPlayerSeek)->SetPosition(mfdPlayerSeek, position, SL_SEEKMODE_FAST);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

int NativeOpenSLES::getTrackDuration()
{
    SLresult result;
    SLmillisecond duration;

    result = (*mFdPlayerPlay)->GetDuration(mFdPlayerPlay, &duration);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "getTrackDuration : %d sec", duration / 1000);
    return (duration / 1000);
}

int NativeOpenSLES::getTrackPosition()
{
    SLresult result;
    SLmillisecond position;

    result = (*mFdPlayerPlay)->GetPosition(mFdPlayerPlay, &position);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    if (mDebug) __android_log_print(ANDROID_LOG_DEBUG, "NativeOpenSLES", "getTrackPosition : %d sec", position / 1000);
    return (position / 1000);
}