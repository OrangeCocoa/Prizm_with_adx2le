
#include<vector>
#include<DirectXTK/SimpleMath.h>

#include"GeometryGenerator.h"
#include"Buffer.h"
#include"..\Framework\Utils.h"

namespace Prizm
{
	namespace GeometryGenerator
	{
		void CalculateTangentsAndBitangents(std::vector<VertexBuffer3D>& vertices, const std::vector<unsigned> indices)
		{
			const size_t vertices_count = vertices.size();
			const size_t indices_count = indices.size();
			assert(indices_count % 3 == 0);

			const DirectX::SimpleMath::Vector3 N = DirectX::SimpleMath::Vector3(0, 0, 1);	// forward

			for (size_t i = 0; i < indices_count; i += 3)
			{
				VertexBuffer3D& v0 = vertices[indices[i]];
				VertexBuffer3D& v1 = vertices[indices[i + 1]];
				VertexBuffer3D& v2 = vertices[indices[i + 2]];

				const DirectX::SimpleMath::Vector4 E1 = v1.position - v0.position;
				const DirectX::SimpleMath::Vector4 E2 = v2.position - v0.position;

				const DirectX::SimpleMath::Vector2 uv1 = DirectX::SimpleMath::Vector2(v1.uv - v0.uv);
				const DirectX::SimpleMath::Vector2 uv2 = DirectX::SimpleMath::Vector2(v2.uv - v0.uv);

				const float f = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);
				assert(!std::isinf(f));

				DirectX::SimpleMath::Vector3 T(
					f * (uv2.y * E1.x - uv1.y * E2.x),
					f * (uv2.y * E1.y - uv1.y * E2.y),
					f * (uv2.y * E1.z - uv1.y * E2.z));
				T.Normalize();

				DirectX::SimpleMath::Vector3 B(
					f * (-uv2.x * E1.x + uv1.x * E2.x),
					f * (-uv2.x * E1.y + uv1.x * E2.y),
					f * (-uv2.x * E1.z + uv1.x * E2.z));
				B.Normalize();

				v0.tangent = T;
				v1.tangent = T;
				v2.tangent = T;

				if (v0.normal == DirectX::SimpleMath::Vector3(0, 0, 0))
				{
					T.Cross(B, v0.normal);
					v0.normal.Normalize();
				}

				if (v1.normal == DirectX::SimpleMath::Vector3(0, 0, 0))
				{
					T.Cross(B, v1.normal);
					v1.normal.Normalize();
				}

				if (v2.normal == DirectX::SimpleMath::Vector3(0, 0, 0))
				{
					T.Cross(B, v2.normal);
					v2.normal.Normalize();
				}
			}
		}

		Geometry Triangle2D(float scale, float center_x, float center_y)
		{
			const float& size = scale;

			std::vector<VertexBuffer2D> vertices(3);
			std::vector<unsigned> indices = { 0, 1, 2 };

			// vertices - CW
			vertices[0].position = DirectX::SimpleMath::Vector2(-size, -size);
			vertices[0].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
			vertices[0].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[1].position = DirectX::SimpleMath::Vector2(0, size);
			vertices[1].uv = DirectX::SimpleMath::Vector2(0.5f, 0.0f);
			vertices[1].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[2].position = DirectX::SimpleMath::Vector2(size, -size);
			vertices[2].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[2].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			for (auto& vertex : vertices)
			{
				vertex.position.x += center_x;
				vertex.position.y += center_y;
			}

			return Geometry(vertices, indices, BufferUsage::DYNAMIC);
		}

		Geometry Triangle3D(float scale)
		{
			const float& size = scale;

			std::vector<VertexBuffer3D> vertices(3);
			std::vector<unsigned> indices = { 0, 1, 2 };

			// vertices - CW
			vertices[0].position = DirectX::SimpleMath::Vector4(-size, -size, 0.0f, 1.0f);
			vertices[0].normal = DirectX::SimpleMath::Vector3(0, 0, -1);	// Backward
			vertices[0].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
			vertices[0].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[1].position = DirectX::SimpleMath::Vector4(0, size, 0.0f, 1.0f);
			vertices[1].normal = DirectX::SimpleMath::Vector3(0, 0, -1);	// Backward
			vertices[1].uv = DirectX::SimpleMath::Vector2(0.5f, 0.0f);
			vertices[1].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[2].position = DirectX::SimpleMath::Vector4(size, -size, 0.0f, 1.0f);
			vertices[2].normal = DirectX::SimpleMath::Vector3(0, 0, -1);	// Backward
			vertices[2].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[2].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		// 
		Geometry Quad2D(float width, float height, float center_x, float center_y)
		{
			const float width_half = width / 2;
			const float height_half = height / 2;

			//	  1	+-----+ 2	0, 1, 2
			//		|	  |		2, 3, 0
			//		|	  |		
			//	  0 +-----+ 3	
			const std::vector<unsigned> indices =
			{
				0, 1, 2,
				2, 3, 0
			};

			std::vector<VertexBuffer2D> vertices(4);

			// vertices - CW
			vertices[0].position = DirectX::SimpleMath::Vector2(-width_half, -height_half);
			vertices[0].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
			vertices[0].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[1].position = DirectX::SimpleMath::Vector2(-width_half, height_half);
			vertices[1].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[1].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[2].position = DirectX::SimpleMath::Vector2(width_half, height_half);
			vertices[2].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[2].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			vertices[3].position = DirectX::SimpleMath::Vector2(width_half, -height_half);
			vertices[3].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[3].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			for (auto& vertex : vertices)
			{
				vertex.position.x += center_x;
				vertex.position.y += center_y;
			}

			return Geometry(vertices, indices, BufferUsage::DYNAMIC);
		}

		Geometry Quad3D(float scale)
		{
			const float& size = scale;

			//	  1	+-----+ 2	0, 1, 2
			//		|	  |		2, 3, 0
			//		|	  |		
			//	  0 +-----+ 3	
			const std::vector<unsigned> indices =
			{
				0, 1, 2,
				2, 3, 0
			};

			std::vector<VertexBuffer3D> vertices(4);

			// vertices - CW
			vertices[0].position = DirectX::SimpleMath::Vector4(-size, -size, 0.0f, 1.0f);
			vertices[0].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[0].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
			vertices[0].color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);

			vertices[1].position = DirectX::SimpleMath::Vector4(-size, size, 0.0f, 1.0f);
			vertices[1].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[1].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[1].color = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f);
			
			vertices[2].position = DirectX::SimpleMath::Vector4(size, size, 0.0f, 1.0f);
			vertices[2].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[2].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[2].color = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f);

			vertices[3].position = DirectX::SimpleMath::Vector4(size, -size, 0.0f, 1.0f);
			vertices[3].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[3].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[3].color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f);

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		Geometry QuadFieldStrip(float width, float height)
		{// Line Strip
			// separate_x * separate_z = all quad mesh num
			unsigned int separate_x = static_cast<int>(width) / 5 * 2;
			unsigned int separate_z = static_cast<int>(height) / 5 * 2;

			// position of first vertex
			float first_x = -width / 2;
			float first_z = height / 2;

			// one mesh width and height
			float padding_x = width / separate_x;
			float padding_z = height / separate_z;

			std::vector<VertexBuffer3D> vertices;

			unsigned int x_vertex_num = separate_x + 1;
			unsigned int z_vertex_num = separate_z + 1;

			std::vector<unsigned> indices;

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		Geometry QuadFieldList(float width, float height)
		{// Line List
			// separate_x * separate_z = all quad mesh num
			unsigned int separate_x = static_cast<int>(width) / 5 * 2;
			unsigned int separate_z = static_cast<int>(height) / 5 * 2;

			//	  0	+-----+ 1	2, 0, 1
			//		|	  |		1, 3, 2
			//		|	  |		
			//	  2 +-----+ 3

			//	  0	+-----+ 1 ---+ 2	3, 0, 1
			//		|	  |		 |		1, 4, 3
			//		|	  |		 |		4, 1, 2
			//	  3 +-----+ 4 ---+ 5	2, 5, 4

			// position of first vertex
			float first_x = -width / 2;
			float first_z = height / 2;

			// one mesh width and height
			float padding_x = width / separate_x;
			float padding_z = height / separate_z;

			std::vector<VertexBuffer3D> vertices;

			unsigned int x_vertex_num = separate_x + 1;
			unsigned int z_vertex_num = separate_z + 1;

			for (unsigned int z = 0; z < z_vertex_num; ++z)
			{
				for (unsigned int x = 0; x < x_vertex_num; x += 2)
				{
					if (z % 2 == 0)
					{
						VertexBuffer3D vertex;
						vertex.position = DirectX::SimpleMath::Vector4(first_x + padding_x * x, 0.0f, first_z - padding_z * z, 1.0f);
						vertex.normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
						vertex.uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
						vertex.color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f);

						vertices.emplace_back(vertex);

						if (x < x_vertex_num - 1)
						{
							vertex.position = DirectX::SimpleMath::Vector4(first_x + padding_x * (x + 1), 0.0f, first_z - padding_z * z, 1.0f);
							vertex.normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
							vertex.uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
							vertex.color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);

							vertices.emplace_back(vertex);
						}
					}
					else
					{
						VertexBuffer3D vertex;

						vertex.position = DirectX::SimpleMath::Vector4(first_x + padding_x * x, 0.0f, first_z - padding_z * z, 1.0f);
						vertex.normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
						vertex.uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
						vertex.color = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f);

						vertices.emplace_back(vertex);

						if (x < x_vertex_num - 1)
						{
							vertex.position = DirectX::SimpleMath::Vector4(first_x + padding_x * (x + 1), 0.0f, first_z - padding_z * z, 1.0f);
							vertex.normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
							vertex.uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
							vertex.color = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f);

							vertices.emplace_back(vertex);
						}
					}
				}
			}

			std::vector<unsigned> indices;

			for (unsigned int z = 0; z < separate_z; ++z)
			{
				for (unsigned int i = 0; i < separate_x * 6; i += 6)
				{
					unsigned int index = x_vertex_num + x_vertex_num * z + i / 6;
					// left bottom
					indices.emplace_back(index);

					index = x_vertex_num * z + i / 6;
					// left up
					indices.emplace_back(index);

					index = x_vertex_num * z + i / 6 + 1;
					// right up
					indices.emplace_back(index);

					// right up
					indices.emplace_back(index);

					index = x_vertex_num + x_vertex_num * z + i / 6 + 1;
					// right bottom
					indices.emplace_back(index);

					index = x_vertex_num + x_vertex_num * z + i / 6;
					// left bottom
					indices.emplace_back(index);
				}
			}

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		Geometry Cube(void)
		{
			std::vector<VertexBuffer3D> vertices(24);
			const std::vector<unsigned> indices =
			{
				0, 1, 2, 0, 2, 3,		// Top
				4, 5, 6, 4, 6, 7,		// back
				8, 9, 10, 8, 10, 11,	// Right
				12, 13, 14, 12, 14, 15, // Back
				16, 17, 18, 16, 18, 19, // Left
				20, 22, 21, 20, 23, 22, // Bottom
			};

			for (int i = 0; i < 24; ++i)
			{
				vertices[i].color = DirectX::SimpleMath::Vector4(Utils::RandomRange<float>(0.0f, 0.5f), Utils::RandomRange<float>(0.0f, 0.5f), Utils::RandomRange<float>(0.0f, 0.5f), 1.0f);
			}

			vertices[0].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[1].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[2].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[3].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);

			vertices[4].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[5].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[6].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[7].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);

			vertices[8].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[9].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[10].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[11].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);

			vertices[12].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[13].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[14].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[15].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);

			vertices[16].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[17].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[18].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
			vertices[19].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);

			vertices[20].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
			vertices[21].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			vertices[22].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
			vertices[23].uv = DirectX::SimpleMath::Vector2(1.0f, 1.0f);

			// TOP
			vertices[0].position = DirectX::SimpleMath::Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
			vertices[0].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
			vertices[0].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[1].position = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			vertices[1].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
			vertices[1].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[2].position = DirectX::SimpleMath::Vector4(1.0f, 1.0f, -1.0f, 1.0f);
			vertices[2].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
			vertices[2].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[3].position = DirectX::SimpleMath::Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
			vertices[3].normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
			vertices[3].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			// FRONT
			vertices[4].position = DirectX::SimpleMath::Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
			vertices[4].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[4].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[5].position = DirectX::SimpleMath::Vector4(1.0f, 1.0f, -1.0f, 1.0f);
			vertices[5].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[5].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[6].position = DirectX::SimpleMath::Vector4(1.0f, -1.0f, -1.0f, 1.0f);
			vertices[6].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[6].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[7].position = DirectX::SimpleMath::Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
			vertices[7].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
			vertices[7].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			// RIGHT
			vertices[8].position = DirectX::SimpleMath::Vector4(1.0f, 1.0f, -1.0f, 1.0f);
			vertices[8].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
			vertices[8].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

			vertices[9].position = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			vertices[9].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
			vertices[9].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

			vertices[10].position = DirectX::SimpleMath::Vector4(1.0f, -1.0f, 1.0f, 1.0f);
			vertices[10].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
			vertices[10].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

			vertices[11].position = DirectX::SimpleMath::Vector4(1.0f, -1.0f, -1.0f, 1.0f);
			vertices[11].normal = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
			vertices[11].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

			// BACK
			vertices[12].position = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			vertices[12].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
			vertices[12].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[13].position = DirectX::SimpleMath::Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
			vertices[13].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
			vertices[13].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[14].position = DirectX::SimpleMath::Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
			vertices[14].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
			vertices[14].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[15].position = DirectX::SimpleMath::Vector4(1.0f, -1.0f, 1.0f, 1.0f);
			vertices[15].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
			vertices[15].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			// LEFT
			vertices[16].position = DirectX::SimpleMath::Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
			vertices[16].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
			vertices[16].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

			vertices[17].position = DirectX::SimpleMath::Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
			vertices[17].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
			vertices[17].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

			vertices[18].position = DirectX::SimpleMath::Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
			vertices[18].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
			vertices[18].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

			vertices[19].position = DirectX::SimpleMath::Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
			vertices[19].normal = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
			vertices[19].tangent = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);


			// BOTTOM
			vertices[20].position = DirectX::SimpleMath::Vector4(1.0f, -1.0f, -1.0f, 1.0f);
			vertices[20].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
			vertices[20].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[21].position = DirectX::SimpleMath::Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
			vertices[21].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
			vertices[21].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[22].position = DirectX::SimpleMath::Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
			vertices[22].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
			vertices[22].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			vertices[23].position = DirectX::SimpleMath::Vector4(1.0f, -1.0f, 1.0f, 1.0f);
			vertices[23].normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
			vertices[23].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		Geometry Sphere(float radius, unsigned ring_count, unsigned slice_count)
		{
			std::vector<VertexBuffer3D> vertices;
			float d_phi = DirectX::XM_PI / (ring_count - 1);
			for (float phi = -DirectX::XM_PIDIV2; phi <= DirectX::XM_PIDIV2 + 0.00001f; phi += d_phi)
			{
				float y = radius * sinf(phi);	// center height
				float r = radius * cosf(phi);

				float d_theta = 2.0f * DirectX::XM_PI / slice_count;
				for (unsigned j = 0; j <= slice_count; ++j)
				{
					VertexBuffer3D vertex;
					float theta = j * d_theta;
					float x = r * cosf(theta);
					float z = r * sinf(theta);
					vertex.position = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);

					float u = (float)j / slice_count;
					float v = (y + radius) / (2 * radius);
					vertex.uv = DirectX::SimpleMath::Vector2(u, v);

					vertex.tangent = DirectX::SimpleMath::Vector3(-z, 0.0f, x);
					vertex.tangent.Normalize();

					DirectX::SimpleMath::Vector3 N = DirectX::SimpleMath::Vector3(0, 1, 0);
					DirectX::SimpleMath::Vector4 ROT = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, DirectX::XM_PI - theta, DirectX::XM_PIDIV2 - phi);
					N = DirectX::SimpleMath::Vector3::Transform(N, ROT);

					vertex.normal = N;
					vertex.color = DirectX::SimpleMath::Vector4(Utils::RandomRange<float>(0.0f, 0.5f), Utils::RandomRange<float>(0.0f, 0.5f), Utils::RandomRange<float>(0.0f, 0.5f), 1.0f);
					vertices.emplace_back(vertex);
				}
			}

			std::vector<unsigned> indices;
			unsigned ring_vertex_count = slice_count + 1;

			for (unsigned i = 0; i < ring_count; ++i)
			{
				for (unsigned j = 0; j < slice_count; ++j)
				{
					indices.emplace_back(i * ring_vertex_count + j);
					indices.emplace_back((i + 1) * ring_vertex_count + j);
					indices.emplace_back((i + 1) * ring_vertex_count + j + 1);
					indices.emplace_back(i * ring_vertex_count + j);
					indices.emplace_back((i + 1) * ring_vertex_count + j + 1);
					indices.emplace_back(i * ring_vertex_count + j + 1);
				}
			}

			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		Geometry Grid(float width, float depth, unsigned m, unsigned n)
		{
			unsigned numQuads = (m - 1) * (n - 1);
			unsigned faceCount = numQuads * 2; // 2 faces per quad = triangle count
			unsigned vertCount = m * n;
			float dx = width / (n - 1);
			float dz = depth / (m - 1);	// m & n mixed up??

										// offsets for centering the grid : V(0,0) = (-halfWidth, halfDepth)
			float halfDepth = depth / 2;
			float halfWidth = width / 2;

			// texture coord increments
			float du = 1.0f / (n - 1);
			float dv = 1.0f / (m - 1);

			std::vector<VertexBuffer3D> vertices(vertCount);
			std::vector<unsigned> indices(faceCount * 3);

			// position the vertices
			for (unsigned i = 0; i < m; ++i)
			{
				float z = halfDepth - i * dz;
				for (unsigned j = 0; j < n; ++j)
				{
					float x = -halfWidth + j * dx;
					float u = j * du;
					float v = i * dv;
					vertices[i*n + j].position = DirectX::SimpleMath::Vector4(x, 0.0f, z, 1.0f);
					vertices[i*n + j].normal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
					vertices[i*n + j].uv = DirectX::SimpleMath::Vector2(u, v);
					vertices[i*n + j].tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
					vertices[i*n + j].color = DirectX::SimpleMath::Vector4(1 ,1 ,1 ,1);
				}
			}

			unsigned k = 0;
			for (unsigned i = 0; i < m - 1; ++i)
			{
				for (unsigned j = 0; j < n - 1; ++j)
				{
					indices[k] = i * n + j;
					indices[k + 1] = i * n + j + 1;
					indices[k + 2] = (i + 1)*n + j;
					indices[k + 3] = (i + 1)*n + j;
					indices[k + 4] = i * n + j + 1;
					indices[k + 5] = (i + 1)*n + j + 1;
					k += 6;
				}
			}

			// apply height function
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				DirectX::SimpleMath::Vector4& pos = vertices[i].position;
				pos.y = 0.2f * (pos.z * sinf(20.0f * pos.x) + pos.x * cosf(10.0f * pos.z));
			}

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}

		Geometry Cylinder(float height, float top_radius, float bottom_radius, unsigned slice_count, unsigned stack_count)
		{
			float stack_height = height / stack_count;
			float radius_step = (top_radius - bottom_radius) / stack_count;
			unsigned ring_count = stack_count + 1;

			// CYLINDER BODY
			//-----------------------------------------------------------
			// Compute vertices for each stack ring starting at the bottom and moving up.
			std::vector<VertexBuffer3D> vertices;
			for (unsigned i = 0; i < ring_count; ++i)
			{
				float y = -0.5f*height + i * stack_height;
				float r = bottom_radius + i * radius_step;

				// vertices of ring
				float d_theta = 2.0f * DirectX::XM_PI / slice_count;
				for (unsigned j = 0; j <= slice_count; ++j)
				{
					VertexBuffer3D vertex;
					float c = cosf(j*d_theta);
					float s = sinf(j*d_theta);
					vertex.position = DirectX::SimpleMath::Vector4(r*c, y, r*s, 0);
					{
						float u = (float)j / slice_count;
						float v = 1.0f - (float)i / stack_count;
						vertex.uv = DirectX::SimpleMath::Vector2(u, v);
					}

					// TangentU us unit length.
					vertex.tangent = DirectX::SimpleMath::Vector3(-s, 0.0f, c);
					float dr = bottom_radius - top_radius;
					DirectX::SimpleMath::Vector3 bitangent(dr*c, -height, dr*s);
					DirectX::SimpleMath::Vector3 T = vertex.tangent;
					DirectX::SimpleMath::Vector3 B = bitangent;
					DirectX::SimpleMath::Vector3 N = DirectX::XMVector3Normalize(XMVector3Cross(T, B));
					vertex.normal = N;
					vertices.push_back(vertex);
				}
			}

			std::vector<unsigned> indices;

			// Add one because we duplicate the first and last vertex per ring since the texture coordinates are different.
			unsigned ringVertexCount = slice_count + 1;

			// Compute indices for each stack.
			for (unsigned i = 0; i < stack_count; ++i)
			{
				for (unsigned j = 0; j < slice_count; ++j)
				{
					indices.push_back(i*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j + 1);
					indices.push_back(i*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j + 1);
					indices.push_back(i*ringVertexCount + j + 1);
				}
			}

			// CYLINDER TOP
			//-----------------------------------------------------------
			{
				unsigned base_index = (unsigned)vertices.size();
				float y = 0.5f*height;
				float d_theta = 2.0f*DirectX::XM_PI / slice_count;

				// Duplicate cap ring vertices because the texture coordinates and normals differ.
				for (unsigned i = 0; i <= slice_count; ++i)
				{
					float x = top_radius * cosf(i*d_theta);
					float z = top_radius * sinf(i*d_theta);

					// Scale down by the height to try and make top cap texture coord area proportional to base.
					float u = x / height + 0.5f;
					float v = z / height + 0.5f;

					VertexBuffer3D vert;
					vert.position = DirectX::SimpleMath::Vector4(x, y, z, 0);
					vert.normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
					vert.tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);	// ?
					vert.uv = DirectX::SimpleMath::Vector2(u, v);
					vertices.push_back(vert);
				}

				// Cap center vertex.
				VertexBuffer3D cap_center;
				cap_center.position = DirectX::SimpleMath::Vector4(0.0f, y, 0.0f, 0.0f);
				cap_center.normal = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				cap_center.tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
				cap_center.uv = DirectX::SimpleMath::Vector2(0.5f, 0.5f);
				vertices.push_back(cap_center);

				// Index of center vertex.
				unsigned center_index = (unsigned)vertices.size() - 1;
				for (unsigned i = 0; i < slice_count; ++i)
				{
					indices.push_back(center_index);
					indices.push_back(base_index + i + 1);
					indices.push_back(base_index + i);
				}
			}


			// CYLINDER BOTTOM
			//-----------------------------------------------------------
			{
				unsigned base_index = (unsigned)vertices.size();
				float y = -0.5f*height;
				float d_theta = 2.0f*DirectX::XM_PI / slice_count;

				// Duplicate cap ring vertices because the texture coordinates and normals differ.
				for (unsigned i = 0; i <= slice_count; ++i)
				{
					float x = bottom_radius * cosf(i*d_theta);
					float z = bottom_radius * sinf(i*d_theta);

					// Scale down by the height to try and make top cap texture coord area proportional to base.
					float u = x / height + 0.5f;
					float v = z / height + 0.5f;

					VertexBuffer3D vert;
					vert.position = DirectX::SimpleMath::Vector4(x, y, z, 0);
					vert.normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
					vert.tangent = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);	// ?
					vert.uv = DirectX::SimpleMath::Vector2(u, v);
					vertices.push_back(vert);
				}
				// Cap center vertex.
				VertexBuffer3D cap_center;
				cap_center.position = DirectX::SimpleMath::Vector4(0.0f, y, 0.0f, 0.0f);
				cap_center.normal = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
				cap_center.tangent = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
				cap_center.uv = DirectX::SimpleMath::Vector2(0.5f, 0.5f);
				vertices.push_back(cap_center);

				// Index of center vertex.
				unsigned center_index = (unsigned)vertices.size() - 1;
				for (unsigned i = 0; i < slice_count; ++i)
				{
					indices.push_back(center_index);
					indices.push_back(base_index + i);
					indices.push_back(base_index + i + 1);
				}
			}

			for (auto& vertex : vertices)
			{
				vertex.color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			}

			CalculateTangentsAndBitangents(vertices, indices);
			return Geometry(vertices, indices, TopologyType::TRIANGLE_LIST);
		}
	}
}