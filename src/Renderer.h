#pragma once

#include <GL/glew.h>
#include <assert.h>

#include "IndexBuffer.h"
#include "VertexArray.h"

class Renderer
{
    public:
        void Clear() const;
        void Draw(const VertexArray& va, const IndexBuffer& ib) const;
};