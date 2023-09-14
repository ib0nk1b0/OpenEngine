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
			{ ShaderDataType::Float3, "a_Position"  },
			{ ShaderDataType::Float3, "a_Normal"    },
			{ ShaderDataType::Float3, "a_Albedo"    },
			{ ShaderDataType::Float,  "a_Roughness" },
			{ ShaderDataType::Float,  "a_Metalic"   },
			{ ShaderDataType::Int,    "a_EntityID"  }
			});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

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
	}

	void Mesh::SetData(const glm::mat4& transform, int entityID)
	{
		/*Vertex* vertexBase = nullptr;
		Vertex* vertexPointer = nullptr;
		vertexBase = new Vertex[m_IndiciesVec.size()];
		vertexPointer = vertexBase;

		for (int i = 0; i < m_IndiciesVec.size(); i++)
		{
			uint32_t vertexIndex = m_IndiciesVec[i];
			uint32_t normalsIndex = m_NormalsVec[i];

			vertexPointer->Position = transform * m_VertexPositions[vertexIndex];
			vertexPointer->Normal = m_VertexNormals[normalsIndex];
			vertexPointer->Albedo = m_Material.Albedo;
			vertexPointer->Roughness = m_Material.Roughness;
			vertexPointer->Metalic = m_Material.Metalic;
			vertexPointer->EntityID = entityID;
			vertexPointer++;
		}

		uint32_t dataSize = (uint32_t)((uint8_t*)vertexPointer - (uint8_t*)vertexBase);
		m_VertexBuffer->SetData(vertexBase, dataSize);
		
		m_VertexPositions = std::vector<glm::vec4>();
		m_VertexNormals = std::vector<glm::vec3>();
		m_IndiciesVec = std::vector<uint32_t>();
		m_NormalsVec = std::vector<uint32_t>();*/
	}

	MeshInstance::MeshInstance(const Ref<Mesh>& mesh, Material material)
		: m_Mesh(mesh), m_Material(material)
	{
	}

	void MeshInstance::SetData(const glm::mat4& transform, int entityID)
	{
		for (int i = 0; i < m_Mesh->m_IndiciesVec.size(); i++)
		{
			uint32_t vertexIndex = m_Mesh->m_IndiciesVec[i];
			uint32_t normalsIndex = m_Mesh->m_NormalsVec[i];

			//m_Mesh->m_Data.VertexBufferPointer->Position = transform * m_Mesh->m_VertexPositions[vertexIndex];
			m_Mesh->m_Data.VertexBufferPointer->Position = { m_Mesh->m_VertexPositions[vertexIndex].x, m_Mesh->m_VertexPositions[vertexIndex].y, m_Mesh->m_VertexPositions[vertexIndex].z };
			m_Mesh->m_Data.VertexBufferPointer->Normal = m_Mesh->m_VertexNormals[normalsIndex];
			m_Mesh->m_Data.VertexBufferPointer->Albedo = m_Material.Albedo;
			m_Mesh->m_Data.VertexBufferPointer->Roughness = m_Material.Roughness;
			m_Mesh->m_Data.VertexBufferPointer->Metalic = m_Material.Metalic;
			m_Mesh->m_Data.VertexBufferPointer->EntityID = entityID;
			m_Mesh->m_Data.VertexBufferPointer++;
		}

		uint32_t dataSize = (uint32_t)((uint8_t*)m_Mesh->m_Data.VertexBufferPointer - (uint8_t*)m_Mesh->m_Data.VertexBufferBase);
		m_Mesh->m_VertexBuffer->SetData(m_Mesh->m_Data.VertexBufferBase, dataSize);
	}

	//Cube::Cube(Material material)
	//	: Mesh(8, 36, material, m_Indicies)
	//{
	//	glm::vec3 Normals[] = {
	//		{ 0.0f,  0.0f, -1.0f },
	//		{ 0.0f,  0.0f, -1.0f },
	//		{ 0.0f,  0.0f, -1.0f },
	//		{ 0.0f,  0.0f, -1.0f },
	//		{ 0.0f,  0.0f, -1.0f },
	//		{ 0.0f,  0.0f, -1.0f },

	//		{ 0.0f,  0.0f, 1.0f },
	//		{ 0.0f,  0.0f, 1.0f },
	//		{ 0.0f,  0.0f, 1.0f },
	//		{ 0.0f,  0.0f, 1.0f },
	//		{ 0.0f,  0.0f, 1.0f },
	//		{ 0.0f,  0.0f, 1.0f },

	//		{ -1.0f,  0.0f,  0.0f },
	//		{ -1.0f,  0.0f,  0.0f },
	//		{ -1.0f,  0.0f,  0.0f },
	//		{ -1.0f,  0.0f,  0.0f },
	//		{ -1.0f,  0.0f,  0.0f },
	//		{ -1.0f,  0.0f,  0.0f },

	//		{ 1.0f,  0.0f,  0.0f },
	//		{ 1.0f,  0.0f,  0.0f },
	//		{ 1.0f,  0.0f,  0.0f },
	//		{ 1.0f,  0.0f,  0.0f },
	//		{ 1.0f,  0.0f,  0.0f },
	//		{ 1.0f,  0.0f,  0.0f },

	//		{ 0.0f, -1.0f,  0.0f },
	//		{ 0.0f, -1.0f,  0.0f },
	//		{ 0.0f, -1.0f,  0.0f },
	//		{ 0.0f, -1.0f,  0.0f },
	//		{ 0.0f, -1.0f,  0.0f },
	//		{ 0.0f, -1.0f,  0.0f },

	//		{ 0.0f,  1.0f,  0.0f },
	//		{ 0.0f,  1.0f,  0.0f },
	//		{ 0.0f,  1.0f,  0.0f },
	//		{ 0.0f,  1.0f,  0.0f },
	//		{ 0.0f,  1.0f,  0.0f },
	//		{ 0.0f,  1.0f,  0.0f }
	//	};

	//	glm::vec4 Vertices[] = {
	//		{ -0.5f, -0.5f, -0.5f, 1.0f },
	//		{  0.5f, -0.5f, -0.5f, 1.0f },
	//		{  0.5f,  0.5f, -0.5f, 1.0f },
	//		{  0.5f,  0.5f, -0.5f, 1.0f },
	//		{ -0.5f,  0.5f, -0.5f, 1.0f },
	//		{ -0.5f, -0.5f, -0.5f, 1.0f },

	//		{ -0.5f, -0.5f,  0.5f, 1.0f },
	//		{  0.5f, -0.5f,  0.5f, 1.0f },
	//		{  0.5f,  0.5f,  0.5f, 1.0f },
	//		{  0.5f,  0.5f,  0.5f, 1.0f },
	//		{ -0.5f,  0.5f,  0.5f, 1.0f },
	//		{ -0.5f, -0.5f,  0.5f, 1.0f },

	//		{ -0.5f,  0.5f,  0.5f, 1.0f },
	//		{ -0.5f,  0.5f, -0.5f, 1.0f },
	//		{ -0.5f, -0.5f, -0.5f, 1.0f },
	//		{ -0.5f, -0.5f, -0.5f, 1.0f },
	//		{ -0.5f, -0.5f,  0.5f, 1.0f },
	//		{ -0.5f,  0.5f,  0.5f, 1.0f },

	//		{ 0.5f,  0.5f,  0.5f, 1.0f },
	//		{ 0.5f,  0.5f, -0.5f, 1.0f },
	//		{ 0.5f, -0.5f, -0.5f, 1.0f },
	//		{ 0.5f, -0.5f, -0.5f, 1.0f },
	//		{ 0.5f, -0.5f,  0.5f, 1.0f },
	//		{ 0.5f,  0.5f,  0.5f, 1.0f },

	//		{ -0.5f, -0.5f, -0.5f, 1.0f },
	//		{  0.5f, -0.5f, -0.5f, 1.0f },
	//		{  0.5f, -0.5f,  0.5f, 1.0f },
	//		{  0.5f, -0.5f,  0.5f, 1.0f },
	//		{ -0.5f, -0.5f,  0.5f, 1.0f },
	//		{ -0.5f, -0.5f, -0.5f, 1.0f },

	//		{ -0.5f,  0.5f, -0.5f, 1.0f },
	//		{  0.5f,  0.5f, -0.5f, 1.0f },
	//		{  0.5f,  0.5f,  0.5f, 1.0f },
	//		{  0.5f,  0.5f,  0.5f, 1.0f },
	//		{ -0.5f,  0.5f,  0.5f, 1.0f },
	//		{ -0.5f,  0.5f, -0.5f, 1.0f }
	//	};

	//	uint32_t indicies[36];
	//	// front face
	//	for (int i = 0; i < 36; i++)
	//	{
	//		indicies[i] = i;
	//		m_VertexPositions[i] = Vertices[i];
	//		m_Normals[i] = Normals[i];
	//	}

	//	m_VertexArray = VertexArray::Create();

	//	uint32_t vertexBufferSize = (uint32_t)(36 * sizeof(Vertex));
	//	m_VertexBuffer = VertexBuffer::Create(vertexBufferSize);
	//	m_VertexBuffer->SetLayout({
	//		{ ShaderDataType::Float3, "a_Position"  },
	//		{ ShaderDataType::Float3, "a_Normal"    },
	//		{ ShaderDataType::Float3, "a_Albedo"    },
	//		{ ShaderDataType::Float,  "a_Roughness" },
	//		{ ShaderDataType::Float,  "a_Metalic"   },
	//		{ ShaderDataType::Int,    "a_EntityID"  }
	//	});
	//	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	//	m_IndexBuffer = IndexBuffer::Create(indicies, 36);
	//	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	//}

	//Cube::Cube(const glm::mat4& transform, Material material, int entityID)
	//	: Mesh(m_VertexPositions, m_Indices, transform, material, entityID, 36) {}

	//void Cube::SetData(const glm::mat4& transform, int entityID)
	//{
	//	Vertex* vertexBase = nullptr;
	//	Vertex* vertexPointer = nullptr;
	//	vertexBase = new Vertex[36];
	//	vertexPointer = vertexBase;

	//	for (int i = 0; i < 36; i++)
	//	{
	//		vertexPointer->Position = transform * m_VertexPositions[i];
	//		vertexPointer->Normal = m_Normals[i];
	//		vertexPointer->Albedo = m_Material.Albedo;
	//		vertexPointer->Roughness = m_Material.Roughness;
	//		vertexPointer->Metalic = m_Material.Metalic;
	//		vertexPointer->EntityID = entityID;
	//		vertexPointer++;
	//	}

	//	uint32_t dataSize = (uint32_t)((uint8_t*)vertexPointer - (uint8_t*)vertexBase);
	//	m_VertexBuffer->SetData(vertexBase, dataSize);
	//}

}