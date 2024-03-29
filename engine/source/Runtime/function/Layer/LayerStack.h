#pragma once

#include "GE_pch.h"

#include "Layer.h"

namespace GE
{
    class GE_API LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);
        void PopLayer(std::shared_ptr<Layer> layer);
        void PopOverlay(std::shared_ptr<Layer> overlay);

        inline size_t GetLayerCount() { return m_layers.size(); }

        inline std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
        inline std::vector<std::shared_ptr<Layer>>::iterator end() { return m_layers.end(); }

    private:
        std::vector<std::shared_ptr<Layer>> m_layers;
        uint                                m_layerInsertIndex = 0;
    };
} // namespace GE