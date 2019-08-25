#pragma once
#include <opencv2/core/core.hpp>

#include <glm/glm.hpp>

namespace sen
{
    class VideoRenderer
    {
    public:
        VideoRenderer()
        {

        }

        bool setup(const char *vertex_shader_code, const char *fragment_shader_code);
        void render(const cv::Mat &frame);
        bool initTexture(const cv::Mat &frame);
        void deleteBuffer();

    private:
        GLuint programID;
        GLuint MatrixID;
        GLuint TextureID;
        GLuint texture_id;
        GLuint vertexBuffer;
        GLuint uvBuffer;
        glm::mat4 MVP;
    };
}