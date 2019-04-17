#pragma once
#include "GraphicsAPI.h"
#include <GL/glew.h>
#include "ShaderGL.h"


/**
* @class API_Opengl
* @brief Specialized GraphicsAPI for Opengl
*
* Contains all graphics specific method used in Opengl
* @author Paolo Ferri
* @version 01
* @date 15/03/2018
*
*
* @bug No known bugs.
*/
class API_Opengl : public GraphicsAPI
{
public:
	/**
	* @brief		Creates the graphics API object
	*
	* @pre			The graphics API object does not exist
	* @post			The graphics API object is created
	*
	*/
	API_Opengl();

	/**
	* @brief		Destroy the graphics API object
	*
	* @pre			The graphics API object must exist
	* @post			The graphics API object is destroyed
	*/
	~API_Opengl();

	/**
	* @brief		Initialize the graphics API object
	*
	* Initialize paramaters specific to this Graphics API
	* @pre			The graphics API object must exist
	* @post			The graphics API object is initialized
	*/
	void Initialize()override;

	/**
	* @brief		Close the graphics API library
	*
	* @pre			The graphics API object must exist
	* @post			The graphics API library is closed
	*/
	void Shutdown()override;

	/**
	* @brief		Clear the color buffer
	*
	* @pre			The graphics API object must exist
	* @post			The colo buffer is cleared with the set color
	*/
	void ClearColorBuffer() override;

	/**
	* @brief		Clear the depth buffer
	*
	* @pre			The graphics API object must exist
	* @post			The depth buffer is cleared 
	*/
	void ClearDepthBuffer() override;

	/**
	* @brief		Set the view port
	*
	* @pre			The graphics API object must exist
	* @post			Theview port is set
	* @param w		The viewport width
	* @param h		The viewport height
	*/
	void SetViewPort(int w, int h) override;

	/**
	* @brief		Create a new shader
	*
	* @pre			The graphics API object must exist
	* @post			A new shader is created
	* @param name	The shader name used when stored
	* @param vertexSource	The vertex shader source code
	* @param fragmentSource	The fragment shader source code
	* @return				The new shader
	*/
	Shader* CreateShader(std::string name, std::string vertexSource, std::string fragmentSource) override;

	/**
	* @brief		Create a new Texture2D
	*
	* @pre					The graphics API object must exist
	* @post					A new Texture2D is created
	* @param textureName	The texture name used when stored
	* @param width			The texture width
	* @param height			The texture height
	* @param channels		The texture channels number
	* @param data			The texture byte data
	* @return				The new Texture2D
	*/
	Texture2D* CreateTexture2D(std::string textureName, int width, int height, int channels, unsigned char* data) override;

	/**
	* @brief				Create a new empty Texture2D
	*
	* @pre					The graphics API object must exist
	* @post					A new Texture2D is created with no data
	* @param textureName	The texture name used when stored
	* @param width			The texture width
	* @param height			The texture height
	* @param isDepth		Whether the texture is a depth buffer texture
	* @return				The newTexture2D
	*/
	Texture2D* CreateTexture2D(std::string name, unsigned width, unsigned height, bool isDepth = false) override;

	/**
	* @brief				Reset the texture units
	*
	* @pre					The graphics API object must exist
	* @post					All texture units are unbinded
	*/
	void ResetTextures() override;

	/**
	* @brief				Create a new vertex buffer
	*
	* @pre					The graphics API object must exist
	* @post					A new vertex buffer is created
	* @return				The new vertex buffer
	*/
	ArrayBuffer<Vertex>* CreateVertexBuffer() override;

	/**
	* @brief				Create a new index buffer
	*
	* @pre					The graphics API object must exist
	* @post					A new index buffer is created
	* @return				The new index buffer
	*/
	ArrayBuffer<unsigned>* CreateIndexBuffer() override;

	/**
	* @brief				Create a new vertex array
	*
	* @pre					The graphics API object must exist
	* @post					A new veretx array is created
	* @return				The new vertex array
	*/
	VertexArray* CreateVertexArray() override;

	/**
	* @brief				Create a new uniform buffer object
	*
	* @pre					The graphics API object must exist
	* @post					A new uniform buffer object is created
	* @param totalSize		The size of the buffer
	* @param binding		The bingind index
	* @return				The new uniform buffer object
	*/
	UniformBuffer* CreateUniformBuffer(unsigned totalSize, unsigned binding) override;

	/**
	* @brief				Create a new frame buffer
	*
	* @pre					The graphics API object must exist
	* @post					A new frame buffer object is created
	* @param w				The width of the buffer texture
	* @param h				The height of the buffer texture
	* @param hasColorAttachment		Whether the frame buffer has a color attachment
	* @return				The new uniform buffer object
	*/
	FrameBuffer* CreateFrameBuffer(int w, int h, bool hasColorAttachment = true) override;

	/**
	* @brief				Enable/disable clipping plane
	*
	* @pre					The graphics API object must exist
	* @post					The clipping plane is enabled/disabled according to argument passed
	* @param cp				The boolean value that will enable/disable plane clipping

	*/
	void SetClipPlaneActive(bool cp) override;

	/**
	* @brief				Create a new Cubemap texture
	*
	* @pre					The graphics API object must exist
	* @post					A new Cubemap is created 
	* @param name			The cubemap name used when stored
	* @param width			The textures width
	* @param height			The textures height
	* @param data			The byte data of the 6 textures
	* @return				The new cubemap
	*/
	virtual CubeMap* CreateCubeMap(std::string name, unsigned char* data[6], int width[6], int height[6]) override;




};