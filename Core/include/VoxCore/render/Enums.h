#pragma once
#include <VoxCore/Define.h>

RENDER_NS
    enum class ShaderResourceType {
        SAMPLER,
        COMBINED_IMAGE_SAMPLER,
        SAMPLED_IMAGE,
        STORAGE_IMAGE,
        UNIFORM_TEXEL_BUFFER,
        STORAGE_TEXEL_BUFFER,
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        UNIFORM_BUFFER_DYNAMIC,
        STORAGE_BUFFER_DYNAMIC,
        INPUT_ATTACHMENT
    };

    enum class FrontFace {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    enum class CullMode {
        NONE,
        BACK,
        FRONT,
        FRONT_BACK
    };

    enum class PolygonMode {
        FILL,
        LINE,
        POINT
    };

    enum class PrimitiveTopology {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        TRIANGLE_FAN
    };

    enum class BufferUsage {
        INDEX,
        VERTEX,
        SSBO,
        UNIFORM,
        TRANSFER_SRC
    };

    enum class IndexType {
        UINT8,
        UINT16,
        UINT32
    };

    enum class Format {
        UNDEFINED,
        RGBA8,
        BGRA8,
        RGBA8_SRGB,
        BGRA8_SRGB,
        RGB10A2,
        RG11B10F,
        RG16F,
        RGBA16F,
        RGBA32F,
        R8,
        R8_SRGB,
        R16F,
        D16,
        D32,
        D24S8,
        D32S8,
        BC1_RGB,
        BC1_RGB_SRGB,
        BC1_RGBA,
        BC1_RGBA_SRGB,
        BC3_RGBA,
        BC3_RGBA_SRGB,
        BC4_R,
        BC5_RG,
        BC7_RGBA,
        BC7_RGBA_SRGB,
        ETC2_RGB,
        ETC2_RGB_SRGB,
        ETC2_RGBA1,
        ETC2_RGBA1_SRGB,
        ETC2_RGBA8,
        ETC2_RGBA8_SRGB,
        ASTC_4x4,
        ASTC_4x4_SRGB
    };

    enum class ShaderStage {
        VERTEX,
        FRAGMENT
    };

    enum class MSAASamples {
        COUNT_1,
        COUNT_2,
        COUNT_4,
        COUNT_8,
        COUNT_16,
        COUNT_32,
        COUNT_64
    };

    enum class BlendOp {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX,
        MULTIPLY,
        SCREEN,
        OVERLAY
    };

    enum class BlendFactor {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
        SRC_ALPHA_SATURATE,
        SRC1_COLOR,
        ONE_MINUS_SRC1_COLOR,
        SRC1_ALPHA,
        ONE_MINUS_SRC1_ALPHA
    };

NS_END
