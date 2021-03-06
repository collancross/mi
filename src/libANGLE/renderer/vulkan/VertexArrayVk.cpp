//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// VertexArrayVk.cpp:
//    Implements the class methods for VertexArrayVk.
//

#include "libANGLE/renderer/vulkan/VertexArrayVk.h"

#include "common/debug.h"

#include "libANGLE/renderer/vulkan/ContextVk.h"

namespace rx
{

VertexArrayVk::VertexArrayVk(const gl::VertexArrayState &data) : VertexArrayImpl(data)
{
}

VertexArrayVk::~VertexArrayVk()
{
}

void VertexArrayVk::syncState(ContextImpl *contextImpl, const gl::VertexArray::DirtyBits &dirtyBits)
{
    ASSERT(dirtyBits.any());

    // TODO(jmadill): Use pipeline cache.
    auto contextVk = GetAs<ContextVk>(contextImpl);
    contextVk->invalidateCurrentPipeline();
}

}  // namespace rx
