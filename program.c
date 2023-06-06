#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define max(a, b) a >= b ? a : b
#define min(a, b) a >= b ? b : a
#define N 100000
#define Print 15

struct num
{
    char *ptr;
    size_t size;
};

// funkcje związane z typami zmiennych
char to_char(int k);
int to_int(char c);
int is_digit(char c);
struct num convert_int_to_num(int k, int base);
char num_to_char(struct num A, int base);
struct num create(char *s);

// funkcje do obsługi plików
void print_number(struct num A, FILE *f);
struct num calculate(struct num A, struct num B, int base, int base_out, char c);

// funkcje pomocniczne dla funkcji działań
void move(struct num A, size_t size_out, int diff);
void mult_by_base(struct num A, size_t size_out, int diff);
int compare(struct num A, struct num B);

// operacje arytmetyczne
struct num add(struct num A, struct num B, int base);
struct num subtract(struct num A, struct num B, int base);
struct num multiply(struct num A, struct num B, int base);
struct num divide(struct num A, struct num B, int base);
struct num modulo(struct num A, struct num B, int base);
struct num convert(struct num A, int base_in, int base_out);
struct num power(struct num A, struct num B, int base);

int main()
{
    FILE *inptr, *outptr;
    inptr = fopen("in.txt", "r");
    outptr = fopen("out.txt", "w");

    if (inptr == NULL)
    {
        printf("Nie udalo sie otworzyc pliku.\nNacisnij enter aby zakonczyc");
        char buffer;
        do
            buffer = getchar();
        while (buffer != '\n');
        return 0;
    }

    struct num num1, num2, result;
    num1.ptr = NULL;
    num2.ptr = NULL;
    result.ptr = NULL;
    char operation, line[N];
    int base, new_base = 0;
    int i, k = 1, step = 0;
    double time, total_time = 0.0;

    enum handle_file
    {
        op,
        arg1,
        arg2,
        calc
    };

    while (fgets(line, sizeof(line), inptr) != NULL)
    {
        if (line[0] == '\n')
            continue;

        switch (step)
        {
        case op:
            printf("Operacja #%d:\nPobieranie danych...\n", k);

            if (!is_digit(line[0])) // gdy operacja inna niż konwersja
            {
                operation = line[0];

                if (line[2] == '1')
                    base = 10 + to_int(line[3]);
                else
                    base = to_int(line[2]);

                if ((base < 2) || (base > 16))
                    operation = 'b';
            }
            else // gdy konwersja
            {
                i = 0;
                operation = 'C';

                if (line[i] == '1')
                {
                    base = 10 + to_int(line[i + 1]);
                    i++;
                }
                else
                    base = to_int(line[i]);

                i++;

                if (line[i + 1] == '1')
                    new_base = 10 + to_int(line[i + 2]);
                else
                    new_base = to_int(line[i + 1]);

                if ((base < 2) || (base > 16) || (new_base < 2) || (new_base > 16))
                    operation = 'b';
            }
            step = arg1;
            break;

        case arg1:
            num1.ptr = malloc(N * sizeof(char));
            i = 0;
            while ((line[i] != '\n') && (line[i] != '\0'))
            {
                num1.ptr[i] = line[i];
                if (to_int(num1.ptr[i]) >= base) // cyfry większe od podstawy systemu
                    operation = 'd';
                i++;
            }
            num1.size = i;
            num1.ptr[num1.size] = '\0';

            while ((num1.ptr[0] == '0') && (num1.size != 1))
            {
                memmove(num1.ptr, num1.ptr + 1, num1.size);
                num1.size--;
            }

            if (operation != 'C')
                step = arg2;
            else
            {
                num2.ptr = NULL;
                step = calc;
            }

            break;

        case arg2:
            num2.ptr = malloc(N * sizeof(char));
            i = 0;
            while ((line[i] != '\n') && (line[i] != '\0'))
            {
                num2.ptr[i] = line[i];
                if (to_int(num2.ptr[i]) >= base)
                    operation = 'd';
                i++;
            }
            num2.size = i;
            num2.ptr[num2.size] = '\0';

            while ((num2.ptr[0] == '0') && (num2.size != 1))
            {
                memmove(num2.ptr, num2.ptr + 1, num2.size);
                num2.size--;
            }

            step = calc;
            break;
        }
        fputs(line, outptr);
        fputs("\n", outptr);

        if (fgets(line, N, inptr) == NULL)
            fputs("\n", outptr);

        if (step == calc)
        {
            printf("Obliczenia rozpoczete\n");

            if (operation != 'C')
            {
                printf("   operacja:\t");
                char *s1, *s2;
                s1 = malloc(Print + 1);
                s2 = malloc(Print + 1);
                memcpy(s1, num1.ptr, Print);
                memcpy(s2, num2.ptr, Print);
                s1[Print] = s2[Print] = '\0';

                if (num1.size <= Print)
                    printf("%s ", s1);
                else
                    printf("%s... (%d) ", s1, num1.size);

                printf("%c ", operation);

                if (num2.size <= Print)
                    printf("%s\n", s2);
                else
                    printf("%s... (%d)\n", s2, num2.size);

                printf("   podstawa: \t%d\n", base);

                free(s1);
                free(s2);
            }
            else
            {
                printf("   liczba:\t");
                char *s1;
                s1 = malloc(Print + 1);
                memcpy(s1, num1.ptr, Print);
                s1[Print] = '\0';

                if (num1.size <= Print)
                    printf("%s\n", s1);
                else
                    printf("%s... (%d)\n", s1, num1.size);
                printf("   konwersja: \t%d -> %d\n", base, new_base);

                free(s1);
            }

            time = 0.0;
            clock_t begin = clock();
            result = calculate(num1, num2, base, new_base, operation);
            clock_t end = clock();
            time = (double)(end - begin) / CLOCKS_PER_SEC;
            total_time += time;

            print_number(result, outptr);
            fputs("\n\n\n", outptr);

            if (result.size != 0)
            {
                char *s1;
                s1 = malloc(Print + 1);
                memcpy(s1, result.ptr, Print);

                printf("   wynik:\t");
                if (result.size <= Print)
                    printf("%s\n", s1);
                else
                    printf("%s... (%d)\n", s1, result.size);

                free(s1);

                printf("Obliczenia zakonczone pomyslnie. Czas wykonania: %f s\n\n", time);
            }
            else
            {
                char *s1;
                s1 = malloc(Print + 1);
                memcpy(s1, result.ptr, Print);
                result.size = strlen(result.ptr);

                printf("   blad:\t");
                if (result.size <= Print)
                    printf("%s\n", s1);
                else
                    printf("%s... (%d)\n", s1, result.size);

                free(s1);
                printf("Rozpoznano blad w danych. Wyswietlono komunikat o bledzie\n\n", k);
            }

            k++;

            free(num1.ptr);
            free(num2.ptr);
            free(result.ptr);

            step = op;
        }
    }

    fclose(inptr);
    fclose(outptr);

    printf("Program zakonczyl obliczenia. Ogolny czas wykonywania: %f s\nNacisnij enter aby zakonczyc", total_time);

    char buffer;
    do
        buffer = getchar();
    while (buffer != '\n');

    system("notepad out.txt");
    return 0;
}

//______________________________________________________________________________
char to_char(int k)
{
    if ((k >= 0) && (k < 10))
        return k + '0';
    else if (k < 16)
        return k - 10 + 'A';
}

int to_int(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    else if ((c >= 'A') && (c <= 'F'))
        return c + 10 - 'A';
    else if ((c >= 'a') && (c <= 'f'))
        return c + 10 - 'a';
}

int is_digit(char c)
{
    if ((c >= '0') && (c <= '9'))
        return 1;
    else
        return 0;
}

struct num convert_int_to_num(int k, int base)
{
    int remainder = 0;
    int digits = 0;
    int tmp = k;
    struct num result;

    do
    {
        tmp = tmp / base;
        digits++;
    } while (tmp > 0);

    result.ptr = malloc(digits + 1);
    result.size = digits;

    for (int i = 0; i < digits; i++)
    {
        remainder = k % base;
        result.ptr[digits - 1 - i] = to_char(remainder);
        k = k / base;
    }

    result.ptr[digits] = '\0';
    return result;
}

char num_to_char(struct num A, int base)
{
    int sum = 0;
    for (int i = 0; i < A.size; i++)
        sum += to_int(A.ptr[A.size - 1 - i]) * pow(base, i);
    return to_char(sum);
}

struct num create(char *s)
{
    struct num A;
    size_t len = strlen(s);
    A.ptr = malloc(len + 1);
    A.size = 0;
    memcpy(A.ptr, s, len + 1);
    return A;
}

void print_number(struct num A, FILE *f)
{
    if (A.size > 0)
        for (int i = 0; i < A.size; i++)
            fputc(A.ptr[i], f);
    if (A.size == 0)
        fputs(A.ptr, f);
}

struct num calculate(struct num A, struct num B, int base, int base_out, char c)
{
    switch (c)
    {
    case '+':
        return add(A, B, base);
    case '*':
        return multiply(A, B, base);
    case '/':
        return divide(A, B, base);
    case '%':
        return modulo(A, B, base);
    case '^':
        return power(A, B, base);
    case 'C':
        return convert(A, base, base_out);
    case 'd':
        return create("invalid_digit");
    case 'b':
        return create("invalid_base");
    default:
        return create("invalid_operation");
    }
}

void move(struct num A, size_t size_out, int diff)
{
    memmove(A.ptr + diff, A.ptr, A.size);
    memset(A.ptr, '0', diff);
    A.ptr[size_out] = '\0';
}

void mult_by_base(struct num A, size_t size_out, int diff)
{
    memset(A.ptr + A.size, '0', diff);
    A.ptr[size_out] = '\0';
}

int compare(struct num A, struct num B)
{ // zwraca 1 jeśli A >= B, 0 jeśli A < B
    if (A.size > B.size)
        return 1;
    else if (A.size < B.size)
        return 0;
    else if (A.size == B.size)
    {
        int k = 0;
        while ((A.ptr[k] == B.ptr[k]) && (k < A.size))
            k++;

        if (k == A.size)
            return 1;
        else if (to_int(A.ptr[k]) > to_int(B.ptr[k]))
            return 1;
        else if (to_int(A.ptr[k]) < to_int(B.ptr[k]))
            return 0;
    }
}

struct num add(struct num A, struct num B, int base)
{
    int diff = abs(A.size - B.size);
    size_t maxsize = max(A.size, B.size);

    struct num num1, num2;
    num1.ptr = malloc(maxsize + 1);
    num2.ptr = malloc(maxsize + 1);
    num1.size = A.size;
    num2.size = B.size;
    memcpy(num1.ptr, A.ptr, A.size + 1);
    memcpy(num2.ptr, B.ptr, B.size + 1);

    struct num result;
    result.ptr = malloc(maxsize + 2);
    result.size = maxsize + 1;
    int temp = 0;

    if (A.size > B.size)
        move(num2, num1.size, diff);
    else if (A.size < B.size)
        move(num1, num2.size, diff);

    for (int i = maxsize - 1; i >= 0; i--)
    {
        result.ptr[i + 1] = to_char((temp + to_int(num1.ptr[i]) + to_int(num2.ptr[i])) % base);

        if (temp + to_int(num1.ptr[i]) + to_int(num2.ptr[i]) >= base)
            temp = 1;
        else
            temp = 0;
    }

    result.ptr[result.size] = '\0';

    if (temp == 1)
        result.ptr[0] = '1';
    else
    {
        memmove(result.ptr, result.ptr + 1, result.size);
        result.size--;
    }

    free(num1.ptr);
    free(num2.ptr);
    return result;
}

struct num subtract(struct num A, struct num B, int base)
{
    int diff = abs(A.size - B.size);

    struct num sub, result;
    result.ptr = malloc(A.size + 1);
    sub.ptr = malloc(A.size + 1);
    result.size = A.size;
    sub.size = B.size;
    memcpy(result.ptr, A.ptr, A.size + 1);
    memcpy(sub.ptr, B.ptr, B.size + 1);

    int temp;

    /*if(compare(A,B))
    {
        struct num error = create("negative_number");
        free(sub.ptr);
        free(result.ptr);
        return error;
    }*/

    if (A.size != B.size)
        move(sub, A.size, diff);

    for (int i = A.size - 1; i >= 0; i--)
    {
        temp = 0;
        if (to_int(result.ptr[i]) - to_int(sub.ptr[i]) < 0)
        {
            if (result.ptr[i - 1] != '0')
                result.ptr[i - 1] = to_char(to_int(result.ptr[i - 1]) - 1);
            else if (result.ptr[i - 1] == '0')
            {
                int j = i - 1;
                do
                {
                    result.ptr[j] = to_char(base - 1);
                    j--;
                } while (result.ptr[j] == '0');
                result.ptr[j] = to_char(to_int(result.ptr[j]) - 1);
            }
            temp = 1;
        }
        else
            temp = 0;

        result.ptr[i] = to_char(temp * base + to_int(result.ptr[i]) - to_int(sub.ptr[i]));
    }

    result.ptr[result.size] = '\0';

    while ((result.ptr[0] == '0') && (result.size != 1))
    {
        memmove(result.ptr, result.ptr + 1, result.size);
        result.size--;
    }

    free(sub.ptr);
    return result;
}

struct num multiply(struct num A, struct num B, int base)
{
    int diff = abs(A.size - B.size);
    size_t maxsize = max(A.size, B.size);

    struct num num1, num2;
    num1.ptr = malloc(maxsize + 1);
    num2.ptr = malloc(maxsize + 1);
    num1.size = A.size;
    num2.size = B.size;
    memcpy(num1.ptr, A.ptr, A.size + 1);
    memcpy(num2.ptr, B.ptr, B.size + 1);

    struct num result;
    result.ptr = malloc(maxsize * 2 + 1);
    result.size = maxsize * 2;

    if (A.size > B.size)
        move(num2, num1.size, diff);
    else if (A.size < B.size)
        move(num1, num2.size, diff);

    memset(result.ptr, '0', result.size);
    result.ptr[result.size] = '\0';

    int product, tmp_mult, tmp_add, oldresult;
    for (int i = maxsize - 1; i >= 0; i--)
    {
        tmp_mult = 0;
        tmp_add = 0;
        for (int j = maxsize - 1; j >= 0; j--)
        {
            product = to_int(num2.ptr[i]) * to_int(num1.ptr[j]) + tmp_mult;
            tmp_mult = product / base;

            oldresult = to_int(result.ptr[i + j + 1]);

            result.ptr[i + j + 1] = to_char((to_int(result.ptr[i + j + 1]) + product + tmp_add) % base);

            if (oldresult + product % base + tmp_add >= base)
                tmp_add = 1;
            else
                tmp_add = 0;
        }
        if (tmp_mult != 0)
            result.ptr[i] = to_char(tmp_mult);

        if (tmp_add != 0)
            result.ptr[i] = to_char(to_int(result.ptr[i]) + tmp_add);
    }

    while ((result.ptr[0] == '0') && (result.size != 1))
    {
        memmove(result.ptr, result.ptr + 1, result.size);
        result.size--;
    }

    free(num1.ptr);
    free(num2.ptr);
    return result;
}

struct num divide(struct num A, struct num B, int base)
{
    if ((B.ptr[0] == '0') && (B.size == 1))
    {
        struct num error = create("division_by_zero");
        return error;
    }

    int diff = abs(A.size - B.size);
    size_t maxsize = max(A.size, B.size);
    size_t minsize = min(A.size, B.size);

    struct num dividend, divisor;
    dividend.ptr = malloc(maxsize + 1);
    divisor.ptr = malloc(maxsize + 1);
    dividend.size = maxsize;
    divisor.size = minsize;

    if (compare(A, B))
    {
        memcpy(dividend.ptr, A.ptr, A.size + 1);
        memcpy(divisor.ptr, B.ptr, B.size + 1);
    }
    else
    {
        free(dividend.ptr);
        free(divisor.ptr);

        struct num zero = create("0");
        return zero;
    }

    struct num result;
    result.ptr = malloc(diff + 2);
    result.size = diff + 1;

    mult_by_base(divisor, maxsize, diff);
    divisor.size = maxsize;

    for (int i = diff; i >= 0; i--)
    {
        int k = 0;

        while (compare(dividend, divisor))
        {
            dividend = subtract(dividend, divisor, base);
            k++;
        }

        result.ptr[diff - i] = to_char(k);

        divisor.ptr[divisor.size - 1] = '\0';
        divisor.size--;
    }

    result.ptr[result.size] = '\0';

    if ((result.ptr[0] == '0') && (result.size != 1))
    {
        memmove(result.ptr, result.ptr + 1, result.size);
        result.size--;
    }

    free(dividend.ptr);
    free(divisor.ptr);
    return result;
}

struct num modulo(struct num A, struct num B, int base)
{
    if ((B.ptr[0] == '0') && (B.size == 1))
    {
        struct num error = create("division_by_zero");
        return error;
    }

    int diff = abs(A.size - B.size);
    size_t maxsize = max(A.size, B.size);
    size_t minsize = min(A.size, B.size);

    struct num dividend, divisor;
    dividend.ptr = malloc(maxsize + 1);
    divisor.ptr = malloc(maxsize + 1);
    dividend.size = maxsize;
    divisor.size = minsize;

    if (compare(A, B))
    {
        memcpy(dividend.ptr, A.ptr, A.size + 1);
        memcpy(divisor.ptr, B.ptr, B.size + 1);
    }
    else
    {
        free(dividend.ptr);
        free(divisor.ptr);

        struct num result = create(A.ptr);
        result.size = A.size;
        return result;
    }

    mult_by_base(divisor, maxsize, diff);
    divisor.size = maxsize;

    for (int i = diff; i >= 0; i--)
    {
        int k = 0;

        while (compare(dividend, divisor))
        {
            dividend = subtract(dividend, divisor, base);
            k++;
        }

        divisor.ptr[divisor.size - 1] = '\0';
        divisor.size--;
    }

    free(divisor.ptr);
    return dividend;
}

struct num convert(struct num A, int base_in, int base_out)
{
    struct num result;

    struct num temp;
    temp.ptr = malloc(A.size + 1);
    temp.size = A.size;
    memcpy(temp.ptr, A.ptr, A.size + 1);

    struct num new_base;
    new_base = convert_int_to_num(base_out, base_in);

    struct num remainder;
    remainder.ptr = malloc(2);
    remainder.size = 2;

    struct num zero = create("0");

    int digits = 0;

    do
    {
        temp = divide(temp, new_base, base_in);
        digits++;
    } while (!compare(zero, temp)); //<=> "temp>zero"

    free(temp.ptr);
    free(zero.ptr);

    result.ptr = malloc(digits + 1);
    result.size = digits;

    for (int i = 0; i < digits; i++)
    {
        remainder = modulo(A, new_base, base_in);
        result.ptr[digits - 1 - i] = num_to_char(remainder, base_in);
        A = divide(A, new_base, base_in);
    }

    result.ptr[digits] = '\0';

    free(remainder.ptr);
    free(new_base.ptr);
    return result;
}

struct num power(struct num A, struct num B, int base)
{
    if ((B.ptr[0] == '0') && (B.size == 1))
    {
        if ((A.ptr[0] == '0') && (A.size == 1))
        {
            struct num error = create("undefined_0^0");
            return error;
        }
        struct num one = create("1");
        return one;
    }

    struct num exp = convert(B, base, 2);
    struct num result;
    result.ptr = malloc(2);
    result.size = 1;
    result.ptr[0] = '1';

    for (int i = 0; i < exp.size; i++)
    {
        result = multiply(result, result, base);
        if (exp.ptr[i] == '1')
            result = multiply(result, A, base);
    }

    return result;
}