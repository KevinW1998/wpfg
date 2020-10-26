#include <AVUITileBrush.h>

namespace AVUI {

class BitmapImage;

class ImageBrush : public TileBrush
{
public:

    DECLARE_ELEMENT(ImageBrush, TileBrush);

    void set_ActiveBrush(Renderer* pRenderer);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ImageSource, BitmapImage, NULL, DependencyPropertyMetadata(PropertyFlag::None, ImageBrush::OnImageSourceChanged));

    float get_NaturalWidth();
    float get_NaturalHeight();

protected:


private:
    static void OnImageSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    BitmapImage* m_pBitmapImage;
};



}; // namespace AVUI
