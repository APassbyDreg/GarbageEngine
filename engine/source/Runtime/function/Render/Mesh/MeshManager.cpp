#pragma once

#include "MeshManager.h"

#include "TriangleMesh.h"

namespace GE
{
    void MeshFactory::EnsureInit()
    {
        if (!m_initialized)
        {
            // add meshes
            AddMesh<TriangleMesh>();

            m_initialized = true;
        }
    }
} // namespace GE