#pragma once

#define GLEW_STATIC 
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm\gtc\type_ptr.hpp>
#include "window.h"
#include "space.h"
#include "texture.h"
#include "shader.h"
#include "quad.h"
#include "pointcloud.h"

#pragma  warning(push)
#pragma warning( disable : 4244 ) // '=': conversion from 'double' to 'float', possible loss of data
#pragma warning( disable : 4018 ) // '>': signed/unsigned mismatch
#pragma warning( disable : 4267 ) // 'function': conversion from 'size_t' to 'GLsizei', possible loss of data
#include "loader_obj.h"
#pragma  warning(pop)

#pragma  warning(push)
#pragma warning( disable : 4334 ) // '<<': result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
#pragma warning( disable : 4267 ) // '=': conversion from 'size_t' to 'int', possible loss of data
#include "lodepng.h"
#pragma  warning(pop)