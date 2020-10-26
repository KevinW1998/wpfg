#pragma once

namespace AVUI {

class RelativeSource : public Object
{
public:
    RelativeSource() : m_ancestorLevel(-1), m_mode(RelativeSourceMode::FindAncestor), m_pAncestorType(NULL) { };
    RelativeSource(RelativeSourceMode::Enum mode) : m_ancestorLevel(-1), m_mode(RelativeSourceMode::FindAncestor), m_pAncestorType(NULL) { InitializeMode(mode); };

    DECLARE_ELEMENT(RelativeSource, Object);

    int get_AncestorLevel() { return m_ancestorLevel; }
    void set_AncestorLevel(int ancestorLevel);

    const Type* get_AncestorType() { return m_pAncestorType; }
    void set_AncestorType(const Type* pType);

    RelativeSourceMode::Enum get_Mode() { return m_mode; }
    void set_Mode(RelativeSourceMode::Enum);


    static RelativeSource* get_PreviousData();
    static RelativeSource* get_Self();
    static RelativeSource* get_TemplatedParent();

private:

    void InitializeMode(RelativeSourceMode::Enum mode);
    bool IsUninitialized() { return m_ancestorLevel == -1; }

    int m_ancestorLevel;
    const Type* m_pAncestorType;
    RelativeSourceMode::Enum m_mode;    
};


}; // End namespace AVUI

