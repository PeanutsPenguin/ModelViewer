#pragma once

#include "ResourceManager.h"
#include "pch.h"

///LibMath Include
#include "LibMath/Vector/Vec2.h"
#include "LibMath/Collision/Collision3D.h"

///Standar Include
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>

///ModernOpenGl Include
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Resources
{
	using Face = std::pair <uint32_t, LibMath::Vector<3, uint32_t>>;

	struct filedata
	{
		std::vector<LibMath::Vector3> m_position;
		std::vector<LibMath::Vector3> m_texPos;
		std::vector<LibMath::Vector3> m_normalPos;
		std::vector<Face> m_indices;
	};

	class Model : public IResources
	{
	public:

		class Vertex
		{
		public:
			LibMath::Vector3 m_position;
			LibMath::Vector3 m_normale;
			LibMath::Vector3 m_texturePosition;

			struct Hash
			{
				size_t operator()(const Vertex& vec) const
				{
					return	std::hash<float>()(vec.m_position.x) ^ std::hash<float>()(vec.m_position.y) ^ std::hash<float>()(vec.m_position.z) ^ 
							std::hash<float>()(vec.m_normale.x) ^ std::hash<float>()(vec.m_normale.y) ^ std::hash<float>()(vec.m_normale.z) ^
						   std::hash<float>()(vec.m_texturePosition.x) ^ std::hash<float>()(vec.m_texturePosition.y);
				}
			};

			Vertex(void) = default;

			Vertex(const Vertex&) = default;

			Vertex& operator=(const Vertex&) = default;

			Vertex(Vertex&&) = default;

			Vertex& operator=(Vertex&&) = default;

			bool operator==(const Vertex& toCompare) const;

			bool operator!=(const Vertex& toCompare) const;

			~Vertex() = default;
		};

		class Buffer
		{
		public:
			Buffer() = default;

			Buffer(std::vector<Model::Vertex>& VertexBuffer);

			Buffer(std::vector<unsigned int>& IndiceBuffer);

			Buffer& operator=(const Buffer&) = default;

			~Buffer() = default;

			unsigned int m_buffer;
		};

		class VertexAttributes
		{
		public:	
			VertexAttributes();

			VertexAttributes& operator=(const VertexAttributes&) = default;

			void SetAttribute(int location, int nbrOfElements, int gapSize, int toFirstSize);

			~VertexAttributes() = default;

			unsigned int m_VAO;
		};

		Model(void) = default;

		~Model(void) = default;

		std::vector<Vertex> m_vertexBuffer;
		std::vector<unsigned int> m_indices;

		Resources::Model::Buffer m_vertexBufferObject;
		Resources::Model::VertexAttributes m_vertexObject;
		Resources::Model::Buffer m_elementBuffer;
	};

	void parseFile(const char* filename, Model* model, bool monoThread, Resources::ResourcesManager* manager);
} 


		/* Ear Clipping */
/// <summary>Handles cutting a quad into 2 triangles.</summary>
/// <param name="vertices">: List of vertices</param>
/// <returns>New array of vertex organized into triangles.</returns>
std::vector<Resources::Model::Vertex> handleQuad(std::vector<Resources::Model::Vertex> const& vertices);
/// <summary>Beginning of ear-clipping algorithm.</summary>
/// <param name="vertices">: List of vertices</param>
/// <returns>New array of vertices organized into triangles.</returns>
std::vector<Resources::Model::Vertex> earClip(std::vector<Resources::Model::Vertex> vertices);
/// <summary>Ending of ear-clipping algorithm.</summary>
/// <param name="vertices">: List of vertices</param>
/// <param name="positions">: Vertices positions</param>
/// <param name="ears">: List of polygon's ears</param>
/// <param name="convexes">: List of polygons's convex vertices</param>
/// <param name="reflexes">: List of polygon's reflexes</param>
/// <returns>New array of vertices organized into triangles.</returns>
std::vector<Resources::Model::Vertex> earClipEnd(std::vector<Resources::Model::Vertex> const& vertices, std::vector<LibMath::Vector3> const& positions,
							   std::vector<int>& ears, std::vector<int>& convexes, std::vector<int>& reflexes);
/// <summary>Assigns correct indices within a list, centered on a given one.</summary>
/// <param name="list">: List of possible indices</param>
/// <param name="minusOne">: index - 1 within the list</param>
/// <param name="plusOne">: index + 1 within the list</param>
/// <param name="index">: "Center" index</param>
void assignIndices(std::vector<int>& list, int& minusOne, int& plusOne, int index);
/// <summary>Triangulate polygon by clipping ears.</summary>
/// <param name="vertices">: List of vertices</param>
/// <param name="ears">: List of polygon's ears</param>
/// <param name="convexes">: List of polygons's convex vertices</param>
/// <param name="reflexes">: List of polygon's reflexes</param>
/// <returns>New array of indices organized into triangles.</returns>
std::vector<int> clipping(std::vector<LibMath::Vector3> const& vertices, std::vector<int>& ears, std::vector<int>& convexes,
						  std::vector<int>& reflexes);
/// <summary>Checks if a polygon is convex at given point.</summary>
/// <param name="vertices">: List of vertices</param>
/// <param name="indices">: Indices to check in the polygon</param>
/// <param name="convexes">: List of polygons's convex vertices</param>
/// <param name="reflexes">: List of polygon's reflexes</param>
/// <returns>True if polygon is convex at given point, else false.</returns>
bool checkConvex(std::vector<LibMath::Vector3> const& vertices, LibMath::Vector3 indices, std::vector<int>& convexes,
				 std::vector<int>& reflexes);
/// <summary>Checks if a given point of a polygon is an ear.</summary>
/// <param name="vertices">: List of vertices</param>
/// <param name="indices">: Indices to check in the polygon</param>
/// <returns>True if the vertex is an ear, else false.</returns>
bool checkEar(std::vector<LibMath::Vector3> const& vertices, LibMath::Vector3 indices);
/// <summary>Re-check convex/reflex vertex in the clipping loop.</summary>
/// <param name="index">: Vertex to check</param>
/// <param name="vectors">: Tuple of vertices, ears, convexes, reflexes and remaining vertices</param>
void clipReCheck(
	int index,
	std::tuple<std::vector<LibMath::Vector3> const&, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&>
		vectors);
/// <summary>Re-check if a vertex is an ear in the clipping loop.</summary>
/// <param name="vertices">: List of vertices</param>
/// <param name="ears">: List of polygon's ears</param>
/// <param name="checkedVertices">: Indices to check in the polygon</param>
/// <param name="index">: Potential ear</param>
void clipReCheckEar(std::vector<LibMath::Vector3> const& vertices, std::vector<int>& ears, LibMath::Vector3& checkedVertices,
					int index);
/* End Ear Clipping */