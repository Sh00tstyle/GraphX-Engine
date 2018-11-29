#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

class VertexArray {
	public: 
		VertexArray();
		~VertexArray();

		unsigned int& getID();

		void bind();

	private:
		unsigned int _id;

		void _generate();
};

#endif