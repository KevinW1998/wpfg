#include <AVUITileBrush.h>

namespace AVUI {

class IClientSubgraphCapture;
class Visual;

class VisualBrush : public TileBrush
{
    friend class Application;
public:

    VisualBrush();
    ~VisualBrush();

    DECLARE_ELEMENT(VisualBrush, TileBrush);

    void set_ActiveBrush(Renderer* pRenderer);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Visual, Visual, NULL, DependencyPropertyMetadata(PropertyFlag::None, VisualBrush::OnVisualChanged));

    virtual void RenderDependencies(Renderer* pRenderer);
    virtual bool ContainsRenderDependencies() const { return true; };

private:
    void OnRenderFinished();
    void UpdateVisualBrushRootProperties(Visual* pOld, Visual* pNew);

    static void OnVisualChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    mutable IClientSubgraphCapture* m_pClientSubgraphCapture;
    bool m_isSimpleMediaBrush;
};



}; // namespace AVUI
