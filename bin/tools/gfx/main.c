#include <a2x.h>

void a2x(void)
{
	a2x_set("title", "a2x_gfx");
	a2x_set("version", "0.2");
	a2x_set("author", "Alex");
	a2x_set("window", "no");
	a2x_set("quiet", "yes");
}

void Main(void)
{
    if(a_argsNum != 4) {
        printf("Error: %s inputFile file.c file.h", a_args[0]);
        return;
    }

    char* const inputFile = a_args[1];
    char* const cFile = a_args[2];
    char* const hFile = a_args[3];

    const int length = a_file_size(inputFile);
    char* const gfxName = a_str_extractName(inputFile);
    uint8_t* const gfxData = a_file_toBuffer(inputFile);

    File* const hf = a_file_open(hFile, "w");
    File* const cf = a_file_open(cFile, "w");

    FILE* const h = a_file_file(hf);
    FILE* const c = a_file_file(cf);

    // header

    fprintf(h, "#ifndef H_%s_H\n", gfxName);
    fprintf(h, "#define H_%s_H\n\n", gfxName);

    fprintf(h, "#include <stdint.h>\n\n");
    fprintf(h, "extern uint8_t gfx_%s[];\n\n", gfxName);

    fprintf(h, "#endif\n");

    // body

    fprintf(c, "#include \"%s\"\n\n", a_str_extractFile(hFile));
    fprintf(c, "uint8_t gfx_%s[] = {", gfxName);

    for(int i = 0; i < length; i++) {
        if(!(i % 8)) fprintf(c, "\n    ");
        fprintf(c, "0x%02x,", gfxData[i]);
    }

    fprintf(c, "\n};\n");

    a_file_close(hf);
    a_file_close(cf);

    free(gfxName);
    free(gfxData);
}
