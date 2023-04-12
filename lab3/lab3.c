#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <locale.h>

int size = 0;
typedef struct queue
{
    long long value;
    struct queue *next;
} queue_c;

queue_c *head = NULL;
queue_c *tail = NULL;

queue_c *push(long long x)
{
    queue_c *ptr = (queue_c *)malloc(sizeof(queue_c));
    if (head != NULL)
    {
        tail->next = ptr;
        ptr->value = x;
        ptr->next = NULL;
        tail = ptr;
    }
    else
    {
        ptr->value = x;
        ptr->next = NULL;
        tail = ptr;
        head = ptr;
    }
    size++;
}

void pop()
{
    if (head != NULL)
    {
        queue_c *temp = head->next;
        free(head);
        head = temp;
    }
    size--;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("ERROR: invalid number of arguments");
        return 1;
    }
    FILE *fin;
    fin = fopen(argv[1], "r");
    if (fin == NULL)
    {
        printf("ERROR: the file does not exist");
        return 1;
    }
    long long counter = 0;
    char sim;
    long long second = atoi(argv[2]);
    long long right_max;
    long long left_max;
    long long count_request = 0;
    char request[1000];
    char month[4];
    long long time_1 = 0;
    long long time_2 = 0;
    int max = 0;
    int a = 0;
    struct tm *time = (struct tm *)malloc(sizeof(struct tm));
    while (sim != EOF)
    {
        while ((sim != '[') && (sim !=EOF))
        {
            sim = fgetc(fin);
        }

        if (sim == '[')
        {
            time -> tm_mday = (fgetc(fin) - 48) * 10 + fgetc(fin) - 48;

            sim = fgetc(fin);
            month[0] = fgetc(fin);
            month[1] = fgetc(fin);
            month[2] = fgetc(fin);
            if (strcmp(month, "Jan") == 0)
                time->tm_mon = 0;
            if (strcmp(month, "Feb") == 0)
                time->tm_mon = 1;
            if (strcmp(month, "Mar") == 0)
                time->tm_mon = 2;
            if (strcmp(month, "Apr") == 0)
                time->tm_mon = 3;
            if (strcmp(month, "May") == 0)
                time->tm_mon = 4;
            if (strcmp(month, "Jun") == 0)
                time->tm_mon = 5;
            if (strcmp(month, "Jul") == 0)
                time->tm_mon = 6;
            if (strcmp(month, "Aug") == 0)
                time->tm_mon = 7;
            if (strcmp(month, "Sep") == 0)
                time->tm_mon = 8;
            if (strcmp(month, "Oct") == 0)
                time->tm_mon = 9;
            if (strcmp(month, "Nov") == 0)
                time->tm_mon = 10;
            if (strcmp(month, "Dec") == 0)
                time->tm_mon = 11;

            sim = fgetc(fin);
            time->tm_year = (fgetc(fin) - 48) * 1000 + (fgetc(fin) - 48) * 100 + (fgetc(fin) - 48) * 10 + (fgetc(fin) - 48) - 1900;;
            sim = fgetc(fin);
            time->tm_hour = (fgetc(fin) - 48) * 10 + (fgetc(fin) - 48);
            sim = fgetc(fin);
            time->tm_min = (fgetc(fin) - 48) * 10 + (fgetc(fin) - 48);
            sim = fgetc(fin);
            time->tm_sec = (fgetc(fin) - 48) * 10 + (fgetc(fin) - 48);
            sim = fgetc(fin);

            time_1 = mktime(time);
            if (max == 0)
            {
                push(time_1);
                time_2 = time_1;
                count_request++;
            }
            else
            {
                if (time_1 - time_2 <= second)
                {
                    push(time_1);
                    count_request++;
                }
                else
                {
                    while ((time_1 - time_2 > second) && (size != 0))
                    {
                        pop();
                        count_request--;
                        if (size != 0)
                            time_2 = head->value;
                    }
                    push(time_1);
                    count_request++;
                    if (size == 1)
                        time_2 = time_1;
                }
            }
        }

        while ((sim != '\"') && (sim != EOF))
        {
            sim = fgetc(fin);
        }

        if (max < count_request)
        {
            max = count_request;
            left_max = time_2;
            right_max = time_1;
        }

        //поиск количества ошибок и сами реквесты
        if (sim == '\"')
        {
            sim = fgetc(fin);
            long long i = 0;
            while (sim != '\"')
            {
                request[i] = sim;
                i++;
                sim = fgetc(fin);
            }
            sim = fgetc(fin);
            sim = fgetc(fin);
            //if ((sim != '1') && (sim != '2'))
            //{
            //counter++;
            if (sim == '5')
            {
                counter++;
                int j;
                for (j = 0; j < i; j++)
                {
                    printf("%c", request[j]);
                }
                printf("\n");
            }
            //while (sim != '\n')
            //{
            //    sim = fgetc(fin);
            //}
            //}
        }
        while ((sim != '\n') && (sim != EOF))
        {
            sim = fgetc(fin);
        }
    }
    printf("count of error %i\n", counter);
    printf("maximum number of requests per % lli seconds:", second);
    printf("% lli \n", max);
    time = localtime(&left_max);
    // time = &left_max;
    printf("from:\n %s", asctime(time));
    time = localtime(&right_max);
    printf("until:\n %s", asctime(time));
    return 0;