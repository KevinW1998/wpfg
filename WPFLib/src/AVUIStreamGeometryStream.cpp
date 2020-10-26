#include <AVUICommon.h>
#include <AVUIStreamGeometryStream.h>
#include <memory.h>

namespace AVUI {


void StreamGeometryStream::WriteRecord(BaseRecord* pRecord)
{
    int cbCur = m_streamBuffer.get_Count();
    m_streamBuffer.AddEmptyItems(pRecord->sizeOfRecord);

    unsigned char* pbCur = m_streamBuffer.get_Ptr() + cbCur;

    memcpy(pbCur, pRecord, pRecord->sizeOfRecord);
}

StreamGeometryStream::BaseRecord* StreamGeometryStream::GetNextRecord(int* pcbCur)
{
    if(*pcbCur >= m_streamBuffer.get_Count())
    {
        return NULL;
    }

    BaseRecord* pRecord = (BaseRecord*) (m_streamBuffer.get_Ptr() + *pcbCur);
    (*pcbCur) += pRecord->sizeOfRecord;

    return pRecord;
}


}; // namespace AVUI
