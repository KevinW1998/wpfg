#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class Transform;

class GeometryBuilder;

class Geometry : public Animatable
{
    friend class RenderData;
public:
    DECLARE_ELEMENT(Geometry, Animatable);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Transform, Transform, NULL, PropertyFlag::None);    

    virtual void BuildGeometry(GeometryBuilder* pGeometry) = 0;

    virtual Rect GetBounds() = 0;
    virtual bool HitTest(const Point& pt, int hitTestFlags, float strokeThickness) = 0;

private:

};

class StreamGeometryContext;
class GeometryTypeConverter : public TypeConverter
{
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }

private:
    static TRefCountedPtr<Geometry> ParseGeometry(const String& pathString);
    static void ParseStringToStreamGeometryContext(StreamGeometryContext* pContext, const String& pathString, FillRule::Enum* pFillRule);

};


}; // namespace AVUI
