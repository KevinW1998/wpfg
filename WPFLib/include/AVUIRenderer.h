#pragma once

namespace AVUI {

class IClientRendererInterface;
class IClientSubgraphCapture;
class Brush;
class Visual;

class Renderer
{

public:
    Renderer() : m_pActiveBrush(NULL) { }; 
    ~Renderer() { };

    void set_ClientRendererInterface(IClientRendererInterface* pClientRendererInterface) { m_pClientRendererInterface = pClientRendererInterface; }
    IClientRendererInterface* get_ClientRendererInterface() { return m_pClientRendererInterface; };

    float get_CurrentOpacity() { return m_currentOpacity; }
    void set_CurrentOpacity(float newOpacity);

    void RegisterSubgraphCapture(Visual* pVisual, IClientSubgraphCapture* pSubgraphCapture);
    IClientSubgraphCapture* GetSubgraphCapture(Visual* pVisual);
    void DestroySubgraphCaptures();

private:
    IClientRendererInterface* m_pClientRendererInterface;
    float m_currentOpacity;
    Brush* m_pActiveBrush;

    struct CapturedSubgraph
    {
        Visual* pVisual;        
        IClientSubgraphCapture* pSubgraph;
    };

    ItemStructList<CapturedSubgraph> m_capturedSubgraphs;
};

};
