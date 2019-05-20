#include <cstdio>
#define STB_IMAGE_IMPLEMENTATION

#pragma warning( push)
#pragma warning( disable : 26451 6011 6262 6308 6387 28182 ) // Arithmetic overflow
#include <stb/stb_image.h>
#pragma warning( pop )
#include <windows.h>
#include <cstdint>

#include <cstring>

#define GET_R(x,y) (data[(x + y * w)*4 + 0])
#define GET_G(x,y) (data[(x + y * w)*4 + 1])
#define GET_B(x,y) (data[(x + y * w)*4 + 2])
#define GET_A(x,y) (data[(x + y * w)*4 + 3])

struct ImageData {
	int w; 
	int h;

	uint8_t* data;
};

int g_image_w;
int g_image_h;
unsigned char* g_image_data;

int InitImageData(int w, int h, ImageData* data)
{
	
}

int open_image()
{
	int comp;
	g_image_data = stbi_load("res/data.png", &g_image_w, &g_image_h, &comp, 4);

	if (g_image_data == nullptr)
	{
		printf("Couldn't load file");
		return 0;
	}
	return 1;
}

void close_image()
{
	stbi_image_free(g_image_data);
}

void parse_image(FILE* output)
{

	int w = g_image_w;
	int h = g_image_h;
	unsigned char* data = g_image_data;

	// Generated data is : 0x1234,

	int position = 0;
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			char bfr[10];
			if (x > 0 || y > 0)
				fwrite(",", 1, 1, output);

			uint16_t bytes =
				((int)((float)GET_R(x, y) / 256.0f * 32.0f)) |
				((int)((float)GET_G(x, y) / 256.0f * 32.0f) << 5) |
				((int)((float)GET_B(x, y) / 256.0f * 32.0f) << 10) |
				((int)((float)GET_A(x, y) / 256.0f * 2.0f) << 15);
			sprintf_s(bfr, sizeof(bfr), "0x%04X", (int)bytes);
			fwrite(bfr, 6, 1, output);
			//printf("(%d,%d,%d,%d),", GET_R(x,y), GET_G(x, y), GET_B(x, y), GET_A(x, y));
		}
	}
}

void parse_template()
{
	FILE* output = fopen("output.c", "w");
	FILE* templ = fopen("tool/template.c", "r");

	if (output && templ)
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
				} while (c != EOF && ptr - buffer < 256 - 1);

				if (strcmp(buffer, "content") == 0)
				{
					parse_image(output);
				}
				else if (strcmp(buffer, "width") == 0)
				{
					fprintf_s(output, "%d", g_image_w);
				}
				else if (strcmp(buffer, "height") == 0)
				{
					fprintf_s(output, "%d", g_image_h);
				}

				fwrite(&c, 1, 1, output);
			}
		} while (c != EOF);
	}
	else
	{
		printf("Couldn't open output");
	}


	if (templ)
		fclose(templ);
	if (output)
		fclose(output);
}

int main(int argc, char* argv[]) {
	printf("Hello world\n");
	printf("%s\n", argv[0]);
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	printf("Current working dir: %s\n", buf);

	if (!open_image());
		return -1;
	parse_template();
	close_image();

	return 1;
}

