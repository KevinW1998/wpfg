#pragma once

namespace AVUI {

class IClientFormattedText;
class IClientTextGeometry;

class FormattedText : public Object
{
public:
    FormattedText(IClientFormattedText* pFormattedText) : m_pClientFormattedText(pFormattedText) { }
    ~FormattedText();

    float get_Width() const;
    float get_Height() const;
    int get_Cch() const;

    void Trim(float trimWidth);

    float GetXLocationBeforeIndex(int index) const;
    IClientFormattedText* get_ClientFormattedText() { return m_pClientFormattedText; }

private:
    IClientFormattedText* m_pClientFormattedText;
};

};
