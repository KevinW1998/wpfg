
#pragma once

#include <AVUIButtonBase.h>

namespace AVUI {

class GridViewColumn;
class Canvas;
class Thumb;
class DragStartedEventArgs;
class DragDeltaEventArgs;
class DragCompletedEventArgs;

class GridViewColumnHeader : public ButtonBase
{
    friend class GridViewHeaderRowPresenter;
public:
    GridViewColumnHeader() : m_flags(Flags::None) { };
    ~GridViewColumnHeader() { };

    DECLARE_ELEMENT(GridViewColumnHeader, ButtonBase);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Role, GridViewColumnHeaderRole, GridViewColumnHeaderRole::Normal, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(Column, GridViewColumn, NULL, PropertyFlag::None);

protected:
    virtual void OnApplyTemplate();

private:

    void OnColumnHeaderGripperDragStarted(Object* pSender, DragStartedEventArgs* pArgs);
    void OnColumnHeaderResize(Object* pSender, DragDeltaEventArgs* pArgs);
    void OnColumnHeaderGripperDragCompleted(Object* pSender, DragCompletedEventArgs* pArgs);
    void OnGripperMouseEnterLeave(Object* pSender, MouseEventArgs* pArgs);
    void OnGripperDoubleClicked(Object* pSender, MouseButtonEventArgs* pArgs);
    void UpdateColumnHeaderWidth(float width);

    void OnColumnHeaderKeyDown(Object* pSender, KeyEventArgs* pArgs);
    void CheckWidthForPreviousHeaderGripper();
    void HideGripperRightHalf(bool hide);
    void UpdateGripperCursor();
    void UpdateFloatingHeaderCanvas();
    void HookupGripperEvents();
    void UnhookGripperEvents();

    Cursor* get_SplitCursor();
    float get_ColumnActualWidth();
    bool HandleIsMouseOverChanged();
    void MakeParentGotFocus();

    class Flags
    {
    public:
        enum Enum
        {
            None = 0,
            StyleSetByUser = 1,
            IgnoreStyle = 2,
            ContentTemplateSetByUser = 4,
            IgnoreContentTemplate = 8,
            ContentTemplateSelectorSetByUser = 0x10,
            IgnoreContentTemplateSelector = 0x20,
            ContextMenuSetByUser = 0x40,
            IgnoreContextMenu = 0x80,
            ToolTipSetByUser = 0x100,
            IgnoreToolTip = 0x200,
            SuppressClickEvent = 0x400,
            IsInternalGenerated = 0x800,
            IsAccessKeyOrAutomation = 0x1000
        };
    };

    void UpdateProperty(const DependencyProperty* pDP, Object* pValue);

    void set_PreviousVisualHeader(GridViewColumnHeader* pHeader)  { m_pPreviousHeader = pHeader; }
    GridViewColumnHeader* get_PreviousVisualHeader()  { return m_pPreviousHeader; }

    void set_IsInternalGenerated(bool isInternalGenerated) { SetFlag(Flags::IsInternalGenerated, isInternalGenerated); }
    bool get_IsInternalGenerated() { return GetFlag(Flags::IsInternalGenerated); }

    void set_SuppressClickEvent(bool suppressClickEvent) { SetFlag(Flags::SuppressClickEvent, suppressClickEvent); }
    bool get_SuppressClickEvent() { return GetFlag(Flags::SuppressClickEvent); }

    static void PropertyToFlags(const DependencyProperty* pDP, Flags::Enum *pFlag, Flags::Enum* pIgnoreFlag);

    void SetFlag(Flags::Enum flag, bool set)
    {
        if (set)
        {
            m_flags = (Flags::Enum) (m_flags | flag);
        }
        else
        {

            m_flags = (Flags::Enum) (m_flags & ~flag);
        }
    }
    bool GetFlag(Flags::Enum flag)
    {
        return ((m_flags & flag) == flag);
    }   

    Flags::Enum m_flags;
    TRefCountedPtr<Canvas> m_pFloatingHeaderCanvas;
    TRefCountedPtr<Thumb> m_pHeaderGripper;
    float m_originalWidth;
    GridViewColumnHeader* m_pPreviousHeader;
    GridViewColumnHeader* m_pSrcHeader;
};

}; // namespace AVUI
