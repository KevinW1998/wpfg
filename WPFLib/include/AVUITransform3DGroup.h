#include <AVUITransform.h>
#include <AVUITransform3DCollection.h>

namespace AVUI {

class Transform3DGroup : public Transform3D
{
public:
    Transform3DGroup();
    ~Transform3DGroup() { m_children.InvalidateWeakReferences(); }


    DECLARE_ELEMENT(Transform3DGroup, Transform3D);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Children, Transform3DCollection, true);

    Transform3DCollection* get_Children() { return &m_children; }

    virtual Matrix3D get_Value();
    void CloneCore(Animatable* pSource);

private:
    Transform3DCollection m_children;
};

typedef TRefCountedPtr<Transform3DGroup> Transform3DGroupPtr;


}; // namespace AVUI
