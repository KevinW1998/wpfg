#pragma once

namespace AVUI {

class Window;
class PopupRoot;
class Renderer;
class GeometryBuilder;

class WindowManager : public Object
{
public:
    WindowManager() : m_pActiveWindow(NULL), m_pWindowZOrder(NULL) { };

public:

    void AddTopLevelWindow(Window* pWindow);
    void AddPopupWindow(PopupRoot* pPopupRoot);

    void RemoveTopLevelWindow(Window* pWindow);
    void RemovePopupWindow(PopupRoot* pPopupRoot);

    bool ActivateWindow(Window* pWindow);
    void DeactivateWindow(Window* pWindow);

    Window* get_ActiveWindow() { return m_pActiveWindow; }

    void BuildGeometryAndRenderDependencies(Renderer* pRenderer, GeometryBuilder* pBuilder);
    void RenderWindows(Renderer* pRenderer);
    void LayoutWindows(const Size& desktopSize);

    void BringToTop(Window* pWindow);
    void RemoveFromZOrderList(Window* pWindow);
    void AddToZOrderList(Window* pWindow);

    Visual* ResolvePointToWindow(const Point& pt);

    void ReleaseRenderData();

    void OnPresentationSourceSizeChanged();
    static Point ToWindowSpace(Visual* pWindow, const Point& absolutePoint);

private:

    void SetActiveWindow(Window* pWindow);

    Window* m_pActiveWindow;
    Window* m_pWindowZOrder;
    ItemStructList<PopupRoot*> m_pPopupWindows;
    Size m_lastDesktopSize;
};


}; // namespace AVUI

