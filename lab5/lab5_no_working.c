#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct BMP
{
    unsigned long long size;
    unsigned long long width;
    unsigned long long height;
    unsigned char **arr;
    // unsigned long long OLDsize;
    // unsigned long long OLDwidth;
    // unsigned long long OLDheight;
    // unsigned char **OLDarr;

} BMP;

unsigned int Change_Size(unsigned char size[4]) 
{
    unsigned int res = 0;
    for (int i = 0; i < 4; i++)
        res |= size[3 - i] << (8 * i); 
    return res;
}

void Back_Change_Size(unsigned int size, unsigned char *res) 
{
    // printf("%d", size);
    for (int i = 1; i < 4; i++)
    {
        res[i] = (size << (8 * i)) >> 24; 
    }
    res[0] = size >> 21;
}

BMP Reading(char *filename)
{
    FILE *file;
    file = fopen(filename, "rb");
    BMP Temp;
    char buffer[62];
    unsigned long long checker = 0;
    fread(buffer, 62, sizeof(char), file);
    char temp1[4] = {buffer[21], buffer[20], buffer[19], buffer[18]};
    char temp2[4] = {buffer[25], buffer[24], buffer[23], buffer[22]};
    char temp3[4] = {buffer[5], buffer[4], buffer[3], buffer[2]};
    Temp.width = Change_Size(temp1);
    Temp.height = Change_Size(temp2);
    Temp.size = Change_Size(temp3);
    Temp.arr = (unsigned char **)malloc(Temp.height * sizeof(unsigned char *)); 
    for (int i = 0; i < Temp.height; i++)                                       
        Temp.arr[i] = (unsigned char *)malloc(Temp.width * sizeof(unsigned char));
    if ((Temp.width % 8) != 0)
        checker = Temp.width / 8 + 1;
    else
        checker = Temp.width / 8;
    if (checker % 4 == 0)
    {
        for (int i = Temp.height - 1; i >= 0; i--)
            for (int j = 0; j < checker; j++)
            {
                unsigned char byte = fgetc(file);
                if (j != checker - 1)
                    for (int bit = 0; bit < 8; bit++)
                        Temp.arr[i][j * 8 + bit] = (byte >> (7 - bit)) % 2;

                else if (j == checker - 1)
                {
                    for (int bit = 0; bit < Temp.width - (checker - 1) * 8; bit++)
                        Temp.arr[i][j * 8 + bit] = (byte >> (7 - bit)) % 2;
                }
            }
    }
    else
    {
        for (int i = Temp.height - 1; i >= 0; i--)
        {
            for (int j = 0; j < checker; j++)
            {
                unsigned char byte = fgetc(file);
                if (j != checker - 1)
                    for (int bit = 0; bit < 8; bit++)
                        Temp.arr[i][j * 8 + bit] = (byte >> (7 - bit)) % 2;
                else if (j == checker - 1)
                {
                    for (int bit = 0; bit < Temp.width - (checker - 1) * 8; bit++)
                        Temp.arr[i][j * 8 + bit] = (byte >> (7 - bit)) % 2;
                }
            }
            for (int k = 0; k < 4 - (checker % 4); k++)
                fgetc(file);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        printf("\n");
        for (int j = 0; j < 10; j++)
        {
            printf("%d ", Temp.arr[i][j]);
        }
    }
    printf("\n\n\n\n\n");
    

    return Temp;
}

int findneighbors(BMP old, int y, int x)
{
    int cnt = 0;

    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
            if (!(dy == 0 && dx == 0))
            {
                if (old.arr[(y + dy + old.height) % old.height][(x + dx + old.width) % old.width] == 1)
                {
                    cnt++;
                    // printf("%d$$", old.arr[y][x]);
                    // printf("%d  %d", y, x);
                    // printf("^^%d  %d", dy, dx);
                    // printf("|||");
                }
            }
    return cnt;
}

void lifegame(BMP old, BMP new)
{            
    int neigh; 
    // for (int i = 0; i < 4; i++)
    // {
    //     // printf("\n");
    //     // for (int j = 0; j < 10; j++)
    //     // {
    //     //     printf("%d ", old.arr[i][j]);
    //     // }
    // }
    // printf("\n\n\n\n\n");
    for (int i = 0; i < old.height; i++)
    { 
        for (int j = 0; j < old.width; j++)
        {                                     
            neigh = findneighbors(old, i, j); 
            if (old.arr[i][j] == 1)
            {
                if (neigh >= 2 && neigh < 4)
                    new.arr[i][j] = 1;
                else
                    new.arr[i][j] = 0;
            }
            else
            {
                if (neigh == 3)
                    new.arr[i][j] = 1;
                else
                    new.arr[i][j] = 0;
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        printf("\n");
        for (int j = 0; j < 10; j++)
        {
            printf("%d ", new.arr[i][j]);
        }
    }
    printf("\n\n\n\n\n");
}

void Create(BMP old, char *file_old, char *file_new)
{
    BMP new;
    new.size = old.size;
    new.width = old.width;
    new.height = old.height;
    FILE *old_file = fopen(file_old, "rb");
    FILE *new_file = fopen(file_new, "wb");
    char buffer[62] = {0};
    fread(buffer, sizeof(char), 62, old_file);
    fwrite(buffer, sizeof(char), 62, new_file);

    unsigned long long checker = 0;
    if ((old.width % 8) == 0) 
        checker = old.width / 8;
    else
        checker = old.width / 8 + 1;
    if (checker % 4 == 0)
	{
		for (int i = old.height - 1; i >= 0; i--)
			for (int j = 0; j < checker; j++)
			{
				unsigned char byte = 0;
				if (j != checker - 1)
					for (int bit = 7; bit >= 0; bit--)
						byte += old.arr[i][j * 8 + 7 - bit] << bit;

				else if (j == checker - 1)
				{
					for (int bit = 7; bit >= 8 - (int)(old.width - (checker - 1) * 8); bit--)
					{
						byte += old.arr[i][j * 8 + 7 - bit] << bit;
					}
				}
				fputc(byte, new_file);
			}
	}
	else
	{
		for (int i = old.height - 1; i >= 0; i--)
		{
			for (int j = 0; j < checker; j++)
			{
				unsigned char byte = 0;
				if (j != checker - 1)
					for (int bit = 7; bit >= 0; bit--)
						byte += old.arr[i][j * 8 + 7 - bit] << bit;

				else if (j == checker - 1)
				{
					for (int bit = 7; bit >= 8 - (int)(old.width - (checker - 1) * 8); bit--)
						byte += old.arr[i][j * 8 + 7 - bit] << bit;
				}
				fputc(byte, new_file);
			}
			for (int k = 0; k < 4 - (checker % 4); k++)
				fputc(0, new_file);
		}
	}
}

int main(int argc, char **argv)
{
    FILE *filename = "D:/university/laba_proga/laba_5/zm.bmp";
    char *directoryname = "D:/university/laba_proga/laba_5/";
    int max_iter = 5;
    int dump_freq = 1;
    char new_adress[100];
    char new_filename[100];
    int flag1 = 1;
    int flag2 = 1;
    // for (int i = 1; i < argc; i += 1)
    // { //тут чтобы четко считывать все данные, я проверяю каждый аргумент на соотвествие тому, что в функции memcmp
    //     if (strcmp(argv[i], "--input") == 0)
    //     {
    //         filename = argv[i + 1]; //сюда название файла
    //         flag1++;                //чекер сделан для проверки ниже, на случай, если у меня не будет дерриктории или файла (стока 273)
    //     }
    //     if (strcmp(argv[i], "--output") == 0)
    //     {
    //         directoryname = argv[i + 1]; //сюда дериктории для создания бмп
    //         flag2++;
    //     }
    //     if (strcmp(argv[i], "--max_iter") == 0)
    //         max_iter = atoi(argv[i + 1]); //количество итераций
    //     if (strcmp(argv[i], "--dump_freq") == 0)
    //         dump_freq = atoi(argv[i + 1]); //через сколько сохранять файл
    // }
    // if (flag1 != 1)
    // {
    //     printf("error:\n not found file"); //та самая проверочка с чекером
    // }
    // if (flag2 != 1)
    // {
    //     printf("error:\n not found file out"); //та самая проверочка с чекером
    // }
    BMP Bmp;
    Bmp = Reading(filename);
    BMP old;
    BMP new;
    old = Bmp;
    new = Bmp;
    for (int i = 0; i < max_iter; i++)
    {
        int counter = 1;
        if (counter >= dump_freq)
        {
            memmove(new_adress, directoryname, strlen(directoryname)); 
            sprintf(new_filename, "%d", i + 1);
            strncat(new_filename, ".bmp ", 5);                           
            strncat(new_adress, new_filename, strlen(new_filename) + 1); 
            Create(old, filename, new_filename);
            counter = 1;
        }

        lifegame(old, new);
        counter++;

        old = new;
    }

    return 0;
}

// char buffer[62];
// fread(buffer, 62, sizeof(char), filename);
//     //временные массивы для более удобного перевода
//     char temp1[4] = {buffer[21], buffer[20], buffer[19], buffer[18]};
//     char temp2[4] = {buffer[25], buffer[24], buffer[23], buffer[22]};
//     char temp3[4] = {buffer[5], buffer[4], buffer[3], buffer[2]};
//     // перевод через побитовый сдвиг
//     Photo.width = Change_Size(temp1);
//     Photo.height = Change_Size(temp2);
//     Photo.size = Change_Size(temp3);
//     // выделение памяти и считывание файла
//     unsigned char *Data = (unsigned char *)malloc((Photo.size - 62) * sizeof(unsigned char));
//     fread(Data, sizeof(unsigned char), Photo.size, file);
//     int **mas = (int **)malloc(Photo.height * sizeof(int *));
//     for (int i = 0; i < Photo.height; i++)
//         mas[i] = (int *)malloc(Photo.width * sizeof(int));
//     int k = 0;
//     for (int i = Photo.height - 1; i >= 0; i--)
//     {
//         for (int j = 0; j < Photo.width; j++)
//         {
//             if (Data[k] == 255)
//             {
//                 printf("%d ", Data[k]);
//                 mas[i][j] = 0;
//             }
//             else
//             {
//                 printf("%d ", Data[k]);
//                 mas[i][j] = 1;
//             }
//             k += 1;
//         }
//     }
//     for (int i = 0; i < Photo.height; i++)
//     {
//         for (int j = 0; j < Photo.width; j++)
//         {
//             printf("%d ", mas[i][j]);
//         }
//         printf("\n");