#pragma once
#include "GraphicsAPI.h"
#include <GL/glew.h>
#include "ShaderGL.h"

class API_Opengl : public GraphicsAPI
{
public:
	API_Opengl();
	~API_Opengl();

	void Initialize()override;
	void Shutdown()override;
	void ClearColorBuffer() override;
	void ClearDepthBuffer() override;
	void SetViewPort(int w, int h) override;
	Shader* CreateShader(std::string name, std::string vertexSource, std::string fragmentSource) override;
	Texture2D* CreateTexture2D(std::string textureName, int width, int height, int channels, unsigned char* data) override;
	Texture2D* CreateTexture2D(std::string name, unsigned width, unsigned height, bool isDepth = false) override;
	void ResetTextures() override;
	ArrayBuffer<Vertex>* CreateVertexBuffer() override;
	ArrayBuffer<unsigned>* CreateIndexBuffer() override;
	VertexArray* CreateVertexArray() override;
	UniformBuffer* CreateUniformBuffer(unsigned totalSize, unsigned binding) override;
	FrameBuffer* CreateFrameBuffer(int w, int h, bool hasColorAttachment = true) override;
	void SetClipPlaneActive(bool hasColorAttachment) override;
	virtual CubeMap* CreateCubeMap(std::string name, unsigned char* data[6], int width[6], int height[6]) override;




};