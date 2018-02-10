#include "common_macros.h"
#include "TextureLoaderWebP.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderWebP::TextureLoaderWebP(const char *filename)
	: TextureLoaderWebP(IFile::createFileHandle(filename))
{

}

TextureLoaderWebP::TextureLoaderWebP(nctl::UniquePtr<IFile> fileHandle)
	: ITextureLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	// Loading the whole file in memory
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	const long int fileSize = fileHandle_->size();
	nctl::UniquePtr<unsigned char []> fileBuffer = nctl::makeUnique<unsigned char []>(fileSize);
	fileHandle_->read(fileBuffer.get(), fileSize);

	if (WebPGetInfo(fileBuffer.get(), fileSize, &width_, &height_) == 0)
	{
		fileBuffer.reset(nullptr);
		FATAL_MSG("Cannot read WebP header");
	}

	LOGI_X("Header found: w:%d h:%d", width_, height_);

	WebPBitstreamFeatures features;
	if (WebPGetFeatures(fileBuffer.get(), fileSize, &features) != VP8_STATUS_OK)
	{
		fileBuffer.reset(nullptr);
		FATAL_MSG("Cannot retrieve WebP features from headers");
	}

	LOGI_X("Bitstream features found: alpha:%d animation:%d format:%d",
	       features.has_alpha, features.has_animation, features.format);

	mipMapCount_ = 1; // No MIP Mapping
	texFormat_ = features.has_alpha ? TextureFormat(GL_RGBA) : TextureFormat(GL_RGB);
	bpp_ = features.has_alpha ? 4 : 3;
	long int decodedSize = width_ * height_ * bpp_;
	pixels_ = nctl::makeUnique<unsigned char []>(decodedSize);

	if (features.has_alpha)
	{
		if (WebPDecodeRGBAInto(fileBuffer.get(), fileSize, pixels_.get(), decodedSize, width_ * bpp_) == nullptr)
		{
			fileBuffer.reset(nullptr);
			pixels_.reset(nullptr);
			FATAL_MSG("Cannot decode RGBA WebP image");
		}
	}
	else
	{
		if (WebPDecodeRGBInto(fileBuffer.get(), fileSize, pixels_.get(), decodedSize, width_ * bpp_) == nullptr)
		{
			fileBuffer.reset(nullptr);
			pixels_.reset(nullptr);
			FATAL_MSG("Cannot decode RGB WebP image");
		}
	}
}

}
