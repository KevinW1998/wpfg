#include <AVUIDependencyObject.h>

namespace AVUI {

class XamlParserImpl;
class ResourceDictionary;
class Style;
class FrameworkTemplate;
class Control;
class Visual;
class UIElement;
class IXamlResolutionContext;
class ILoadInitialize;
class RoutedCommand;

class XamlParser
{
    friend class Application;
    friend class FrameworkTemplate;

public:

    XamlParser();
    ~XamlParser() { };

    
    bool StartDocument(void);
    bool BeginAttributes();
    bool AddAttribute(const WCHAR* pwchName, int cchName, const WCHAR* pwchValue, int cchValue);
    bool EndAttributes();
    bool StartElement(const WCHAR *pwchName, int cchName);
    bool EndElement();
    bool EndDocument(void);
    bool Characters(const WCHAR *pwchChars, int cchChars);

    ObjectPtr get_RootElement() { return m_pRootElement; }

    void set_CurrentLine(int currentLine) { m_currentLine = currentLine; }

private:

    class ParserContext
    {
    public:
        enum ParserContextEnum
        {
            Object,
            Property,
            ResourceDictionary,
            RequiresTypeConverter,
        };

    };
    struct XamlParserContextStack
    {
        ObjectPtr pObject;
        const Property* pProperty;
        const Type* pType;
        ParserContext::ParserContextEnum parserContext;
        Object* pKey;
    };

    void AddToInitializeList(Object* pObject);
    ObjectPtr ParseMarkupExtension(const String& localValue);
    void ParseMarkupExtensionArguments(Object* pObject, const String& extensionArgument);
    void AddMarkupExtension(Object* pObject, const String& extensionArgument, int idxPropertyStart, int idxPropertyEnd, int idxArgumentStart, int idxArgumentEnd);

    Object* get_PreviousObject();
    const Property* GetPropertyForName(const WCHAR* pwchData, int cchData, const Type* pObjectType);
    const RoutedEvent* GetRoutedEventForName(const WCHAR* pwchData, int cchData, const Type* pType);
    const RoutedCommand* GetRoutedCommandForName(const WCHAR* pwchData, int cchData, const Type* pType);

    bool IsQualifiedProperty(const WCHAR* pwchData, int cchData);

    void AddChild(Object* pObjectParent, Object* pObjectChild);
    void SetPropertyValue(Object* pObjectParent, 
                          const Property* pDependencyProperty,
                          Object* pDependencyObjectValue);
    bool IsMarkupExtension(const String& str);
    ObjectPtr ResolvePropertyAttribute(const Property* pProperty, const String& localValue);
    void InitializeTree();
    void set_TemplatedParent(DependencyObject* pParent) { m_pTemplatedParent = pParent; }
    void set_TemplateInstantiating(FrameworkTemplate* pTemplate) { m_pTemplateInstantiating = pTemplate; }
    IXamlResolutionContext* get_ResolutionContext();
    void DeferAttributeHandling(const Property* pProperty, const String& localValue);
    void HandleDeferredAttributes();
    void HandleDeferredAttribute(const Property* pProperty, const String& value);
    bool IsDeferredAttribute(const Property* pProperty, const String& value);
    int IndexOf(const WCHAR* pwch, int cch, WCHAR wchFind);


    void XamlParseError(const char* szMessage, ...);


    ObjectPtr m_pRootElement;

    ItemStructList<XamlParserContextStack> m_parserContextStack;
    static const int c_parserInitialSize = 64;

    Style* m_pStyleCurrent;
    ItemStructList<ObjectPtr > m_keyStack;

    struct DeferredAttribute
    {
        const Property* pProperty;
        String Value;
    };

    ItemStructList<DeferredAttribute> m_deferredAttributeStack;

    FrameworkTemplate* m_pTemplateCurrent;
    const Type* m_pTemplateCurType;


    DependencyObject* m_pTemplatedParent;
    FrameworkTemplate* m_pTemplateInstantiating;

    bool m_isTemplateStarted;
    int m_cCurrentStackDepth;

    struct InitializeItem
    {
        ObjectPtr pObject;
        ILoadInitialize* pInitialize;
    };

    ItemStructList<InitializeItem > m_initializeList;
    UIElement* m_pLastUIElement;
    int m_currentLine;
};


}; // namespace AVUI
