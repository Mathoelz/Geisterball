#include "rgbimage.h"
#include "color.h"
#include "assert.h"

#include <math.h>       /* sqrt */

RGBImage::RGBImage( unsigned int Width, unsigned int Height):m_Image(NULL)
{
	this->m_Width = Width;
	this->m_Height = Height;
	this->m_Image = new Color[Width * Height];
}

RGBImage::~RGBImage()
{
	//delete [] this->m_Image;
    this->m_Image = NULL;
}

void RGBImage::setPixelColor( unsigned int x, unsigned int y, const Color& c)
{
	if (x + y > this->m_Width * this->m_Height || x+y < 0) {

	}
	else {
		this->m_Image[x + this->m_Width * y] = c;
	}
}

const Color& RGBImage::getPixelColor( unsigned int x, unsigned int y) const
{
    if(x >= this->width()){
        x = this->width() -1;
    }
    if(y >= this->height()){
        y = this->height()-1;
    }
    return this->m_Image[x + this->m_Width * y];
}

unsigned int RGBImage::width() const
{
	return this->m_Width;
}
unsigned int RGBImage::height() const
{
	return this->m_Height;
}

unsigned char RGBImage::convertColorChannel( float v)
{
	if (v > 1.0f) {
		return 255;
	}
	else if (v < 0.0f) {
		return 0;
	}
	else
	{
		return 255 * v;
	}
}

bool RGBImage::saveToDisk( const char* Filename)
{
	unsigned int headers[13];
	FILE* outfile;
	int paddedsize;
	unsigned int x;
	int y; 
	int n;
	int red;
	int blue;
	int green;

	paddedsize = (this->m_Width * 3 * this->m_Height);

	headers[0] = paddedsize + 54;	// whole file size
	headers[1] = 0;					// bfReserved
	headers[2] = 54;				// bfOffbits
	headers[3] = 40;				// biSize
	headers[4] = this->m_Width;
	headers[5] = this->m_Height;

	headers[7] = 0;
	headers[8] = paddedsize;
	headers[9] = 0;
	headers[10] = 0;
	headers[11] = 0;
	headers[12] = 0;

	outfile = fopen(Filename, "wb");

	fprintf(outfile, "BM");

	for (n = 0; n <= 5; ++n)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	fprintf(outfile, "%c", 1);
	fprintf(outfile, "%c", 0);
	fprintf(outfile, "%c", 24);
	fprintf(outfile, "%c", 0);

	for (n = 7; n <= 12; ++n)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	for (y = this->m_Height - 1; y >= 0; --y)
	{
		for (x = 0; x <= this->m_Width - 1; ++x)
		{
			red = this->convertColorChannel(this->getPixelColor(x, y).R);
			blue = this->convertColorChannel(this->getPixelColor(x, y).B);
			green = this->convertColorChannel(this->getPixelColor(x, y).G);

			fprintf(outfile, "%c", blue);
			fprintf(outfile, "%c", green);
			fprintf(outfile, "%c", red);
		}
	}

	fclose(outfile);
	return true;
}
/*
  Quellen:  https://www.proggen.org/doku.php?id=c:lib:assert:assert
            https://www.c-plusplus.net/forum/topic/59867/was-ist-assert-und-was-macht-das/3
            https://stackoverflow.com/questions/45047672/sobel-filter-algorithm-c-no-libraries
            https://www.computerbase.de/forum/threads/c-angew-bildverarbeitung-sobel-prewitt-filter.77641/
 */
RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor){
        // mit assert die Bildgrößen auf Gleichheit überprüfen
    assert((src.height() == dst.height()) && (src.width() == dst.width()));
    
        // K ist die Filter-Maske für den Sobel- Operator
    float K[3][3] = {
        {1.0f, 0.0f, -1.0f},
        {2.0f, 0.0f, -2.0f},
        {1.0f, 0.0f, -1.0F}
    };
    
        // KT ist die Transponierte Filter-Maske für den Sobel- Operator
    float KT[3][3] = {
        { 1.0f,  2.0f,  1.0f},
        { 0.0f,  0.0f,  0.0f},
        {-1.0f, -2.0f, -1.0f}
    };
    RGBImage U(src.m_Width, src.m_Height);
    RGBImage V(src.m_Width, src.m_Height);
        //I ist das Eingangsbild
    RGBImage I = src;
    
    for (int x = 1; x < src.height() ; x++) {
    for (int y = 1; y < src.width() ; y++) {
        
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                
                Color U_color = U.getPixelColor(x, y) + I.getPixelColor(x + i, y + j) * K[i + 1][j + 1];
                Color V_color = V.getPixelColor(x, y) + I.getPixelColor(x + i, y + j) * KT[i + 1][j + 1];
                
                U.setPixelColor(x, y, U_color);
                V.setPixelColor(x, y, V_color);
            }
        }
            // Da bei Graustufen alle Werte gleich sind, nehmen wir R
        float U_pow_2 = U.getPixelColor(x, y).R * U.getPixelColor(x, y).R;
        float V_pow_2 = V.getPixelColor(x, y).R * V.getPixelColor(x, y).R;
    
        float S = sqrt(U_pow_2 + V_pow_2) * factor;
        dst.setPixelColor(x, y, Color(S,S,S));
        }
        
    }
    return dst;
   
}
