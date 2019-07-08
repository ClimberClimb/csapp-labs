#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include "cachelab.h"

#define MIN_ARGUMENTS 3


typedef struct line
{
    int valid;
    int accessTime;
    int tag;
} Line;

int main(int argc, char * argv[])
{
    int ch;
    unsigned int verbose = 0;
    unsigned int s = 0;
    unsigned int E = 0;
    unsigned int b = 0;
    unsigned int S;
    unsigned int hits = 0;
    unsigned int misses = 0;
    unsigned int evictions = 0;
    unsigned int startTime = 0;


    char filepath[100];
    FILE *fp;
    char chunk[128];

    while ((ch = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (ch)
        {
            case 'h':
                printf("./csim -v -s 4 -E 1 -b 4 -t traces/yi.trace\n");
                break;
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = (unsigned int) atoi(optarg);
                break;
            case 'E':
                E = (unsigned int) atoi(optarg);
                break;
            case 'b':
                b = (unsigned int) atoi(optarg);
                break;
            case 't':
                strcpy(filepath, optarg);
                break;
            default:
                exit(-1);
        }
    }

    if (optind < MIN_ARGUMENTS)
    {
        printf("lack of arguments -s -E -b -t\n");
        exit(-1);
    }

    S = (int) pow(2, s);

    Line *ePointer;
    Line *startPointer;

    ePointer = (Line *) malloc(sizeof(Line) * S * E);
    startPointer = ePointer;

    for (int i = 0; i < S * E; i++)
    {
        startPointer->tag = -1;
        startPointer->valid = -1;
        startPointer->accessTime = -1;
        startPointer++;
    }

    fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        printf("error open files\n");
        exit(-1);
    }

    while (fgets(chunk, sizeof(chunk), fp) != NULL)
    {
        if (chunk[0] == 'I')
        {
            ;
        }
        else if (chunk[0] == ' ')
        {
            unsigned int address;
            int tagValue;
            int setValue;
            address = (unsigned int) strtol(chunk + 2, NULL, 16);

            tagValue = address  >> (s + b);
            setValue = (address << (32 - s - b)) >> (32 - s);
            chunk[strlen(chunk) - 1] = '\0';
            printf("%d %d\n", tagValue, setValue);
            if (verbose == 1)
            {
                printf("%s", chunk);
            }

            if (chunk[1] == 'M')
            {
                int isHit = 0;
                startPointer = ePointer + setValue * E;
                for (int i = 0; i < E; i++)
                {
                    if (startPointer->valid == 1 && startPointer->tag == tagValue)
                    {
                        isHit = 1;
                        startPointer->accessTime = startTime;
                        startTime++;
                        break;
                    }
                    startPointer++;
                }

                if (isHit)
                {
                    hits += 2;
                    printf("hit, hit");
                }
                else
                {
                    misses++;
                    if (verbose == 1)
                        printf("miss, ");
                    int min_num = INT_MAX;
                    int min_index = 0;
                    int notPushed = 0;
                    startPointer = ePointer + setValue * E;
                    for (int i = 0; i < E; i++)
                    {
                        if (startPointer->valid == -1)
                        {
                            // get out of  the invalid data
                            min_index = i;
                            notPushed = 1;
                            break;
                        }

                        if (startPointer->accessTime < min_num)
                        {
                            min_index = i;
                            min_num = startPointer->accessTime;
                        }
                        startPointer++;
                    }
                    startPointer = ePointer + setValue * E;
                    startPointer += min_index;
                    if (!notPushed)
                    {
                        evictions++;
                        if (verbose == 1)
                            printf(", eviction");
                    }
                    startPointer->tag = tagValue;
                    startPointer->accessTime = startTime;
                    startPointer->valid = 1;
                    startTime++;
                    hits++;
                    if (verbose == 1)
                        printf(", hit");
                }

            }
            else if (chunk[1] == 'S' || chunk[1] == 'L' )
            {
                int isHit = 0;
                startPointer = ePointer + setValue * E;
                for (int i = 0; i < E; i++)
                {
                    if (startPointer->valid == 1 && startPointer->tag == tagValue)
                    {
                        isHit = 1;
                        startPointer->accessTime = startTime;
                        startTime++;
                        break;
                    }
                    startPointer++;
                }
                if (isHit)
                {
                    hits++;
                    if (verbose == 1)
                        printf(", hit");
                }
                else
                {
                    misses++;
                    if (verbose == 1)
                        printf(", miss");
                    int min_num = INT_MAX;
                    int min_index = 0;
                    int notPushed = 0;
                    startPointer = ePointer + setValue * E;
                    for (int i = 0; i < E; i++)
                    {
                        if (startPointer->valid == -1)
                        {
                            // get out of  the invalid data
                            min_index = i;
                            notPushed = 1;
                            break;
                        }

                        if (startPointer->accessTime < min_num)
                        {
                            min_index = i;
                            min_num = startPointer->accessTime;
                        }
                        startPointer++;
                    }
                    startPointer = ePointer + setValue * E;
                    startPointer += min_index;
                    if (!notPushed)
                    {
                        evictions++;
                        if (verbose == 1)
                            printf(", eviction");
                    }
                    startPointer->tag = tagValue;
                    startPointer->accessTime = startTime;
                    startPointer->valid = 1;
                    startTime++;
                }
            }
        }
        if (verbose == 1)
            printf("\n");

    }
    fclose(fp);
    printSummary(hits, misses, evictions);
    return 0;
}