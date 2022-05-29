#include "LayerStack.h"

namespace GE
{
    LayerStack::LayerStack() { m_layerInsert = m_layers.begin(); }

    LayerStack::~LayerStack() {}

    void LayerStack::PushLayer(std::shared_ptr<Layer> layer)
    {
        m_layers.emplace(m_layerInsert, layer);
        m_layerInsert++;
    }

    void LayerStack::PushOverlay(std::shared_ptr<Layer> overlay) { m_layers.emplace_back(overlay); }

    void LayerStack::PopLayer(std::shared_ptr<Layer> layer)
    {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);
        if (it != m_layers.end())
        {
            m_layers.erase(it);
            m_layerInsert--;
        }
    }

    void LayerStack::PopOverlay(std::shared_ptr<Layer> overlay)
    {
        auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
        if (it != m_layers.end())
        {
            m_layers.erase(it);
        }
    }
} // namespace GE
