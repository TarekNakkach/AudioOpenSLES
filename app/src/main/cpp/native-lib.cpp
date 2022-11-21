//
// Created by tnakkach on 19/11/2022.
//

#include "NativeOpenSLES.h"
static NativeOpenSLES player(true);

extern "C" JNIEXPORT void JNICALL
Java_com_example_audioopensles_MainActivity_NativeLibAudioSetup(
        JNIEnv* env,
        jobject /* this */,
        jobject assetManager,
        jstring filename) {
    player.setup(env, assetManager, filename);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audioopensles_MainActivity_NativeLibAudioDestroy(
        JNIEnv* env,
        jobject /* this */) {
    player.destroy();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audioopensles_MainActivity_NativeLibAudioSetState(
        JNIEnv* env,
        jobject /* this */,
        int state) {
    player.playSetState(state);
}

extern "C" JNIEXPORT void
Java_com_example_audioopensles_MainActivity_NativeLibAudioSeek(
        JNIEnv* env,
        jobject /* this */,
        int position /* in seconds */) {
    player.playSeek(position);
}

extern "C" JNIEXPORT int JNICALL
Java_com_example_audioopensles_MainActivity_NativeLibAudioGetState(
        JNIEnv* env,
        jobject /* this */) {
    return player.playGetState();
}

extern "C" JNIEXPORT int JNICALL
Java_com_example_audioopensles_MainActivity_NativeLibAudioGetDuration(
        JNIEnv* env,
        jobject /* this */) {
    return player.getTrackDuration();
}
extern "C" JNIEXPORT int JNICALL
Java_com_example_audioopensles_MainActivity_NativeLibAudioGetPosition(
        JNIEnv* env,
        jobject /* this */) {
    return player.getTrackPosition();
}
