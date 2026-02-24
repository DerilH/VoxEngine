#pragma once
#include <exception>
#include <string>

constexpr int BYTE_SIZE = 1;
constexpr int KILOBYTE_SIZE = BYTE_SIZE * 1024;
constexpr int MEGABYTE_SIZE = KILOBYTE_SIZE * 1024;
constexpr int GIGABYTE_SIZE = MEGABYTE_SIZE * 1024;

typedef std::string InternedString;

#ifdef VOX_DEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

#define VOX_NS namespace Vox {
#define RENDER_NS namespace Vox::Render {
#define VULKAN_NS namespace Vox::Render::Vulkan {
#define RESOURCES_NS namespace Vox::Resources {
#define SERIALIZATION_NS namespace Vox::Resources::Serialization {
#define NS_END }

#define BUILDER_ENTRY(builderClass, name, type, dst) \
inline builderClass& name(type name) {dst = name; return *this;}\

#define NON_COPYABLE_NON_MOVABLE(ClassName)   \
ClassName() = delete;     \
ClassName(const ClassName&) = delete;     \
ClassName(ClassName&&) = delete;          \
ClassName& operator=(const ClassName&) = delete; \
ClassName& operator=(ClassName&&) = delete;

#define NON_COPYABLE(ClassName)   \
ClassName(const ClassName&) = delete;     \
ClassName& operator=(const ClassName&) = delete; \
