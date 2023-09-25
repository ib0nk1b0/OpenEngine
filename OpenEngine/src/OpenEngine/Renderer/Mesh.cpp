#include "oepch.h"
#include "Mesh.h"

#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Core/Application.h"

namespace OpenEngine {

	static bool StartWith(const std::string& line, const char* text)
	{
		size_t textLength = strlen(text);
		if (line.size() < textLength)
			return false;

		for (size_t i = 0; i < textLength; i++)
		{
			if (line[i] == text[i])
				continue;
			else
				return false;
		}

		return true;
	}

	Mesh::Mesh(const std::string& filepath)
		: m_Filepath(filepath)
	{
		LoadFromFile(filepath);

		m_Data.MaxMeshVerticies = m_Data.MaxMeshes * m_IndiciesVec.size();
		m_Data.MaxMeshIndicies = m_Data.MaxMeshes * m_IndiciesVec.size();

		m_Data.VertexBufferBase = new Vertex[m_Data.MaxMeshVerticies];
		m_Data.VertexBufferPointer = m_Data.VertexBufferBase;

		m_Data.InstanceBufferBase = new InstanceVertex[m_Data.MaxMeshes];
		m_Data.InstanceBufferPointer = m_Data.InstanceBufferBase;

		uint32_t* indicies = new uint32_t[m_Data.MaxMeshIndicies];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_Data.MaxMeshIndicies; i++)
		{
			indicies[i] = i;
		}

		m_VertexArray = VertexArray::Create();

		uint32_t vertexBufferSize = (uint32_t)(m_Data.MaxMeshVerticies * sizeof(Vertex));
		m_VertexBuffer = VertexBuffer::Create(vertexBufferSize);
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"   },
			{ ShaderDataType::Float3, "a_Normal"     }
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t instanceBufferSize = (uint32_t)(m_Data.MaxMeshes * sizeof(InstanceVertex));
		m_InstanceBuffer = VertexBuffer::Create(instanceBufferSize);
		m_InstanceBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Albedo",     true },
			{ ShaderDataType::Float,  "a_Roughness",  true },
			{ ShaderDataType::Float,  "a_Metalic",    true },
			{ ShaderDataType::Int,    "a_EntityID",   true },
			{ ShaderDataType::Float4, "a_Transform0", true },
			{ ShaderDataType::Float4, "a_Transform1", true },
			{ ShaderDataType::Float4, "a_Transform2", true },
			{ ShaderDataType::Float4, "a_Transform3", true }
		});
		m_VertexArray->AddVertexBuffer(m_InstanceBuffer, 2);

		m_IndexBuffer = IndexBuffer::Create(indicies, m_Data.MaxMeshIndicies);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		delete[] indicies;
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::LoadFromFile(const std::string& filepath)
	{
		OE_PROFILE_FUNCTION();
		std::ifstream file(filepath);
		if (file)
		{
			std::string line;
			while (std::getline(file, line))
			{
				if (StartWith(line, "v "))
				{
					glm::vec3 Position;
					sscanf_s(line.c_str(), "v %f %f %f", &Position.x, &Position.y, &Position.z);
					m_VertexPositions.push_back({ Position, 1.0 });
					
				}
				if (StartWith(line, "vn "))
				{
					glm::vec3 Normal;
					sscanf_s(line.c_str(), "vn %f %f %f", &Normal.x, &Normal.y, &Normal.z);
					m_VertexNormals.push_back(Normal);
				}
				if (StartWith(line, "f "))
				{
					int v1, v2, v3;
					int n1, n2, n3;
					(void)sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
					
					m_IndiciesVec.push_back(v1 - 1);
					m_IndiciesVec.push_back(v2 - 1);
					m_IndiciesVec.push_back(v3 - 1);

					m_NormalsVec.push_back(n1 - 1);
					m_NormalsVec.push_back(n2 - 1);
					m_NormalsVec.push_back(n3 - 1);
				}
			}
		}
		else
		{
			OE_CORE_ERROR("Could not open file '{0}'", filepath);
		}
	}

	void Mesh::ResetData()
	{
		m_Data.VertexBufferPointer = m_Data.VertexBufferBase;
		m_Data.InstanceBufferPointer = m_Data.InstanceBufferBase;
		m_Data.IndexCount = 0;
		m_Data.MeshCount = 0;
	}

	void Mesh::SetData(const glm::mat4& transform, Material material, int entityID)
	{
		for (int i = 0; i < m_IndiciesVec.size(); i++)
		{
			uint32_t vertexIndex = m_IndiciesVec[i];
			uint32_t normalsIndex = m_NormalsVec[i];

			//m_Data.VertexBufferPointer->Position = transform * m_VertexPositions[vertexIndex];
			m_Data.VertexBufferPointer->Position = m_VertexPositions[vertexIndex];
			m_Data.VertexBufferPointer->Normal = m_VertexNormals[normalsIndex];
			m_Data.VertexBufferPointer++;
		}
		
		m_Data.InstanceBufferPointer->Albedo = material.Albedo;
		m_Data.InstanceBufferPointer->Roughness = material.Roughness;
		m_Data.InstanceBufferPointer->Metalic = material.Metalic;
		m_Data.InstanceBufferPointer->EntityID = entityID;
		m_Data.InstanceBufferPointer->Transform0 = transform[0];
		m_Data.InstanceBufferPointer->Transform1 = transform[1];
		m_Data.InstanceBufferPointer->Transform2 = transform[2];
		m_Data.InstanceBufferPointer->Transform3 = transform[3];
		m_Data.InstanceBufferPointer++;

		m_Data.IndexCount += m_IndiciesVec.size();
		m_Data.MeshCount++;
	}

	void Mesh::Flush()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.VertexBufferPointer - (uint8_t*)m_Data.VertexBufferBase);
		m_VertexBuffer->SetData(m_Data.VertexBufferBase, dataSize);

		dataSize = (uint32_t)((uint8_t*)m_Data.InstanceBufferPointer - (uint8_t*)m_Data.InstanceBufferBase);
		m_InstanceBuffer->SetData(m_Data.InstanceBufferBase, dataSize);
	}

}