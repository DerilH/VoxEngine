//
// Created by deril on 2/17/26.
//

#pragma once

RENDER_NS
class RenderTarget {
protected:
    ~RenderTarget() = default;

public:
    char getImageCount();
};
NS_END