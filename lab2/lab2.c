#include <stdio.h>
#include <string.h>

typedef struct Num
{
    unsigned int arr[32];
} Num;

Num uint1024_t_from_uint(unsigned int value) // перевод числа в uint1024
{
    Num res;
    for (int i = 0; i < 31; i++)
        res.arr[i] = 0;
    res.arr[31] = value;
    return res;
}
Num add_op(Num value1, Num value2) // cложение uint1024
{
    Num res;
    for (int i = 0; i < 32; i++) // зануляем
        res.arr[i] = 0;
    res.arr[0] += value1.arr[0] + value2.arr[0]; // мы не можем проверить на переполнение, поэтому делаем это сразу(отрежем если переполнение)
    for (int i = 31; i > 0; i--)
    {
        res.arr[i] += value1.arr[i] + value2.arr[i];
        if ((value1.arr[i] > res.arr[i]) && (value2.arr[i] > res.arr[i])) // проверяем на переполнение
            res.arr[i - 1]++;                                             // тк при сложении может быть переполнение только на 1
    }

    return res;
}
Num subtr_op(Num value, Num y) //разность
{
    Num res;
    for (int i = 0; i < 32; i++) // зануляем
        res.arr[i] = 0;
    res.arr[0] += value.arr[0] - y.arr[0];
    for (int i = 31; i >= 0; i--)
    {
        res.arr[i] += value.arr[i] - y.arr[i];
        if (res.arr[i] > value.arr[i]) // проверяем переполнение
            res.arr[i - 1]--;
    }

    return res;
}
Num multiplication(Num value, unsigned int value_int) // Произведение с unsigned int
{
    Num res;
    for (int i = 0; i < 32; i++) // зануляем
        res.arr[i] = 0;
    unsigned long long a = value_int;
    for (int i = 31; i >= 0; i--)
    {
        unsigned long long composition;
        unsigned long long b = value.arr[i];
        composition = a * b;                    // перемножение интов не даст больше лонга, переполнения тут не будет, но нужно учесть, что в лонге как бы другая система счисления
        res.arr[i] += composition % 4294967296; // делим на систему(как бы обратный перевод)
        if (i != 0)                             // переполнение
            res.arr[i - 1] += composition / 4294967296;
    }
    return res;
}
Num mult_op(Num value, Num value2) //Произведение uint1024
{
    Num res;
    for (int i = 0; i < 32; i++) // зануляем
        res.arr[i] = 0;
    for (int i = 31; i >= 0; i--)
    {
        if (value2.arr[i] != 0)
        {
            Num temp = multiplication(value, value2.arr[i]);
            int k = 31 - i;
            for (int j = k; j < 32; j++) // сдвигаем на разряд при умножении
            {
                temp.arr[j - k] = temp.arr[j];
            }
            for (int j = 32 - k; j < 32; j++)
            {
                temp.arr[j] = 0;
            }
            res = add_op(res, temp);
        }
    }
    return res;
}
void printf_value(Num value) //вывод
{
    char buf[1024] = {0};
    for (int i = 0; i < 1024; i++)
    {

        Num res;
        for (int i = 0; i < 32; i++)
        {
            res.arr[i] = 0;
        }
        unsigned long long remains = 0;
        for (int i = 0; i < 32; i++) // находим остаток от деления на 10 и делим на 10
        {
            res.arr[i] = (remains * 4294967296 + value.arr[i]) / 10;
            remains = (remains * 4294967296 + value.arr[i]) % 10;
        }
        unsigned mod = remains;
        value = res; // после деления на 10

        buf[1023 - i] = mod + '0'; // записываем чарами с конца
    }

    unsigned begin = 0;
    while (buf[begin] == '0') //пропускаем ненужные 0 в начале
        begin++;
    if (begin != 1024)                     //если все 0, значит число 0
        for (int i = begin; i < 1024; i++) // выписываем начиная с момента, когда встретим первую цифру
        {
            printf("%c", buf[i]);
        }
    else
    {
        printf("0");
    }
}
void scanf_value(Num *value) //ввод
{
    char buffer[1024];
    scanf("%s", buffer);
    int counter = 0;
    for (int i = 0; i < 32; i++) // зануляем
        value->arr[i] = 0;
    while (buffer[counter] != '\0' && counter < 1024) // пока не дойдем до конца или не выйдем за 1024
    {
        Num templete;
        for (int i = 0; i < 32; i++) // зануляем
            templete.arr[i] = 0;
        for (int i = 31; i >= 0; i--)
        {
            unsigned long long composition;
            unsigned long long a = value->arr[i];
            composition = a * 10;
            templete.arr[i] += composition % 4294967296;
            if (i != 0) // переполнение
                templete.arr[i - 1] += composition / 4294967296;
        }
        *value = templete;
        Num temp = uint1024_t_from_uint(buffer[counter] - '0'); //перевод символа в uint1024
        *value = add_op(*value, temp);
        counter++;
    }
}

int main()
{
    Num a;
    Num b;
    Num temp;
    printf("Введите первое число: ");
    scanf_value(&a);
    printf("\n");
    printf("Введите второе число: ");
    scanf_value(&b);
    printf("\nСумма: ");
    temp = add_op(a, b);
    printf_value(temp);
    printf("\nРазность: ");
    temp = subtr_op(a, b);
    printf_value(temp);
    printf("\nПроизведение: ");
    temp = mult_op(a, b);
    printf_value(temp);
    return 0;
}