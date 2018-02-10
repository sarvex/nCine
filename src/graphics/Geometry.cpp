#include "Geometry.h"
#include "RenderResources.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Geometry::Geometry()
	: vboSharingType_(SharingType::SHARED),
	  primitiveType_(GL_TRIANGLES), firstVertex_(0), numVertices_(0),
	  vbo_(nullptr), ibo_(nullptr)
{

}

Geometry::~Geometry()
{
	if (vboSharingType_ == SharingType::UNIQUE)
		delete vbo_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::setDrawParameters(GLenum primitiveType, GLint firstVertex, GLsizei numVertices)
{
	primitiveType_ = primitiveType;
	firstVertex_ = firstVertex;
	numVertices_ = numVertices;
}

void Geometry::createCustomVbo(unsigned int numFloats, GLenum usage)
{
	if (vboSharingType_ != SharingType::UNIQUE)
	{
		// VBO is now a custom one
		vboSharingType_ = SharingType::UNIQUE;
	}
	else
	{
		// There is already a custom VBO, it needs to be deleted
		delete vbo_;
	}

	vbo_ = new GLBufferObject(GL_ARRAY_BUFFER);
	vbo_->bufferData(numFloats * sizeof(GLfloat), nullptr, usage);
}

/*! \note If the buffer is shared across the application it will not be modified. */
void Geometry::updateVboData(unsigned int floatOffset, unsigned int floatSize, const GLfloat *data)
{
	// Common resources cannot be altered
	if (vboSharingType_ != SharingType::COMMON_RESOURCE)
		vbo_->bufferSubData(floatOffset * sizeof(GLfloat), floatSize * sizeof(GLfloat), data);
}

void Geometry::shareVbo(const Geometry &geometry)
{
	if (vboSharingType_ == SharingType::UNIQUE)
		delete vbo_;

	vboSharingType_ = SharingType::SHARED;
	vbo_ = geometry.vbo_;
}

void Geometry::makeSharedQuad()
{
	if (vboSharingType_ == SharingType::UNIQUE)
		delete vbo_;

	vboSharingType_ = SharingType::COMMON_RESOURCE;
	vbo_ = const_cast<GLBufferObject *>(RenderResources::quadVbo());
	setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Geometry::bind()
{
	if (ibo_)
		ibo_->bind();

	if (vbo_)
		vbo_->bind();
}

}
