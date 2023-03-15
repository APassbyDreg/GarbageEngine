#include "MaterialManager.h"

/* --------------------------------- forward -------------------------------- */
#include "Forward/ForwardPhongMaterial.h"
#include "Forward/ForwardSolidMaterial.h"

/* -------------------------------- deferred -------------------------------- */

namespace GE
{
    void MaterialFactory::EnsureInit()
    {
        if (!m_initialized)
        {
            // add forward materials
            AddForwardMaterial<ForwardSolidMaterial>();
            AddForwardMaterial<ForwardPhongMaterial>();

            // add deferred materials

            m_initialized = true;
        }
    }
} // namespace GE