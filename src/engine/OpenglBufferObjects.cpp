#include "engine/OpenglBufferObjects.h"

VertexBuffer::VertexBuffer() {
}

VertexBuffer::~VertexBuffer()
{
	freeMemory();
}

VertexBuffer::VertexBuffer(GLuint existingBuffer) {
	id = existingBuffer;
}

VertexBuffer::VertexBuffer(void * data, size_t dataSizeInBytes) : VertexBuffer() {
	bufferData(data, dataSizeInBytes);
}

void VertexBuffer::bufferData(void * data, size_t dataSizeInBytes) {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}

	bind();
	glBufferData(GL_ARRAY_BUFFER, dataSizeInBytes, data, GL_STATIC_DRAW);
	unbind();
}

void VertexBuffer::bufferSubData(void * data, size_t dataSizeInBytes) {
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSizeInBytes, data);
	unbind();
}

void VertexBuffer::bind() {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint VertexBuffer::getID() {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}

	return id;
}

void VertexBuffer::freeMemory() {
	if (isInitialized) {
		glDeleteBuffers(GL_ARRAY_BUFFER, &id);
	}
	unbind();
}




IndexBuffer::IndexBuffer() {
}

IndexBuffer::~IndexBuffer() {
	freeMemory();
}

IndexBuffer::IndexBuffer(GLuint existingBuffer) {
	id = existingBuffer;
	isInitialized = true;
}

IndexBuffer::IndexBuffer(void * data, size_t dataSizeInBytes) : IndexBuffer() {
	bufferData(data, dataSizeInBytes);
	isInitialized = true;
}

void IndexBuffer::bufferData(void * data, size_t dataSizeInBytes) {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}

	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSizeInBytes, data, GL_STATIC_DRAW);
	unbind();

	indexCount = dataSizeInBytes / sizeof(unsigned int);
}

void IndexBuffer::bufferSubData(void * data, size_t dataSizeInBytes) {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}

	bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, dataSizeInBytes, data);
	unbind();

	indexCount = dataSizeInBytes / sizeof(unsigned int);
}

unsigned int IndexBuffer::getCount() {
	return indexCount;
}

void IndexBuffer::bind() {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint IndexBuffer::getID() {
	if (!isInitialized) {
		glGenBuffers(1, &id);
		isInitialized = true;
	}

	return id;
}

void IndexBuffer::freeMemory()
{
	if (isInitialized) {
		glDeleteBuffers(1, &id);
	}

	unbind();
}



VertexArray::VertexArray(const std::string & vertexDataFormat, VertexBuffer & vb, IndexBuffer & ib) {
	glGenVertexArrays(1, &id);
	setAttributes(vertexDataFormat, vb, ib);
}

VertexArray::VertexArray(GLuint exisingBufferID) {
	id = exisingBufferID;
	isInitialized = true;
}

VertexArray::VertexArray() {
}

VertexArray::~VertexArray()
{
	if (isInitialized) {
		freeMemory();
	}
}

void VertexArray::setAttributes(const std::string & vertexDataFormat, VertexBuffer & vb, IndexBuffer & ib) {

	if (!isInitialized) {
		glGenVertexArrays(1, &id);
		isInitialized = true;
	}

	bind();
	vb.bind();
	ib.bind();

	int currentAttributeIndex = 0;
	int totalOffset = 0;
	std::string currentAttribFormat = "";
	int totalStride = 0;

	for (char c : vertexDataFormat) {
		if (c == 'f') totalStride += sizeof(float);
		if (c == 'i') totalStride += sizeof(int);
	}

	for (char c : vertexDataFormat) {
		if (c == ' ') {
			glEnableVertexAttribArray(currentAttributeIndex);

			if (currentAttribFormat.at(0) == 'f') {
				glVertexAttribPointer
				(
					currentAttributeIndex,
					currentAttribFormat.length(),
					GL_FLOAT,
					false,
					totalStride,
					(const void*)(totalOffset)
				);

				totalOffset += currentAttribFormat.length() * sizeof(float);
			}

			else if (currentAttribFormat.at(0) == 'i') {
				glVertexAttribIPointer
				(
					currentAttributeIndex,
					currentAttribFormat.length(),
					GL_INT,
					totalStride,
					(const void*)(totalOffset)
				);

				totalOffset += currentAttribFormat.length() * sizeof(int);
			}

			currentAttributeIndex++;
			currentAttribFormat = "";
		}
		else {
			currentAttribFormat += c;
		}
	}

	glEnableVertexAttribArray(currentAttributeIndex);

	if (currentAttribFormat.at(0) == 'f') {
		glVertexAttribPointer
		(
			currentAttributeIndex,
			currentAttribFormat.length(),
			GL_FLOAT,
			false,
			totalStride,
			(const void*)(totalOffset)
		);

		totalOffset += currentAttribFormat.length() * sizeof(float);
	}
	else if (currentAttribFormat.at(0) == 'i') {
		glVertexAttribIPointer
		(
			currentAttributeIndex,
			currentAttribFormat.length(),
			GL_INT,
			totalStride,
			(const void*)(totalOffset)
		);

		totalOffset += currentAttribFormat.length() * sizeof(int);
	}

	unbind();
	vb.unbind();
	ib.unbind();
}

void VertexArray::bind() {
	if (!isInitialized) {
		glGenVertexArrays(1, &id);
		isInitialized = true;
	}

	glBindVertexArray(id);
}

void VertexArray::unbind() {
	glBindVertexArray(0);
}

GLuint VertexArray::getID() {
	if (!isInitialized) {
		glGenVertexArrays(1, &id);
		isInitialized = true;
	}

	return id;
}

void VertexArray::freeMemory()
{
	if (isInitialized) {
		glDeleteVertexArrays(1, &id);
	}
	
	unbind();
}
