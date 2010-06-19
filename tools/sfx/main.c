#include <a2x.h>

void a2x(void)
{
	a2x_set("title", "a2x_sfx");
	a2x_set("version", "0.1");
	a2x_set("author", "Alex");
	a2x_set("window", "no");
	a2x_set("quiet", "yes");
}

#define TYPE uint16_t

void Main(void)
{
    if(a_argsNum != 4) {
        printf("Error: %s inputFile file.c file.h", a_args[0]);
        return;
    }

    char* const inputFile = a_args[1];
    char* const cFile = a_args[2];
    char* const hFile = a_args[3];

    char* const soundName = a_str_extractName(inputFile);

    int eLength;
    const int oLength = a_file_size(inputFile);

    File* const soundFile = a_file_openRead(inputFile);
    TYPE* const soundData = malloc(oLength);
    a_file_rp(soundFile, soundData, oLength);
    a_file_close(soundFile);

    TYPE* const dst = a_mem_encodeRLE(soundData, oLength / sizeof(TYPE), sizeof(TYPE), &eLength);

    free(soundData);

    File* const h = a_file_openWriteText(hFile);
    File* const c = a_file_openWriteText(cFile);

    // header

    fprintf(h, "#ifndef H_%s_H\n", soundName);
    fprintf(h, "#define H_%s_H\n\n", soundName);

    fprintf(h, "#include <stdint.h>\n\n");
    fprintf(h, "extern " a_stringify(TYPE) " sfx_%s_data[%d];\n\n", soundName, eLength);

    fprintf(h, "#endif\n");

    // body

    fprintf(c, "#include \"%s\"\n\n", a_str_extractFile(hFile));
    fprintf(c, a_stringify(TYPE) " sfx_%s_data[%d] = {", soundName, eLength);

    for(int i = 0; i < eLength; i++) {
        if(!(i % 8)) fprintf(c, "\n    ");
        fprintf(c, "0x%04x,", dst[i]);
    }

    fprintf(c, "\n};\n");

    a_file_close(h);
    a_file_close(c);

    printf("Wrote '%s' and '%s', saved %d%%\n", cFile, hFile, 100 - 100 * eLength * sizeof(TYPE) / oLength);
}
