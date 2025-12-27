#pragma once

#include <fg/window/event.h>
#include <fg/window/window.h>
#include <fg/graphics/opengl/gl.h>
#include <fg/graphics/gl_scene.h>
#include <fg/scene/view/camera.h>
#include <fg/scene/view/projection.h>
#include <fg/utils/utils.h>

// 感觉这些 include 也不应该放在这里，不过为了方便先放着吧
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 方便的使用入口，否则 namespace 太长了，fgi 的 i 表示 interface
namespace fgi {

// event header 中提供的东西基本都需要
using namespace fg::window::event;
using namespace fg::graphics::opengl;

using fg::window::CreateWindow;
using fg::window::WindowState;
using fg::window::WindowConfig;
using fg::window::Button;
using fg::window::ButtonMove;

using fg::scene::view::EulerAngleCamera;
using fg::scene::view::PerspProjGenerator;

using fg::utils::GetTime;
using fg::utils::ToString;
using fg::utils::GetRuntimeAbsPath;

}