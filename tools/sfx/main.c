#include <a2x.h>

void a2x(void)
{
	a2x_set("title", "a2x_sfx");
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

    File* const sfxFile = a_file_openRead(inputFile);
    char* const sfxName = a_str_extractName(inputFile);

    const int length = a_file_size(inputFile) / sizeof(uint16_t);
    uint16_t* const sfxData = (uint16_t*)a_file_toBuffer(inputFile);

    File* const h = a_file_openWriteText(hFile);
    File* const c = a_file_openWriteText(cFile);

    // header

    fprintf(h, "#ifndef H_%s_H\n", sfxName);
    fprintf(h, "#define H_%s_H\n\n", sfxName);

    fprintf(h, "#include <stdint.h>\n\n");
    fprintf(h, "extern uint16_t sfx_%s[%d];\n\n", sfxName, length);

    fprintf(h, "#endif\n");

    // body

    fprintf(c, "#include \"%s\"\n\n", a_str_extractFile(hFile));
    fprintf(c, "uint16_t sfx_%s[%d] = {", sfxName, length);

    for(int i = 0; i < length; i++) {
        if(!(i % 8)) fprintf(c, "\n    ");
        fprintf(c, "0x%04x,", sfxData[i]);
    }

    fprintf(c, "\n};\n");

    a_file_close(h);
    a_file_close(c);

    free(sfxName);
    free(sfxData);
}
