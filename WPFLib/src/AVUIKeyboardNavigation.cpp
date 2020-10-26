#include <AVUICommon.h>
#include <AVUIKeyboardNavigation.h>
#include <AVUIDispatcher.h>
#include <AVUIInputManager.h>
#include <AVUIControl.h>

REGISTER_ELEMENT_BEGIN(AVUI::KeyboardNavigation)
    REGISTER_PROPERTY(TabIndex);
    REGISTER_PROPERTY(IsTabStop);
    REGISTER_PROPERTY(AcceptsReturn);
    REGISTER_PROPERTY(TabNavigation);
    REGISTER_PROPERTY(DirectionalNavigation);
    REGISTER_PROPERTY(DirectionalNavigationMargin);
REGISTER_ELEMENT_END()

namespace AVUI {

KeyboardNavigationMode::Enum KeyboardNavigation::GetKeyNavigationMode(UIElement* pDO)
{
    return (KeyboardNavigationMode::Enum)UnboxValue<int>(pDO->GetValue(*m_pNavigationProperty));
}


bool KeyboardNavigation::Navigate(UIElement* pCurrentElement, FocusNavigationDirection::Enum focusNavigationDirection, ModifierKeys::Enum modifierKeys, UIElement* pFirstElement)
{
    UIElement* pNavigateTo = NULL;

    switch (focusNavigationDirection)
    {
    case FocusNavigationDirection::Next:
        m_pNavigationProperty = &(get_TabNavigationProperty());
        pNavigateTo = GetNextTab(pCurrentElement, GetGroupParent(pCurrentElement, true), false);
        break;

    case FocusNavigationDirection::Previous:
        m_pNavigationProperty = &(get_TabNavigationProperty());
        pNavigateTo = GetPrevTab(pCurrentElement, NULL, false);
        break;

    case FocusNavigationDirection::First:
        m_pNavigationProperty = &(get_TabNavigationProperty());
        pNavigateTo = GetNextTab(NULL, pCurrentElement, true);
        break;

    case FocusNavigationDirection::Last:
        m_pNavigationProperty = &(get_TabNavigationProperty());
        pNavigateTo = GetPrevTab(NULL, pCurrentElement, true);
        break;

    case FocusNavigationDirection::Left:
    case FocusNavigationDirection::Right:
    case FocusNavigationDirection::Up:
    case FocusNavigationDirection::Down:
        m_pNavigationProperty = &(get_DirectionalNavigationProperty());
        pNavigateTo = GetNextInDirection(pCurrentElement, focusNavigationDirection);
        break;
    }

    if (pNavigateTo == NULL)
    {
        if ((focusNavigationDirection == FocusNavigationDirection::Next) || (focusNavigationDirection == FocusNavigationDirection::Previous))
        {
            Visual* pRootVisual = GetVisualRoot(pCurrentElement);
            if (pRootVisual != NULL && pRootVisual->IsType(GET_TYPE(UIElement)))
            {
                return Navigate((UIElement*)pRootVisual, (focusNavigationDirection == FocusNavigationDirection::Next) ? FocusNavigationDirection::First : FocusNavigationDirection::Last, modifierKeys, NULL);
            }
        }
        return false;
    }

    return true;
    //    return pNavigateTo->Focus();
    //        return element2.IsKeyboardFocusWithin; -- ?Property?
}

Visual* KeyboardNavigation::GetVisualRoot(Visual* pVisual)
{
    Visual* pVisualParent = pVisual->get_VisualParent();

    while(pVisualParent != NULL)
    {
        pVisual = pVisualParent;
        pVisualParent = pVisual->get_VisualParent();
    }

    return pVisual;
}


UIElement* KeyboardNavigation::GetNextInDirection(UIElement* pDO, FocusNavigationDirection::Enum direction)
{
//    m_containerHashtable.Clear();

    UIElement* pNext = MoveNext(pDO, NULL, direction, Float::get_NegativeInfinity(), Float::get_NegativeInfinity());

//    this._containerHashtable.Clear();
    return pNext;
}

UIElement* KeyboardNavigation::MoveNext(UIElement* pSource, UIElement* pContainer, FocusNavigationDirection::Enum direction, float startRange, float endRange)
{
    if (pContainer == NULL)
    {
        pContainer = GetGroupParent(pSource);
    }
    if (pContainer == pSource)
    {
        return NULL;
    }
    if (IsEndlessLoop(pSource, pContainer))
    {
        return NULL;
    }

    KeyboardNavigationMode::Enum mode = GetKeyNavigationMode(pContainer);
    bool isSourceMissing = (pSource == NULL);

    if ((mode == KeyboardNavigationMode::None) && isSourceMissing)
    {
        return NULL;
    }
    Rect rect = GetRectangle(isSourceMissing ? pContainer : pSource);

    bool isLeftRight = (direction == FocusNavigationDirection::Right) || (direction == FocusNavigationDirection::Left);

    ResetBaseLines(isLeftRight ? rect.get_Top() : rect.get_Left(), isLeftRight);

    if ((startRange == Float::get_NegativeInfinity()) || (endRange == Float::get_NegativeInfinity()))
    {
        startRange = isLeftRight ? rect.get_Top() : rect.get_Left();
        endRange = isLeftRight ? rect.get_Bottom() : rect.get_Right();
    }

    UIElement* pNext = FindNextInDirection(pSource, rect, pContainer, direction, startRange, endRange);
    if (pNext != NULL)
    {
        if (IsTabStop(pNext))
        {
            return pNext;
        }

        UIElement* pChildNext = MoveNext(NULL, pNext, direction, startRange, endRange);
        if (pChildNext != NULL)
        {
            return pChildNext;
        }
        return MoveNext(pNext, NULL, direction, startRange, endRange);
    }
    switch (mode)
    {
    case KeyboardNavigationMode::Cycle:
        return MoveNext(NULL, pContainer, direction, startRange, endRange);

    case KeyboardNavigationMode::Contained:
        return NULL;
    }
    return MoveNext(pContainer, NULL, direction, startRange, endRange);
}


bool KeyboardNavigation::IsTabStop(UIElement* pDO)
{
    Control* pControl = object_cast<Control>(pDO);

    if (pControl != NULL && pControl->get_Focusable() && 
        (UnboxValue<bool>(pControl->GetValue(get_IsTabStopProperty()))) && pControl->get_IsEnabled())
    {
        return pControl->get_IsVisible();
    }

    return false;
}

Rect KeyboardNavigation::GetRectangle(UIElement* pDO)
{
    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL && pElement->get_IsArrangeValid())
    {
        Visual* pVisual = GetVisualRoot(pElement);
        if (pVisual != NULL)
        {
            Matrix matrix = pElement->TransformToAncestor(pVisual);

            Thickness directionalMargin = UnboxValue<Thickness>(pElement->GetValue(get_DirectionalNavigationMarginProperty()));

            float x = -directionalMargin.get_Left();
            float y = -directionalMargin.get_Top();
            float width = (pElement->get_RenderSize().get_Width() + directionalMargin.get_Left()) + directionalMargin.get_Right();
            float height = (pElement->get_RenderSize().get_Height() + directionalMargin.get_Top()) + directionalMargin.get_Bottom();
            if (width < 0)
            {
                x = pElement->get_RenderSize().get_Width() * 0.5f;
                width = 0.0f;
            }
            if (height < 0)
            {
                y = pElement->get_RenderSize().get_Height() * 0.5f;
                height = 0.0f;
            }
            return matrix.TransformBounds(Rect(x, y, width, height));
        }
    }
    return Rect();
}



void KeyboardNavigation::ResetBaseLines(float value, bool horizontalDirection)
{
    if (horizontalDirection)
    {
        m_verticalBaseline = Float::get_NegativeInfinity();
        if (m_horizontalBaseline == Float::get_NegativeInfinity())
        {
            m_horizontalBaseline = value;
        }
    }
    else
    {
        m_horizontalBaseline = Float::get_NegativeInfinity();
        if (m_verticalBaseline == Float::get_NegativeInfinity())
        {
            m_verticalBaseline = value;
        }
    }
}
 

UIElement* KeyboardNavigation::FindNextInDirection(UIElement* pSource, const Rect& sourceRect, UIElement* pContainer, FocusNavigationDirection::Enum direction, float startRange, float endRange)
{
    UIElement* pNext = NULL;
    Rect rect;
    float nextDistance = 0;
    bool isEmptySource = (pSource == NULL);

    UIElement* pCandidate = pContainer;

    while ((pCandidate = GetNextInTree(pCandidate, pContainer)) != NULL)
    {
        if ((pCandidate != pSource) && (IsTabStop(pCandidate) || IsGroup(pCandidate)))
        {
            Rect rectCandidate = GetRectangle(pCandidate);

            bool isInDirection = IsInDirection(sourceRect, rectCandidate, direction);
            bool isInRange = IsInRange(pSource, pCandidate, sourceRect, rectCandidate, direction, startRange, endRange);
            if (isEmptySource || isInDirection || isInRange)
            {
                float candidateDistance = isInRange ? GetPerpDistance(sourceRect, rectCandidate, direction) : GetDistance(sourceRect, rectCandidate, direction);
                if (pNext == NULL)
                {
                    pNext = pCandidate;
                    rect = rectCandidate;
                    nextDistance = candidateDistance;
                }
                else if (Float::LessThan(candidateDistance, nextDistance) || (Float::AreClose(candidateDistance, nextDistance) && (GetDistance(sourceRect, rect, direction) > GetDistance(sourceRect, rectCandidate, direction))))
                {
                    pNext = pCandidate;
                    rect = rectCandidate;
                    nextDistance = candidateDistance;
                }
            }
        }
    }
    return pNext;
}

UIElement* KeyboardNavigation::GetNextInTree(UIElement* pSource, UIElement* pContainer)
{
    UIElement* pNext = NULL;
    if ((pSource == pContainer) || !IsGroup(pSource))
    {
        pNext = GetFirstChild(pSource);
    }
    if (pNext != NULL || pSource == pContainer)
    {
        return pNext;
    }

    pNext = pSource;

    do
    {
        UIElement* pSibling = GetNextSibling(pNext);
        if (pSibling != NULL)
        {
            return pSibling;
        }

        pNext = GetParent(pNext);
    }
    while ((pNext != NULL) && (pNext != pContainer));

    return NULL;
}
 
bool KeyboardNavigation::IsGroup(UIElement* pDO)
{
    return (GetKeyNavigationMode(pDO) != KeyboardNavigationMode::Continue);
}


UIElement* KeyboardNavigation::GetFirstChild(UIElement* pParent)
{
    if (pParent->get_IsVisible())
    {
        int cChildren = pParent->get_VisualChildCount();
        for (int idxChild = 0; idxChild < cChildren; idxChild++)
        {
            Visual* pVisualChild = pParent->get_VisualChild(idxChild);

            UIElement* pUIEChild = object_cast<UIElement>(pVisualChild);
            if(pUIEChild == NULL)
            {
                continue;
            }

            if (pUIEChild->get_IsVisible())
            {
                return pUIEChild;
            }

            UIElement* pChildChild = GetFirstChild(pUIEChild);
            if (pChildChild != NULL)
            {
                return pChildChild;
            }
        }
    }
    return NULL;
}

UIElement* KeyboardNavigation::GetNextSibling(UIElement* pParent)
{
    UIElement* pOverallParent = GetParent(pParent);

    if (pOverallParent != NULL)
    {
        int cChildren = pOverallParent->get_VisualChildCount();
        int childIndex = 0;
        while (childIndex < cChildren)
        {
            if(pOverallParent->get_VisualChild(childIndex) == pParent)
            {
                break;
            }
            childIndex++;
        }
        childIndex++;
        while (childIndex < cChildren)
        {
            UIElement* pUIEChild = object_cast<UIElement>(pOverallParent->get_VisualChild(childIndex));
            if (pUIEChild != NULL && pUIEChild->get_IsVisible())
            {
                return pUIEChild;
            }
            childIndex++;
        }
    }
    return NULL;
}

UIElement* KeyboardNavigation::GetParent(UIElement* pUIE)
{
    while (pUIE != NULL)
    {
        pUIE = object_cast<UIElement>(pUIE->get_VisualParent());

        if (pUIE != NULL && pUIE->get_IsVisible())
        {
            return pUIE;
        }
    }
    return NULL;
}

bool KeyboardNavigation::IsEndlessLoop(UIElement* pSource, UIElement* pContainer)
{
    // Error case with the code. TBD whether we impl this at all.
    return false;
}

bool KeyboardNavigation::IsInDirection(const Rect& rectFrom, const Rect& rectTo, FocusNavigationDirection::Enum direction)
{
      switch (direction)
      {
      case FocusNavigationDirection::Left:
          return Float::GreaterThanOrClose(rectFrom.get_Left(), rectTo.get_Right());

      case FocusNavigationDirection::Right:
          return Float::LessThanOrClose(rectFrom.get_Right(), rectTo.get_Left());

      case FocusNavigationDirection::Up:
          return Float::GreaterThanOrClose(rectFrom.get_Top(), rectTo.get_Bottom());

      case FocusNavigationDirection::Down:
          return Float::LessThanOrClose(rectFrom.get_Bottom(), rectTo.get_Top());
      }

      LibraryCriticalError();

      return false;
}


UIElement* KeyboardNavigation::GetGroupParent(UIElement* pDO, bool includeCurrent)
{
    UIElement* pGroupParent = pDO;
    if (!includeCurrent)
    {
          pGroupParent = pDO;
          pDO = GetParent(pDO);
          if (pDO == NULL)
          {
                return pGroupParent;
          }
    }
    while (pDO != NULL)
    {
          if (IsGroup(pDO))
          {
                return pDO;
          }
          pGroupParent = pDO;
          pDO = GetParent(pDO);
    }
    return pGroupParent;
}


bool KeyboardNavigation::IsInRange(UIElement* pSource, UIElement* pTarget, const Rect& sourceRect, const Rect& targetRect, FocusNavigationDirection::Enum direction, float startRange, float endRange)
{
    switch (direction)
    {
    case FocusNavigationDirection::Left:
    case FocusNavigationDirection::Right:
        if (m_horizontalBaseline != Float::get_NegativeInfinity())
        {
            startRange = Float::Min(startRange, m_horizontalBaseline);
            endRange = Float::Max(endRange, m_horizontalBaseline);
        }
        if (!Float::GreaterThan(targetRect.get_Bottom(), startRange) || !Float::LessThan(targetRect.get_Top(), endRange))
        {
            break;
        }
        if (pSource == NULL)
        {
            return true;
        }
        if (direction == FocusNavigationDirection::Right)
        {
            if (Float::GreaterThan(targetRect.get_Left(), sourceRect.get_Left()))
            {
                return true;
            }
            if (Float::AreClose(targetRect.get_Left(), sourceRect.get_Left()))
            {
                return pSource->IsAncestorOf(pTarget);
            }
            return false;
        }
        if (Float::LessThan(targetRect.get_Right(), sourceRect.get_Right()))
        {
            return true;
        }
        if (Float::AreClose(targetRect.get_Right(), sourceRect.get_Right()))
        {
            return pSource->IsAncestorOf(pTarget);
        }
        return false;

    case FocusNavigationDirection::Up:
    case FocusNavigationDirection::Down:
        if (m_verticalBaseline != Float::get_NegativeInfinity())
        {
            startRange = Float::Min(startRange, m_verticalBaseline);
            endRange = Float::Max(endRange, m_verticalBaseline);
        }
        if (Float::GreaterThan(targetRect.get_Right(), startRange) && Float::LessThan(targetRect.get_Left(), endRange))
        {
            if (pSource == NULL)
            {
                return true;
            }
            if (direction == FocusNavigationDirection::Down)
            {
                if (Float::GreaterThan(targetRect.get_Top(), sourceRect.get_Top()))
                {
                    return true;
                }
                if (Float::AreClose(targetRect.get_Top(), sourceRect.get_Top()))
                {
                    return pSource->IsAncestorOf(pTarget);
                }
                return false;
            }
            if (Float::LessThan(targetRect.get_Bottom(), sourceRect.get_Bottom()))
            {
                return true;
            }
            if (Float::AreClose(targetRect.get_Bottom(), sourceRect.get_Bottom()))
            {
                return pSource->IsAncestorOf(pTarget);
            }
            return false;
        }
        break;

    default:
        LibraryCriticalError();
    }
    return false;
}

float KeyboardNavigation::GetPerpDistance(const Rect& sourceRect, const Rect& targetRect, FocusNavigationDirection::Enum direction)
{
    switch (direction)
    {
    case FocusNavigationDirection::Left:
                return (sourceRect.get_Right() - targetRect.get_Right());

    case FocusNavigationDirection::Right:
                return (targetRect.get_Left() - sourceRect.get_Left());

    case FocusNavigationDirection::Up:
                return (sourceRect.get_Bottom() - targetRect.get_Bottom());

    case FocusNavigationDirection::Down:
                return (targetRect.get_Top() - sourceRect.get_Top());
    }

    LibraryCriticalError();

    return 0.0f;
}

float KeyboardNavigation::GetDistance(const Rect& sourceRect, const Rect& targetRect, FocusNavigationDirection::Enum direction)
{
    Point sourcePoint;
    Point targetPoint;

    switch (direction)
    {
    case FocusNavigationDirection::Left:
        sourcePoint = sourceRect.get_TopRight();
        if (m_horizontalBaseline != Float::get_NegativeInfinity())
        {
            sourcePoint.set_Y(m_horizontalBaseline);
        }
        targetPoint = targetRect.get_TopRight();
        break;

    case FocusNavigationDirection::Right:
        sourcePoint = sourceRect.get_TopLeft();
        if (m_horizontalBaseline != Float::get_NegativeInfinity())
        {
            sourcePoint.set_Y(m_horizontalBaseline);
        }
        targetPoint = targetRect.get_TopLeft();
        break;

    case FocusNavigationDirection::Up:
        sourcePoint = sourceRect.get_BottomLeft();
        if (m_verticalBaseline != Float::get_NegativeInfinity())
        {
            sourcePoint.set_X(m_verticalBaseline);
        }
        targetPoint = targetRect.get_BottomLeft();
        break;

    case FocusNavigationDirection::Down:
        sourcePoint = sourceRect.get_TopLeft();
        if (m_verticalBaseline != Float::get_NegativeInfinity())
        {
            sourcePoint.set_X(m_verticalBaseline);
        }
        targetPoint = targetRect.get_TopLeft();

        break;

    default:
        LibraryCriticalError();
    }
    return GetDistance(sourcePoint, targetPoint);
}

 
float KeyboardNavigation::GetDistance(const Point& ptFrom, const Point& ptTo)
{
    float xDiff = ptFrom.get_X() - ptTo.get_X();
    float yDiff = ptFrom.get_Y() - ptTo.get_Y();

    return sqrtf(xDiff * xDiff + yDiff * yDiff);
} 
 
UIElement* KeyboardNavigation::GetNextTab(UIElement* pDO, UIElement* pContainer, bool goDownOnly)
{
    return NULL; // TODO -- Tab stops
}

UIElement* KeyboardNavigation::GetPrevTab(UIElement* pDO, UIElement* pContainer, bool goDownOnly)
{
    return NULL; // TODO -- Tab stops
}

UIElement* KeyboardNavigation::PredictFocusedElement(UIElement* pSource, FocusNavigationDirection::Enum direction)
{
    if(pSource == NULL)
    {
        return NULL;
    }

    m_pNavigationProperty = &(get_DirectionalNavigationProperty());
    m_verticalBaseline = Float::get_NegativeInfinity();
    m_horizontalBaseline = Float::get_NegativeInfinity();

    return GetNextInDirection(pSource, direction);
}


KeyboardNavigation* KeyboardNavigation::get_Current()
{
    return Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_Keyboard()->get_KeyboardNavigation();
}

FocusNavigationDirection::Enum KeyboardNavigation::KeyToTraversalDirection(Key::Enum key)
{
    switch (key)
    {
    case Key::Left:
        return FocusNavigationDirection::Left;

    case Key::Up:
        return FocusNavigationDirection::Up;

    case Key::Right:
        return FocusNavigationDirection::Right;

    case Key::Down:
        return FocusNavigationDirection::Down;
    }
    LibraryCriticalError();

    return FocusNavigationDirection::Down;
}

 

 



}
