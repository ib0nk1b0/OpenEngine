#pragma once

#include "OpenEngine/Renderer/Shader.h"

namespace OpenEngine {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const;
		virtual void UnBind() const;
	private:
		uint32_t m_RendererID;
	};

}