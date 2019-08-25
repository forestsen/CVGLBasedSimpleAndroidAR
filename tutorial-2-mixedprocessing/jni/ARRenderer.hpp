#pragma once
#include <GLES3/gl3.h>

#include "Shader.hpp"

namespace sen
{
    class ARRenderer
    {
    public:
        ARRenderer()
        {

        }

        bool setup(const char *vertex_shader_code, const char *fragment_shader_code);
        void render(double proj_matrix[16], double modelview_matrix[16]);
        void deleteBuffer();

    private:
        GLuint programID;
        GLuint MatrixID;
        GLuint TextureID;
        GLuint texture_id;
        GLuint vertexBuffer;
        GLuint colorBuffer;
        glm::mat4 MVP;
    };
}