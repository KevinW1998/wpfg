#pragma once

namespace AVUI {

class IClientCursor;

class Cursor : public Object
{
    friend class Cursors;
public:
    DECLARE_ELEMENT(Cursor, Object);

    Cursor(String* pCursorName);

    CursorType::Enum get_CursorType() { return m_cursorType; }

private:
    Cursor(CursorType::Enum cursorType);

    IClientCursor* m_pClientCursor;
    CursorType::Enum m_cursorType;
};


class CursorTypeConverter : public TypeConverter
{
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};



};
