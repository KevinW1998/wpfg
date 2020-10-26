#include <Windows.h>
#include <stdio.h>

// Fonts are constructed such that there is a 1 pixel border around every glyph
const int c_iPadding = 1;

#pragma pack(1)
typedef struct
{
    byte  identsize;          // size of ID field that follows 18 byte header (0 usually)
    byte  colourmaptype;      // type of colour map 0=none, 1=has palette
    byte  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    short colourmapstart;     // first colour map entry in palette
    short colourmaplength;    // number of colours in palette
    byte  colourmapbits;      // number of bits per palette entry 15,16,24,32

    short xstart;             // image x origin
    short ystart;             // image y origin
    short width;              // image width in pixels
    short height;             // image height in pixels
    byte  bits;               // image bits per pixel 8,16,24,32
    byte  descriptor;         // image descriptor bits (vh flip bits)
    
    // pixel data follows header
    
} TGA_HEADER;

typedef struct
{
    WCHAR wzFontName[64];
    float fontSize;
    float cellHeight;
    int beginGlyph;
    int endGlyph;
    int unknownGlyph;
} FTD_HEADER;

typedef struct
{
    int leadIn;
    int glyphWidth;
    int leadOut;
    float U1;
    float V1;
    float U2;
    float V2;
} FTD_ENTRY;

void WriteGlyphData(const char* szFontName, TEXTMETRIC* pTextMetric, float fontSize, HDC hdc, unsigned int startGlyph, unsigned int endGlyph, 
                    unsigned int textureWidth, unsigned int textureHeight, const char* szOutputFile)
{
    unsigned long cbWritten;
    bool isTrueType = pTextMetric->tmPitchAndFamily & TMPF_TRUETYPE;
    char szFileName[80];
    
    _snprintf_s(szFileName, 80, "%s.ftd", szOutputFile);

    HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    

    FTD_HEADER ftdHeader;

    ZeroMemory(&ftdHeader, sizeof(FTD_HEADER));

    for(unsigned int idx = 0; idx < strlen(szFontName); idx++)
    {
        ftdHeader.wzFontName[idx] = szFontName[idx];
    }

    ftdHeader.fontSize = fontSize;
    ftdHeader.cellHeight = pTextMetric->tmHeight;
    ftdHeader.beginGlyph = startGlyph;
    ftdHeader.endGlyph = endGlyph;
    ftdHeader.unknownGlyph = pTextMetric->tmDefaultChar;

    WriteFile(hFile, &ftdHeader, sizeof(FTD_HEADER), &cbWritten, NULL);

    unsigned int xPos = c_iPadding;
    unsigned int yPos = c_iPadding;

    for(unsigned int curGlyph = startGlyph; curGlyph < endGlyph; curGlyph++)
    {
        int glyphWidth = 0;
        int xOffset = 0;
        int leadIn = 0;
        int leadOut = 0;
        
        if(isTrueType)
        {
            ABC abc;
            ABCFLOAT abcFloat;

            GetCharABCWidthsW(hdc, curGlyph, curGlyph, &abc);            

            glyphWidth = abc.abcB;
            leadIn = abc.abcA;
            leadOut = abc.abcC;
        }
        else
        {
            int width;

            GetCharWidth32W(hdc, curGlyph, curGlyph, &width);
            
            glyphWidth = width;
        }
        
        if((xPos + glyphWidth + c_iPadding) >= textureWidth)
        {
            xPos = c_iPadding;
            yPos += pTextMetric->tmHeight + (2 * c_iPadding);
        }

        FTD_ENTRY ftdEntry;

        ftdEntry.leadIn = leadIn;
        ftdEntry.glyphWidth = glyphWidth;
        ftdEntry.leadOut = leadOut;
        ftdEntry.U1 = (xPos * 1.0f) / textureWidth;
        ftdEntry.V1 = (yPos * 1.0f) / textureHeight;
        ftdEntry.U2 = ((xPos + glyphWidth) * 1.0f) / textureWidth;
        ftdEntry.V2 = ((yPos + pTextMetric->tmHeight) * 1.0f) / textureHeight;

        WriteFile(hFile, &ftdEntry, sizeof(FTD_ENTRY), &cbWritten, NULL);

        xPos += glyphWidth + 2 * c_iPadding;
    }


    CloseHandle(hFile);
}

void WriteTGA(unsigned char* pbData, unsigned int width, unsigned int height, const char* szOutputFile)
{
    TGA_HEADER tgaHeader;
    
    ZeroMemory(&tgaHeader, sizeof(TGA_HEADER));
    
    tgaHeader.imagetype = 2;
    tgaHeader.width = width;
    tgaHeader.height = height;
    tgaHeader.bits = 32;
    tgaHeader.descriptor = 8;

    char szFileName[80];
    
    _snprintf_s(szFileName, 80, "%s.tga", szOutputFile);

    HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    unsigned long cbWritten;

    
    WriteFile(hFile, &tgaHeader, sizeof(TGA_HEADER), &cbWritten, NULL);

    unsigned char* rgData = (unsigned char*) malloc(width * height * 4);
    unsigned int cPixels = width * height;
    
    unsigned char* pbIn = pbData;
    unsigned char* pbOut = rgData;
        
    for(unsigned int idx = 0; idx < cPixels; idx++)
    {
        *pbOut = 0xFF;
        pbOut++;

        *pbOut = 0xFF;
        pbOut++;

        *pbOut = 0xFF;
        pbOut++;

        *pbOut = *pbIn;
        pbOut++;
        
        pbIn += 3;
    }
    
        
    WriteFile(hFile, rgData, width*height*4, &cbWritten, NULL);
    
    CloseHandle(hFile);
    

    free(rgData);    
}


void GetTextureSize(HDC hdc, TEXTMETRIC* pTextMetrics, unsigned int startGlyph, unsigned int endGlyph, unsigned int* pWidth, unsigned int* pHeight)
{
    bool isTrueType = pTextMetrics->tmPitchAndFamily & TMPF_TRUETYPE;
    
    unsigned int totalWidth = 0;    

    for(unsigned int curGlyph = startGlyph; curGlyph < endGlyph; curGlyph++)
    {
        if(isTrueType)
        {
            ABC abc;

            GetCharABCWidthsW(hdc, curGlyph, curGlyph, &abc);            
            
            totalWidth += abc.abcB;
        }
        else
        {
            int width;

            GetCharWidth32W(hdc, curGlyph, curGlyph, &width);
            
            totalWidth += width;
        }
        
        totalWidth += 2 * c_iPadding;
    }

    unsigned int totalArea = (pTextMetrics->tmHeight + 2 * c_iPadding) * totalWidth;
    
    unsigned int targetWidth = 32;
    unsigned int targetHeight = 32;
    
    while(targetWidth * targetHeight < totalArea)
    {
        if(targetWidth <= targetHeight)
        {
            targetWidth *= 2;
        }    
        else
        {
            targetHeight *= 2;
        }
    }
    

    unsigned int calculatedWidth = c_iPadding;
    unsigned int calculatedHeight = c_iPadding;
    for(unsigned int curGlyph = startGlyph; curGlyph < endGlyph; curGlyph++)
    {
        unsigned int glyphWidth = 0;
        
        if(isTrueType)
        {
            ABC abc;

            GetCharABCWidthsW(hdc, curGlyph, curGlyph, &abc);            
            
            glyphWidth = abc.abcB;
        }
        else
        {
            int width;

            GetCharWidth32W(hdc, curGlyph, curGlyph, &width);
            
            glyphWidth = width;
        }        
        
        if( ((calculatedWidth + glyphWidth) + 2 * c_iPadding) > targetWidth)
        {
            calculatedWidth = c_iPadding;
            calculatedHeight += pTextMetrics->tmHeight + (2 * c_iPadding);
        }

        calculatedWidth += glyphWidth + 2 * c_iPadding;
    }
    
    while(targetHeight < calculatedHeight)
    {
        targetHeight *= 2;
    }


    *pWidth = targetWidth;
    *pHeight = targetHeight;
}

void CreateFontBitmap(char *fontname, unsigned int fontSize, unsigned int startGlyph, unsigned int endGlyph, const char* outputFile)
{
    HDC hdc=GetDC(NULL);
    HDC mdc = CreateCompatibleDC(hdc);

    // This is negative for character height versus cell height
    HFONT hf = CreateFont(-fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
    PROOF_QUALITY, DEFAULT_PITCH, fontname);         

    HBITMAP bmDpi = CreateCompatibleBitmap(hdc, 32, 32);
    SelectObject(mdc, bmDpi);

    SelectObject(mdc, hf);      
    SetMapMode(mdc, MM_TEXT);
    SetBkMode(mdc, TRANSPARENT);
    SetTextColor(mdc, 0xFFFFFF);

    TEXTMETRIC textMetric;        
    GetTextMetrics(mdc, &textMetric);
    bool isTrueType = textMetric.tmPitchAndFamily & TMPF_TRUETYPE;
    

    unsigned int textureWidth;
    unsigned int textureHeight;
    GetTextureSize(mdc, &textMetric, startGlyph, endGlyph, &textureWidth, &textureHeight);


    HBITMAP bm = CreateCompatibleBitmap(hdc, textureWidth, textureHeight);
    SelectObject(mdc, bm);

    RECT r;
    r.top=0;
    r.left=0;
    r.right=textureWidth;
    r.bottom=textureHeight;
    FillRect(mdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));    

    unsigned int xPos = c_iPadding;
    unsigned int yPos = c_iPadding;
    for(unsigned int curGlyph = startGlyph; curGlyph < endGlyph; curGlyph++)
    {
        int glyphWidth = 0;
        int xOffset = 0;
        
        if(isTrueType)
        {
            ABC abc;

            GetCharABCWidthsW(mdc, curGlyph, curGlyph, &abc);            
            
            glyphWidth = abc.abcB; // + abc.abcA + abc.abcC;
            xOffset = -abc.abcA;
        }
        else
        {
            int width;

            GetCharWidth32W(mdc, curGlyph, curGlyph, &width);
            
            glyphWidth = width;
        }
        
        if((xPos + glyphWidth + c_iPadding) >= textureWidth)
        {
            xPos = c_iPadding;
            yPos += textMetric.tmHeight + (2 * c_iPadding);
        }

        TextOutW(mdc, xPos + xOffset, yPos, (WCHAR*) &curGlyph, 1);
        
        xPos += glyphWidth + 2 * c_iPadding;
    }


    BITMAPFILEHEADER bfh;
    
    bfh.bfType = 19778;
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + textureWidth * textureHeight * 3;
    bfh.bfReserved1 = bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    
    BITMAPINFO bmi;    

    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth=textureWidth;
    bmi.bmiHeader.biHeight=textureHeight;
    bmi.bmiHeader.biPlanes=1;
    bmi.bmiHeader.biBitCount=24;
    bmi.bmiHeader.biCompression=BI_RGB;

    unsigned char* pRGBAData = (unsigned char*)malloc(textureWidth * textureHeight * 3);

    GetDIBits( mdc, bm, 0, textureHeight, pRGBAData, &bmi, DIB_RGB_COLORS);

    DeleteObject(hf);
    DeleteObject(bm);    

    WriteTGA(pRGBAData, textureWidth, textureHeight, outputFile);
    WriteGlyphData(fontname, &textMetric, fontSize, mdc, startGlyph, endGlyph, textureWidth, textureHeight, outputFile);
    
    free(pRGBAData);
}

int main(unsigned int argc, char** argv)
{
    const unsigned int startGlyph = 31;
    const unsigned int endGlyph = 128;
    
    if(argc != 4)
    {
        printf("Usage: %s <FontName> <FontSize> <OutputFile>\n", argv[0]);
        return 1;
    }

    int fontSize = atoi(argv[2]);
    
    CreateFontBitmap(argv[1], fontSize, startGlyph, endGlyph, argv[3]);

    return 0;
}
