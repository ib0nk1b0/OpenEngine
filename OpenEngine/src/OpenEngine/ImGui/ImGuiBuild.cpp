#include "oepch.h"

#define OPENGL

#if defined(VULKAN)
	#include <backends/imgui_impl_glfw.cpp>
	#include <backends/imgui_impl_vulkan.cpp>
#else if defined(OPENGL)
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include <backends/imgui_impl_glfw.cpp>
	#include <backends/imgui_impl_opengl3.cpp>
#endif