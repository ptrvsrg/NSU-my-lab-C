#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf position;

enum
{
    SUCCESS_JUMP = 1,
    MAX_POINT_COUNT = 5000
};

typedef struct
{
    int X;
    int Y;
} TPoint;

///////////////////////////////////  VECTOR TYPE  ///////////////////////////////////

typedef struct
{
    int Count;
    TPoint* Array;
} TVector;

TVector CreateVector(int count, TPoint* array)
{
    TVector vector = { count, array };
    return vector;
}

void DestroyVector(TVector* vector)
{
    free(vector->Array);
}

///////////////////////////////////  ERRORS  ///////////////////////////////////

void BadNumberOfPointsError(int pointCount)
{
    if(pointCount > MAX_POINT_COUNT)
    {
        printf("bad number of points");
        longjmp(position, SUCCESS_JUMP);
    }
}

void BadCoordinatesError(TPoint point)
{
    if(point.X > INT16_MAX || point.X < INT16_MIN || point.Y > INT16_MAX || point.Y < INT16_MIN)
    {
        printf("bad coordinates");
        longjmp(position, SUCCESS_JUMP);
    }
}

void BadNumberOfLinesError(void)
{
    printf("bad number of lines");
    longjmp(position, SUCCESS_JUMP);
}

///////////////////////////////////  INPUT  ///////////////////////////////////

int InputPointCount(void)
{
    int pointCount = 0;
    if (scanf("%d", &pointCount) != 1)
    {
        BadNumberOfLinesError();
    }
    BadNumberOfPointsError(pointCount);

    return pointCount;
}

TPoint* InputPointArray(int pointCount)
{
    TPoint* pointArray = calloc(pointCount, sizeof(*pointArray));
    assert(pointArray != NULL);

    for (int i = 0; i < pointCount; ++i)
    {
        if (scanf("%d%d", &pointArray[i].X, &pointArray[i].Y) != 2)
        {
            free(pointArray);
            BadNumberOfLinesError();
        }

        BadCoordinatesError(pointArray[i]);
    }

    return pointArray;
}

TVector InputVector(void)
{
    int count = InputPointCount();
    TPoint* array = InputPointArray(count);
    return CreateVector(count, array);
} 

///////////////////////////////////  STACK TYPE  ///////////////////////////////////

typedef struct Tstack* TStack;
struct Tstack 
{
    TPoint Value;
    TStack Next;
};

TStack CreateStack(void)
{
    return NULL;
}

bool IsEmptyStack(TStack stack)
{
    return stack == NULL;
}

TStack CreateItem(TPoint value)
{
    TStack new = malloc(sizeof(*new));
    assert(!IsEmptyStack(new));

    new->Value = value;
    new->Next = NULL;
    
    return new;
}

void PushStack(TPoint value, TStack* stack)
{
    TStack new = CreateItem(value);
    new->Next = *stack;
    (*stack) = new;
}

TPoint TopStack(TStack stack)
{
    assert(!IsEmptyStack(stack));
    return stack->Value;
}

TPoint NextToTopSTack(TStack stack)
{
    assert(!IsEmptyStack(stack) && !IsEmptyStack(stack->Next));
    return stack->Next->Value;
}

void PopStack(TStack* stack)
{
    assert(!IsEmptyStack(*stack));

    TStack removeItem = *stack;
    *stack = (*stack)->Next;

    free(removeItem);
}

void DestroyStack(TStack* stack) 
{
    if (!IsEmptyStack(*stack))
    {
        DestroyStack(&(*stack)->Next);
        free(*stack);
    }
}

void PrintStack(TStack stack)
{
    if(!IsEmptyStack(stack))
    {
        printf("%d %d\n", stack->Value.X, stack->Value.Y);
        PrintStack(stack->Next);
    }
}

///////////////////////////////////  CREATE UPPER AND LOWER SET  ///////////////////////////////////

void FindMinMax(TVector vector, TPoint* min, TPoint* max)
{
    TPoint minimum = { INT16_MAX, INT16_MAX };
    TPoint maximum = { INT16_MIN, INT16_MIN };

    for (int i = 0; i < vector.Count; ++i)
    {
        if (vector.Array[i].Y < minimum.Y || (vector.Array[i].Y == minimum.Y && vector.Array[i].X < minimum.X))
        {
            minimum = vector.Array[i];
        }

        if (maximum.Y < vector.Array[i].Y || (vector.Array[i].Y == maximum.Y && maximum.X < vector.Array[i].X))
        {
            maximum = vector.Array[i];
        }  
    }

    *min = minimum;
    *max = maximum;
}

TVector CreateUpperVector(TVector vector, TPoint min, TPoint max)
{
    TPoint* upperSet = calloc(vector.Count, sizeof(*vector.Array));
    assert(upperSet != NULL);
    
    int64_t dx1 = (int64_t)max.X - min.X;
    int64_t dy1 = (int64_t)max.Y - min.Y;

    int index = 0;

    for (int i = 0; i < vector.Count; ++i)
    {
        int64_t dx2 = (int64_t)vector.Array[i].X - min.X;
        int64_t dy2 = (int64_t)vector.Array[i].Y - min.Y;

        if(dy2 * dx1 > dy1 * dx2)
        {
            upperSet[index] = vector.Array[i];
            ++index;
        }
    }

    upperSet[index] = min;
    upperSet[index + 1] = max;
    index += 2;
    
    return CreateVector(index, upperSet);
}

TVector CreateLowerVector(TVector vector, TPoint min, TPoint max)
{
    TPoint* lowerSet = calloc(vector.Count, sizeof(*vector.Array));
    assert(lowerSet != NULL);
    
    int64_t dx1 = (int64_t)max.X - min.X;
    int64_t dy1 = (int64_t)max.Y - min.Y;

    int index = 0;

    for (int i = 0; i < vector.Count; ++i)
    {
        int64_t dx2 = (int64_t)vector.Array[i].X - min.X;
        int64_t dy2 = (int64_t)vector.Array[i].Y - min.Y;

        if(dy2 * dx1 < dy1 * dx2)
        {
            lowerSet[index] = vector.Array[i];
            ++index;
        }
    }

    lowerSet[index] = min;
    lowerSet[index + 1] = max;
    index += 2;
    
    return CreateVector(index, lowerSet);
}

///////////////////////////////////  COMPARISON FUNCTION FOR QUICK SORT  ///////////////////////////////////

int CompareLower(const void* a, const void* b)
{
    const TPoint* A = a;
    const TPoint* B = b;

    int difference = A->X - B->X;

    return (difference == 0) ? A->Y - B->Y : difference;
}

int CompareUpper(const void* a, const void* b)
{
    const TPoint* A = a;
    const TPoint* B = b;

    int difference = B->X - A->X;

    return (difference == 0) ? B->Y - A->Y : difference;
}

///////////////////////////////////  CHANGED GRAHAM ALGORITHM  ///////////////////////////////////

int64_t VectorMultiplication(TPoint a, TPoint b, TPoint c)
{
    int64_t dx1 = (int64_t)b.X - a.X;
    int64_t dx2 = (int64_t)c.X - a.X;
    int64_t dy1 = (int64_t)b.Y - a.Y;
    int64_t dy2 = (int64_t)c.Y - a.Y;

    return (dx1 * dy2) - (dy1 * dx2);
}

bool RotationCheck(TPoint a, TPoint b, TPoint c)
{
    return VectorMultiplication(a, b, c) > 0;
}

TStack GrahamAlgorithm(TVector vector, int (*compare)(const void*, const void*))
{
    if (vector.Count == 0)
    {
        return NULL;
    }
    else if (vector.Count == 1)
    {
        return CreateItem(*vector.Array);
    }
    
    qsort(vector.Array, vector.Count, sizeof(*vector.Array), compare);

    TStack stack = CreateStack();

    PushStack(vector.Array[0], &stack);
    PushStack(vector.Array[1], &stack);

    for (int i = 2; i < vector.Count; ++i)
    {
        while (!IsEmptyStack(stack->Next) && !RotationCheck(TopStack(stack), vector.Array[i], NextToTopSTack(stack)))
        {
            PopStack(&stack);
        }
        
        PushStack(vector.Array[i], &stack);
    }
    
    return stack;
}

///////////////////////////////////  ANDREW ALGORITHM  ///////////////////////////////////

void AndrewAlgorithm(TVector vector)
{
    TPoint min = { INT16_MAX, INT16_MAX };
    TPoint max = { INT16_MIN, INT16_MIN };

    FindMinMax(vector, &min, &max);

    TVector lowerVector = CreateLowerVector(vector, min, max);
    TVector upperVector = CreateUpperVector(vector, min, max);

    TStack lowerStack = GrahamAlgorithm(lowerVector, CompareLower);
    PopStack(&lowerStack);
    TStack upperStack = GrahamAlgorithm(upperVector, CompareUpper);
    PopStack(&upperStack);

    PrintStack(lowerStack);
    PrintStack(upperStack);

    DestroyVector(&lowerVector);
    DestroyVector(&upperVector);

    DestroyStack(&lowerStack);
    DestroyStack(&upperStack);
}

///////////////////////////////////  MAIN  ///////////////////////////////////

int main(void)
{
    assert(freopen("in.txt", "r", stdin) != NULL);
    assert(freopen("out.txt", "w", stdout) != NULL);

    if (setjmp(position) == 0)
    {
        TVector vector = InputVector();
        AndrewAlgorithm(vector);
        DestroyVector(&vector);
    }
    
    fclose(stdin);
    fclose(stdout);

    return EXIT_SUCCESS;
}