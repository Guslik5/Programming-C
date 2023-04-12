#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Tag   //структуры тега ID3
{
    char buffer[10];
    char signature[3];
    char version[1];
    char sub_version[1];
    char flag[1];
    unsigned char size[4];
} Tag;

typedef struct Frame   // структура фреймов
{
    char buffer[10];
    char ID[4];
    unsigned char size[4];
    char flag[2];
} Frame;

unsigned int Change_Size(unsigned char size[4])   //побитовый сдвиг(из массивы в инт)
{
    unsigned int res = 0;
    for (int i = 0; i < 4; i++)
        res |= size[3 - i] << (7 * i);   // так как незначащий 0 в начале байта
    return res;
}

void Back_Change_Size(unsigned int size, unsigned char *res)  // обратный побитовый сдвиг(из инта в массив)
{
    // printf("%d", size);
    for (int i = 1; i < 4; i++)
    {
        res[i] = (size << (7 * i)) >> 21;   // так как незначащий 0 в начале байта
    }
    res[0] = size >> 21;
}

void addTag(char buffer[], Tag *Tag) // функция разбития тега в структуру из буфера тега
{
    int j = 0;
    for (int i = 0; i < 10; i++)
    {
        if (i < 3)
            Tag->signature[i] = Tag->buffer[i];
        else if (i == 3)
            Tag->version[0] = Tag->buffer[i];
        else if (i == 4)
            Tag->sub_version[0] = Tag->buffer[i];
        else if (i == 5)
            Tag->flag[0] = Tag->buffer[i];
        else if (i > 5)
        {
            Tag->size[j] = Tag->buffer[i];
            j++;
        }
    }
}

void addFrame(char buffer[], Frame *Frame)    //  функция разбития фрейма в структуру из буфера фрейма
{
    int q = 0;
    int j = 0;
    for (int i = 0; i < 10; i++)
    {
        if (i < 4)
            Frame->ID[i] = Frame->buffer[i];
        else if ((i >= 4) && (i < 8))
        {
            Frame->size[j] = Frame->buffer[i];
            j++;
        }
        else if (i >= 8)
        {
            Frame->flag[q] = Frame->buffer[i];
            q++;
        }
    }
}

void show(FILE *file) // функция выводящая всю метоинформацию из mp3 файла
{
    Tag Tag;
    fread(Tag.buffer, sizeof(char), 10, file); // считывание в буфер тега и его разбитие
    addTag(Tag.buffer, &Tag); // и его разбитие
    unsigned int tagsize;
    tagsize = Change_Size(Tag.size);  // воспользуемся побитовым сдвигом, чтобы узнать размер тега
    printf("%.3sv2.%d\n", Tag.signature, Tag.version[0]);  // выведем тег

    while (ftell(file) <= tagsize) // будем работать, пока не выйдем за границы тега(считываем фреймы)
    {
        Frame Frame;
        fread(Frame.buffer, sizeof(char), 10, file);  // считываем все в буфер
        addFrame(Frame.buffer, &Frame);  // разбиваем буфер
        if (Frame.ID[0] == 0)
            break;
        // аналогичная работа с фреймами как с тегом
        unsigned int framesize;
        framesize = Change_Size(Frame.size);
        char arr[framesize];
        fread(arr, sizeof(char), framesize, file);
        printf("%.4s: ", Frame.ID);
        for (int i = 0; i < framesize; i++)
        {
            printf("%c", arr[i]);
        }
        printf("%c", '\n');
    }
}

void get(FILE *file, char prop_name[]) // функция вывода определенного фрейма
{
    Tag Tag;
    // считываем тег и находим его размер
    fread(Tag.buffer, sizeof(char), 10, file);
    addTag(Tag.buffer, &Tag);
    unsigned int tagsize;
    tagsize = Change_Size(Tag.size);
    while (ftell(file) <= tagsize)
    {
        Frame Frame;
        fread(Frame.buffer, sizeof(char), 10, file);  // считываем фреймы в буфер и после их разбиваем в структуру
        addFrame(Frame.buffer, &Frame);
        if (Frame.ID[0] == 0)
            break;
        unsigned int framesize;
        framesize = Change_Size(Frame.size);
        if (strcmp(Frame.ID, prop_name) == 0) // поиск нужного нам фрейма и дальнеший вывод если нашли нужный
        {
            printf("%s: ", Frame.ID);
            char arr[framesize];
            fread(arr, sizeof(char), framesize, file);
            for (int i = 0; i < framesize; i++)
            {
                printf("%c", arr[i]);
            }
        }
        else
        {
            fseek(file, framesize, 1);  // если фрейм не искомый, тогда перепрыгиваем его
        }
    }
}

void set(FILE *file, char prop_name[], char value[], char *file_name)
{
    int nowptr = 0;
    FILE *file2;
    file2 = fopen("C:\\Users\\Guslik\\Desktop\\copy.mp3", "wb");  // создание копии, чтобы пока все положить туда
    fseek(file, 0, SEEK_END);
    int lastptr = ftell(file); // находим размер 
    fseek(file, 0, SEEK_SET);
    Tag Tag;
    fread(Tag.buffer, sizeof(char), 10, file); // считывание тега
    addTag(Tag.buffer, &Tag);
    unsigned int tagsize;
    tagsize = Change_Size(Tag.size);
    while (ftell(file) <= tagsize)
    {
        Frame Frame;
        fread(Frame.buffer, sizeof(char), 10, file); // считывание фрейма в буфер
        addFrame(Frame.buffer, &Frame);
        if (Frame.ID[0] == 0)
            break;
        unsigned int framesize;
        framesize = Change_Size(Frame.size); // нахождение размера фрейма
        if (strcmp(Frame.ID, prop_name) == 0)  // перезапись нужного нам фрейма
        {
            char arr[framesize];
            fread(arr, sizeof(char), framesize, file);
            fseek(file, -10 - framesize, SEEK_CUR);
            nowptr = ftell(file);
            fseek(file, 0, SEEK_SET);
            tagsize = tagsize - framesize + strlen(value) + 1;
            Back_Change_Size(tagsize, Tag.size);

            tagsize = Change_Size(Tag.size);
            // складываем все в копию в правильном порядке
            for (int i = 0; i < 6; i++)
            {
                char temp;
                temp = fgetc(file);
                fputc(temp, file2);
            }
            for (int i = 0; i < 4; i++)
            {
                fputc(Tag.size[i], file2);
            }
            fseek(file, 4, SEEK_CUR);

            while (ftell(file) < nowptr + 4)
            {
                char temp;
                temp = fgetc(file);
                fputc(temp, file2);
            }
            Back_Change_Size(strlen(value) + 1, Frame.size);
            for (int i = 0; i < 4; i++)
            {
                fputc(Frame.size[i], file2);
                fgetc(file);
            }
            for (int i = 0; i < 2; i++)
            {
                fputc(Frame.flag[i], file2);
                fgetc(file);
            }
            fputc('\0', file2);
            for (int i = 0; i < strlen(value); i++)
            {
                fputc(value[i], file2);
            }
            fseek(file, framesize, SEEK_CUR);
            while (ftell(file) < lastptr)  // дописываем в копию весь оставшийся материал
            {
                char temp;
                temp = fgetc(file);
                fputc(temp, file2);
            }
        }
        else
        {
            fseek(file, framesize, 1); // пропуск
        }
    }
    fclose(file);
    fclose(file2);
    // открываем файлы по другому и перезаписываем из копии в наш файл
    file2 = fopen("C:\\Users\\Guslik\\Desktop\\copy.mp3", "rb");
    file = fopen(file_name, "wb");
    while (!feof(file2)) // возвращаем все обратно в исходный файл
    {
        char temp;
        temp = fgetc(file2);
        fputc(temp, file);
    }
}
int main(int argc, char **argv)
{
    char *file_name, *prop_name, *value;
    if (argc > 4)  // проверка, что в терминале все верно введено
    {
        printf("error");
        return 1;
    }
    else
    {
        if (strstr(argv[1], "--filepath"))
        {
            file_name = strpbrk(argv[1], "=") + 1;
            FILE *file = fopen(file_name, "rb");
            if (strcmp(argv[2], "--show") == 0)  // вызов функции show
            {
                if (argc != 3) //проверка, что функция show вызывается корректно
                {
                    printf("error");
                    return 1;
                }
                else
                    show(file);
            }
            else if (strstr(argv[2], "--get") != NULL)  // вызов функции get
            {
                if (argc != 3) // проверка,  что функция get вызывается корректно
                {
                    printf("error");
                    return 1;
                }
                else
                {
                    prop_name = strpbrk(argv[2], "=") + 1;
                    get(file, prop_name);
                }
            }
            else if (strstr(argv[2], "--set") != NULL)  // вызов функции set
            {
                if (argc != 4) // проверка, что set вызвана корректно
                {
                    printf("error");
                    return 1;
                }
                else
                {
                    prop_name = strpbrk(argv[2], "=") + 1;
                    value = strpbrk(argv[3], "=") + 1;
                    set(file, prop_name, value, file_name);
                }
            }
        }
        else
        {
            printf("error");
            return 1;
        }
    }
    return 0;
}