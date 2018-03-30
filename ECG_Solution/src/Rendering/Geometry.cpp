#include "Geometry.h"

Geometry::Geometry()
{
}

Geometry::Geometry(glm::mat4 modelMatrix, const char * path, std::shared_ptr<Material> material) : _material(material), _modelMatrix(modelMatrix)
{
	GeometryData data;
	std::vector<glm::vec3> out_vertices;
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;
	std::vector<unsigned int> out_indices;

	bool success = ObjectLoader::loadOBJ(path, out_vertices, out_uvs, out_normals, out_indices);
	data.positions = out_vertices;
	data.indices = out_indices;
	_elements = data.indices.size();

	// create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// create positions VBO
	glGenBuffers(1, &_vboPositions);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
	glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);

	// bind positions to location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// create and bind indices VBO
	glGenBuffers(1, &_vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::Geometry(glm::mat4 modelMatrix, GeometryData& data, std::shared_ptr<Material> material)
	: _elements(data.indices.size()), _material(material), _modelMatrix(modelMatrix)
{
	// create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// create positions VBO
	glGenBuffers(1, &_vboPositions);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
	glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);

	// bind positions to location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// create and bind indices VBO
	glGenBuffers(1, &_vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::~Geometry()
{
	glDeleteBuffers(1, &_vboPositions);
	glDeleteBuffers(1, &_vboIndices);
	glDeleteVertexArrays(1, &_vao);
}

void Geometry::draw()
{
	glm::mat4 accumModel = _transformMatrix * _modelMatrix;
	_Shader* shader = _material->getShader();
	shader->use();

	shader->setUniform("modelMatrix", accumModel);
	_material->setUniforms();

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Geometry::transform(glm::mat4 transformation)
{
	_modelMatrix = transformation * _modelMatrix;
}

void Geometry::setTransformMatrix(glm::mat4 transformMatrix)
{
	_transformMatrix = transformMatrix;
}

void Geometry::resetModelMatrix()
{
	_modelMatrix = glm::mat4(1);
}

GeometryData Geometry::createTestObject(float width, float height, float depth)
{
	GeometryData data;

	data.positions = {
		// front
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f,  height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f,  height / 2.0f,  depth / 2.0f),
		// back
		glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f, -depth / 2.0f),
		glm::vec3(width / 2.0f,  height / 2.0f, -depth / 2.0f),
		glm::vec3(-width / 2.0f,  height / 2.0f, -depth / 2.0f)
	};

	data.indices = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3
	};

	return std::move(data);
}
