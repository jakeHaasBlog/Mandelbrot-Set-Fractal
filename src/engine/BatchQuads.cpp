#include "engine/BatchQuads.h"

#include "engine/Window.h"
#include "engine/ViewportManager.h"

Shader* BatchQuads::defaultShader = nullptr;

BatchQuads::BatchQuads()
{
}

BatchQuads::BatchQuads(int numOfQuads) :
	vertices(numOfQuads)
{
}

BatchQuads::~BatchQuads()
{
	if (vb)
		delete vb;

	if (ib)
		delete ib;

	if (va)
		delete va;
}

void BatchQuads::addQuad(float x, float y, float width, float height, float r, float g, float b, float a, int _texture)
{
	float texture = (float)_texture;
	if (texture < 0) texture = -1;

	// bl, tl, tr, br
	vertices.push_back({});

	(*(vertices.end() - 1)).x1 = x;
	(*(vertices.end() - 1)).x2 = x;
	(*(vertices.end() - 1)).x3 = x + width;
	(*(vertices.end() - 1)).x4 = x + width;

	(*(vertices.end() - 1)).y1 = y;
	(*(vertices.end() - 1)).y2 = y + height;
	(*(vertices.end() - 1)).y3 = y + height;
	(*(vertices.end() - 1)).y4 = y;

	(*(vertices.end() - 1)).r1 = r;
	(*(vertices.end() - 1)).r2 = r;
	(*(vertices.end() - 1)).r3 = r;
	(*(vertices.end() - 1)).r4 = r;

	(*(vertices.end() - 1)).g1 = g;
	(*(vertices.end() - 1)).g2 = g;
	(*(vertices.end() - 1)).g3 = g;
	(*(vertices.end() - 1)).g4 = g;

	(*(vertices.end() - 1)).b1 = b;
	(*(vertices.end() - 1)).b2 = b;
	(*(vertices.end() - 1)).b3 = b;
	(*(vertices.end() - 1)).b4 = b;

	(*(vertices.end() - 1)).a1 = a;
	(*(vertices.end() - 1)).a2 = a;
	(*(vertices.end() - 1)).a3 = a;
	(*(vertices.end() - 1)).a4 = a;

	(*(vertices.end() - 1)).texture1 = texture;
	(*(vertices.end() - 1)).texture2 = texture;
	(*(vertices.end() - 1)).texture3 = texture;
	(*(vertices.end() - 1)).texture4 = texture;

	setQuadTextureSampleBounds(vertices.size() - 1, 0.0f, 0.0f, 1.0f, 1.0f);

	buffersReady = false;
}

void BatchQuads::removeQuad(int i)
{
	vertices.erase(vertices.begin() + i);
	buffersReady = false;
}

BatchQuad& BatchQuads::getQuad(int i)
{
	buffersReady = false;
	return vertices[i];
}

void BatchQuads::clear()
{
	vertices.clear();
	buffersReady = false;
}

void BatchQuads::setQuad(int i, float x, float y, float width, float height, float r, float g, float b, float a)
{
	vertices[i].x1 = x;
	vertices[i].x2 = x;
	vertices[i].x3 = x + width;
	vertices[i].x4 = x + width;

	vertices[i].y1 = y;
	vertices[i].y2 = y + height;
	vertices[i].y3 = y + height;
	vertices[i].y4 = y;

	vertices[i].r1 = r;
	vertices[i].r2 = r;
	vertices[i].r3 = r;
	vertices[i].r4 = r;

	vertices[i].g1 = g;
	vertices[i].g2 = g;
	vertices[i].g3 = g;
	vertices[i].g4 = g;

	vertices[i].b1 = b;
	vertices[i].b2 = b;
	vertices[i].b3 = b;
	vertices[i].b4 = b;

	vertices[i].a1 = a;
	vertices[i].a2 = a;
	vertices[i].a3 = a;
	vertices[i].a4 = a;

	buffersReady = false;
}

void BatchQuads::setQuad(int i, float x, float y, float width, float height, int texture)
{
	// bl, tl, tr, br

	vertices[i].x1 = x;
	vertices[i].x2 = x;
	vertices[i].x3 = x + width;
	vertices[i].x4 = x + width;

	if (texture < 0) texture = -1;
	vertices[i].texture1 = texture;
	vertices[i].texture2 = texture;
	vertices[i].texture3 = texture;
	vertices[i].texture4 = texture;

	buffersReady = false;
}

void BatchQuads::setQuadPos(int i, float x, float y, float width, float height)
{
	vertices[i].x1 = x;
	vertices[i].x2 = x;
	vertices[i].x3 = x + width;
	vertices[i].x4 = x + width;

	buffersReady = false;
}

void BatchQuads::setQuadColor(int i, float r, float g, float b, float a)
{
	vertices[i].r1 = r;
	vertices[i].r2 = r;
	vertices[i].r3 = r;
	vertices[i].r4 = r;

	vertices[i].g1 = g;
	vertices[i].g2 = g;
	vertices[i].g3 = g;
	vertices[i].g4 = g;

	vertices[i].b1 = b;
	vertices[i].b2 = b;
	vertices[i].b3 = b;
	vertices[i].b4 = b;

	vertices[i].a1 = a;
	vertices[i].a2 = a;
	vertices[i].a3 = a;
	vertices[i].a4 = a;

	buffersReady = false;
}

void BatchQuads::setTopLeftQuadColor(int i, float r, float g, float b, float a)
{
	vertices[i].r2 = r;
	vertices[i].g2 = g;
	vertices[i].b2 = b;
	vertices[i].a2 = a;

	buffersReady = false;
}

void BatchQuads::setTopRightQuadColor(int i, float r, float g, float b, float a)
{
	vertices[i].r3 = r;
	vertices[i].g3 = g;
	vertices[i].b3 = b;
	vertices[i].a3 = a;

	buffersReady = false;
}

void BatchQuads::setBottomLeftQuadColor(int i, float r, float g, float b, float a)
{
	vertices[i].r1 = r;
	vertices[i].g1 = g;
	vertices[i].b1 = b;
	vertices[i].a1 = a;

	buffersReady = false;
}

void BatchQuads::setBottomRightQuadColor(int i, float r, float g, float b, float a)
{
	vertices[i].r4 = r;
	vertices[i].g4 = g;
	vertices[i].b4 = b;
	vertices[i].a4 = a;

	buffersReady = false;
}

void BatchQuads::setQuadTexture(int i, int textureSlot)
{
	if (textureSlot < 0) textureSlot = -1;
	vertices[i].texture1 = textureSlot;
	vertices[i].texture2 = textureSlot;
	vertices[i].texture3 = textureSlot;
	vertices[i].texture4 = textureSlot;
}

void BatchQuads::setQuadTextureSampleBounds(int i, int pixX, int pixY, int pixWidth, int pixHeight)
{
	Texture* setTex = boundTextures[vertices[i].texture1]; // if you get an exception on this line it is because the texture slot being indexed by this quad is not set
	float screenCoordsX = (float)pixX / setTex->getWidth();
	float screenCoordsY = (float)pixY / setTex->getHeight();
	float normWidth = (float)pixWidth / setTex->getWidth();
	float normHeight = (float)pixHeight / setTex->getHeight();

	setQuadTextureSampleBounds(i, screenCoordsX, screenCoordsY, normWidth, normHeight);
	buffersReady = false;
}

void BatchQuads::setQuadTextureSampleBounds(int i, float screenCoordX, float screenCoordY, float width, float height)
{
	// bl, tl, tr, br
	vertices[i].texX1 = screenCoordX;
	vertices[i].texY1 = screenCoordY;

	vertices[i].texX2 = screenCoordX;
	vertices[i].texY2 = screenCoordY + height;

	vertices[i].texX3 = screenCoordX + width;
	vertices[i].texY3 = screenCoordY + height;

	vertices[i].texX4 = screenCoordX + width;
	vertices[i].texY4 = screenCoordY;

	buffersReady = false;
}

void BatchQuads::setTextureSlot(int slot, Texture* tex)
{
	boundTextures[slot] = tex;
}

void BatchQuads::useShader(Shader& shader)
{
	quadShader = &shader;
}

void BatchQuads::renderAll(float scale, std::array<float, 2> translation)
{
	if (vertices.size() > 0) {
		if (!buffersReady) {
			updateBuffers();
			buffersReady = true;
		}

		bindTextures();

		quadShader->setUniform1f("u_aspectRatio", ViewportManager::getCurrentAspectRatio());
		quadShader->setUniform1f("u_stretch", scale);
		quadShader->setUniform2f("u_translation", translation[0], translation[1]);
		quadShader->bind();
		va->bind();
		glDrawElements(GL_TRIANGLES, ib->getCount(), GL_UNSIGNED_INT, nullptr);
		quadShader->unbind();
		va->unbind();
	}
}

int BatchQuads::size()
{
	return vertices.size();
}

void BatchQuads::updateBuffers()
{
	if (!defaultShader) {
		std::string vertexShader =
			"#version 330 core\n"
			"\n"
			"layout(location = 0) in vec2 position;\n"
			"layout(location = 1) in vec2 uvCoord;\n"
			"layout(location = 2) in vec4 color;\n"
			"layout(location = 3) in float tex\n;"
			"\n"
			"out vec2 v_uvCoord;\n"
			"out vec4 v_color;\n"
			"out float v_tex;\n"
			"\n"
			"uniform float u_aspectRatio;\n"
			"uniform float u_stretch;\n"
			"uniform vec2 u_translation;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4((position[0] * u_stretch + u_translation[0]) / u_aspectRatio, position[1]  * u_stretch + u_translation[1], 0, 1);\n"
			"	gl_Position[2] = 0;\n"
			"   v_uvCoord = uvCoord;\n"
			"   v_color = color;\n"
			"   v_tex = tex;\n"
			"};\n";

		std::string fragmentShader =
			"#version 330 core\n"
			"\n"
			"layout(location = 0) out vec4 color;\n"
			"\n"
			"in vec2 v_uvCoord;\n"
			"in vec4 v_color;\n"
			"in float v_tex;\n"
			"\n"
			"uniform sampler2D u_textures[15];\n"
			"\n"
			"void main()\n"
			"{\n"
			"	int t = int(v_tex);\n"
			"	color = v_color;\n"
			"   if (t != -1)\n"
			"      color = texture(u_textures[t], v_uvCoord); \n"
			"};\n";

		defaultShader = new Shader(vertexShader, fragmentShader);
	}

	if (!quadShader) {
		quadShader = defaultShader;
	}

	std::vector<unsigned int> indices(vertices.size() * 6);
	for (int i = 0; i < vertices.size(); i++) {
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;

		indices[i * 6 + 3] = i * 4 + 0;
		indices[i * 6 + 4] = i * 4 + 2;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	if (buffersInitialized) {
		vb->bufferData(&vertices[0], vertices.size() * sizeof(BatchQuad));
		ib->bufferData(&indices[0], indices.size() * sizeof(unsigned int));
	}
	else {
		ib = new IndexBuffer();
		ib->bufferData(&indices[0], indices.size() * sizeof(unsigned int));

		vb = new VertexBuffer();
		vb->bufferData(&vertices[0], vertices.size() * sizeof(BatchQuad));

		va = new VertexArray();
		va->setAttributes("ff ff ffff f", *vb, *ib);

		buffersInitialized = true;
	}
}

void BatchQuads::bindTextures()
{
	for (int i = 0; i < boundTextures.size(); i++) {
		if (boundTextures[i]) {
			boundTextures[i]->bind(i);
		}
		else {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		quadShader->setUniform1i("u_textures[" + std::to_string(i) + "]", i);
	}

}