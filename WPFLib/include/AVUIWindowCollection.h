#pragma once

namespace AVUI {

class Window;

class WindowCollection
{
    friend class Window;
    friend class Application;
public:
    WindowCollection() { }
    Window* operator[](int idx) { return m_windows[idx]; }
    bool HasItem(Window* pWindow) { return m_windows.Contains(pWindow); }
    int get_Count() { return m_windows.get_Count(); }

private:
    void Clear() { m_windows.Clear(); }
    void Add(Window* pWindow);
    void Remove(Window* pWindow);

    ItemStructList<TRefCountedPtr<Window> > m_windows;
};


};

