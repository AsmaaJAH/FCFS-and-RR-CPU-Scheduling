#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 50
#define SIZE 10

int main(int argc, char** argv)
{
    int i = 0;
    char line[20];
    int n=0;
    typedef struct
    {
            char name[SIZE]; //process name
            int at;  //arrive time
            int pt;  //process time
            int ft;  //finish time
            int rt;  //round time
            int wt;  //wait time
    } Process;
    Process pcs[N];
    FILE *file = fopen( "InputFile.txt" , "r");
    while (fgets(line,sizeof(line),file) != NULL)
    {

            sscanf(line, "%s %d %d", pcs[i].name, &pcs[i].at, &pcs[i].pt);
            line[strlen(line)-1] = '\0';
            printf("%s %d %d\n",pcs[i].name, pcs[i].at, pcs[i].pt);
            i++;
    }
    fclose(file);
    pcs[0].ft=pcs[0].at+pcs[0].pt;
    pcs[0].rt=pcs[0].ft-pcs[0].at;
    pcs[0].wt=0;
    for (;n<4;n++)
    {
            if (pcs[n].at<pcs[n-1].ft)
            {
                    pcs[n].ft=pcs[n-1].ft+pcs[n].pt;
                    pcs[n].rt=pcs[n].ft-pcs[n].at;
                    pcs[n].wt=pcs[n-1].ft-pcs[n].at;
            }
            else
            {
                    pcs[n].ft=pcs[n].at+pcs[n].pt;
                    pcs[n].rt=pcs[n].ft-pcs[n].at;
                    pcs[n].wt=pcs[n-1].ft-pcs[n].at;
            }
    }
            int x = 0;
    for (;x<n;x++)
    {
            printf("process name: %s", pcs[x].name);
            printf("Turnaround Time: %d", pcs[x].rt);
            printf("Wait Time: %d\n", pcs[x].wt);
    }
    return(0);
}
