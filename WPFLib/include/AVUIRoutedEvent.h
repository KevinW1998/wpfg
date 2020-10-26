#pragma once

namespace AVUI {

class RoutedEvent : public Object
{
public:
    DECLARE_ELEMENT(RoutedEvent, Object);

    RoutedEvent(const char* szName, const Type& ownerType, RoutingStrategy::Enum routingStrategy) :
        m_szName(szName), m_ownerType(ownerType), m_routingStrategy(routingStrategy)
    { 

    }

    bool operator==(const RoutedEvent& rhs) const
    {
        if(this == &rhs)
        {
            return true;
        }

        return false;
    }

    bool operator!=(const RoutedEvent& rhs) const
    {
        return !((*this) == rhs);
    }

    const char* get_Name() const { return m_szName; }
    const Type& get_OwnerType() const { return m_ownerType; }
    RoutingStrategy::Enum get_RoutingStrategy() const { return m_routingStrategy; }
private:

    RoutedEvent(const RoutedEvent&);

    const char* m_szName;
    const Type& m_ownerType;
    RoutingStrategy::Enum m_routingStrategy;
};

};
