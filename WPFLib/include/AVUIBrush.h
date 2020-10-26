#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class Renderer;
class Transform;

class Brush : public Animatable
{
public:

    DECLARE_ELEMENT(Brush, Animatable);

    virtual void set_ActiveBrush(Renderer* pRenderer) = 0;
    virtual void RenderDependencies(Renderer* pRenderer) { };
    virtual bool ContainsRenderDependencies() const { return false; };

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(Transform, Transform, NULL, DependencyPropertyMetadata(PropertyFlag::None, OnTransformChanged));
    Transform* get_Transform() const { return m_pTransform; }
    void set_Transform(Transform* pTransform) { SetValue(get_TransformProperty(), (Object*) pTransform); }

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(RelativeTransform, Transform, NULL, DependencyPropertyMetadata(PropertyFlag::None, OnRelativeTransformChanged));
    Transform* get_RelativeTransform() const { return m_pRelativeTransform; }
    void set_RelativeTransform(Transform* pTransform) { SetValue(get_TransformProperty(), (Object*) pTransform); }

protected:
    Brush() : m_pTransform(NULL), m_pRelativeTransform(NULL) { }
    virtual ~Brush() { }

private:

    static void OnTransformChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnRelativeTransformChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    Transform* m_pTransform;
    Transform* m_pRelativeTransform;

};

class BrushTypeConverter : public TypeConverter
{
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};



typedef TRefCountedPtr<Brush> BrushPtr;

}; // namespace AVUI
