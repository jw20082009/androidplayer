//
// Created by Administrator on 2019/7/30.
//
#include <string>
#include "include/svVideoPlayer.h"
#include "include/cgeGlobal.h"
#include <cmath>

static const char *const s_vshYUV420p = CGE_SHADER_STRING_PRECISION_M
                                        (
                                                attribute
                                                vec4 vPosition;
                                                varying
                                                vec2 vTexCoord;
                                                uniform
                                                mat2 rotation;
                                                uniform
                                                vec2 flipScale;

                                                void main() {
                                                    gl_Position = vPosition;
                                                    // vTexCoord = vec2(1.0 + vPosition.x, 1.0 - vPosition.y) / 2.0;
                                                    vTexCoord = flipScale *
                                                                (vPosition.xy / 2.0 * rotation) +
                                                                0.5;
                                                }
                                        );

static CGEConstString s_fshYUV420p = CGE_SHADER_STRING_PRECISION_M
                                     (
                                             varying
                                             vec2 vTexCoord;
                                             uniform
                                             sampler2D textureY;
                                             uniform
                                             sampler2D textureU;
                                             uniform
                                             sampler2D textureV;
                                             uniform
                                             mat3 m3ColorConversion;
                                             uniform float texScale;
                                             void main() {
                                                 vec3 yuv;
                                                 yuv.x = texture2D(textureY,
                                                                   vec2(vTexCoord.x * texScale,
                                                                        vTexCoord.y)).r; //fix: use full range
                                                 yuv.y = texture2D(textureU,
                                                                   vec2(vTexCoord.x * texScale,
                                                                        vTexCoord.y)).r - 0.5;
                                                 yuv.z = texture2D(textureV,
                                                                   vec2(vTexCoord.x * texScale,
                                                                        vTexCoord.y)).r - 0.5;
                                                 vec3 videoColor = m3ColorConversion * yuv;

                                                 gl_FragColor = vec4(videoColor, 1.0);
                                             }
                                     );

static const GLfloat s_colorConversion601[] = {
        1, 1, 1,
        0, -0.34413, 1.772,
        1.402, -.71414, 0
};

namespace CGE {

    VideoPlayer::VideoPlayer() : m_posAttribLocation(0), m_decodeHandler(nullptr),
                                 m_vertexBuffer(0) {
        CGE_LOG_INFO("VideoPlayer constructor");
        m_program.bindAttribLocation("vPosition", m_posAttribLocation);
        if (!m_program.initWithShaderStrings(s_vshYUV420p, s_fshYUV420p)) {
            CGE_LOG_ERROR("cgeVideoPlayerYUV420P program init failed!");
            return;
        }
        m_program.bind();
        m_program.sendUniformMat3("m3ColorConversion", 1, GL_FALSE, s_colorConversion601);

        //textureY

        m_texYLoc = m_program.uniformLocation("textureY");
        m_texULoc = m_program.uniformLocation("textureU");
        m_texVLoc = m_program.uniformLocation("textureV");
        CGE_LOG_INFO("VideoPlayer m_texYLoc %d,m_texULoc %d,m_texVLoc %d", m_texYLoc, m_texULoc,
                     m_texVLoc);
        glUniform1i(m_texYLoc, 1);
        glUniform1i(m_texULoc, 2);
        glUniform1i(m_texVLoc, 3);

        if (m_texYLoc < 0 || m_texULoc < 0 || m_texVLoc < 0) {
            CGE_LOG_ERROR("Invalid YUV Texture Uniforms %d,%d,%d\n", m_texULoc, m_texVLoc,
                          m_texYLoc);
        }

        memset(m_texYUV, 0, sizeof(m_texYUV));

        m_rotLoc = m_program.uniformLocation("rotation");
        m_texScale = m_program.uniformLocation("texScale");
        m_flipScaleLoc = m_program.uniformLocation("flipScale");
        CGE_LOG_INFO("VideoPlayer m_rotLoc %d,m_flipScaleLoc %d", m_rotLoc, m_flipScaleLoc);
        setRotation(0.0f);
        setFlipScale(1.0f, -1.0f);
        cgeCheckGLError("cgeVideoPlayerYUV420P");
    }

    VideoPlayer::~VideoPlayer() {
        close();
    }

    bool VideoPlayer::open(const char *filename) {
        if (m_decodeHandler != nullptr)
            close();

        m_decodeHandler = new CGEVideoDecodeHandler();
        if (!m_decodeHandler->open(filename)) {
            CGE_LOG_ERROR("Open %s failed!\n", filename);
            return false;
        }
        m_linesize[0] = m_linesize[1] = m_linesize[2] = 0;
        return initWithDecodeHandler(m_decodeHandler);
    }

    bool VideoPlayer::initWithDecodeHandler(CGEVideoDecodeHandler *handler) {
        assert(handler != nullptr); //handler == nullptr
        if (m_decodeHandler != handler && m_decodeHandler != nullptr)
            delete m_decodeHandler;
        m_decodeHandler = handler;
        m_linesize[0] = m_videoWidth = m_decodeHandler->getWidth();
        m_linesize[2] = m_linesize[1] = m_linesize[0] / 2;
        m_videoHeight = m_decodeHandler->getHeight();
        CGE_LOG_INFO(
                "CGEVideoPlayerYUV420P initWithDecodeHandlerm_linesize[0], %d,m_linesize[2] %d",
                m_linesize[0], m_linesize[2]);
        m_texYUV[0] = cgeGenTextureWithBuffer(nullptr, m_linesize[0], m_videoHeight, GL_LUMINANCE,
                                              GL_UNSIGNED_BYTE, 1, 1);
        m_texYUV[1] = cgeGenTextureWithBuffer(nullptr, m_linesize[1], m_videoHeight / 2,
                                              GL_LUMINANCE, GL_UNSIGNED_BYTE, 1, 2);
        m_texYUV[2] = cgeGenTextureWithBuffer(nullptr, m_linesize[2], m_videoHeight / 2,
                                              GL_LUMINANCE, GL_UNSIGNED_BYTE, 1, 3);
        if (m_vertexBuffer == 0)
            m_vertexBuffer = cgeGenCommonQuadArrayBuffer();
        CGE_LOG_INFO("CGEVideoPlayerYUV420P vertex buffer id: %d", m_vertexBuffer);
        return m_vertexBuffer != 0;
    }

    void VideoPlayer::close() {
        glDeleteTextures(3, m_texYUV);
        memset(m_texYUV, 0, sizeof(m_texYUV));
        delete m_decodeHandler;
        m_decodeHandler = nullptr;
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }

    void VideoPlayer::setRotation(float rad) {
        float cosRad = cosf(rad);
        float sinRad = sinf(rad);
        float mat2[] = {
                cosRad, sinRad,
                -sinRad, cosRad
        };

        m_program.bind();
        glUniformMatrix2fv(m_rotLoc, 1, GL_FALSE, mat2);
    }

    void VideoPlayer::setFlipScale(float x, float y) {
        m_program.bind();
        glUniform2f(m_flipScaleLoc, x, y);
    }

    void VideoPlayer::render() {
        CGE_LOG_CODE(
                if (m_texYUV[0] == 0) {
                    CGE_LOG_ERROR("You should get a frame first!\n");
                    return;
                }
        );
        m_program.bind();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texYUV[0]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_texYUV[1]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_texYUV[2]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glEnableVertexAttribArray(m_posAttribLocation);
        glVertexAttribPointer(m_posAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    bool VideoPlayer::updateVideoFrame(const CGEVideoFrameBufferData *data) {
        CGE_LOG_INFO("svVideoPlayer updateVideoFrame ");
        const CGEVideoFrameBufferData *pFramebuffer =
                data == nullptr ? m_decodeHandler->getCurrentVideoFrame() : data;

        if (pFramebuffer == nullptr) {
            return false;
        }
        const CGEVideoFrameBufferData &framebuffer = *pFramebuffer;

        m_program.bind();
        if (framebuffer.linesize[0] !=
            framebuffer.width) {//当linesize不等于视频宽度时会导致绿边，linesize是指每一行占多少字节，可能比宽度nwidth要大，它是根据cpu来对齐的，可能是16或32的整数倍，不同的cpu有不同的对齐方式。
            CGE_LOG_INFO("mLineSize not the same:%d,%d", framebuffer.linesize[0],framebuffer.width);
            glUniform1f(m_texScale,1.0f*framebuffer.width / framebuffer.linesize[0]);
        }else{
            glUniform1f(m_texScale, 1.0f);
        }
        if (m_linesize[0] != framebuffer.linesize[0]) {
            m_linesize[0] = framebuffer.linesize[0];
            m_linesize[1] = framebuffer.linesize[1];
            m_linesize[2] = framebuffer.linesize[2];
            CGE_LOG_INFO("mLineSize:%d,%d,%d", m_linesize[0], m_linesize[1], m_linesize[2]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_texYUV[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_linesize[0], m_videoHeight, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[0]);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_texYUV[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_linesize[1], m_videoHeight / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[1]);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, m_texYUV[2]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_linesize[2], m_videoHeight / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[2]);
        } else {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_texYUV[0]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_linesize[0], m_videoHeight, GL_LUMINANCE,
                            GL_UNSIGNED_BYTE, framebuffer.data[0]);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_texYUV[1]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_linesize[1], m_videoHeight / 2, GL_LUMINANCE,
                            GL_UNSIGNED_BYTE, framebuffer.data[1]);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, m_texYUV[2]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_linesize[2], m_videoHeight / 2, GL_LUMINANCE,
                            GL_UNSIGNED_BYTE, framebuffer.data[2]);
        }
        glEnableVertexAttribArray(m_posAttribLocation);
        glVertexAttribPointer(m_posAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        cgeCheckGLError("cgeVideoPlayerYUV420P::updateVideoFrame");
        return true;
    }

    bool VideoPlayer::nextVideoFrame() {
        const CGEVideoFrameBufferData *data = m_decodeHandler->getNextVideoFrame();
        if (data == nullptr) {
            CGE_LOG_INFO("svVideoPlayer nextVideoFrame data == nullptr");
            return false;
        }
        return VideoPlayer::updateVideoFrame(data);
    }

    bool VideoPlayer::update(double time) {
        double ts = m_decodeHandler->getCurrentTimestamp();
        CGE_LOG_INFO("update:%lf,%lf", time, ts);
        if (time < ts)
            return true;
        return nextVideoFrame();
    }

    VideoPlayer *m_videoPlayer = NULL;
    bool initedPlayer = false;

    bool cgeInitPlayer(const char *inputFilename) {
        if (!initedPlayer) {
//            CGEVideoDecodeHandler *decodeHandler = new CGEVideoDecodeHandler();
//            if (!decodeHandler->open(inputFilename)) {
//                CGE_LOG_ERROR("Open %s failed!\n", inputFilename);
//                delete decodeHandler;
//                return false;
//            }
//            m_videoPlayer = new VideoPlayer();
            m_videoPlayer = new VideoPlayer();
            m_videoPlayer->open(inputFilename);
            initedPlayer = true;
//            m_videoPlayer.initWithDecodeHandler(decodeHandler);
        }
        return initedPlayer;
    }

    void cgeDrawFrame(jlong timestamp) {
        m_videoPlayer->update(timestamp);
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_wilbert_player_JniVideoPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_wilbert_player_JniVideoPlayer_initPlayer(JNIEnv *env, jobject, jstring filepath) {
    const char *_filepath = env->GetStringUTFChars(filepath, 0);
    CGE::cgeInitPlayer(_filepath);
    env->ReleaseStringUTFChars(filepath, _filepath);
}

extern "C" JNIEXPORT void JNICALL
Java_com_wilbert_player_JniVideoPlayer_onDrawFrame(JNIEnv *env, jobject, jlong timestamp) {
    CGE::cgeDrawFrame(timestamp);
}