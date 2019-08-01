//
// Created by Administrator on 2019/7/30.
//

#ifndef ANDROIDPLAYER_SVVIDEOPLAYER_H
#define ANDROIDPLAYER_SVVIDEOPLAYER_H

#include "cgeVideoDecoder.h"
#include "cgeGLFunctions.h"
#ifdef __cplusplus
extern "C" {
#endif

namespace CGE
{
    class VideoPlayer{
    public:
        VideoPlayer();
        ~VideoPlayer();
        bool open(const char* filename);
        bool initWithDecodeHandler(CGEVideoDecodeHandler*);
        bool update(double time);
        bool updateVideoFrame(const CGEVideoFrameBufferData* data = nullptr);
        void setRotation(float rad);
        void setFlipScale(float x, float y);
        void render();
        bool nextVideoFrame();
        CGEFrameTypeNext queryNextFrame() { return m_decodeHandler->queryNextFrame(); }
        void close();
    protected:
        ProgramObject m_program;
        GLuint m_texYUV[3];
        GLint m_texYLoc, m_texULoc, m_texVLoc;
        GLuint m_posAttribLocation;
        GLuint m_rotLoc, m_flipScaleLoc,m_texScale;
        CGEVideoDecodeHandler* m_decodeHandler;

        GLuint m_vertexBuffer;
        int m_videoWidth, m_videoHeight;
        int m_linesize[3];
    };
}

JNIEXPORT jstring JNICALL Java_com_wilbert_player_JniVideoPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */);

JNIEXPORT void JNICALL Java_com_wilbert_player_JniVideoPlayer_onDrawFrame(
        JNIEnv *env,
        jobject /* this */,jlong timestamp);

JNIEXPORT void JNICALL Java_com_wilbert_player_JniVideoPlayer_initPlayer(JNIEnv *env,jobject ,jstring filepath);

#ifdef __cplusplus
}
#endif
#endif //ANDROIDPLAYER_SVVIDEOPLAYER_H
