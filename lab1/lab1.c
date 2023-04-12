#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 3) 
    {
        printf("error\n");
        printf("invalid number of arguments");
        return 1;
    }


        if ((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--words") == 0) || (strcmp(argv[1], "--bytes") == 0) || (strcmp(argv[1], "--lines") == 0))
        {
            FILE *fin;
            fin = fopen(argv[2], "r");
            int pred_cim, slov = 1, ctr = 1, counter = 0;
            int cim;
            cim = fgetc(fin);
            pred_cim = cim;
            if (fin != NULL)
            {
                while (cim != EOF)
                {
                    counter++;
                    if (cim == '\n') 
                    {
                        ctr++;
                    }
                    if (isspace(cim) && (isspace(pred_cim) == 0))
                    {
                        slov++;
                    }
                    pred_cim = cim;
                    cim = fgetc(fin);
                }
                if (counter == 0)
                {
                    slov = 0;
                    ctr = 0;
                }
                fclose(fin);
                if ((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--bytes") == 0))
                {
                    printf("%d\n", counter);
                }
                if ((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "--lines") == 0))
                {
                    printf("%d\n", ctr);
                }
                if ((strcmp(argv[1], "-w")==0) || (strcmp(argv[1], "--words")==0))
                {
                    printf("%d\n", slov);
                }
                
                return 0;
            }
            else
            {
                printf("error\n");
                printf("file not found");
                return 1;
            }
        }
        else
        {
            printf("error\n");
            printf("invalid argument");
            return 1;
        }
}