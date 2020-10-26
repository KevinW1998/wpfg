#include <AVUITransform.h>

namespace AVUI {

class TransformCollection;

class TransformGroup : public Transform
{
public:
    TransformGroup();
    ~TransformGroup() { }


    DECLARE_ELEMENT(TransformGroup, Transform);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Children, TransformCollection, true);

    TransformCollection* get_Children() { return m_pChildren; }

    virtual Matrix get_Value();
    void CloneCore(Animatable* pSource);

private:
    TRefCountedPtr<TransformCollection> m_pChildren;
};

typedef TRefCountedPtr<TransformGroup> TransformGroupPtr;


}; // namespace AVUI
