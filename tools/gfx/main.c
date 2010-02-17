#include <a2x.h>

void a2x(void)
{
	a2x_set("title", "a2x_gfx");
	a2x_set("version", "0.1");
	a2x_set("author", "Alex");
	a2x_set("window", "no");
	a2x_set("quiet", "yes");
}

#define TYPE uint16_t

#define A_GC
#define a_mem_gcOn()
#define a_mem_gcOff()

void Main(void)
{
    if(a_argsNum != 4) {
        a_error("%s inputFile file.c file.h", a_args[0]);
        return;
    }

    a_mem_gcOn();

    char* const inputFile = a_args[1];
    char* const cFile = a_args[2];
    char* const hFile = a_args[3];

    char* const graphicName = A_GC a_str_extractName(inputFile);
    Sheet* const sheet = a_blit_makeSheetFromFile(inputFile);

    const int width = sheet->w;
    const int height = sheet->h;

    int eLength;
    const int oLength = width * height;

    TYPE* const dst = a_mem_encodeRLE(sheet->data, oLength, sizeof(TYPE), &eLength);

    a_blit_freeSheet(sheet);

    File* const h = a_file_openWriteText(hFile);
    File* const c = a_file_openWriteText(cFile);

    // header

    fprintf(h, "#ifndef H_%s_H\n", graphicName);
    fprintf(h, "#define H_%s_H\n\n", graphicName);

    fprintf(h, "#include <stdint.h>\n\n");
    fprintf(h, "extern int gfx_%s_w;\n", graphicName);
    fprintf(h, "extern int gfx_%s_h;\n", graphicName);
    fprintf(h, "extern " a_stringify(TYPE) " gfx_%s_data[%d];\n\n", graphicName, eLength);

    fprintf(h, "#endif\n");

    // body

    fprintf(c, "#include \"%s\"\n\n", a_str_extractFile(hFile));
    fprintf(c, "int gfx_%s_w = %d;\n", graphicName, width);
    fprintf(c, "int gfx_%s_h = %d;\n\n", graphicName, height);
    fprintf(c, a_stringify(TYPE) " gfx_%s_data[%d] = {", graphicName, eLength);

    for(int i = 0; i < eLength; i++) {
        if(!(i % 8)) fprintf(c, "\n    ");
        fprintf(c, "0x%04x,", dst[i]);
    }

    fprintf(c, "\n};\n");

    a_file_close(h);
    a_file_close(c);

    printf("Wrote '%s' and '%s', saved %d%%\n", cFile, hFile, 100 - 100 * eLength / oLength);

    a_mem_gcOff();
}
