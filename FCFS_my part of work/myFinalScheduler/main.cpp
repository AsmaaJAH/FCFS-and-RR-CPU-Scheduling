#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <list>
#include <vector>
#include <bits/stdc++.h>
#include <string.h>
using namespace std;


#define INITIALIZE 0
#define MAX 100
#define DEBUG 1
#define EXIT -1

#define onlyONE 1
#define RR 1
#define FCFS 0

// --------------------------------- FCFS Structs process nodes data types -----------------------------
struct processNode
{
                            unsigned int id;
                            unsigned int cpu;
                            unsigned int io;
                            unsigned int arrivalTime;
                            unsigned int turnaroundTIME;

                            //to sorting according to the arrival time and processNode id
                            bool operator <(const struct processNode & processNodeObj) const
                            {

                                if (arrivalTime < processNodeObj.arrivalTime)
                                {
                                      return true;

                                } else if (arrivalTime == processNodeObj.arrivalTime && cpu!=0 && processNodeObj.cpu!=0)
                                {
                                    return (id < processNodeObj.id);

                                }else if (arrivalTime == processNodeObj.arrivalTime && cpu!=0 && processNodeObj.cpu==0)
                                {
                                      return true;

                                }else
                                {
                                    return false;
                                }

                            }
};
typedef struct processNode Process;

vector<Process> processARR(MAX);                           //same as an array of process nodes .. as a dynamic array.. stores the elements at memory locations which are next to each other
list<Process> readyLIST,readyQueue,queueCopy, blockLIST;   // same as doubly linked lists .. stores the elements at memory locations which are not next to each other with head and tail pointers


//-------------------------------------------------------------------- main driver -----------------------------------------------------------------//
int main()
{

            // ----------------------------- data types definitions and declarations ----------------------

            int i = INITIALIZE;                      //iterations counter
            int notBusyTick = INITIALIZE;
            int scheduler = EXIT;
            int quantum = INITIALIZE;
            int processCounter = INITIALIZE;
            int flag=INITIALIZE;
            int idleSystem=INITIALIZE;
            char singleline[MAX];
            float cpuUtilize = INITIALIZE;
            unsigned int tick = INITIALIZE;
            int runningDuplicatePRINTING= EXIT;

//------------------------------------- input from user and initializing the process array from input file -----------------------------------------------------

            FILE *inputFile;
            FILE *outputFile;

            inputFile = fopen("inFile.txt", "r");
            outputFile = fopen("outFile.txt","w");

            if(inputFile == NULL){
                printf("File Not Found\n");
                return -1;
            }

            while (!feof(inputFile))
            {
                fgets(singleline, MAX, inputFile);
                sscanf(singleline,"%d %d %d %d",&processARR[i].id,&processARR[i].cpu,&processARR[i].io,&processARR[i].arrivalTime);
                processARR[i].turnaroundTIME = 0;
                processCounter++;
                i++;
            }
            int remainingPROCESSES = processCounter;
            fclose(inputFile);
            #if DEBUG == 0
                printf("lines: %d",processCounter);
                for(int i= 0 ; i < processCounter ; i++)
                {
                    printf("id: %d, cpu: %d, io: %d, arrival: %d\n",processARR[i].id,processARR[i].cpu,processARR[i].io,processARR[i].arrivalTime);
                }
            #endif
            printf("Hello Dude, I am your scheduling system, Choose What u wanna do:\n0:FCFS \n1:RR \n-1:exit \n");
            scanf("%d",&scheduler);
            system("cls");  //to avoid a system exit error with -ve status instead of exit with status 0

            if(scheduler == EXIT )
            {
                    return 0;
            }

            if(scheduler == RR){
                printf("Enter your desirable quantum length: \n");
                scanf("%d",&quantum);
                system("cls");
            }
////------------------------------------------------------------ operations and processing the input data -----------------------------------------------
           while(1)
            {
                        fprintf(outputFile,"%d: ",tick);
                        fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                            // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                        // traverse processARR to check if process as arrived and add them to queue
                        for(i = 0 ; i < processCounter ; i++)
                        {
                            // check for arrival
                            if(processARR[i].arrivalTime == tick)
                            {
                                readyLIST.push_back(processARR[i]);
                                flag ++;
                                idleSystem++;

                            }
                        }
                        //if
                        if ( flag ==0  && tick==0)
                        {
                                        fprintf(outputFile,"No process has arrived yet.\t");
                                        fflush(outputFile);
                        }




                        //For Debugging purposes only:
                        #if DEBUG==0
                                            fprintf(outputFile,"Hello Asmaa \n\n");
                                            fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                /*
                //--------------------------currently, there is no need for these next few lines ,so i comment it for now ---------------------------------------------
                                            // if the arrival time of all processes < current tick
                                            //int n = sizeof(processARR) / sizeof(processARR[0]);
                                            unsigned int tickCopy= tick;
                                            if(readyLIST.size()==0 )
                                               {
                                                    int n=processCounter -  (int) tickCopy;   //type casting for the tick copy
                                                    // sort the process array in increasing order of arrival time
                                                    sort(processARR.begin() , processARR.end() , compareArrivalTime);
                                                    for(i = 0 ; i < n && processARR[i].arrivalTime != tickCopy ; i++)
                                                    {

                                                            fprintf(outputFile,"some processes haven't arrived yet.\t");
                                                            fflush(outputFile);
                                                            readyLIST.push_back(processARR[i]);
                                                            tickCopy++;
                                                    }

                                               }
                        */
                        #endif // DEBUG
                //-------------------------------------------------------------------------------------------------------------------------------------------------------

                        if(blockLIST.size() > 0)
                        {
                            for(auto it = blockLIST.begin(); it != blockLIST.end();++it)    //i2 for this element ==> it;

                                {
                                // decrement io_block and check if io request handled
                                if(it->io <= 0)
                                {
                                    for(int i = 0 ; i < processCounter ; i++)
                                    {
                                        if(processARR[i].id == it->id)
                                        {
                                            it->cpu = processARR[i].cpu;
                                        }
                                    }
                                    readyLIST.push_back(*it);
                                    it = blockLIST.erase(it);
                                    --it;
                                }else
                                {
                                    fprintf(outputFile,"process %d: blocked\t",it->id);
                                    fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                        // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                                    it->io--;           //decrease the i/o delay of this iterator element
                                    idleSystem++;
                                }
                            }
                        }

                        // sort the processes in increasing order of arrival time and id
                        readyLIST.sort();
                                                                                       //it==> iterator acts like a pointer indicates the items inside the list
                        for(auto it = readyLIST.begin(); it != readyLIST.end(); ++it) //auto ==> this keyword so the type of the "it" declared variable will be automatically deducted from its initializer.
                        {
                                readyQueue.push_back(*it);
                                it = readyLIST.erase(it);
                                it--;

                                // DEBUG
                                #if DEBUG == 0
                                         // printf("\n \n\n %d \n \n \n \n",*it);

                                 // if ( (it->cpu) ==0 )       //If this process does not need a CPU usage ,and, it ONLY needs i/o usage
                                    // readyQueue.pop_back();   //then pop it from the ready queue of the CPU as a waiting process for the CPU to finish
                                #endif
                        }
                        if(readyQueue.size() <= 0 && remainingPROCESSES> 0)
                        {
                            notBusyTick++; //if this one is an Empty processor tick one, with no thing to update or do

                        }
//-----------------------------------------------------FCFS scheduling system --------------------------------------------------------------------------
                        if(scheduler == FCFS)
                        {
                            if(readyQueue.size() > 0)
                            {
                                if(readyQueue.front().cpu > 0)
                                {
                                    fprintf(outputFile,"process %d: running \t",readyQueue.front().id);
                                    fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                        // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file


                                    runningDuplicatePRINTING=readyQueue.front().id;         //to solve the repeated issue in printing the ready and the running
                                    readyQueue.front().cpu--;
                                    idleSystem++;

                                    #if DEBUG == 0
                                        printf("id: %d, cpu: %d, io: %d, arrival: %d\n",readyQueue.front().id,readyQueue.front().cpu,readyQueue.front().io,readyQueue.front().arrivalTime);
                                    #endif
                                }
                                if(readyQueue.front().cpu <= 0 && readyQueue.front().io > 0)
                                {
                                    blockLIST.push_back(readyQueue.front());
                                    readyQueue.pop_front();


                                    #if DEBUG == 0
                                        printf("id from ready: %d,id from blocked: %d , readyQueue size: %d\n",readyQueue.front().id,blockLIST.front().id,readyQueue.size());

                                    #endif
                                }
                                if(readyQueue.front().cpu <= 0 && readyQueue.front().io <= 0)
                                {
                                    for(int i = 0 ; i < processCounter ; i++)
                                        {
                                                if(readyQueue.front().id == processARR[i].id)
                                                {
                                                    processARR[i].turnaroundTIME = tick - processARR[i].arrivalTime + 1; // or ready queue>=0 in if condition
                                                }
                                        }

                                    remainingPROCESSES--;
                                    if (readyQueue.size()) // this "if condition" purpose is to fix and debug test 0 in the pdf file of assignment 3
                                         readyQueue.pop_front();



                                    #if DEBUG == 0

                                            printf("remaining %d processes \t the id of the current process in the readyQueue front:%d   process in the blockQueue front:%d \n ",remainingPROCESSES,readyQueue.front().id,blockLIST.front().id);
                                    #endif // DEBUG

                                }
                            }

                        }



/*
//------------------------------------------------------------------Round Robin scheduling system -------------------------------------------------------------

                        else if(scheduler == RR)
                          {

                          }
*/
//------------------------------------------------------------ finishing the final operations and printing the output ----------------------------------

                        if (tick !=0 && remainingPROCESSES == onlyONE)
                        {
                            if (readyQueue.front().cpu== INITIALIZE && readyQueue.front().io==INITIALIZE)
                            {
                                remainingPROCESSES--;
                                readyQueue.pop_front();
                            }
                        }

                        //int tot=0;
                        if(remainingPROCESSES <= INITIALIZE)
                        {
                                #if DEBUG==0

                                //to make sure or to make indicator that all process finishes there i/o and cpu
                                for (int i =0 ; i<processCounter ;i++)
                                {
                                    tot= tot + processARR[i].cpu + processARR[i].io;
                                }

                                if (tot==INITIALIZE)
                                #endif
                                    break;

                        }

                        #if DEBUG == 0
                        //tot=0;

                        /*
                                                queueCopy= readyQueue; // just to keep the data
                                                for(auto j = queueCopy.begin() ; j == queueCopy.end() ; j++)
                                                {
                                                        //if the current process finished its cpu and io time
                                                      if( tick!=0 && (j->cpu) <=INITIALIZE && (j->io) <= INITIALIZE)
                                                        {
                                                            j= queueCopy.erase(j);
                                                            j--;
                                                            remainingPROCESSES--;
                                                        }

                                                }
                                                readyQueue=queueCopy;
                        */
                        #endif
                        //ready states printing  of the processes
                        for(i = 0 ; i < processCounter ; i++)
                        {
                                if(   processARR[i].arrivalTime == tick &&   i!=runningDuplicatePRINTING )
                                {

                                    fprintf(outputFile,"process %d: ready\t",processARR[i].id);
                                    fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                        // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file
                                 }
                        }
                        // to solve the empty printing line in the output file if the the i/o, CPU and the whole sys are idle for this current time tick
                        if(idleSystem==0 && tick!=0)
                        {
                                fprintf(outputFile,"your whole system is idle for now.");
                                fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                    // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                        }
                        idleSystem=0; // getting ready for the next while (1) looping

                        tick++;
                        fprintf(outputFile,"\n");
                        fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                            // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                        #if DEBUG==0
                         if(readyQueue.front().cpu <= 0 && readyQueue.front().io > 0 && remainingPROCESSES )
                            {
                                    continue;
                            }
                        #endif
            }

            fprintf(outputFile,"\n\n");
            fflush(outputFile); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file


//------------------------------------------------------------calculating the final outputs  and exit ------------------------------------------------

            fprintf(outputFile,"Finishing time: %d\n",tick);
            fflush(outputFile);

            cpuUtilize = (float)(tick + 1 - notBusyTick)/(tick+1);
            fprintf(outputFile,"CPU utilization : %.2f\n",cpuUtilize);
            fflush(outputFile);

            for(int i =0 ; i < processCounter; i++)
            {
                if( processARR[i].cpu ==INITIALIZE && processARR[i].io!=INITIALIZE)
                {
                    processARR[i].turnaroundTIME = processARR[i].io + processARR[i].arrivalTime;
                    fprintf(outputFile,"Turnaround time of Process %d: %d\n",processARR[i].id,processARR[i].turnaroundTIME );

                }else
                {
                    fprintf(outputFile,"Turnaround time of Process %d: %d\n",processARR[i].id,processARR[i].turnaroundTIME);
                    fflush(outputFile);
                }

            }
            fclose(inputFile);
            fclose(outputFile);
            printf("You gonna find your desired output in the output file so check it\nHave a nice day\nBye!");
            return 0;
}
