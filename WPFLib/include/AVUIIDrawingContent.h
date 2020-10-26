
namespace AVUI {

class IDrawingContent
{
public:
    virtual ~IDrawingContent() { };
    virtual const Rect& get_ContentBounds() = 0;
    virtual bool HitTestPoint(const Point& point) = 0;
};
 
};
