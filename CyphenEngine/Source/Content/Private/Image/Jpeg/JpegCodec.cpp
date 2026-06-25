#include "pch.h"

#include "Content/Private/Image/Jpeg/JpegCodec.h"

#include "Content/Private/Image/Jpeg/WindowsJpegCodec.h"
#include "Resource/Public/Resource.h"
#include "Resource/Public/Texture.h"

bool JpegCodec::Decode(const std::vector<uint8>& bytes, Resource& outResource)
{
	if (bytes.empty() || outResource.kind != ResourceKind::Texture2D)
	{
		return false;
	}

	Texture2D& outTexture = static_cast<Texture2D&>(outResource);

	return WindowsJpegCodec::Decode(bytes, outTexture);
}
