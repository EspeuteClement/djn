#include <cstdio>

#pragma warning( push)
#pragma warning( disable : 26451 6011 6262 6308 6387 28182 ) // Arithmetic overflow
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#pragma warning( pop )
#include <windows.h>
#include <cstdint>

#include <cstring>

#include <direct.h>

struct ImageData {
	int w; 
	int h;

	uint16_t* data;

	ImageData(int _w, int _h) : w(_w), h(_h)
	{
		data = (uint16_t*) malloc(w*h * sizeof(uint16_t));
	}

	~ImageData()
	{
		free(data);
	}

	inline uint16_t& getPixel(int x, int y)
	{
		return data[x + y*w];
	}
};

char* gImagePath = "res/data.png";

ImageData* djnPakLoadImage(const char* imagePath)
{
	int comp;
	int w, h;
	unsigned char* tempimage = stbi_load(imagePath, &w, &h, &comp, 4);

	if (tempimage == nullptr)
	{
		printf("Couldn't load image file %s.\n", imagePath);
		return nullptr;
	}

	ImageData* data = new ImageData(w, h);

	for (int y = 0; y < data->h; ++y)
	{
		for (int x = 0; x < data->w; ++x)
		{
			uint16_t bytes =
				((int)((float)tempimage[(x + y * w) * 4 + 0] / 256.0f * 32.0f))			|	// R
				((int)((float)tempimage[(x + y * w) * 4 + 1] / 256.0f * 32.0f) << 5)	|	// G
				((int)((float)tempimage[(x + y * w) * 4 + 2] / 256.0f * 32.0f) << 10)	|	// B
				((int)((float)tempimage[(x + y * w) * 4 + 3] / 256.0f * 2.0f) << 15);		// A
			data->getPixel(x, y) = bytes;
		}
	}

	stbi_image_free(tempimage);

	return data;
}

void parse_image(FILE* output, ImageData* Data)
{
	int position = 0;
	for (int y = 0; y < Data->h; ++y)
	{
		for (int x = 0; x < Data->w; ++x)
		{
			char bfr[10];
			if (x > 0 || y > 0)
				fwrite(",", 1, 1, output);
			fprintf_s(output, "0x%04X", (int)Data->getPixel(x,y));
		}
	}
}

void parse_template()
{
	FILE* output = nullptr;
	FILE* templ = nullptr;

	ImageData* Data = djnPakLoadImage(gImagePath);

	const char* templPath = "res/template.c";
	errno_t outerr = fopen_s(&output, "_bin/output.c", "w");
	errno_t err = fopen_s(&templ, templPath, "r");

	if (Data != nullptr 
		&& outerr == 0
		&& err == 0)
	{
		int c = 0;
		do
		{
			c = getc(templ);
			if (c == EOF)
				break;
			if (c != '$')
			{
				fwrite(&c, 1, 1, output);
			}
			else
			{
				char buffer[256] = { 0 };
				char* ptr = buffer;
				do
				{
					c = getc(templ);
					if (c < 'a' || c > 'z' )
					{
						break;
					}
					else
					{
						*ptr = c;
						ptr++;
					}
				} while (c != EOF && ptr - buffer < sizeof(buffer) - 1);

				if (strcmp(buffer, "content") == 0)
				{
					parse_image(output, Data);
				}
				else if (strcmp(buffer, "width") == 0)
				{
					fprintf_s(output, "%d", Data->w);
				}
				else if (strcmp(buffer, "height") == 0)
				{
					fprintf_s(output, "%d", Data->h);
				}

				fwrite(&c, 1, 1, output);
			}
		} while (c != EOF);
	}
	else
	{
		if (Data == nullptr) {}
		else if (output == nullptr)
			printf("Couldn't open output file\n");
		else if (templ == nullptr)
			printf("Couldn't open template file %s\n", templPath);
	}

	if (Data)
		delete Data;
	if (templ)
		fclose(templ);
	if (output)
		fclose(output);
}

int main(int argc, char* argv[]) {
	char bfr[256];
	_getcwd(bfr, 256);
	printf("%s\n", bfr);

	parse_template();
	return 1;
}

