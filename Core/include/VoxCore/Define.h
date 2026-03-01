#pragma once
#include <exception>
#include <string>

constexpr int BYTE_SIZE = 1;
constexpr int KILOBYTE_SIZE = BYTE_SIZE * 1024;
constexpr int MEGABYTE_SIZE = KILOBYTE_SIZE * 1024;
constexpr int GIGABYTE_SIZE = MEGABYTE_SIZE * 1024;

#define VOX_NS namespace Vox {
#define RENDER_NS namespace Vox::Render {
#define VULKAN_NS namespace Vox::Render::Vulkan {
#define RESOURCES_NS namespace Vox::Resources {
#define SERIALIZATION_NS namespace Vox::Resources::Serialization {
#define NS_END }

#define BUILDER_ENTRY(builderClass, name, type, dst) \
inline builderClass& name(type name) {dst = name; return *this;}\

#define NO_COPY_MOVE_DEFAULT(ClassName)   \
ClassName() = delete;     \
ClassName(const ClassName&) = delete;     \
ClassName(ClassName&&) = delete;          \
ClassName& operator=(const ClassName&) = delete; \
ClassName& operator=(ClassName&&) = delete;

#define NO_COPY_MOVE(ClassName)   \
ClassName(const ClassName&) = delete;     \
ClassName(ClassName&&) = delete;          \
ClassName& operator=(const ClassName&) = delete; \
ClassName& operator=(ClassName&&) = delete;

#define NON_COPYABLE(ClassName)   \
ClassName(const ClassName&) = delete;     \
ClassName& operator=(const ClassName&) = delete; \
