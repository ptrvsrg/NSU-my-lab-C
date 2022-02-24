#include "testLab.h"
#include <stdio.h>
#include <string.h>

#define NO_POINTS 2147483647
typedef struct 
{
    int x, y;
} point_t;

static int testN = 0;
static const struct { const char *const in, *const outError; int pointCount; point_t outPoints[8]; } testInOut[] = {
    { "4\n0 0\n2 1\n4 0\n0 4\n", NULL, 3, { {0, 0}, {4, 0}, {0, 4} } }, // 1
    { "2001", "bad number of points", 0, { NO_POINTS } }, // 2
    { "3\n200000 1\n0 300000\n400000 500000\n", "bad coordinates", 0, { NO_POINTS } }, // 3
    { "5\n1 1\n4 5\n", "bad number of lines", 0, { NO_POINTS } }, // 4

    { "4\n-32768 0\n0 32767\n0 -32768\n32767 0", NULL, 4, { {0, -32768}, {32767, 0}, {0, 32767}, {-32768, 0} } }, // 5
    { "5\n-32768 0\n0 32767\n0 -32768\n32767 0\n0 0", NULL, 4, { {0, -32768}, {32767, 0}, {0, 32767}, {-32768, 0} } }, // 6
    { "8\n-32768 0\n0 32767\n0 -32768\n32767 0\n-32768 -32768\n-32768 32767\n32767 -32768\n32767 32767", NULL, 4, { {-32768, -32768}, {32767, -32768}, {32767, 32767}, {-32768, 32767} } }, // 7
    { "4\n32767 32767\n32767 32767\n32767 32767\n32767 32767", NULL, 1, { {32767, 32767} } }, //8
    
    { "8\n2 3", NULL, 1, { {2, 3} } }, // 9
    { "1\n2 3", NULL, 1, { {2, 3} } }, // 10
    { "2\n1 1\n4 5\n", NULL, 2, { {1, 1}, {4, 5} } }, // 11
    { "5\n0 0\n2 3\n4 0\n0 4\n4 4\n", NULL, 4, { {0, 0}, {4, 0}, {0, 4}, {4, 4} } }, // 12

    { "0", NULL, 0, { NO_POINTS } }, // 13
    { "2001\n200000 1\n0 300000\n400000 500000\n", "bad number of points", 0, { NO_POINTS } }, // 14
    { "5\n200000 1\n0 300000\n400000 500000\n", "bad coordinates", 0, { NO_POINTS } }, // 15
    { "3\n1 2\n 4 3\n 5\n", "bad number of lines", 0, { NO_POINTS } }, // 16
    
    { "8\n0 0\n0 0\n0 0\n0 0\n0 0\n0 0\n0 0\n0 0", NULL, 1, { {0, 0} } }, // 17
    { "8\n0 7\n0 4\n0 0\n0 3\n0 2\n0 1\n0 5\n0 6\n", NULL, 2, { {0, 0}, {0, 6} } }, // 18
    { "8\n3 3\n-1 -1\n5 5\n-4 -4\n1 1\n2 2\n-2 -2\n3 3\n", NULL, 2, { {-4 -4}, {5, 5} } }, // 19
    { "8\n4 -3\n3 4\n-3 -4\n-4 3\n4 3\n3 -4\n-4 -3\n-3 4\n", NULL, 8, { {-3, -4}, {3, -4}, {4, -3}, {4, 3}, {3, 4}, {-3, 4}, {-4, 3}, {-4, -3} } }, // 20
};

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

const char* ScanPoint(FILE* out, point_t* a) {
    return (ScanInt(out, &a->x) == Fail) ? Fail : ScanInt(out, &a->y);
}

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "r");

    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    if (testInOut[testN].outPoints->x != NO_POINTS) {
        int finded[8] = {0};
        int findedCount = 0;

        for (int i = 0; i < testInOut[testN].pointCount; i++) {
            point_t point = { 0, 0 };
            const char* status = ScanPoint(out, &point);

            if (status == Fail) {
                fclose(out);
                printf("%s\n", status);
                return status == Fail;
            }
            
            for (int j = 0; j < testInOut[testN].pointCount; j++) {
                if (point.x == testInOut[testN].outPoints[j].x && point.y == testInOut[testN].outPoints[j].y) {
                    if (finded[j] == 1) {
                        fclose(out);
                        printf("%s\n", status);
                        return status == Fail;
                    }
                    
                    ++finded[j];
                    ++findedCount;
                    break;
                }
            }
        }
        
        if (findedCount != testInOut[testN].pointCount) {
            fclose(out);
            printf("%s\n", Fail);
            return 1;
        }

        ++testN;
    }
    
    if (testInOut[testN].outError != NULL) {
        char error[32] = {0};
        const char* status = ScanChars(out, sizeof(error), error);
        fclose(out);

        if(status == Pass && _strnicmp(testInOut[testN].outError, error, strlen(testInOut[testN].outError)) != 0) {
            status = Fail;
        }

        printf("%s\n", status);
        ++testN;
        return status == Fail;
    }

    int passed = !HaveGarbageAtTheEnd(out);

    fclose(out);
    printf((passed) ? Pass : Fail);
    return passed;
}

static int LabTimeout;
static size_t LabMemoryLimit;

/*static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large array... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d\n", 2000000);
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i^0x1234567) == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES+4*2000000;
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1, last = 0;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 2000000; i++) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if ((n^0x1234567) < 0 || 2000000 <= (n^0x1234567)) {
            passed = 0;
            printf("wrong output -- ");
            break;
        } else if (i == 0) {
            last = n;
            continue;
        } else if (n <= last ) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large array... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d\n", 2000000);
    for (i = 0; i < 200000; i++) {
        if (fprintf(in, "0 0 0 0 0 0 0 0 0 0 ") == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES+4*2000000;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 2000000; i++) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if (n != 0) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}*/

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
    //{feederBig, checkerBig},
    //{feederBig2, checkerBig2}
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
