//
// Created by Administrator on 2019/7/30.
//
#include <string>
#include "include/svVideoPlayer.h"
#include "include/cgeGlobal.h"

static const char* const s_vshYUV420p = CGE_SHADER_STRING
        (
                attribute vec4 vPosition;
varying vec2 vTexCoord;
uniform mat2 rotation;
uniform vec2 flipScale;

void main()
{
    gl_Position = vPosition;
    // vTexCoord = vec2(1.0 + vPosition.x, 1.0 - vPosition.y) / 2.0;
    vTexCoord = flipScale * (vPosition.xy / 2.0 * rotation) + 0.5;
}
);

static CGEConstString s_fshYUV420p = CGE_SHADER_STRING_PRECISION_M
        (
                varying vec2 vTexCoord;

uniform sampler2D textureY;
uniform sampler2D textureU;
uniform sampler2D textureV;
uniform mat3 m3ColorConversion;

void main()
{
    vec3 yuv;
    yuv.x = texture2D(textureY, vTexCoord).r; //fix: use full range
    yuv.y = texture2D(textureU, vTexCoord).r - 0.5;
    yuv.z = texture2D(textureV, vTexCoord).r - 0.5;
    vec3 videoColor = m3ColorConversion * yuv;
    gl_FragColor = vec4(videoColor, 1.0);
}
);

extern "C" JNIEXPORT jstring JNICALL
Java_com_wilbert_player_JniVideoPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL Java_com_wilbert_player_JniVideoPlayer_onDrawFrame(JNIEnv *env,jobject){
    glClearColor(0.0,0.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

namespace CGE{

}