#include <AVUIIDrawingContent.h>
#include <AVUICollections.h>

namespace AVUI {

class IClientRenderGeometry;
class IClientTextGeometry;
class IClientRendererInterface;

class Renderer;
class GeometryBuilder;
class Geometry;
class Pen;
class Brush;
class FormattedText;
class Transform;

class RenderData : public Object, public IDrawingContent
{
public:
    class RenderCommand
    {
    public:
        enum RenderCommandEnum
        {
            DrawEllipse,
            DrawImage,
            DrawLine,
            DrawRoundedRectangle,
            DrawGeometry,
            DrawText
        };
    };

    struct RenderDataCommand
    {
        virtual RenderCommand::RenderCommandEnum get_CommandType() const = 0;
        virtual const void* get_Data() const = 0;
        virtual unsigned int get_Len() const = 0;

        virtual void Render(Renderer* pRenderer) = 0;
        virtual void RenderDependencies(Renderer* pRenderer) = 0;
        virtual bool ContainsRenderDependencies() const = 0;
        virtual void BuildGeometry(GeometryBuilder* pGeometryBuilder) { };
        virtual void Clear() { };
        virtual void ClearClientGeometry() { };
        virtual bool IsPointInside(const Point& pt) const = 0;
        virtual Rect get_Bounds() const = 0;
        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const = 0;
    };

    struct DrawTextCmd : public RenderDataCommand
    {
    public:
        DrawTextCmd(RenderData* pRenderData, FormattedText* pFormattedText, Brush* pBrush, const Point& origin);

        RenderCommand::RenderCommandEnum get_CommandType() const { return RenderCommand::DrawText; }
        const void* get_Data() const { return this; }
        unsigned int get_Len() const { return sizeof(*this); }
        virtual bool IsPointInside(const Point& pt) const { return get_Bounds().Contains(pt); }
        Rect get_Bounds() const { return m_bounds; }

        Brush* get_Brush() { return m_pBrush; }

        virtual void Render(Renderer* pRenderer);
        virtual void Clear();

        virtual bool ContainsRenderDependencies() const;
        void RenderDependencies(Renderer* pRenderer);

        virtual void BuildGeometry(GeometryBuilder* pGeometryBuilder);
        virtual void ClearClientGeometry();

        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const;

    private:
        FormattedText* m_pFormattedText;
        Brush* m_pBrush;
        IClientTextGeometry* m_pTextGeometry;
        Point m_origin;
        Rect m_bounds;
        bool m_isCombined;
    };

    struct RenderGeometryDataCommand : public RenderDataCommand
    {
    public:
        RenderGeometryDataCommand(Brush* pBrush, Pen* pPen);

        Brush* get_Brush() { return m_pBrush; }
        Pen* get_Pen() { return m_pPen; }

        virtual void Render(Renderer* pRenderer);
        virtual void Clear();
        virtual void ClearClientGeometry();
        virtual bool ContainsRenderDependencies() const;
        void RenderDependencies(Renderer* pRenderer);

        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const;

    protected:
        Brush* m_pBrush;
        Pen* m_pPen;

        IClientRenderGeometry* m_pRenderGeometryStroke;
        IClientRenderGeometry* m_pRenderGeometryFill;
    };

    struct DrawEllipseCmd : public RenderGeometryDataCommand
    {
        DrawEllipseCmd(Brush* pBrush, Pen* pPen, const Point& center, float radiusX, float radiusY) : RenderGeometryDataCommand(pBrush, pPen), m_center(center), m_radiusX(radiusX), m_radiusY(radiusY) { }
        RenderCommand::RenderCommandEnum get_CommandType() const { return RenderCommand::DrawEllipse; }
        const void* get_Data() const { return this; }
        unsigned int get_Len() const { return sizeof(*this); }

        const Point& get_Center() const { return m_center; }
        float get_RadiusX() const { return m_radiusX; }
        float get_RadiusY() const { return m_radiusY; }

        virtual void BuildGeometry(GeometryBuilder* pGeometryBuilder);
        virtual bool IsPointInside(const Point& pt) const;
        virtual Rect get_Bounds() const;

        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const;

    private:
        Point m_center;
        float m_radiusX;
        float m_radiusY;
    };

    struct DrawLineCmd : public RenderGeometryDataCommand
    {
        DrawLineCmd(Pen* pPen, const Point& point0, const Point& point1) : RenderGeometryDataCommand(NULL, pPen), m_point0(point0), m_point1(point1) { }
        RenderCommand::RenderCommandEnum get_CommandType() const { return RenderCommand::DrawLine; }
        const void* get_Data() const { return this; }
        unsigned int get_Len() const { return sizeof(*this); }


        const Point& GetPoint0() const { return m_point0; }
        const Point& GetPoint1() const { return m_point1; }

        virtual void BuildGeometry(GeometryBuilder* pGeometryBuilder);

        virtual bool IsPointInside(const Point& pt) const;
        virtual Rect get_Bounds() const;

        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const;

    private:
        Point m_point0;
        Point m_point1;
        unsigned int m_penIndex;
    };

    struct DrawGeometryCmd : public RenderGeometryDataCommand
    {
        DrawGeometryCmd(Brush* pBrush, Pen* pPen, Geometry* pGeometry, Transform* pTransform);
        RenderCommand::RenderCommandEnum get_CommandType() const { return RenderCommand::DrawGeometry; }
        const void* get_Data() const { return this; }
        unsigned int get_Len() const { return sizeof(*this); }

        virtual void BuildGeometry(GeometryBuilder* pGeometryBuilder);

        virtual bool IsPointInside(const Point& pt) const;
        virtual Rect get_Bounds() const;

        virtual void Clear();

        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const;

    private:
        Geometry* m_pGeometry;
        Transform* m_pTransform;
    };

    struct DrawRoundedRectangleCmd : public RenderGeometryDataCommand
    {
        DrawRoundedRectangleCmd(Brush* pBrush, Pen* pPen, const Rect& rectangle, float radiusX, float radiusY) : RenderGeometryDataCommand(pBrush, pPen), m_rect(rectangle), m_radiusX(radiusX), m_radiusY(radiusY) { }
        RenderCommand::RenderCommandEnum get_CommandType() const { return RenderCommand::DrawRoundedRectangle; }
        const void* get_Data() const { return this; }
        unsigned int get_Len() const { return sizeof(*this); }

        const Rect& get_Rect() const { return m_rect; }
        float get_RadiusX() const { return m_radiusX; }
        float get_RadiusY() const { return m_radiusY; }

        virtual void BuildGeometry(GeometryBuilder* pGeometryBuilder);
        virtual bool IsPointInside(const Point& pt) const;
        virtual Rect get_Bounds() const;

        virtual bool IsIdenticalTo(RenderDataCommand* pCommand) const;

    private:
        float m_radiusX;
        float m_radiusY;
        Rect m_rect;

    };

    void BuildGeometry(GeometryBuilder* pGeometryBuilder);
    void Render(Renderer* pRenderer);

    void WriteDataRecord(const RenderDataCommand& cmd);

    const Rect& get_ContentBounds();
    bool HitTestPoint(const Point& point);

    void ClearRenderData();
    void ClearClientGeometry();

    RenderDataCommand* get_FirstCommand();
    RenderDataCommand* get_NextCommand(RenderDataCommand* pPreviousCommand);

    unsigned int get_CommandCount() const { return m_cCmds; }

    bool ContainsRenderDependencies() { return m_containsRenderDependencies; }
    void RenderDependencies(Renderer* pRenderer);


    RenderData();
    ~RenderData();
private:
    unsigned int m_cCapacity;
    unsigned int m_cCmds;
    unsigned int m_cCount;
    unsigned char* m_pbData;
    float m_geometryScale;
    bool m_containsRenderDependencies;

    Rect m_contentBounds;
    DrawTextCmd* m_pCommandLastText;
};

}; // namespace AVUI
