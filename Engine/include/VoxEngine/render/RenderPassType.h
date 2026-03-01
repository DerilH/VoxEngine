//
// Created by deril on 2/17/26.
//

#pragma once
RENDER_NS
    typedef enum {
        GBUFFER_PASS,
        DEPTH_PASS,
        SHADOW_PASS,
        LIGHTING_PASS,
        FORWARD_PASS,
        POSTPROCESS_PASS,
        UI_PASS,
        RESOLVE_PASS,
        TRANSPARENCY_PASS,
        PRESENT_PASS

    } RenderPassType;

NS_END
