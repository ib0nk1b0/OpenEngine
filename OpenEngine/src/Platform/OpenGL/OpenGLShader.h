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

		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& values) const;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;
	private:
		uint32_t m_RendererID;
	};

}