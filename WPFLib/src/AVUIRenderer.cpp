#include <AVUICommon.h>
#include <AVUIRenderer.h>
#include <AVUIBrush.h>
#include <AVUIIClientRendererInterface.h>

namespace AVUI {

void Renderer::RegisterSubgraphCapture(Visual* pVisual, IClientSubgraphCapture* pSubgraphCapture)
{
    CapturedSubgraph newCapturedSubgraph;

    newCapturedSubgraph.pVisual = pVisual;
    newCapturedSubgraph.pSubgraph = pSubgraphCapture;

    m_capturedSubgraphs.AddItem(newCapturedSubgraph);

}

IClientSubgraphCapture* Renderer::GetSubgraphCapture(Visual* pVisual)
{
    for(int idx = 0; idx < m_capturedSubgraphs.get_Count(); idx++)
    {
        if(m_capturedSubgraphs[idx].pVisual == pVisual)
        {
            return m_capturedSubgraphs[idx].pSubgraph;
        }
    }
    return NULL;
}

void Renderer::DestroySubgraphCaptures()
{
    for(int idx = 0; idx < m_capturedSubgraphs.get_Count(); idx++)
    {
        m_capturedSubgraphs[idx].pSubgraph->Destroy();
    }
    m_capturedSubgraphs.Clear();
}


}; // namespace AVUI

