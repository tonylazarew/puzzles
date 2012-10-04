/* S***y bestbefore puzzle by tony@lazarew.me */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_BUFFER 64
#define MAX_TOKENS 3
#define MAX_PERMUTATIONS 6
#define ALLOCATEZERO(buffer, size, block) \
    if (((buffer) = malloc((size))) == NULL) \
        { block } \
    memset((buffer), 0, (size))


int parse_input(const char * /* in */, unsigned int * /* out */);
int calc_date(const unsigned int * /* in */, char * /* out */);
int date_repr(unsigned int y, unsigned int m, unsigned int d); /* all in */

int ph_year(unsigned int);
int ph_month(unsigned int);
int ph_day(unsigned int);


/* Possible date format permutations. */
typedef int (*ph_func_t)(unsigned int);
typedef enum {YEAR, MONTH, DAY} ph_component_t;

ph_func_t formats[MAX_PERMUTATIONS][MAX_TOKENS] = {
    {ph_year, ph_month, ph_day},
    {ph_year, ph_day, ph_month},
    {ph_day, ph_month, ph_year},
    {ph_month, ph_day, ph_year},
    {ph_day, ph_year, ph_month},
    {ph_month, ph_year, ph_day}
};

ph_component_t components[MAX_PERMUTATIONS][MAX_TOKENS] = {
    {YEAR, MONTH, DAY},
    {YEAR, DAY, MONTH},
    {DAY, MONTH, YEAR},
    {MONTH, DAY, YEAR},
    {DAY, YEAR, MONTH},
    {MONTH, YEAR, DAY}
};


int main()
{
    unsigned int numbers[MAX_TOKENS] = {0, 0, 0};
    char *date = NULL,
         *chunk = NULL,
         *buf = NULL;

    /* Reading stdin. */
    ALLOCATEZERO(buf, MAX_INPUT_BUFFER, {return 1;});
    ALLOCATEZERO(chunk, 1, {free(buf); return 1;});
    while (read(0, chunk, 1) != 0) {
        if (*chunk == '\n' || *chunk == 0 || strlen(buf) == MAX_INPUT_BUFFER)
            break;
        strncat(buf, chunk, 1);
    }
    free(chunk);

    /* Parsing and processing the date. */
    if (!parse_input(buf, numbers)) {
        printf("%s is illegal\n", buf);
        free(buf);
        return 1;
    }

    ALLOCATEZERO(date, 10, {free(buf); return 1;});
    if (!calc_date(numbers, date)) {
        printf("%s is illegal\n", buf);
        free(date);
        free(buf);
        return 1;
    }

    free(buf);
    puts(date);
    free(date);
    return 0;
}


/*
 * Process the data from stdin.
 */
int parse_input(const char *buf_in, unsigned int *numbers)
{
    char *tok = NULL,
         *buf = NULL,
         i = 0;

    ALLOCATEZERO(buf, strlen(buf_in) * sizeof(char), {return 0;});
    strncpy(buf, buf_in, strlen(buf_in));
    for (tok = strtok(buf, "/");
         i < MAX_TOKENS && tok;
         i++, numbers++, tok = strtok(NULL, "/")) {
        if (sscanf(tok, "%u", numbers) != 1
            || *numbers < 0 || *numbers > 2999 ) {
            return 0;
        }
    }
    free(buf);

    if (i < MAX_TOKENS)
        return 0;
    return 1;
}


/*
 * Calculate the date.
 */
int calc_date(const unsigned int *numbers, char *date)
{
    unsigned char i = 0, j = 0;
    unsigned int y = 0, m = 0, d = 0;
    unsigned int date_i[4] = {0, 0, 0, 0};
    int v = 0;

    for (i = 0; i < MAX_PERMUTATIONS; i++) {
        for (j = 0; j < MAX_TOKENS; j++) {
            if ((v = (*formats[i][j])(numbers[j])) == -1)
                break;
            switch (components[i][j]) {
                case YEAR: y = (unsigned int)v; break;
                case MONTH: m = (unsigned int)v; break;
                case DAY: d = (unsigned int)v; break;
            }
        }

        if (v == -1 || (v = date_repr(y, m, d)) == -1)
            continue;

        if (*date_i == 0 || v < *date_i) {
            date_i[0] = v;
            date_i[1] = y;
            date_i[2] = m;
            date_i[3] = d;
        }
    }

    if (*date_i == 0)
        return 0;

    sprintf(date, "%u-%02u-%02u", date_i[1], date_i[2], date_i[3]);
    return 1;
}


/*
 * Check whether this date exists and returns single numerical representation
 * which can be used to compare two dates.
 * Returns -1 if the date is not valid.
 */
int date_repr(unsigned int y, unsigned int m, unsigned int d)
{
    unsigned int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Leap year fix. */
    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
        mdays[1]++;

    if (! (1 <= m && m <= 12 && 1 <= d && d <= mdays[m - 1]) )
        return -1;

    return ((y * 100 + m) * 100 + d);
}


/*
 * Format placeholders.  Return valid number for each date part, or -1 if input
 * is incorrect.
 */
int ph_year(unsigned int y)
{
    if (0 <= y && y <= 999)
        return y + 2000;
    else if (2000 <= y && y <= 2999)
        return y;
    return -1;
}


int ph_month(unsigned int m)
{
    if (1 <= m && m <= 12)
        return m;
    return -1;
}


int ph_day(unsigned int d)
{
    if (1 <= d && d <= 31)
        return d;
    return -1;
}
