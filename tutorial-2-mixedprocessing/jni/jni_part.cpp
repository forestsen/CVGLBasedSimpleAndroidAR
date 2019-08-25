#include <jni.h>
#include <android/log.h>
#include <mutex>

#include <GLES3/gl3.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <aruco/aruco.h>

#include "Texture.hpp"
#include "Shader.hpp"
#include "VideoRenderer.hpp"
#include "ARRenderer.hpp"

#define  LOG_TAG    "libgl3cv4"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using namespace std;
using namespace cv;
using namespace glm;
using namespace sen;

int texture_width = 1920;
int texture_height = 1080;

VideoRenderer videoRenderer;
ARRenderer arRenderer;

mutex mtx;
int bufferIndex = 0;
cv::Mat initTexture;
cv::Mat colorImageBuffer;
std::array<cv::Mat, 30> colorBuffer;
cv::Mat outColorFrame;

cv::Mat camera_matrix_color;
cv::Mat dist_coeffs_color;

aruco::Dictionary dic;
aruco::CameraParameters CamParam;
aruco::MarkerDetector MDetector;
std::map<int, aruco::MarkerPoseTracker> MTracker;
float MarkerSize = 0.1f;
double proj_matrix[16];
double modelview_matrix[16];

const char TransformVertexShader [] =
        "#version 300 es\n"
        "\n"
        "// Input vertex data, different for all executions of this shader.\n"
        "layout(location = 0) in vec2 vertexPosition_modelspace;\n"
        "layout(location = 1) in vec2 vertexUV;\n"
        "\n"
        "// Output data ; will be interpolated for each fragment.\n"
        "out vec2 UV;\n"
        "\n"
        "// Values that stay constant for the whole mesh.\n"
        "uniform mat4 MVP;\n"
        "\n"
        "void main(){\n"
        "\n"
        "	// Output position of the vertex, in clip space : MVP * position\n"
        "	gl_Position =  MVP * vec4(vertexPosition_modelspace,0,1);\n"
        "	\n"
        "	// UV of the vertex. No special space for this one.\n"
        "	UV = vertexUV;\n"
        "}\n"
        "\n"
;
const char TextureFragmentShader [] =
        "#version 300 es\n"
        "\n"
        "// Interpolated values from the vertex shaders\n"
        "in vec2 UV;\n"
        "\n"
        "// Ouput data\n"
        "out vec4 color;\n"
        "\n"
        "// Values that stay constant for the whole mesh.\n"
        "uniform sampler2D myTextureSampler;\n"
        "\n"
        "void main(){\n"
        "\n"
        "	// Output color = color of the texture at the specified UV\n"
        "	color = vec4(texture( myTextureSampler, UV ).rgba);\n"
        "}\n"
;
const char ColorFragmentShader [] =
        "#version 300 es\n"
        "\n"
        "// Interpolated values from the vertex shaders\n"
        "in vec3 fragmentColor;\n"
        "\n"
        "// Ouput data\n"
        "out vec3 color;\n"
        "\n"
        "void main(){\n"
        "\n"
        "	// Output color = color specified in the vertex shader, \n"
        "	// interpolated between all 3 surrounding vertices\n"
        "	color = fragmentColor;\n"
        "\n"
        "}\n"
;
const char TransformVertexShader_AR [] =
        "#version 300 es\n"
        "\n"
        "// Input vertex data, different for all executions of this shader.\n"
        "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
        "layout(location = 1) in vec3 vertexColor;\n"
        "\n"
        "// Output data ; will be interpolated for each fragment.\n"
        "out vec3 fragmentColor;\n"
        "// Values that stay constant for the whole mesh.\n"
        "uniform mat4 MVP;\n"
        "\n"
        "void main(){	\n"
        "\n"
        "	// Output position of the vertex, in clip space : MVP * position\n"
        "	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);\n"
        "\n"
        "	// The color of each vertex will be interpolated\n"
        "	// to produce the color of each fragment\n"
        "	fragmentColor = vertexColor;\n"
        "}\n"
        "\n"
;
bool setupGraphics(int w, int h) {

    {
        std::vector<float> _camera_matrix = {
                1778.098850689996f, 0.f, 963.615357815964f,
                0.f, 1779.977728100252f, 539.7192247491328f,
                0.f, 0.f, 1.f};
        camera_matrix_color = cv::Mat(3, 3, CV_32F, &_camera_matrix[0]);
        std::vector<float> _dist_coeffs = {0.1041723054980447f, -0.5810923077984841f, 0.9660986913297525f,
                                      0.f, 0.f};
        dist_coeffs_color = cv::Mat::zeros(5, 1, CV_32F);
        {
            dic = aruco::Dictionary::load("ARUCO_MIP_36h12");
            CamParam.setParams(camera_matrix_color, dist_coeffs_color, cv::Size(texture_width, texture_height));
            MDetector.setDictionary("ARUCO_MIP_36h12", 0.05f);
            MDetector.setDetectionMode(aruco::DM_VIDEO_FAST);
            LOGI("aruco");
        }
    }

    LOGI("setupGraphics(%d, %d)", w, h);//w <== screen_width, h <== screen_height

    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    CamParam.glGetProjectionMatrix(cv::Size(texture_width, texture_height), cv::Size(texture_width, texture_height), proj_matrix, 0.05, 10);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    videoRenderer.setup(TransformVertexShader, TextureFragmentShader);
    initTexture = cv::Mat(texture_height, texture_width, CV_8UC4, cv::Scalar(0,0,0,255));
    videoRenderer.initTexture(initTexture);

    arRenderer.setup(TransformVertexShader_AR, ColorFragmentShader);

    return true;
}
void drawBackground()
{
    mtx.lock();
    outColorFrame = colorBuffer[(bufferIndex - 1) % 30];
    mtx.unlock();

    videoRenderer.render(outColorFrame);
}
void drawAugmentedScene()
{
    arRenderer.render(proj_matrix, modelview_matrix);
}
void display()
{
    drawBackground();
    drawAugmentedScene();
}

//external calls for Java
extern "C" {
    JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_GL3CV4Lib_init(JNIEnv * env, jobject,  jint width, jint height);
    JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_GL3CV4Lib_step(JNIEnv * env, jobject);
    JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_GL3CV4Lib_setImage(JNIEnv * jenv, jobject, jlong imageRGBA);
};

JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_GL3CV4Lib_init(JNIEnv * env, jobject obj, jint width, jint height)
{
    LOGI("setupGraphics");
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_GL3CV4Lib_step(JNIEnv * env, jobject obj)
{
    display();
}

void processFrame(const cv::Mat &frame)
{
    cv::Mat colorFrame = frame;

    cvtColor(colorFrame, colorFrame, COLOR_RGBA2BGR);
    {
        vector<aruco::Marker> Markers = MDetector.detect(colorFrame);
        for (auto &marker : Markers)
        {
            MTracker[marker.id].estimatePose(marker, CamParam, MarkerSize);
        }
        if (CamParam.isValid() && MarkerSize != -1)
        {
            for (unsigned int i = 0; i < Markers.size(); ++i)
            {
                if (Markers[i].isPoseValid())
                {
                    Markers[i].glGetModelViewMatrix(modelview_matrix);
                }
            }
        }
    }
}
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_GL3CV4Lib_setImage(
        JNIEnv * jenv, jobject, jlong imageRGBA) {

    colorImageBuffer = *((Mat*)imageRGBA);

    processFrame(colorImageBuffer);

    mtx.lock();
    colorBuffer[(bufferIndex++) % 30] = colorImageBuffer;
    mtx.unlock();

}


