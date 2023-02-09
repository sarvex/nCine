#include <nctl/StaticHashMapIterator.h>
#include "GLShaderProgram.h"
#include "GLShader.h"
#include "GLDebug.h"
#include "RenderResources.h"
#include "RenderVaoPool.h"
#include "BinaryShaderCache.h"
#include "tracy.h"

namespace ncine {

namespace {

	unsigned int bufferSize = 0;
	nctl::UniquePtr<uint8_t[]> bufferPtr;

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLuint GLShaderProgram::boundProgram_ = 0;
char GLShaderProgram::infoLogString_[MaxInfoLogLength];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderProgram::GLShaderProgram()
    : GLShaderProgram(QueryPhase::IMMEDIATE)
{
}

GLShaderProgram::GLShaderProgram(QueryPhase queryPhase)
    : glHandle_(0), attachedShaders_(AttachedShadersInitialSize), hashName_(0),
      status_(Status::NOT_LINKED), queryPhase_(queryPhase), shouldLogOnErrors_(true),
      uniformsSize_(0), uniformBlocksSize_(0), uniforms_(UniformsInitialSize),
      uniformBlocks_(UniformBlocksInitialSize), attributes_(AttributesInitialSize)
{
	glHandle_ = glCreateProgram();

	if (RenderResources::binaryShaderCache().isAvailable())
		glProgramParameteri(glHandle_, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection, QueryPhase queryPhase)
    : GLShaderProgram(queryPhase)
{
	const bool hasCompiledVS = attachShaderFromFile(GL_VERTEX_SHADER, vertexFile);
	const bool hasCompiledFS = attachShaderFromFile(GL_FRAGMENT_SHADER, fragmentFile);

	if (hasCompiledVS && hasCompiledFS)
		link(introspection);
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection)
    : GLShaderProgram(vertexFile, fragmentFile, introspection, QueryPhase::IMMEDIATE)
{
}

GLShaderProgram::GLShaderProgram(const char *vertexFile, const char *fragmentFile)
    : GLShaderProgram(vertexFile, fragmentFile, Introspection::ENABLED, QueryPhase::IMMEDIATE)
{
}

GLShaderProgram::~GLShaderProgram()
{
	if (boundProgram_ == glHandle_)
		glUseProgram(0);

	glDeleteProgram(glHandle_);

	RenderResources::removeCameraUniformData(this);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLShaderProgram::isLinked() const
{
	return (status_ == Status::LINKED ||
	        status_ == Status::LINKED_WITH_DEFERRED_QUERIES ||
	        status_ == Status::LINKED_WITH_INTROSPECTION);
}

unsigned int GLShaderProgram::retrieveInfoLogLength() const
{
	GLint length = 0;
	glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

	return static_cast<unsigned int>(length);
}

void GLShaderProgram::retrieveInfoLog(nctl::String &infoLog) const
{
	GLint length = 0;
	glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

	if (length > 0 && infoLog.capacity() > 0)
	{
		const unsigned int capacity = infoLog.capacity();
		glGetProgramInfoLog(glHandle_, capacity, &length, infoLog.data());
		infoLog.setLength(static_cast<unsigned int>(length) < capacity - 1 ? static_cast<unsigned int>(length) : capacity - 1);
	}
}

bool GLShaderProgram::attachShaderFromFile(GLenum type, const char *filename)
{
	return attachShaderFromStringsAndFile(type, nullptr, filename);
}

bool GLShaderProgram::attachShaderFromString(GLenum type, const char *string)
{
	static const char *strings[2] = { nullptr, nullptr };

	strings[0] = string;
	return attachShaderFromStringsAndFile(type, strings, nullptr);
}

bool GLShaderProgram::attachShaderFromStrings(GLenum type, const char **strings)
{
	return attachShaderFromStringsAndFile(type, strings, nullptr);
}

bool GLShaderProgram::attachShaderFromStringsAndFile(GLenum type, const char **strings, const char *filename)
{
	nctl::UniquePtr<GLShader> shader = nctl::makeUnique<GLShader>(type);
	shader->loadFromStringsAndFile(strings, filename);
	glAttachShader(glHandle_, shader->glHandle());

	const GLShader::ErrorChecking errorChecking = (queryPhase_ == GLShaderProgram::QueryPhase::IMMEDIATE)
	                                                  ? GLShader::ErrorChecking::IMMEDIATE
	                                                  : GLShader::ErrorChecking::DEFERRED;
	const bool hasCompiled = shader->compile(errorChecking, shouldLogOnErrors_);

	if (hasCompiled)
		attachedShaders_.pushBack(nctl::move(shader));
	else
		status_ = Status::COMPILATION_FAILED;

	return hasCompiled;
}

bool GLShaderProgram::link(Introspection introspection)
{
	introspection_ = introspection;

	hashName_ = 0;
	for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
		hashName_ += shader->sourceHash();
	LOGI_X("Shader program %u - hash: 0x%016lx", glHandle_, hashName_);

	bool loaded = false;
	if (RenderResources::binaryShaderCache().isEnabled())
	{
		const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
		const int numBinaryFormats = gfxCaps.value(IGfxCapabilities::GLIntValues::NUM_PROGRAM_BINARY_FORMATS);

		for (unsigned int i = 0; i < numBinaryFormats; i++)
		{
			const int binFormat = gfxCaps.arrayValue(IGfxCapabilities::GLArrayIntValues::PROGRAM_BINARY_FORMATS, i);
			const unsigned int binLength = RenderResources::binaryShaderCache().binarySize(binFormat, hashName_);
			if (binLength > 0)
			{
				const void *buffer = RenderResources::binaryShaderCache().loadFromCache(binFormat, hashName_);
				loaded = loadBinary(binFormat, buffer, binLength);
				break;
			}
		}
	}

	if (loaded == false)
	{
		glLinkProgram(glHandle_);
		if (RenderResources::binaryShaderCache().isEnabled())
		{
			const int binLength = binaryLength();
			if (binLength > 0)
			{
				if (bufferSize < binLength)
				{
					bufferSize = binLength;
					bufferPtr = nctl::makeUnique<uint8_t[]>(bufferSize);
				}

				unsigned int format = 0;
				saveBinary(binLength, format, bufferPtr.get());

				RenderResources::binaryShaderCache().saveToCache(binLength, bufferPtr.get(), format, hashName_);
			}
		}
	}

	if (queryPhase_ == QueryPhase::IMMEDIATE)
	{
		const bool linkCheck = checkLinking();
		if (linkCheck == false)
			return false;

		// After linking, shader objects are not needed anymore
		for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
			glDetachShader(glHandle_, shader->glHandle());
		attachedShaders_.clear();

		performIntrospection();
		return linkCheck;
	}
	else
	{
		status_ = GLShaderProgram::Status::LINKED_WITH_DEFERRED_QUERIES;
		return true;
	}
}

void GLShaderProgram::use()
{
	if (boundProgram_ != glHandle_)
	{
		deferredQueries();

		glUseProgram(glHandle_);
		boundProgram_ = glHandle_;
	}
}

bool GLShaderProgram::validate()
{
	glValidateProgram(glHandle_);
	GLint status;
	glGetProgramiv(glHandle_, GL_VALIDATE_STATUS, &status);
	return (status == GL_TRUE);
}

bool GLShaderProgram::loadBinary(unsigned int binaryFormat, const void *buffer, int bufferSize)
{
	ASSERT(buffer);
	ASSERT(bufferSize > 0);

	status_ = Status::NOT_LINKED;
	glProgramBinary(glHandle_, binaryFormat, buffer, bufferSize);
	const bool linked = checkLinking();

	return linked;
}

int GLShaderProgram::binaryLength() const
{
	GLint length = 0;
	glGetProgramiv(glHandle_, GL_PROGRAM_BINARY_LENGTH, &length);
	return length;
}

bool GLShaderProgram::saveBinary(int bufferSize, unsigned int &binaryFormat, void *buffer) const
{
	ASSERT(bufferSize > 0);
	ASSERT(buffer);

	GLsizei length = 0;
	if (buffer != nullptr && bufferSize > 0)
		glGetProgramBinary (glHandle_, bufferSize, &length, &binaryFormat, buffer);

	return (length > 0 && bufferSize >= length);
}

GLVertexFormat::Attribute *GLShaderProgram::attribute(const char *name)
{
	ASSERT(name);
	GLVertexFormat::Attribute *vertexAttribute = nullptr;

	int location = -1;
	const bool attributeFound = attributeLocations_.contains(name, location);

	if (attributeFound)
		vertexAttribute = &vertexFormat_[location];

	return vertexAttribute;
}

void GLShaderProgram::defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset)
{
	if (vbo)
	{
		for (int location : attributeLocations_)
		{
			vertexFormat_[location].setVbo(vbo);
			vertexFormat_[location].setBaseOffset(vboOffset);
		}
		vertexFormat_.setIbo(ibo);

		RenderResources::vaoPool().bindVao(vertexFormat_);
	}
}

void GLShaderProgram::reset()
{
	if (status_ != Status::NOT_LINKED && status_ != Status::COMPILATION_FAILED)
	{
		uniforms_.clear();
		uniformBlocks_.clear();
		attributes_.clear();

		attributeLocations_.clear();
		vertexFormat_.reset();

		if (boundProgram_ == glHandle_)
			glUseProgram(0);

		for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
			glDetachShader(glHandle_, shader->glHandle());
		attachedShaders_.clear();

		glDeleteProgram(glHandle_);

		RenderResources::removeCameraUniformData(this);
		RenderResources::unregisterBatchedShader(this);

		glHandle_ = glCreateProgram();
		hashName_ = 0;
	}

	status_ = Status::NOT_LINKED;
}

void GLShaderProgram::setObjectLabel(const char *label)
{
	GLDebug::objectLabel(GLDebug::LabelTypes::PROGRAM, glHandle_, label);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GLShaderProgram::deferredQueries()
{
	if (status_ == GLShaderProgram::Status::LINKED_WITH_DEFERRED_QUERIES)
	{
		for (nctl::UniquePtr<GLShader> &attachedShader : attachedShaders_)
		{
			const bool compileCheck = attachedShader->checkCompilation(shouldLogOnErrors_);
			if (compileCheck == false)
				return false;
		}

		const bool linkCheck = checkLinking();
		if (linkCheck == false)
			return false;

		// After linking, shader objects are not needed anymore
		for (const nctl::UniquePtr<GLShader> &shader : attachedShaders_)
			glDetachShader(glHandle_, shader->glHandle());
		attachedShaders_.clear();

		performIntrospection();
	}
	return true;
}

bool GLShaderProgram::checkLinking()
{
	if (status_ == Status::LINKED || status_ == Status::LINKED_WITH_INTROSPECTION)
		return true;

	GLint status;
	glGetProgramiv(glHandle_, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		if (shouldLogOnErrors_)
		{
			GLint length = 0;
			glGetProgramiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

			if (length > 0)
			{
				glGetProgramInfoLog(glHandle_, MaxInfoLogLength, &length, infoLogString_);
				LOGW_X("%s", infoLogString_);
			}
		}

		status_ = Status::LINKING_FAILED;
		return false;
	}

	status_ = Status::LINKED;
	return true;
}

void GLShaderProgram::performIntrospection()
{
	if (introspection_ != Introspection::DISABLED && status_ != Status::LINKED_WITH_INTROSPECTION)
	{
		const GLUniformBlock::DiscoverUniforms discover = (introspection_ == Introspection::NO_UNIFORMS_IN_BLOCKS)
		                                                      ? GLUniformBlock::DiscoverUniforms::DISABLED
		                                                      : GLUniformBlock::DiscoverUniforms::ENABLED;

		discoverUniforms();
		discoverUniformBlocks(discover);
		discoverAttributes();
		initVertexFormat();
		status_ = Status::LINKED_WITH_INTROSPECTION;
	}
}

void GLShaderProgram::discoverUniforms()
{
	static const unsigned int NumIndices = 512;
	static GLuint uniformIndices[NumIndices];
	static GLint blockIndices[NumIndices];

	ZoneScoped;
	GLint uniformCount = 0;
	glGetProgramiv(glHandle_, GL_ACTIVE_UNIFORMS, &uniformCount);

	if (uniformCount > 0)
	{
		unsigned int uniformsOutsideBlocks = 0;
		GLuint indices[MaxNumUniforms];
		unsigned int remainingIndices = static_cast<unsigned int>(uniformCount);

		while (remainingIndices > 0)
		{
			const unsigned int uniformCountStep = (remainingIndices > NumIndices) ? NumIndices : remainingIndices;
			const unsigned int startIndex = static_cast<unsigned int>(uniformCount) - remainingIndices;

			for (unsigned int i = 0; i < uniformCountStep; i++)
				uniformIndices[i] = startIndex + i;

			glGetActiveUniformsiv(glHandle_, uniformCountStep, uniformIndices, GL_UNIFORM_BLOCK_INDEX, blockIndices);

			for (unsigned int i = 0; i < uniformCountStep; i++)
			{
				if (blockIndices[i] == -1)
				{
					indices[uniformsOutsideBlocks] = startIndex + i;
					uniformsOutsideBlocks++;
				}
				if (uniformsOutsideBlocks > MaxNumUniforms - 1)
					break;
			}

			remainingIndices -= uniformCountStep;
		}

		for (unsigned int i = 0; i < uniformsOutsideBlocks; i++)
		{
			GLUniform uniform(glHandle_, indices[i]);
			uniformsSize_ += uniform.memorySize();
			uniforms_.pushBack(uniform);

			LOGD_X("Shader program %u - uniform %d : \"%s\"", glHandle_, uniform.location(), uniform.name());
		}
	}
}

void GLShaderProgram::discoverUniformBlocks(GLUniformBlock::DiscoverUniforms discover)
{
	ZoneScoped;
	GLint count;
	glGetProgramiv(glHandle_, GL_ACTIVE_UNIFORM_BLOCKS, &count);

	for (int i = 0; i < count; i++)
	{
		GLUniformBlock uniformBlock(glHandle_, i, discover);
		uniformBlocksSize_ += uniformBlock.size();
		uniformBlocks_.pushBack(uniformBlock);

		LOGD_X("Shader program %u - uniform block %u : \"%s\" (%d bytes with %u align)", glHandle_,
		       uniformBlock.index(), uniformBlock.name(), uniformBlock.size(), uniformBlock.alignAmount());
	}
}

void GLShaderProgram::discoverAttributes()
{
	ZoneScoped;
	GLint count;
	glGetProgramiv(glHandle_, GL_ACTIVE_ATTRIBUTES, &count);

	for (int i = 0; i < count; i++)
	{
		GLAttribute attribute(glHandle_, i);
		attributes_.pushBack(attribute);

		LOGD_X("Shader program %u - attribute %d : \"%s\"", glHandle_, attribute.location(), attribute.name());
	}
}

void GLShaderProgram::initVertexFormat()
{
	ZoneScoped;
	const unsigned int count = attributes_.size();
	if (count > GLVertexFormat::MaxAttributes)
		LOGW_X("More active attributes (%d) than supported by the vertex format class (%d)", count, GLVertexFormat::MaxAttributes);

	for (unsigned int i = 0; i < attributes_.size(); i++)
	{
		const GLAttribute &attribute = attributes_[i];
		const int location = attribute.location();
		if (location < 0)
			continue;

		attributeLocations_[attribute.name()] = location;
		vertexFormat_[location].init(attribute.location(), attribute.numComponents(), attribute.basicType());
	}
}

}
