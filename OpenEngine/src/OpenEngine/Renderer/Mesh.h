#pragma once

#include "OpenEngine/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	struct Material
	{
		glm::vec3 Albedo{ 1.0f };
		float Roughness = 1.0f;
		float Metalic = 0.0f;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
	};

	struct InstanceVertex
	{
		glm::vec3 Albedo;
		float Roughness;
		float Metalic;
		int EntityID;
		glm::vec4 Transform0;
		glm::vec4 Transform1;
		glm::vec4 Transform2;
		glm::vec4 Transform3;
	};

	struct InstanceData
	{
		static const uint32_t MaxMeshes = 1000;
		uint32_t MaxMeshVerticies;
		uint32_t MaxMeshIndicies;

		uint32_t IndexCount = 0;
		uint32_t MeshCount = 0;

		Vertex* VertexBufferBase = nullptr;
		Vertex* VertexBufferPointer = nullptr;

		InstanceVertex* InstanceBufferBase = nullptr;
		InstanceVertex* InstanceBufferPointer = nullptr;
	};
	
	class Mesh
	{
	public:
		Mesh(const std::string& filepath);
		~Mesh();

		void LoadFromFile(const std::string& filepath);
		void Flush();

		void ResetData();

		void AddIndexCount(uint32_t indexCount) { m_Data.IndexCount += indexCount; }

		std::string GetFilePath() { return m_Filepath; }
		Ref<VertexArray>& GetVertexArray() { return m_VertexArray; }
		uint32_t GetIndexCount() { return m_Data.IndexCount; }
		bool HasMeshes() { return m_Data.MeshCount > 0; }
		uint32_t InstanceCount() { return m_Data.MeshCount; }

		void SetData(const glm::mat4& transform, Material material, int entityID);
	private:
		std::vector<glm::vec4> m_VertexPositions;
		std::vector<glm::vec3> m_VertexNormals;
		std::vector<uint32_t> m_IndiciesVec;
		std::vector<uint32_t> m_NormalsVec;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<VertexBuffer> m_InstanceBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		std::string m_Filepath;

		InstanceData m_Data;
		uint32_t m_Offset = 0;
	};

}