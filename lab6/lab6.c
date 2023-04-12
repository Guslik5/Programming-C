#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned int Back_Change_Size(unsigned char size[4])
{
    unsigned int res = 0;
    for (int i = 0; i < 4; i++)
        res += size[3 - i] << (8 * i);
    return res;
}

void Change_Size(unsigned int size, unsigned char *res)
{
    // printf("%d", size);
    for (int i = 0; i < 4; i++)
    {
        res[i] = (size << (8 * i)) >> 24;
    }
}

void extract(FILE *arhive)
{
    FILE *file;
    char sim;
    sim = fgetc(arhive);
    fseek(arhive, 0, SEEK_SET);
    while (!feof(arhive))
    {
        if (sim == EOF)
            break;

        char arr_size_fileName[4];
        fread(arr_size_fileName, sizeof(char), sizeof(arr_size_fileName), arhive);
        int size_fileName_int = Back_Change_Size(arr_size_fileName);
        char arr_fileName[size_fileName_int + 1];
        fread(arr_fileName, sizeof(char), size_fileName_int, arhive);
        arr_fileName[size_fileName_int] = '\0';
        file = fopen(arr_fileName, "wb");
        char arr_size_file[4];
        fread(arr_size_file, sizeof(char), sizeof(arr_size_file), arhive);
        unsigned long long size_file_int = Back_Change_Size(arr_size_file);
        for (int i = 0; i < size_file_int; i++)
        {
            sim = fgetc(arhive);
            fputc(sim, file);
        }
        sim = fgetc(arhive);
        fseek(arhive, -1, SEEK_CUR);
        printf("\nextract: ");
        for (int i = 0; i < size_fileName_int; i++)
        {
            printf("%c", arr_fileName[i]);
        }
    }
    fclose(arhive);
}

void list(FILE *arhive)
{
    int counter_files = 0;
    char sim = fgetc(arhive);
    fseek(arhive, 0, SEEK_SET);
    // fseek(arhive, 0, SEEK_END);
    // long long size_file = ftell(arhive);
    // fseek(arhive, 0, SEEK_SET);
    while (!feof(arhive))
    {
        if (sim == EOF)
            break;
        char arr_size_fileName[4];
        fread(arr_size_fileName, sizeof(char), sizeof(arr_size_fileName), arhive);
        int size_fileName_int = Back_Change_Size(arr_size_fileName);
        char arr_fileName[size_fileName_int];
        fread(arr_fileName, sizeof(char), size_fileName_int, arhive);
        char arr_size_file[4];
        fread(arr_size_file, sizeof(char), sizeof(arr_size_file), arhive);
        unsigned long long size_file_int = Back_Change_Size(arr_size_file);
        fseek(arhive, size_file_int, SEEK_CUR);
        counter_files++;
        printf("%d)",counter_files);
        for (int i = 0; i < size_fileName_int; i++)
        {
            printf("%c", arr_fileName[i]);
        }
        printf("  size: %lli\n", size_file_int);
        sim = fgetc(arhive);
        fseek(arhive, -1, SEEK_CUR);

    }
    printf("                       <<number of files: %d >>", counter_files);
}

void creat(FILE *arhive, char *file_name)
{
    char sim;
    FILE *file;
    file = fopen(file_name, "rb");
    if (file == NULL)
    {
        printf("\nerror: file ");
        for (int i = 0; i < strlen(file_name); i++)
        {
            printf("%c", file_name[i]);
        }
        
        printf(" not found");
    }
    else
    {
        fseek(file, 0, SEEK_END);
        unsigned long long size_file_int = ftell(file);
        fseek(file, 0, SEEK_SET);
        char arr_size_file[4];
        Change_Size(size_file_int, arr_size_file);

        char arr_size_fileName[4];
        int size_fileName_int = strlen(file_name);
        Change_Size(size_fileName_int, arr_size_fileName);

        fwrite(arr_size_fileName, sizeof(char), sizeof(arr_size_fileName), arhive);
        fwrite(file_name, sizeof(char), size_fileName_int, arhive);
        fwrite(arr_size_file, sizeof(char), sizeof(arr_size_file), arhive);
        for (int i = 0; i < size_file_int; i++)
        {
            sim = fgetc(file);
            fputc(sim, arhive);
        }

        printf("\nadd: ");
        for (int i = 0; i < size_fileName_int; i++)
        {
            printf("%c", file_name[i]);
        }
        fclose(file);
        remove(file_name);
    }
}
// 1085176    12048461


int main(int argc, char **argv)
{
    FILE *file;
    FILE *arhive ;//= fopen("arhive.arc", "rb");
    // char file_name[] = "tib.mp3";
    // char file_name2[] = "file2.txt";
    // char file_name3[] = "bitok.mp3";
    char *arhive_name;
    if (argc < 4)
    {
        printf("error\n invalid numbers arguments");
        return 1;
    }
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "--file"))
        {
            arhive_name = argv[i + 1];
        }

        if (!strcmp(argv[i], "--create"))
        {
            printf("\n============================== create ==============================\n");
            arhive = fopen(arhive_name, "ab");
            for (int i = 4; i < argc; i++)
            {
                creat(arhive, argv[i]);
            }
            fclose(arhive);
            printf("\n====================================================================\n");
        }
        if (!strcmp(argv[i], "--list"))
        {
            printf("=============================== list ===============================\n");
            arhive = fopen(arhive_name, "rb");
            list(arhive);
            fclose(arhive);
            printf("\n====================================================================\n");
        }
        if (!strcmp(argv[i], "--extract"))
        {
            printf("\n============================== extract ==============================\n");
            arhive = fopen(arhive_name, "rb");
            extract(arhive);
            fclose(arhive);
            remove(arhive_name);
            printf("\n=====================================================================\n");
            // if (remove(arhive_name) != 0)
            // {
            //    printf("no") ;
            // }
            // else
            //     printf("yes");
        }
    }

    // arc --file data.arc --create a.txt b.bin c.bmp

    // creat(arhive, file_name);
    // creat(arhive,file_name2);
    // creat(arhive,file_name3);
    // list(arhive);
    // extract(arhive);
    return 0;
}