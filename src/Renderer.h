#pragma once

#include <GL/glew.h>
#include <assert.h>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Debug.h"
#include "Shader.h"

class Renderer
{
    public:
        void Clear() const;
        void Draw(const VertexArray& va, const IndexBuffer& ib, Shader &shader) const;
};