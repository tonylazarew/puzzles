/* S***y lottery puzzle by tony@lazarew.me */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_BUFFER 32
#define MAX_TOKENS 4
#define ALLOCATEZERO(buffer, size, block) \
    if (((buffer) = malloc((size))) == NULL) \
        { block } \
    memset((buffer), 0, (size))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


typedef struct {
    unsigned int lower;
    unsigned int upper;
} ival_t;


int parse_input(const char * /* in */, unsigned int * /* out */);
double calc_probability(unsigned int, unsigned int, unsigned int, unsigned int);
void combs(unsigned int /* in */, unsigned int /* in */, ival_t * /* out */);
void ival_create(unsigned int /* in */, unsigned int /* in */, ival_t * /* out */);
void expand_ival(const ival_t /* in */, unsigned int * /* out */);
void reduce_multipliers(
    unsigned int * /* in/out */, unsigned int * /* in/out */,
    unsigned int /* in */);


int main()
{
    double probability = 0;
    unsigned int numbers[4] = {
        /* m */ 0,
        /* n */ 0,
        /* t */ 0,
        /* p */ 0
    };
    char *buf = NULL,
         *chunk = NULL;

    /* Reading stdin. */
    ALLOCATEZERO(buf, MAX_INPUT_BUFFER + 1, {return 1;});
    ALLOCATEZERO(chunk, 1, {free(buf); return 1;});
    while (read(0, chunk, 1) != 0) {
        if (*chunk == '\n' || *chunk == 0 || strlen(buf) == MAX_INPUT_BUFFER)
            break;
        strncat(buf, chunk, 1);
    }
    free(chunk);

    if (!parse_input(buf, numbers)) {
        free(buf);
        return 1;
    }
    free(buf);

    probability = calc_probability(
        numbers[0], numbers[1], numbers[2], numbers[3]);

    ALLOCATEZERO(buf, 13, {printf("%.10f\n", probability); return 0;});
    snprintf(buf, 13, "%.10f", probability);
    if (probability > 0 && probability < 1)
        for (chunk = buf + strlen(buf); chunk != buf; chunk--)
            if (*chunk == '0') *chunk = 0;
            else if (*chunk != 0) break;
    printf("%s\n", buf);
    free(buf);

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

    ALLOCATEZERO(buf, strlen(buf_in) * sizeof(char) + 1, {return 0;});
    strncpy(buf, buf_in, strlen(buf_in));
    for (tok = strtok(buf, " ");
         i < MAX_TOKENS && tok;
         i++, numbers++, tok = strtok(NULL, " ")) {
        if (sscanf(tok, "%u", numbers) != 1 || *numbers > 1000 ) {
            return 0;
        }
    }
    free(buf);

    if (i < MAX_TOKENS)
        return 0;
    return 1;
}


/*
 * Calculate the probability.
 *
 * m -- the total number of people who entered the lottery
 * n -- the total number of winners drawn
 * t -- the number of tickets each winner is allowed to buy
 * p -- the number of people in your group
 */
double calc_probability(unsigned int m, unsigned int n,
                        unsigned int t, unsigned int p)
{
    unsigned int
        /* w -- required minimum for winners */
        w = (unsigned int)ceil((double)p / t),
        idx_lower = 0, idx_upper = 0,
        fr_width = 0,
        i = 0, j = 0,
        *nums = NULL,
        *denoms = NULL;
    ival_t
        total[2] = {{0, 0}, {0, 0}},
        ours[2] = {{0, 0}, {0, 0}},
        theirs[2] = {{0, 0}, {0, 0}};
    double
        probability = 0,
        fraction = 1;

    if (w > n)
        return (double)0;

    combs(n, m, total);

    /* Figuring out the amount of fractions we'll need. */
    idx_lower = (n - w + p) > m ? n - w - m + p : 0;
    idx_upper = MIN(p, n) - w;

    for (i = idx_lower; i <= idx_upper; i++) {
        fraction = 1;
        combs(w + i, p, ours);
        combs(n - w - i, m - p, theirs);

        /*
         * Figuring out the amount of multipliers in numerator and denominator
         * and allocating the memory.
         */
        fr_width = MAX(
            ours[0].upper - ours[0].lower +
            theirs[0].upper - theirs[0].lower +
            total[1].upper - total[1].lower + 3,
            ours[1].upper - ours[1].lower +
            theirs[1].upper - theirs[1].lower +
            total[0].upper - total[0].lower + 3) + 1;

        ALLOCATEZERO(nums, sizeof(unsigned int) * fr_width, {return -1;});
        ALLOCATEZERO(denoms, sizeof(unsigned int) * fr_width,
            {free(nums); return -1;});

        /* Expanding intervals into multipliers */
        /*  - for numerator */
        expand_ival(ours[0], nums);
        expand_ival(theirs[0], nums + (ours[0].upper - ours[0].lower + 1));
        expand_ival(
            total[1],
            nums + (ours[0].upper - ours[0].lower + 1)
                + (theirs[0].upper - theirs[0].lower + 1));

        /*  - for denominator */
        expand_ival(ours[1], denoms);
        expand_ival(theirs[1], denoms + (ours[1].upper - ours[1].lower + 1));
        expand_ival(
            total[0],
            denoms + (ours[1].upper - ours[1].lower + 1)
                + (theirs[1].upper - theirs[1].lower + 1));

        /* Reducing and calculating the fractions */
        reduce_multipliers(nums, denoms, fr_width);
        for (j = 0; j < fr_width && nums[j] != 0 && denoms[j] != 0; j++)
            fraction *= (double)nums[j] / (double)denoms[j];

        probability += fraction;

        free(nums);
        free(denoms);
    }

    return probability;
}


/*
 * C(x, y) = x! / (y! * (x - y)!)
 */
void combs(unsigned int items, unsigned int slots, ival_t *comb)
{
    if (items == 0)
        items = slots = 1;

    if (slots - 2 * items > 1) {
        ival_create(slots - items + 1, slots, &comb[0]);
        ival_create(1, items, &comb[1]);
    } else {
        ival_create(items + 1, slots, &comb[0]);
        ival_create(1, slots - items, &comb[1]);
    }
}


void ival_create(unsigned int lower, unsigned int upper, ival_t *ival)
{
    ival->lower = lower;
    ival->upper = MAX(lower, upper); /* ?? */
}


/*
 * Expand interval into multipliers.
 * `multipliers' must be alloc'd.
 */
void expand_ival(const ival_t ival, unsigned int *multipliers)
{
    unsigned int i = 0;
    for (i = ival.lower; i <= ival.upper; i++, multipliers++)
        *multipliers = i;
}


/*
 * Reduce multipliers in fraction.
 * `nums' and `denoms' must be alloc'd and initialized.
 * `width' is used to limit the size of processed vectors.
 */
void reduce_multipliers(unsigned int *nums, unsigned int *denoms,
    unsigned int width)
{
    unsigned int
        i = 0, j = 0,
        i_nums = 0, i_denoms = 0,
        *ptr_nums = nums,
        *ptr_denoms = denoms;

    /* Reducing multipliers */
    for (i = 0; i < width && nums[i] != 0; i++)
        for (j = 0; j < width && denoms[j] != 0; j++)
            if (nums[i] == denoms[j]) {
                nums[i] = denoms[j] = 1;
                break;
            }

    /* Moving meaningful multipliers to the beginning of the vectors */
    for (i = 0; i < width; i++) {
        if (nums[i] > 1) {
            *(ptr_nums++) = nums[i];
            i_nums++;
        }
        if (denoms[i] > 1) {
            *(ptr_denoms++) = denoms[i];
            i_denoms++;
        }
    }

    /* Aligning vectors with 1's and adding trailing zeroes */
    for (i = i_nums; *ptr_nums != 0; ptr_nums++, i++)
        if (i < MAX(i_nums, i_denoms))
            *ptr_nums = 1;
        else
            *ptr_nums = 0;

    for (i = i_denoms; *ptr_denoms != 0; ptr_denoms++, i++)
        if (i < MAX(i_nums, i_denoms))
            *ptr_denoms = 1;
        else
            *ptr_denoms = 0;
}
