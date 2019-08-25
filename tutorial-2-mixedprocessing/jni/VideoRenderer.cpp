#include <GLES/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.hpp"
#include "Shader.hpp"
#include "VideoRenderer.hpp"

#define LOG_TAG "VideoRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool sen::VideoRenderer::setup(const char *vertex_shader_code, const char *fragment_shader_code)
{
    programID = createShaderProgram(vertex_shader_code, fragment_shader_code);
    MatrixID = glGetUniformLocation(programID, "MVP");
    return true;
}

bool sen::VideoRenderer::initTexture(const cv::Mat &frame)
{
    texture_id = createTexture();
    TextureID = glGetUniformLocation(programID, "myTextureSampler");

    int texture_width = frame.size().width;
    int texture_height = frame.size().height;

    const GLfloat bgTextureVertices[] = {0, 0, (float)texture_width, 0, 0, (float)texture_height, (float)texture_width, (float)texture_height};
    const GLfloat bgTextureCoords[] = {1, 0, 1, 1, 0, 0, 0, 1};

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgTextureVertices), bgTextureVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgTextureCoords), bgTextureCoords, GL_STATIC_DRAW);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.data);

    return true;
}

void sen::VideoRenderer::render(const cv::Mat &frame)
{
    int texture_width = frame.size().width;
    int texture_height = frame.size().height;

    const GLfloat proj[] = {
        0, -2.f / texture_width, 0, 0,
        -2.f / texture_height, 0, 0, 0,
        0, 0, 1, 0,
        1, 1, 0, 1};
    glm::mat4 Projection = glm::make_mat4(proj);
    glm::mat4 View = glm::mat4(1.0f);
    glm::mat4 Model = glm::mat4(1.0f);
    MVP = Projection * View * Model;

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(TextureID, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.data);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glColor4f(1, 1, 1, 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void sen::VideoRenderer::deleteBuffer()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &texture_id);
}