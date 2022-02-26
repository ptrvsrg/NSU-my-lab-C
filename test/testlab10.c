#include "testLab.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#define MAX_POINT_COUNT 100000
typedef struct 
{
    int x, y;
} point_t;

static int testN = 0;
static const struct { const char *const in, *const outError; int pointCount; point_t outPoints[8]; } testInOut[] = {
    { "4\n0 0\n2 1\n4 0\n0 4\n", NULL, 3, { {0, 0}, {4, 0}, {0, 4} } }, // 1
    { "4\n2 9\n2 1\n2 -6\n2 4\n", NULL, 2, { {2, -6}, {2, 9} } }, // 2
    { "100001\n", "bad number of points", 0, { {0, 0} } }, // 3
    { "5\n1 1\n4 5\n", "bad number of lines", 0, { {0, 0} } }, // 4

    { "0\n", NULL, 0, { {0, 0} } }, // 5
    { "4\n-2147483648 0\n0 2147483647\n0 -2147483648\n2147483647 0\n", NULL, 4, { {0, INT_MIN}, {INT_MAX, 0}, {0, INT_MAX}, {INT_MIN, 0} } }, // 6
    { "5\n-2147483648 0\n0 2147483647\n0 -2147483648\n2147483647 0\n0 0\n", NULL, 4, { {0, INT_MIN}, {INT_MAX, 0}, {0, INT_MAX}, {INT_MIN, 0} } }, // 7
    { "8\n-2147483648 0\n0 2147483647\n0 -2147483648\n2147483647 0\n-2147483648 -2147483648\n-2147483648 2147483647\n2147483647 -2147483648\n2147483647 2147483647\n", NULL, 4, { {INT_MIN, INT_MIN}, {INT_MAX, INT_MIN}, {INT_MAX, INT_MAX}, {INT_MIN, INT_MAX} } }, // 8
    
    { "4\n4 4\n2 3\n4 0\n0 4\n", NULL, 3, { {4, 4}, {4, 0}, {0, 4} } }, // 9
    { "1\n2 3\n", NULL, 1, { {2, 3} } }, // 10
    { "2\n1 1\n4 5\n", NULL, 2, { {1, 1}, {4, 5} } }, // 11
    { "5\n0 0\n2 3\n4 0\n0 4\n4 4\n", NULL, 4, { {0, 0}, {4, 0}, {0, 4}, {4, 4} } }, // 12

    { "-1\n", "bad number of points", 0, { {0, 0} } }, // 13
    { "3\n1 2\n4 3\n5\n", "bad number of lines", 0, { {0, 0} } }, // 14
    { "3\n4 0\n0 0\n0 4\n", NULL, 3, { {0, 0}, { 0, 4 }, { 4, 0 } } }, // 15
    { "3\n-7 3\n0 0\n7 -3\n", NULL, 3, { {-7, 3}, { 7, -3 } } }, // 16
    
    { "8\n7 0\n4 0\n0 0\n3 0\n2 0\n1 0\n5 0\n6 0\n", NULL, 1, { {0, 0}, { 6, 0 } } }, // 17
    { "8\n0 7\n0 4\n0 0\n0 3\n0 2\n0 1\n0 5\n0 6\n", NULL, 2, { {0, 0}, {0, 6} } }, // 18
    { "8\n3 3\n-1 -1\n5 5\n-4 -4\n1 1\n2 2\n-2 -2\n3 3\n", NULL, 2, { {-4, -4}, {5, 5} } }, // 19
    { "8\n4 -3\n3 4\n-3 -4\n-4 3\n4 3\n3 -4\n-4 -3\n-3 4\n", NULL, 8, { {-3, -4}, {3, -4}, {4, -3}, {4, 3}, {3, 4}, {-3, 4}, {-4, 3}, {-4, -3} } }, // 20
};

const char* ScanPoint(FILE* file, point_t* point)
{
    return ScanIntInt(file, &point->x, &point->y);
}

static int FeedFromArray(void)
{
    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    fprintf(in, "%s", testInOut[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        ++testN;
        return -1;
    }

    if (testInOut[testN].pointCount != 0) {
        int finded[8] = {0};
        int findedCount = 0;

        for (int i = 0; i < testInOut[testN].pointCount; ++i) {
            point_t point = { 0, 0 };
            if (ScanPoint(out, &point) == Fail) {
                fclose(out);
                printf("short output -- %s\n", Fail);
                return 1;
            }
            
            for (int j = 0; j < testInOut[testN].pointCount; ++j) {
                if (point.x == testInOut[testN].outPoints[j].x && point.y == testInOut[testN].outPoints[j].y) {
                    if (finded[j] == 1) {
                        fclose(out);
                        printf("wrong output -- %s\n", Fail);
                        return 1;
                    }
                    
                    ++finded[j];
                    ++findedCount;
                    break;
                }
            }
        }
        
        if (findedCount != testInOut[testN].pointCount) {
            fclose(out);
            printf("wrong output -- %s\n", Fail);
            return 1;
        }

        ++testN;
    }   
    else if (testInOut[testN].outError != NULL) {
        char error[32] = {0};
        const char* status = ScanChars(out, sizeof(error), error);

        if(status == Pass && _strnicmp(testInOut[testN].outError, error, strlen(testInOut[testN].outError)) != 0) {
            status = Fail;
        }

        fclose(out);
        if (status == Fail) {
            printf("wrong output -- ");
        }
        printf("%s\n", status);
        ++testN;
        return status == Fail;
    }

    int failed = HaveGarbageAtTheEnd(out);

    fclose(out);
    printf("%s\n", (failed) ? Fail : Pass);
    return failed;
}

static int bigSegmentsN = 1;
static int LabTimeout;
static size_t LabMemoryLimit;

static int FeederBigSegment(void)
{
    FILE *const in = fopen("in.txt", "w");

    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating big segment... ");
    fflush(stdout);
    fprintf(in, "%d\n", MAX_POINT_COUNT);

    for (int i = MAX_POINT_COUNT - 1; i >= 0; --i) {
        if (bigSegmentsN == 1) {
            if (fprintf(in, "%d %d\n", i, 0) < 2) {
                fclose(in);
                printf("%s\n", "can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
        else if (bigSegmentsN == 2) {
            if (fprintf(in, "%d %d\n", 0, i) < 2) {
                fclose(in);
                printf("%s\n", "can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
        else if (bigSegmentsN == 3) {
            if (fprintf(in, "%d %d\n", i, i) < 2) {
                fclose(in);
                printf("%s\n", "can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
    }

    fclose(in);
    printf("done. Starting exe... ");
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 8*MAX_POINT_COUNT;
    return 0;    
}

static int CheckerBigSegment(void)
{
    FILE *const out = fopen("out.txt", "r");

    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    int finded[2] = { 0 };
    int findedCount = 0;

    point_t pointOut[2] = { { 0, 0 } };

    if (bigSegmentsN == 1) {
        pointOut[1].x = MAX_POINT_COUNT - 1;
        pointOut[1].y = 0;
    }
    else if (bigSegmentsN == 2) {
        pointOut[1].x = 0;
        pointOut[1].y = MAX_POINT_COUNT - 1;
    }
    else if (bigSegmentsN == 3) {
        pointOut[1].x = MAX_POINT_COUNT - 1;
        pointOut[1].y = MAX_POINT_COUNT - 1;
    }  

    for (int i = 0; i < 2; ++i) {
        point_t point = { 0, 0 };
        if (ScanPoint(out, &point) == Fail) {
            fclose(out);
            printf("short output -- %s\n", Fail);
            return 1;
        }  

        for (int j = 0; j < 2; ++j) {
            if (point.x == pointOut[j].x && point.y == pointOut[j].y) {
                if (finded[j] == 1) {
                    fclose(out);
                    printf("wrong output -- %s\n", Fail);
                    return 1;
                }
                
                ++finded[j];
                ++findedCount;
                break;
            }
        }
    }
    
    if (findedCount != 2) {
        fclose(out);
        printf("wrong output -- %s\n", Fail);
        return 1;
    }

    int failed = HaveGarbageAtTheEnd(out);

    ++bigSegmentsN;
    fclose(out);
    printf("%s\n", (failed) ? Fail : Pass);
    return failed;
}

static int bigTrianglesN = 1;
static int FeederBigTriangle(void)
{
    FILE *const in = fopen("in.txt", "w");

    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating big triangle... ");
    fflush(stdout);
    fprintf(in, "%d\n", MAX_POINT_COUNT);

    for (int i = MAX_POINT_COUNT - 2, j = 0; i >= 0; --i, ++j) {
        if (fprintf(in, "%d %d\n", i, j) < 2) {
            fclose(in);
            printf("can't create in.txt. No space on disk?\n");
            return -1;
        }
    }

    if (bigTrianglesN == 1) {
        fprintf(in, "0 0\n");
    }
    else if (bigTrianglesN == 2) {
        fprintf(in, "99998 99998\n");
    }

    fclose(in);
    printf("done. Starting exe... ");
    fflush(stdout);
    return 0;    
}

static int CheckerBigTriangle(void)
{
    FILE *const out = fopen("out.txt", "r");

    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    int finded[3] = { 0 };
    int findedCount = 0;

    point_t pointOut[3] = { { MAX_POINT_COUNT - 2, 0 }, { 0, MAX_POINT_COUNT - 2 } };

    if (bigTrianglesN == 1) {
        pointOut[2].x = 0;
        pointOut[2].y = 0;
    }
    else if (bigTrianglesN == 2) {
        pointOut[2].x = MAX_POINT_COUNT - 2;
        pointOut[2].y = MAX_POINT_COUNT - 2;
    } 

    for (int i = 0; i < 3; ++i) {
        point_t point = { 0, 0 };
        if (ScanPoint(out, &point) == Fail) {
            fclose(out);
            printf("short output -- %s\n", Fail);
            return 1;
        }

        for (int j = 0; j < 3; ++j) {
            if (point.x == pointOut[j].x && point.y == pointOut[j].y) {
                if (finded[j] == 1) {
                    fclose(out);
                    printf("wrong output -- %s\n", Fail);
                    return 1;
                }
                
                ++finded[j];
                ++findedCount;
                break;
            }
        }
    }
    
    if (findedCount != 3) {
        fclose(out);
        printf("wrong output -- %s\n", Fail);
        return 1;
    }

    int failed = HaveGarbageAtTheEnd(out);

    ++bigTrianglesN;
    fclose(out);
    printf("%s\n", (failed) ? Fail : Pass);
    return failed;
}

static int bigParabolsN = 1;
static int FeederBigParabol(void)
{
    FILE *const in = fopen("in.txt", "w");

    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating big parabol... ");
    fflush(stdout);
    fprintf(in, "%d\n", MAX_POINT_COUNT);

    for (long long i = 1; i <= MAX_POINT_COUNT / 2; ++i) {
        if (bigParabolsN == 1) {
            if (fprintf(in, "%lld %lld\n%lld %lld\n", i, i*i + INT_MIN, -i, i*i + INT_MIN) < 4) {
                fclose(in);
                printf("can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
        else if (bigParabolsN == 2) {
            if (fprintf(in, "%lld %lld\n%lld %lld\n", i, -i*i + INT_MAX, -i, -i*i + INT_MAX) < 4) {
                fclose(in);
                printf("can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
        else if (bigParabolsN == 3) {
            if (fprintf(in, "%lld %lld\n%lld %lld\n", i*i + INT_MIN, i, i*i + INT_MIN, -i) < 4) {
                fclose(in);
                printf("can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
    }

    fclose(in);
    printf("done. Starting exe... ");
    fflush(stdout);
    return 0;    
}

static int CheckerBigParabol(void)
{
    FILE *const out = fopen("out.txt", "r");

    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    int finded[MAX_POINT_COUNT + 1] = { 0 };
    int findedCount = 0;

    for (int i = 0; i < MAX_POINT_COUNT; ++i) {
        point_t point = { 0, 0 };
        if (ScanPoint(out, &point) == Fail) {
            fclose(out);
            printf("short output -- %s\n", Fail);
            return 1;
        }

        int index = 0;

        if(bigParabolsN == 1 || bigParabolsN == 2) {
            index = MAX_POINT_COUNT / 2 + point.x;
        }
        else if(bigParabolsN == 3) {
            index = MAX_POINT_COUNT / 2 + point.y;
        }

        if (finded[index] == 1) {
            fclose(out);
            printf("wrong output -- %s\n", Fail);
            return 1;
        }

        ++finded[index];
        ++findedCount;
    }
    
    if (findedCount != MAX_POINT_COUNT) {
        fclose(out);
        printf("wrong output -- %s\n", Fail);
        return 1;
    }

    int failed = HaveGarbageAtTheEnd(out);

    ++bigParabolsN;
    fclose(out);
    printf("%s\n", (failed) ? Fail : Pass);
    return failed;
}

const TLabTest LabTests[] = {
    {FeedFromArray, CheckFromArray}, // 1
    {FeedFromArray, CheckFromArray}, // 2
    {FeedFromArray, CheckFromArray}, // 3
    {FeedFromArray, CheckFromArray}, // 4
    
    {FeedFromArray, CheckFromArray}, // 5
    {FeedFromArray, CheckFromArray}, // 6
    {FeedFromArray, CheckFromArray}, // 7
    {FeedFromArray, CheckFromArray}, // 8
    
    {FeedFromArray, CheckFromArray}, // 9
    {FeedFromArray, CheckFromArray}, // 10
    {FeedFromArray, CheckFromArray}, // 11
    {FeedFromArray, CheckFromArray}, // 12
    
    {FeedFromArray, CheckFromArray}, // 13
    {FeedFromArray, CheckFromArray}, // 14
    {FeedFromArray, CheckFromArray}, // 15
    {FeedFromArray, CheckFromArray}, // 16
    
    {FeedFromArray, CheckFromArray}, // 17
    {FeedFromArray, CheckFromArray}, // 18
    {FeedFromArray, CheckFromArray}, // 19
    {FeedFromArray, CheckFromArray}, // 20
    
    {FeederBigSegment, CheckerBigSegment}, // 21
    {FeederBigSegment, CheckerBigSegment}, // 22
    {FeederBigSegment, CheckerBigSegment}, // 23

    {FeederBigTriangle, CheckerBigTriangle}, // 24
    {FeederBigTriangle, CheckerBigTriangle}, // 25

    {FeederBigParabol, CheckerBigParabol}, // 26
    {FeederBigParabol, CheckerBigParabol}, // 27
    {FeederBigParabol, CheckerBigParabol} // 28
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 10 Andrew Convex hull";
}

static int LabTimeout = 3000;
int GetTestTimeout() {
    return LabTimeout;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit() {
    return LabMemoryLimit;
}
