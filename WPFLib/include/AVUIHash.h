#pragma once

namespace AVUI {

// Hash function taken from Bob Jenkins. 

class Hash
{
public:
    static unsigned int HashData(const unsigned char* pbData, unsigned int cbLen, unsigned int initialValue);
};

};
