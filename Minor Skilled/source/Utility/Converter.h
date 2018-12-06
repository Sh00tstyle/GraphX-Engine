#ifndef CONVERTER_H
#define COVNERTER_H

#include <vector>

class Texture;
class Shader;
class VertexArray;
class Buffer;
class Framebuffer;
class Renderbuffer;

class Converter {
	public:
		static Texture* EquirectangularToCubemap(Texture* input);

	private:
		static const std::vector<float> _CubeVertices;

		static Shader* _ConversionShader;

		static VertexArray* _CubeVAO;
		static Buffer* _CubeVBO;

		static Framebuffer* _ConversionFBO;
		static Renderbuffer* _ConversionRBO;

		static void _InitConversionObjects();

};

#endif