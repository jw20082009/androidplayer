//
// Created by Administrator on 2019/7/30.
//

#ifndef ANDROIDPLAYER_SVVIDEOPLAYER_H
#define ANDROIDPLAYER_SVVIDEOPLAYER_H

#include "cgeVideoDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_com_wilbert_player_JniVideoPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */);

JNIEXPORT void JNICALL Java_com_wilbert_player_JniVideoPlayer_onDrawFrame(
        JNIEnv *env,
        jobject /* this */);

namespace CGE
{
    class VideoPlayer{
    public:
        VideoPlayer();
        void onDrawFrame();
        ~VideoPlayer();

    private:
        const char* m_filename;
        int m_mode;//0:阻塞模式，1：流畅模式
    };
}
#ifdef __cplusplus
}
#endif
#endif //ANDROIDPLAYER_SVVIDEOPLAYER_H
