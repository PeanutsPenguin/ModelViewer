#include "Resources/Model.h"
#include "fast_float/fast_float.h"
#include <chrono>
#include <fstream>
#include <sstream>

#define USE_FASTFLOAT 0 // Bool to use fast float library or no

#define USE_HARDWARE 0 // Bool to use hardwareCore for threads or no

#define CHUNK_SIZE (1 << 20)

#define MIN_FILE_SIZE ((1 << 20) * 5)

namespace Resources
{
	bool Model::Vertex::operator==(const Vertex& toCompare) const
	{
		if (this->m_position == toCompare.m_position && this->m_normale == toCompare.m_normale &&
			this->m_texturePosition == toCompare.m_texturePosition)
			return true;
		else return false;
	}

	bool Model::Vertex::operator!=(const Vertex& toCompare) const
	{
		return this->m_position != toCompare.m_position || this->m_normale != toCompare.m_normale ||
			   this->m_texturePosition != toCompare.m_texturePosition;
	}

	Model::Buffer::Buffer(std::vector<Model::Vertex>& m_vertexBuffer)
	{
		/// Initialize a VBO
		glGenBuffers(1, &this->m_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
		glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(m_vertexBuffer[0]), &m_vertexBuffer[0], GL_STATIC_DRAW);
	}

	Model::Buffer::Buffer(std::vector<GLuint>& IndiceBuffer)
	{
		/// Initialize a EBO
		glGenBuffers(1, &this->m_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndiceBuffer.size() * sizeof(IndiceBuffer[0]), &IndiceBuffer[0], GL_STATIC_DRAW);
	}

	Model::VertexAttributes::VertexAttributes()
	{
		/// Initialize a VAO
		glGenVertexArrays(1, &this->m_VAO);
		glBindVertexArray(m_VAO);
	}

	void Model::VertexAttributes::SetAttribute(int location, int nbrOfElements, int gapSize, int toFirstSize)
	{
		/// Set the right attributes to the Vertex Array Object
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, nbrOfElements, GL_FLOAT, GL_FALSE, gapSize, (void*)toFirstSize);
	}

	std::vector<char> ReadFile(const char* filename)
	{
		std::vector<char> data = std::vector<char>();

		std::ifstream fstream = std::ifstream(filename); // Open the file
		if (fstream.is_open() && fstream.good())		 // If the file is correctly open
		{
			fstream.seekg(0, std::ios::end);	   // Go tot he end
			std::streampos flen = fstream.tellg(); // Get the length
			fstream.seekg(0, std::ios::beg);	   // Go back to the beginning

			data.resize(flen);				 // Reserve space in our vector
			fstream.read(data.data(), flen); // Put the file in our vector
		}

		fstream.close(); // Close the file
		return data;	 // Return our vector
	}

	LibMath::Vector3 ProcessVLine(std::string_view line)
	{
		LibMath::Vector3 vector;

		if (line.empty()) return vector;

		{
#if USE_FASTFLOAT // fast float library jsut to compare the effiency, use std
			using namespace fast_float;
#else
			using namespace std;
#endif
			int idx = 0;
			for (idx; idx < 3; ++idx)
			{
				if (line.front() == ' ') line.remove_prefix(1);
				if (const char* ptr = static_cast<const char*>(memchr(line.data(), ' ', line.size()))) // Seek the next space
				{
					size_t dst = static_cast<size_t>(ptr - line.data());
					auto   result =
						from_chars(line.data(), line.data() + dst, vector[idx]); // Convert the text befor the ' ' into a float
					if (result.ec != std::errc()) std::cerr << "Failed to parse...\n";

					line.remove_prefix(dst + 1);
				}
				else break;
			}

			auto result = from_chars(line.data(), line.data() + line.size(), vector[idx]); // Convert the last one and stock it
			if (result.ec != std::errc())
			{
				std::cerr << "Failed to parse...\n";
				vector.z = 0;
			}
		}

		return vector;
	}

	void prepareVline(std::string_view line, filedata* dataContainer)
	{
		line.remove_prefix(2);
		while (line.front() == ' ') line.remove_prefix(1);
		dataContainer->m_position.push_back(ProcessVLine(line));
	}

	void prepareVtline(std::string_view line, filedata* dataContainer)
	{
		line.remove_prefix(3);
		while (line.front() == ' ') line.remove_prefix(1);
		dataContainer->m_texPos.push_back(ProcessVLine(line));
	}

	void prepareVnline(std::string_view line, filedata* dataContainer)
	{
		line.remove_prefix(3);
		while (line.front() == ' ') line.remove_prefix(1);
		dataContainer->m_normalPos.push_back(ProcessVLine(line));
	}

	void processFLine(std::string_view line, filedata* dataContainer)
	{
		Face m_indices = std::make_pair((uint32_t)dataContainer->m_indices.size(), LibMath::Vector<3, uint32_t>());
		int	 toInt	   = 0;

		if (line.empty()) return;

		for (int i = 0; i < 3; i++)
		{
			if (const char* secondPtr = static_cast<const char*>(memchr(line.data(), '/', line.size()))) // Seek the next '/'
			{
				size_t secondDst = static_cast<size_t>(secondPtr - line.data());
				if (secondDst > 0)
					std::from_chars(line.data(), line.data() + secondDst, toInt); // Convert the line before '/' in float
				else toInt = 0;
				line.remove_prefix(secondDst + 1);

				if (i == 0) m_indices.second.x = toInt;
				else if (i == 1)
				{
					m_indices.second.y = toInt;

					if (const char* ptr = static_cast<const char*>(memchr(line.data(), ' ', line.size())))
					{
						size_t dst = static_cast<size_t>(ptr - line.data());
						if (dst > 0) std::from_chars(line.data(), line.data() + dst, toInt);
						else toInt = 0;
						line.remove_prefix(dst + 1);
						m_indices.second.z = toInt;
						dataContainer->m_indices.push_back(m_indices);
						i = -1;
					}
					else
					{
						std::from_chars(line.data(), line.data() + line.size(), toInt);
						m_indices.second.z = toInt;
						dataContainer->m_indices.push_back(m_indices);
					}
				}
			}
		}
	}

	void ProcessLine(std::string_view line, filedata* dataContainer)
	{
		if (line.empty()) return;

		switch (line.front()) // Switch case for the first character of the line
		{
			case 'f':
				line.remove_prefix(2);
				processFLine(line, dataContainer); // If it's a f, create a face
				break;
			case 'v':
				if (line.starts_with("v ") || line.starts_with("v\t"))
				{
					prepareVline(line, dataContainer);
				}
				else if (line.starts_with("vt ") || line.starts_with("vt\t"))
				{
					prepareVtline(line, dataContainer);
				}
				else if (line.starts_with("vn ") || line.starts_with("vn\t"))
				{
					prepareVnline(line, dataContainer);
				}
				break;
			default: return;
		}
	}

	void ProcessChunk(const char* data, size_t offset, size_t len, filedata* container, bool is_last)
	{
		const bool beginAfterEol = offset > 0;	  // Boolen to check if we have to begin after the end of the line
		int		   maxLineRange	 = (1 << 10) * 4; // Magic number for the length of the check

		std::string_view text = std::string_view(data + offset, len); // String view that contain our chunk
		std::string_view line = std::string_view();					  // String view that's contain the current line

		if (beginAfterEol)
		{
			// Seek the next \n in the max Range and modify our length
			if (const char* ptr = static_cast<const char*>(memchr(text.data(), '\n', maxLineRange)))
			{
				size_t dst = static_cast<size_t>(ptr - text.data());
				text.remove_prefix(dst + 1);
				len -= dst + 1;
			}
			else return;
		}

		for (size_t bread = 0; bread < len;)
		{
			size_t maxLength = is_last ? text.size() : maxLineRange; // Set the right seeking length

			// Seek the next \n in the max Range and modify our length
			if (const char* ptr = static_cast<const char*>(memchr(text.data(), '\n', maxLength)))
			{
				size_t dst = static_cast<size_t>(ptr - text.data());
				line	   = std::string_view(text.data(), dst);
				text.remove_prefix(dst + 1 > text.size() ? text.size() : dst + 1);

				bread += dst + 1;
				ProcessLine(line, container); // Convert the line into usable data
			}
			else break;
		}
	}

	Model* isExisting(ResourcesManager* manager, const char* filename)
	{
		Model* ptr = nullptr;

		if (manager->getRessource<Resources::IResources>(filename) != nullptr)
		{
			std::cout << "ALREADY EXISTING"
					  << "\n";
			ptr = (Resources::Model*)manager->getRessource<Resources::IResources>(filename);
		}

		return ptr;
	}

	filedata processVector(std::vector<filedata>& vec)
	{
		filedata result = filedata();

		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i].m_position.size() != 0)
				result.m_position.insert(result.m_position.end(), vec[i].m_position.begin(), vec[i].m_position.end());

			if (vec[i].m_texPos.size() != 0)
				result.m_texPos.insert(result.m_texPos.end(), vec[i].m_texPos.begin(), vec[i].m_texPos.end());

			if (vec[i].m_normalPos.size() != 0)
				result.m_normalPos.insert(result.m_normalPos.end(), vec[i].m_normalPos.begin(), vec[i].m_normalPos.end());

			if (vec[i].m_indices.size() != 0)
				result.m_indices.insert(result.m_indices.end(), vec[i].m_indices.begin(), vec[i].m_indices.end());
		}

		return result;
	}

	Model::Vertex createVertex(filedata& dataContainer, int posIdx, int texIdx, int normIdx, int i)
	{
		Model::Vertex result = Model::Vertex();

		if ((posIdx = dataContainer.m_indices[i].second.x - 1) < 0) posIdx += dataContainer.m_position.size();

		if ((texIdx = dataContainer.m_indices[i].second.y - 1) < 0) texIdx += dataContainer.m_texPos.size();

		if ((normIdx = dataContainer.m_indices[i].second.z - 1) < 0) normIdx += dataContainer.m_normalPos.size();

		if (dataContainer.m_position.size() > posIdx) result.m_position = dataContainer.m_position[posIdx];
		if (dataContainer.m_texPos.size() > texIdx) result.m_texturePosition = dataContainer.m_texPos[texIdx];
		if (dataContainer.m_normalPos.size() > normIdx) result.m_normale = dataContainer.m_normalPos[normIdx];

		return result;
	}

	void handleLessThanFiveVertices(std::vector<Model::Vertex>&										vertices,
									std::unordered_map<Model::Vertex, size_t, Model::Vertex::Hash>& verticeMap, Model* model)
	{
		if (vertices.size() == 3)
		{
			model->m_indices.push_back(verticeMap[vertices[0]]);
			model->m_indices.push_back(verticeMap[vertices[1]]);
			model->m_indices.push_back(verticeMap[vertices[2]]);
			return;
		}
		else if (vertices.size() == 4)
		{
			model->m_indices.push_back(verticeMap[vertices[0]]);
			model->m_indices.push_back(verticeMap[vertices[1]]);
			model->m_indices.push_back(verticeMap[vertices[3]]);
			model->m_indices.push_back(verticeMap[vertices[3]]);
			model->m_indices.push_back(verticeMap[vertices[1]]);
			model->m_indices.push_back(verticeMap[vertices[2]]);
		}
	}

	void createVertices(filedata& dataContainer, Model* model)
	{
		std::unordered_map<Model::Vertex, size_t, Model::Vertex::Hash> vertices;
		std::vector<Model::Vertex>									   tempVertexArray;
		Model::Vertex												   curVertex = Model::Vertex();
		model->m_vertexBuffer.reserve(std::max(dataContainer.m_normalPos.size(), dataContainer.m_position.size()));

		int id = dataContainer.m_indices[0].first;

		for (int i = 0; i < dataContainer.m_indices.size();)
		{
			if (i == dataContainer.m_indices.size()) break;

			while (id == dataContainer.m_indices[i].first)
			{
				curVertex = createVertex(dataContainer, dataContainer.m_indices[i].second.x, dataContainer.m_indices[i].second.y,
										 dataContainer.m_indices[i].second.z, i);

				if (!vertices.contains(curVertex))
				{
					// Stock it in our map
					vertices[curVertex] = vertices.size();
					model->m_vertexBuffer.push_back(curVertex);
				}

				tempVertexArray.push_back(curVertex);

				i++;

				if (i == dataContainer.m_indices.size()) break;
			}

			if (tempVertexArray.size() < 5) handleLessThanFiveVertices(tempVertexArray, vertices, model);
			else
			{
				std::vector<Model::Vertex> temp = earClip(tempVertexArray);
				for (int j = 0; j < temp.size(); j++) model->m_indices.push_back(vertices[temp[j]]);
			}
			
			tempVertexArray.clear();
			if (i < dataContainer.m_indices.size()) id = dataContainer.m_indices[i].first;
		}
	}

	void endParse(Model* model)
	{
		/// Create the Vertex Buffer Object
		Model::Buffer tempVBO(model->m_vertexBuffer);

		model->m_vertexBufferObject = tempVBO;

		/// Create the Vertex Array Object
		Model::VertexAttributes tempVAO;

		/// Set all the attributes we have in our Vertices
		tempVAO.SetAttribute(0, 3, 9 * sizeof(float), 0);
		tempVAO.SetAttribute(1, 3, 9 * sizeof(float), 3 * sizeof(float));
		tempVAO.SetAttribute(2, 3, 9 * sizeof(float), 6 * sizeof(float));

		model->m_vertexObject = tempVAO;

		/// Create the Ellement Buffer
		Resources::Model::Buffer tempEBO(model->m_indices);

		model->m_elementBuffer= tempEBO;
	}

	void launchThread(std::vector<filedata>& dataContainer, std::vector<char>& reader, int nbThreads,
					  std::vector<uint32_t>& nbTasks)
	{
		std::vector<std::thread> threads; // Vector of threads
		threads.reserve(nbThreads);

		dataContainer = std::vector<filedata>(nbThreads);

		for (size_t idx = 0; idx < nbThreads; ++idx)
		{
			bool   is_last	  = idx + 1 == nbThreads;
			size_t multiplier = nbTasks[idx + 1] - nbTasks[idx];

			const size_t offset = nbTasks[idx] * CHUNK_SIZE;
			const size_t len	= is_last ? reader.size() - offset : CHUNK_SIZE * multiplier;

			threads.emplace_back(ProcessChunk, reader.data(), offset, len, &dataContainer[idx], is_last);
		}

		for (auto& thread : threads) thread.join();
	}

	void initializeThreads(std::vector<filedata>& dataContainer, std::vector<char>& reader)
	{
		int nbThreads = std::thread::hardware_concurrency() - 1;

		const uint32_t nbBlock =
			reader.size() / CHUNK_SIZE + (reader.size() % CHUNK_SIZE != 0); // Number of block in the file

		if (nbBlock < nbThreads) nbThreads = nbBlock;
		const uint32_t nbBlockPerThread = nbBlock / nbThreads; // Nbr of block per thread
		int32_t		   num_remainder_blocks =
			nbBlock - (nbBlockPerThread * nbThreads); // Nbr of block left because of the division

		std::vector<uint32_t> nbTasks =
			std::vector<uint32_t>({0}); // Vector that'll handle how many block a thread will have
		nbTasks.reserve(nbThreads);

		while (nbTasks.back() < nbBlock)
		{
			uint32_t block = nbTasks.back();
			block += nbBlockPerThread + (num_remainder_blocks-- > 0);

			nbTasks.emplace_back(block);
		}

		const size_t num_task = nbTasks.size();

		launchThread(dataContainer, reader, nbThreads, nbTasks);
	}

	void parseFile(const char* filename, Model* model, bool monoThread, ResourcesManager* manager)
	{
		Model* tempModel = isExisting(manager, filename);

		if (tempModel != nullptr)
		{
			*model = *tempModel;
			return;
		}
		else manager->addResource<Resources::IResources>(model, filename);

		// Chrono to measure how much time it took to parse
		std::chrono::time_point<std::chrono::system_clock> start, end;
		start = std::chrono::system_clock::now();

		/// Open the file and read it
		std::vector<char> reader = ReadFile(filename);
		if (reader.empty()) return;

		if (reader.size() < MIN_FILE_SIZE) monoThread = true;

		filedata dataContainer = filedata();

		if (!monoThread)
		{
			std::vector<filedata> splittedData;

			initializeThreads(splittedData, reader);

			dataContainer = processVector(splittedData);

			createVertices(dataContainer, model);

			endParse(model);
		}
		else
		{
			ProcessChunk(reader.data(), 0, reader.size(), &dataContainer, true);

			createVertices(dataContainer, model);

			endParse(model);
		}

		end											  = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "The parse took : " << elapsed_seconds.count() << "s\n";
	}
} // namespace Resources

    std::vector<Resources::Model::Vertex> handleQuad(std::vector<Resources::Model::Vertex> const& vertices)
{
	std::vector<Resources::Model::Vertex> newFace(6);

	newFace[0] = vertices[0];
	newFace[1] = vertices[1];
	newFace[2] = vertices[3];
	newFace[3] = vertices[3];
	newFace[4] = vertices[1];
	newFace[5] = vertices[2];

	return newFace;
}

std::vector<Resources::Model::Vertex> earClip(std::vector<Resources::Model::Vertex> vertices)
{
	std::vector<LibMath::Vector3> positions(vertices.size());
	for (size_t n = 0; n < vertices.size(); n++)
	{
		positions[n] = vertices[n].m_position;
	}

	std::vector<int> ears;
	std::vector<int> convexes;
	std::vector<int> reflexes;

	for (int i = 0; i < vertices.size(); i++)
	{
		int h = i == 0 ? vertices.size() - 1 : i - 1;
		int j = i == vertices.size() - 1 ? 0 : i + 1;

		if (checkConvex(positions, LibMath::Vector3(h, i, j), convexes, reflexes))
		{
			if (checkEar(positions, LibMath::Vector3(h, i, j)))
			{
				ears.push_back(i);
			}
		}
	}

	return earClipEnd(vertices, positions, ears, convexes, reflexes);
}

std::vector<Resources::Model::Vertex> earClipEnd(std::vector<Resources::Model::Vertex> const& vertices, std::vector<LibMath::Vector3> const& positions,
									  std::vector<int>& ears, std::vector<int>& convexes, std::vector<int>& reflexes)
{
	if (ears.size() < 2)
	{
		return std::vector<Resources::Model::Vertex>();
	}
	std::vector<int>	newFaceIndices = clipping(positions, ears, convexes, reflexes);
	std::vector<Resources::Model::Vertex> newFace(newFaceIndices.size());
	for (int j = 0; j < newFaceIndices.size(); j++)
	{
		newFace[j] = vertices[newFaceIndices[j]];
	}

	return newFace;
}

void assignIndices(std::vector<int>& list, int& minusOne, int& plusOne, int index)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] == index)
		{
			minusOne = index == list[0] ? list.back() : list[i - 1];
			plusOne	 = index == list.back() ? list[0] : list[i + 1];
		}
	}
}

std::vector<int> clipping(std::vector<LibMath::Vector3> const& vertices, std::vector<int>& ears, std::vector<int>& convexes,
								 std::vector<int>& reflexes)
{
	std::vector<int> out;
	std::vector<int> remainingVertices;
	remainingVertices.reserve((vertices.size()));
	for (int n = 0; n < vertices.size(); n++)
	{
		remainingVertices.push_back(n);
	}

	while (remainingVertices.size() > 3 && ears.size() != 0)
	{
		int current = ears[0];
		int prev	= -1;
		int next	= -1;
		assignIndices(remainingVertices, prev, next, current);

		out.insert(out.end(), {prev, current, next});

		auto it = remainingVertices.begin();
		while (it != remainingVertices.end() && (*it != ears[0]))
		{
			it++;
		}
		remainingVertices.erase(it);
		ears.erase(ears.begin());

		clipReCheck(prev, std::make_tuple(std::cref(vertices), std::ref(ears), std::ref(convexes), std::ref(reflexes),
										  std::ref(remainingVertices)));
		clipReCheck(next, std::make_tuple(std::cref(vertices), std::ref(ears), std::ref(convexes), std::ref(reflexes),
										  std::ref(remainingVertices)));
	}

	out.insert(out.end(), {remainingVertices[0], remainingVertices[1], remainingVertices[2]});

	return out;
}

bool checkConvex(std::vector<LibMath::Vector3> const& vertices, LibMath::Vector3 indices, std::vector<int>& convexes,
						std::vector<int>& reflexes)
{
	for (int i = 0; i < convexes.size(); i++)
	{
		if (convexes[i] == indices.y)
		{
			return true;
		}
	}

	LibMath::Vector3 gamma(vertices[indices.x].x - vertices[indices.y].x,
						vertices[indices.x].y - vertices[indices.y].y,
						vertices[indices.x].z - vertices[indices.y].z);
	LibMath::Vector3 delta(vertices[indices.z].x - vertices[indices.y].x,
						vertices[indices.z].y - vertices[indices.y].y,
						vertices[indices.z].z - vertices[indices.y].z);

	/* obj should always be counterclockwise (cf wikipedia) so we don't check the "order" if you want to it would resemble: */
	/* if ((gamma.cross(delta).z < 0 && !counterClockwise) || (gamma.cross(delta).z > 0 && counterClockwise)) */
	if (gamma.angleFrom(delta, false).raw() > LibMath::g_pi)
	{
		for (int i = 0; i < reflexes.size(); i++)
		{
			if (reflexes[i] == indices.y)
			{
				return false;
			}
		}
		reflexes.push_back(indices.y);
		return false;
	}

	convexes.push_back(indices.y);
	return true;
}

bool checkEar(std::vector<LibMath::Vector3> const& vertices, LibMath::Vector3 indices)
{
	int prev	= (int)indices.x;
	int current = (int)indices.y;
	int next	= (int)indices.z;

	for (int k = 0; k < vertices.size(); k++)
	{
		if (k == prev || k == current || k == next)
		{
			continue;
		}

		if (LibMath::CollisionCheck::isInTriangle({vertices[k], vertices[prev], vertices[current]}, vertices[next]))
		{
			return false;
		}
	}

	return true;
}

void clipReCheck(
	int index,
	std::tuple<std::vector<LibMath::Vector3> const&, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&>
		vectors)
{
	std::vector<LibMath::Vector3> const& vertices	= std::get<0>(vectors);
	std::vector<int>&				  ears		= std::get<1>(vectors);
	std::vector<int>&				  convexes	= std::get<2>(vectors);
	std::vector<int>&				  reflexes	= std::get<3>(vectors);
	std::vector<int>&				  remaining = std::get<4>(vectors);

	int prev = -1;
	int next = -1;
	assignIndices(remaining, prev, next, index);

	LibMath::Vector3 check(prev, index, next);
	if (!checkConvex(vertices, check, convexes, reflexes))
	{
		return;
	}

	clipReCheckEar(vertices, ears, check, index);
}

void clipReCheckEar(std::vector<LibMath::Vector3> const& vertices, std::vector<int>& ears, LibMath::Vector3& checkedVertices,
						   int index)
{
	bool treated = false;
	bool isEar	 = checkEar(vertices, checkedVertices);
	for (int idx = 0; idx < ears.size(); idx++)
	{
		if (ears[idx] == index && isEar)
		{
			treated = true;
			break;
		}
		else if (ears[idx] == index && !isEar)
		{
			ears.erase(ears.begin() + idx);
			treated = true;
			break;
		}
	}
	if (isEar && !treated)
	{
		for (auto it = ears.begin(); it != ears.end(); it++)
		{
			if (*it > index)
			{
				ears.insert(it, index);
				break;
			}
		}
	}
}
