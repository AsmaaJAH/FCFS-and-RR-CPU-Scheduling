#include <stdio.h>
#include <stdlib.h>
int n=0,n_io=0;
int clk=0,q=2;
int top=-1;
int idx=0;
int top_io=-1,fIO=0;
int que [100],block[100];
int counter=0, ff_r=-1,ff_cpu=-1,ff_clk=-1,ff_io=-1,ff_io2=-1,ff=-1,ff_id=-1,ff_id2=-1,ff_for=-1,ff_for2=-1;

struct processes
{
    int ID;             //name of the process
    int cpu;            //CPU time
    int aTime;             //arrival time
    int IO;             //IO time
    int cpu_c;          //finished time of the process
    int c;              //2nd usage of CPU time
    int flag;           //no of times of using cpu
    int arrConst;            //first arrival time

};
struct processes queue_array[10];

//sort elements in the queue according to their arrival time
void sortElements()
{
    int iterator,j2,t;
    for(iterator=idx; iterator<top; iterator++)
    {

        for(j2=iterator+1; j2<=top; j2++)
        {
            if(queue_array[que[iterator]].aTime > queue_array[que[j2]].aTime)
            {
                t = que[iterator];
                que[iterator]= que[j2];
                que[j2] = t;
            }
            //if two processes arrived aTime the same time put the smallest ID first in the Queue
            else if (queue_array[que[iterator]].aTime == queue_array[que[j2]].aTime)
            {
                if (queue_array[que[iterator]].ID > queue_array[que[j2]].ID)
                {
                    t = que[iterator];
                    que[iterator]= que[j2];
                    que[j2] = t;
                }

            }
        }
    }
}
//get input from file
void inputs(FILE *fp)
{
    char chr ='a';
    chr =getc(fp);
    while (chr!=EOF)
    {
        queue_array[n].ID=chr-'0';

        chr =getc(fp);//white space
        chr =getc(fp);

        queue_array[n].cpu=chr-'0';
        queue_array[n].c=chr-'0';

        chr =getc(fp);
        chr =getc(fp);

        queue_array[n].IO=chr-'0';
        chr =getc(fp);
        chr =getc(fp);

        queue_array[n].aTime=chr-'0';
        queue_array[n].arrConst=chr-'0';
        queue_array[n].cpu_c=0;
        queue_array[n].flag=2;
        chr =getc(fp);//new line
        //in case the process doesn't use the CPU put it in the block queue
        if ( queue_array[n].cpu == 0)
        {   //finished time of this process will be the time which it finished IO
            queue_array[n].cpu_c= queue_array[n].aTime+ queue_array[n].IO ;
            block[++top_io]=queue_array[n].ID;
        }
        else
            que[++top]=queue_array[n].ID; //insert the pro in the que
        n++;
        chr =getc(fp);
    }
}
////////////////////////////////////////////////////////////////print the output/////////////////////////////////////////////////////////
void print ()
{
    FILE *fptr;
    fptr = fopen("output.txt","a");
    if (ff_clk != clk)
    {  fprintf(fptr,"%d ",clk);
        printf("%d",clk );
        ff_clk=clk;
    }
     //run the process if its arrived while clk is greater than or equal it
    if (queue_array[que[idx]].aTime<=clk && ff_cpu!=clk)
    {   fprintf(fptr," %d:running ",queue_array[que[idx]].ID);
        printf(" %d:running ",queue_array[que[idx]].ID);
        ff_cpu=clk;
        //counter++;
    }
    //the process is blocked when it finishes the first usage of CPU
    else if (queue_array[que[idx]].aTime > clk && queue_array[que[idx]].aTime != queue_array[que[idx]].arrConst && ff_io!=clk)
    {   fprintf(fptr," %d:block ",queue_array[que[idx]].ID);
        printf(" %d:block ",queue_array[que[idx]].ID);
        ff_io=clk;
    }
    if (ff_for2 != clk)
    {

    for (int iterator=fIO ; iterator<=top_io ; iterator++)
    {   //print the blocked processes which doesn't use the CPU
        if (queue_array[block[iterator]].arrConst+queue_array[block[iterator]].IO > clk && clk> queue_array[que[0]].arrConst && (ff_io2!=clk|| ff_id2!= queue_array[block[iterator]].ID))
        {   fprintf(fptr," %d:block ",queue_array[block[iterator]].ID);
            printf(" %d:block ",queue_array[block[iterator]].ID);
            ff_io2=clk;
            ff_id2= queue_array[block[iterator]].ID;
        }
    }
    ff_for2 = clk;
    }
    if (ff_for != clk)
    {
        for (int i=idx+1 ; i<=top ; i++)
        {   //print the ready process if when it arrives there is another process with smaller ID use the CPU
            if (queue_array[que[i]].aTime <= clk && (ff_r!=clk|| ff_id!= queue_array[que[i]].ID) )
            {   fprintf(fptr," %d:ready ",queue_array[que[i]].ID);
                printf(" %d:ready ",queue_array[que[i]].ID);

                ff_r=clk;
                ff_id= queue_array[que[i]].ID;
            }
            //the process is blocked when it finishes the first usage of CPU
            else if (queue_array[que[i]].aTime > clk && queue_array[que[i]].aTime != queue_array[que[i]].arrConst && ff_io!=clk)
            {   fprintf(fptr," %d:block ",queue_array[que[i]].ID);
                printf(" %d:block ",queue_array[que[i]].ID);
                ff_io=clk;
            }

        }
        ff_for = clk;
    }
    if (ff != clk)
    {   fprintf(fptr,"\n");
        printf("\n");
        ff=clk;
    }
}

int main()
{
    ////////////////////////////////////////////////////////taking  the file name/////////////////////////////////////////////////////////////
    char file[100];
    char in;
    printf("Enter the inputs File name : ");
    int iterator=0;
    scanf("%c",&in);
    while(in!='\n')
    {
        file[iterator]=in;
        scanf("%c",&in);
        iterator++;
    }

//read from the file
    FILE *fp=  freopen(file, "r",stdin);
    inputs(fp);
//sortElements the input processes according to arrival time
    sortElements();
    print();
///while queue of processes isn't empty
    while (top>=idx )
    {
        print();                                               //to print the output

        if (queue_array[que[idx]].aTime<=clk)                 //check for arrived process to run it
        {
            if (queue_array[que[idx]].cpu-q>=0)              //if the process use CPU with time greaTimeer than or equal the quantum time
            {   //increase the clk by quantum time
                clk+=q;
                counter+=q;                                    //increase no of usage of CPU
            }
            //if the process use CPU with time smaller the quantum time
            else
            {
                //increase the clk by CPU time
                clk+=(queue_array[que[idx]].cpu);
                counter+=queue_array[que[idx]].cpu;           //increase no of usage of CPU
            }

            queue_array[que[idx]].cpu-=q;
             //if the process is done its first usage of CPU
            if ( queue_array[que[idx]].cpu<=0 && queue_array[que[idx]].flag==2)
            {
                queue_array[que[idx]].flag--;                              //decrease the flag of no of usage cpu
                queue_array[que[idx]].cpu= queue_array[que[idx]].c;      //store the CPU time for 2nd usage
                queue_array[que[idx]].aTime=clk+queue_array[que[idx]].IO; //updaTimee the arrived time of the process to enter the queue for 2nd usage of CPU
                que[++top]=queue_array[que[idx]].ID;                       //push the process at the end of the queue
                idx++;                                                     //increase the front pointer of the queue
                sortElements();                                                      //sort the processes according to their arrival time
            }
            //if the process is done its 2nd usage of CPU
            else if   ( queue_array[que[idx]].cpu<=0&&queue_array[que[idx]].flag==1)
            {   //finishing time of the process
                queue_array[que[idx]].cpu_c=clk;
                idx++;                            //remove the process from the queue
            }
            //if the process was in ready queue
            else
            {
                queue_array[que[idx]].aTime=clk;         //update its arrived time
                que[++top]=queue_array[que[idx]].ID;   //push the process at the end of the queue
                idx++;                                //increase the front pointer of the queue
                sortElements();                                //sort the processes according to their arrival time
                print();                               //for print the output
            }
        }
        //if there aren't any running process
        else
        {
            print();
            clk++;
        }
    }
    //print at output file
    FILE *fptr;
    fptr = fopen("output.txt","a");
    //print finish time,CPU Utilization and Turnaround time
    fprintf(fptr,"\nFinishing time: %d",clk);
    printf("\nFinishing time: %d",clk);
    fprintf(fptr,"\nCPU Utilization:%.3f\n",(float)counter/(float)clk);
    printf("\nCPU Utilization:%.3f\n",(float)counter/(float)clk);
    for (int i=0 ; i< n; i++)
    {   fprintf(fptr,"Turnaround time of Process %d:",queue_array[i].ID);
        printf("Turnaround time of Process %d:",queue_array[i].ID);
        fprintf(fptr," %d\n",queue_array[i].cpu_c-queue_array[i].arrConst);
        printf(" %d\n",queue_array[i].cpu_c-queue_array[i].arrConst);

    }
return 0;
}
