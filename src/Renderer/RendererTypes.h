#pragma once
#include <cstdint>

namespace Config {
	inline constexpr unsigned int MAX_LIGHTS = 8;
	inline constexpr int MAX_LOCAL_TEXTURES = 5;
	inline constexpr int MAX_TEXTURE_SLOTS = 32;
}

enum class TextureSlot : uint8_t {
	Diffuse = 0,
	Specular = 1,
	Normal = 2,
	Emission = 3,
	HeightMap = 4,

	ShadowMap = 8,
	Reflection = 9,
	Environment = 10,
};
inline constexpr const char* ToString(TextureSlot slot)
{
	switch (slot) {
	case TextureSlot::Diffuse:   return "u_DiffuseMap";
	case TextureSlot::Specular:  return "u_SpecularMap";
	case TextureSlot::Normal:    return "u_NormalMap";
	case TextureSlot::Emission:  return "u_EmissionMap";
	case TextureSlot::HeightMap: return "u_HeightMap";
	case TextureSlot::ShadowMap: return "u_ShadowMap";
	default: return "u_Texture"; // Fallback
	}
}

enum class UBOslot : unsigned int {
	Camera = 0,		// Expected to be CameraUBOData
	Ambient = 1,	// Expected to be vec4
	Lights = 2,		// Expected to be LightsUBOData
	Sun = 3,		// Expected to be SunUBOData

	Slot8 = 8,		// Expected to be whatever you decide
	Slot9 = 9,		// Expected to be whatever you decide
	Slot10 = 10,	// Expected to be whatever you decide
	Slot11 = 11,	// Expected to be whatever you decide
};
inline constexpr const char* ToString(UBOslot slot)
{
	switch (slot) {
	case UBOslot::Camera:  return "CameraUBO";
	case UBOslot::Ambient: return "AmbientUBO";
	case UBOslot::Lights:  return "LightsUBO";
	case UBOslot::Sun:     return "SunUBO";

	default: return "";
	}
};
namespace Config {
	inline constexpr UBOslot STANDART_UBO_SLOTS[] = {
		UBOslot::Camera, UBOslot::Ambient, UBOslot::Lights, UBOslot::Sun,
	};
}
