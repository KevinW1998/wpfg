#pragma once

namespace AVUI
{

class PropertyChangedEventArgs : public EventArgs
{
public:
    DECLARE_ELEMENT(PropertyChangedEventArgs, EventArgs);

    PropertyChangedEventArgs(const Property& property) : m_property(property)
    {

    }

    const Property& get_Property() const { return m_property; };

private:
    const Property& m_property;
};


class INotifyPropertyChanged
{
public:
    virtual void add_PropertyChanged(TDelegate<PropertyChangedEventArgs>* pEventHandler) = 0;
    virtual void remove_PropertyChanged(TDelegate<PropertyChangedEventArgs>* pEventHandler) = 0;
};


DEFINE_EVENTHANDLER(PropertyChanged);

};

