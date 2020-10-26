#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class Renderer;
class AnimationTimeline;

class Visual3D : public DependencyObject
{
    friend class Visual3DCollection;
public:
    
    Visual3D() : m_pInheritanceContext(NULL) { };

    DECLARE_ELEMENT(Visual3D, DependencyObject);

    virtual void Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix) { };
    virtual void RenderDependencies3D(Renderer* pRenderer) { };

    void BeginAnimation(const DependencyProperty& dp, AnimationTimeline* pTimeline, HandoffBehavior::Enum handoffBehavior);


    virtual DependencyObject* get_InheritanceContext();
    virtual void AddInheritanceContext(DependencyObject*, const DependencyProperty* pDP);
    virtual void RemoveInheritanceContext(DependencyObject*, const DependencyProperty* pDP);

private:

    virtual void EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry);
    DependencyObject* m_pInheritanceContext;

};


}; // namespace AVUI
