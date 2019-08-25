#pragma once

#include <jni.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sen
{
	GLuint loadShader(GLenum shader_type, const char *p_source);
	GLuint createShaderProgram(const char *vertex_shader_code, const char *fragment_shader_code);
}