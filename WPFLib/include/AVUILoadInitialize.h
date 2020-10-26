#pragma once

namespace AVUI {

class INameScope;
class ILoadInitialize
{
public:

    virtual void BeginInit() = 0;
    virtual void EndInit() = 0;
};

};
