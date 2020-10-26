#include <AVUICommon.h>
#include <AVUICursor.h>
#include <AVUICursors.h>

REGISTER_ELEMENT_BEGIN(AVUI::Cursor)
    REGISTER_TYPE_CONVERTER(CursorTypeConverter)
REGISTER_ELEMENT_END()

namespace AVUI {

Cursor::Cursor(String* pCursorName)
{

}

Cursor::Cursor(CursorType::Enum cursorType) : m_pClientCursor(NULL), m_cursorType(cursorType)
{

}





bool CursorTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}
ObjectPtr CursorTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    if(pObjectFrom == NULL)
    {
        LibraryCriticalError();
        return NULL;
    }

    if(!CanConvertFrom(pObjectFrom->get_Type()))
    {
        LibraryCriticalError();
        return NULL;
    }


    const TypeConverter* pTypeConverter = GET_TYPE(CursorType).get_TypeConverter();

    ObjectPtr pCursorType = pTypeConverter->ConvertFrom(pObjectFrom);

    CursorType::Enum cursorType = (CursorType::Enum) UnboxValue<int>(pCursorType);

    Cursor* pCursor = NULL;

    switch(cursorType)
    {
    case CursorType::None:
        return Cursors::get_None();

    case CursorType::No:
        return Cursors::get_No();

    case CursorType::Arrow:
        return Cursors::get_Arrow();

    case CursorType::AppStarting:
        return Cursors::get_AppStarting();

    case CursorType::Cross:
        return Cursors::get_Cross();

    case CursorType::Help:
        return Cursors::get_Help();

    case CursorType::IBeam:
        return Cursors::get_IBeam();

    case CursorType::SizeAll:
        return Cursors::get_SizeAll();

    case CursorType::SizeNESW:
        return Cursors::get_SizeNESW();

    case CursorType::SizeNS:
        return Cursors::get_SizeNS();

    case CursorType::SizeNWSE:
        return Cursors::get_SizeNWSE();

    case CursorType::SizeWE:
        return Cursors::get_SizeWE();

    case CursorType::UpArrow:
        return Cursors::get_UpArrow();

    case CursorType::Wait:
        return Cursors::get_Wait();

    case CursorType::Hand:
        return Cursors::get_Hand();

    case CursorType::Pen:
        return Cursors::get_Pen();

    case CursorType::ScrollNS:
        return Cursors::get_ScrollNS();

    case CursorType::ScrollWE:
        return Cursors::get_ScrollWE();

    case CursorType::ScrollAll:
        return Cursors::get_ScrollAll();

    case CursorType::ScrollN:
        return Cursors::get_ScrollN();

    case CursorType::ScrollS:
        return Cursors::get_ScrollS();

    case CursorType::ScrollW:
        return Cursors::get_ScrollW();

    case CursorType::ScrollE:
        return Cursors::get_ScrollE();

    case CursorType::ScrollNW:
        return Cursors::get_ScrollNW();

    case CursorType::ScrollNE:
        return Cursors::get_ScrollNE();

    case CursorType::ScrollSW:
        return Cursors::get_ScrollSW();

    case CursorType::ScrollSE:
        return Cursors::get_ScrollSE();

    case CursorType::ArrowCD:
        return Cursors::get_ArrowCD();
    }

    return pCursor;
}




}; // namespace AVUI


