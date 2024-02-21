
#include <stdio.h>

#define MAX_TRACE_VARS 20

struct tr {
    char name;
    int value;
} Traces[MAX_TRACE_VARS];

int nTr = 0;

void add_TT(char var) 
{
    if (nTr < MAX_TRACE_VARS) 
    {
        Traces[nTr].name = var;
        Traces[nTr].value = 0; 
        nTr++;
    } else 
    {
       printf("Error: Trace table is full.\n");
    }
}
void display_TT(char var) 
{
    for (int i = 0; i < nTr; i++) 
    {
        if (Traces[i].name == var) 
        {
            printf("%c: %d\n", var, Traces[i].value);
            char c;
            getchar();
            return;
        }
    }
    printf("Error: Variable %c not found in trace table.\n", var);
}
void update_TT(char var, int val) 
{
    for (int i = 0; i < nTr; i++) 
    {
        if (Traces[i].name == var) 
        {
            Traces[i].value = val;
            return;
        }
    }
    printf("Error: Variable %c not found in trace table for update.\n", var);
}
