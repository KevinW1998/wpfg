
namespace AVUI
{
    class XamlParser;

class IXMLParserDataProvider
{
public:
    virtual void ReadData(unsigned char* pbData, int cbMaxData, int *pcbRead) = 0;
};

class XmlParser
{
public:
    XmlParser(IXMLParserDataProvider* pDataSource, XamlParser* pXMLSink);
    ~XmlParser();

    void Parse();

private:

    void ParseNextSection();
    void ParseNextTag();
    void ParseAttributes();

    void ReadBuffer();
    void AdvanceToNextCharacter();
    void SkipWhitespace();
    bool IsAtEOF() { return m_isAtEOF == true && m_idxBuffer == m_cbBuffered; }
    bool IsWhitespace(WCHAR wch);
    void ReadNextName();
    void ParseAttribute();
    void ParseComment();

    static const int mc_cbBufferMax = 32*1024;
    static const int mc_cbTagMax = 32*1024;

    ItemStructList<WCHAR> m_rgTagData;
    ItemStructList<unsigned char> m_rgBufferedChars;
    int m_cbBuffered;

    int m_idxBuffer;

    int m_currentLine;

    WCHAR m_wchCurrent;

    IXMLParserDataProvider* m_pDataSource;
    XamlParser* m_pXMLSink;
    bool m_isAtEOF;
};


}; // namespace AVUI
