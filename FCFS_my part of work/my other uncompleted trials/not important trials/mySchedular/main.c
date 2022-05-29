#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
//#include <stdbool.h>


#define ROUNDROBIN 1
#define FCFS 0
#define MAX_NUM_OF_PROCESSES 100
#define DEBUG 1
//--------------------------------------Queue and data STRUCTs definitions and declerations ------------------------------
struct process{
    unsigned int id;
    unsigned int cpu_usage;
    unsigned int io_delay;
    unsigned int arrival_time;
    unsigned int turnaround_time;
};
typedef struct process Process;


// process node for the queue
typedef struct processNode
{
    Process* process;
    struct processNode* prev;
    struct processNode* next;
} processNode;

//process node's queue
typedef struct processQueue
{
    processNode* head;
    processNode* tail;
    size_t size;
} processQueue;

// push back a node to queue
void push(processQueue* queue, processNode* process)
{
    processNode* node = malloc(sizeof(processNode)); // create new node and input node
    node = process;
    node->prev = queue->tail;
    node->next = NULL;
    if (queue->size > 0) // if queue is  empty
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    else
    {
        queue->head = node;
        queue->tail = node;
    }
    queue->size += 1;
}

// bring the node from queue
processNode* pop(processQueue* queue, processNode* node)
{
    if (queue->head == node) // if the node exists in frist queue
    {
        queue->head = node->next;
    }
    else
    {
        node->prev->next = node->next;
    }

    if (queue->tail == node) // if the node exist in last queue
    {
        queue->tail = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
    }

    processNode* process = node;
    free(node);
    queue->size -= 1;
    return process; // return popped process node
}

// find where the node exist
processNode* find(processQueue* queue, Process* process)
{
    for (processNode* i = queue->head; i != NULL; i = i->next)
    {
        if (i->process == process)  // if find the node
            return i; // return the node
    }
    return NULL;
}

// to reuse the queue, free all nodes in the queue and initialize the queue
void clear(processQueue* queue)
{
    processNode* node = queue->head;
    while (node != NULL)
    {
        processNode* next_node = node->next;
        free(node);
        node = next_node;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

//--------------------------------------------------- sort---------------------------------------------
// bubblesort the linked list by swapping
void swap(processNode *a, processNode*b)
{
    processNode* temp = a;
    a = b;
    b= temp;
}
void sort(processNode * start)
{
    int swapped;
    processNode *ptr1;
    processNode *lptr = NULL;

    /* Checking for empty list */
    if (start == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = start;

        while (ptr1->next != lptr)
        {
            if (ptr1->process->id > ptr1->next->process->id)
            {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);

}


//-------------------------------------------------------------------------main driver -----------------------------------
int main(int argc, char* argv[])
{
//--------------------------definitions and declerations
        FILE *input_file;
        FILE *output_file;

        int i = 0;
        int quantum = 0;
        int current_quantum = 0;
        int numOfProcesses = 0;
        int not_busy_ticks = 0;
        unsigned int tick = 0;

        char singleline[100];
        float cpu_utilization = 0;
        int scheduler = -1;


//------------------------------------------ input data and file reading ------------------------------------------

        printf("Choose your Scheduling system: 0:FCFS 1:RR\t or choose: -1:exit \n");
        scanf("%d",&scheduler);

        if(scheduler == -1)
        {
            exit (0);
        }

        if(scheduler == ROUNDROBIN)
        {
            printf("Enter your quantum: ");
            scanf("%d",&quantum);
            current_quantum = quantum;
        }


        input_file = fopen("input_file.txt", "r");
        output_file = fopen("output_file.txt", "w");

        if(input_file == NULL )
            {
                    printf("your files are not exist or Cannot be found\n");
                    exit(0);
            }

//For Debugging purposes only:
#if DEBUG==1
                    fprintf(output_file,"hello asmaa \n\n");
                    fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file

#endif

// ------------------ initialize my queues
        processQueue ready_queue = { NULL, NULL, 0 };
        processQueue blocked_list = { NULL, NULL, 0 };
        //double linked list not a queue, i just named it queue
        // because i created it in the first place for the ready queue
        // then i discovered that no need to re create a new double linked list and i can use the old one

        processQueue ready_list={ NULL, NULL, 0 };
        processNode process_array[MAX_NUM_OF_PROCESSES]; //array of STRUCTs

        while (!feof(input_file))
            {
                fgets(singleline, 100, input_file);
                sscanf(singleline,"%d %d %d %d",&process_array[i].process->id,&process_array[i].process->cpu_usage,&process_array[i].process->io_delay,&process_array[i].process->arrival_time);
                process_array[i].process->turnaround_time = 0;
                numOfProcesses++;
                i++;
            }



//---------------------------------------------- FCFS--------------------------------------------------------------
        int leftProcesses = numOfProcesses;
        while(1)
        {
                            fprintf(output_file,"%d: ",tick);
                            // traverse process_array to check if process has arrived and add them to queue
                            for(i = 0 ; i < numOfProcesses ; i++){
                                // check for arrival
                                if(process_array[i].process->arrival_time == tick){
                                    // set the state
                                    // push in ready queue
                                    fprintf(output_file,"process %d: ready---\t",process_array[i].process->id);
                                    fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                         // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                                    push(&ready_list,& process_array[i]);
                                }
                            }

                            if(blocked_list.size > 0)
                            {
                                            // copy but i don't need now
                                            //GList* listCopy = NULL;
                                            // listCopy = g_list_append(listCopy,blocked_list );
                                            for(processNode* it = blocked_list.head ; it != blocked_list.tail->next ;++it) //it == iteration index
                                            {
                                                // decrement io_block
                                                // check if io request handled
                                                if(it->process-> io_delay <= 0)
                                                {
                                                                for(int i = 0 ; i < numOfProcesses ; i++)
                                                                {
                                                                    if(process_array[i].process->id == it->process-> id)
                                                                    {
                                                                        it->process-> cpu_usage = process_array[i].process->cpu_usage;
                                                                    }
                                                                }

                                                                push(&ready_list, it);

                                                                it = pop (&blocked_list, it);
                                                                --it;
                                                }else
                                                {
                                                                fprintf(output_file,"process %d: blocked\t",it->process-> id);
                                                                fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                                                     // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                                                                it->process-> io_delay--;
                                                }
                                            }
                            }

                            sort(ready_list.head);
                            for(processNode* it = ready_list.head ; it != ready_list.tail->next ;++it) //it == iteration index
                            {
                                push(&ready_queue,it);
                                it = pop (&ready_list,it);
                                it--;
                            }
                            if(ready_queue.size <= 0 && leftProcesses > 0){
                                not_busy_ticks++;

                            }
                            if(scheduler == FCFS){
                                // running
                                // check if there is something to run
                                if(ready_queue.size > 0){
                                    // check if there is cpu time left
                                    if(ready_queue.head->process-> cpu_usage > 0){
                                        fprintf(output_file,"process %d: running \t",ready_queue.head->process->id);
                                        fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                             // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                                        ready_queue.head->process-> cpu_usage--;
                                    }
                                    if(ready_queue.head->process-> cpu_usage <= 0 && ready_queue.head->process-> io_delay > 0){
                                        push(&blocked_list,ready_queue.head);
                                        pop(&ready_queue,ready_queue.head );
                                    }
                                    if(ready_queue.head->process-> cpu_usage <= 0 && ready_queue.head->process-> io_delay <= 0){
                                        for(int i = 0 ; i < numOfProcesses ; i++){
                                            if(ready_queue.head->process-> id == process_array[i].process->id){
                                                process_array[i].process->turnaround_time = tick - process_array[i].process->arrival_time + 1;
                                            }
                                        }
                                        leftProcesses--;
                                        pop(&ready_queue, ready_queue.head );
                                    }
                                }
// ----------------------------------------------RR Algorithm------------------------------------------------------------------------------------
                            }else if(scheduler == ROUNDROBIN){
                                if(ready_queue.size > 0){
                                    // check if there is cpu time left
                                    if(ready_queue.head->process->cpu_usage > 0 && current_quantum > 0){
                                        current_quantum--;

                                        fprintf(output_file,"process %d: running \t",ready_queue.head->process-> id);
                                        fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                             // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file

                                        ready_queue.head->process->cpu_usage--;
                                    }

                                    if(current_quantum <= 0 && ready_queue.head->process->cpu_usage > 0){
                                        current_quantum = quantum;
                                        processNode* temp = ready_queue.head;
                                        pop(&ready_queue,ready_queue.head);
                                        push(&ready_queue,temp);
                                    }

                                    if(ready_queue.head->process->cpu_usage <= 0 && ready_queue.head->process->io_delay > 0)
                                    {
                                        current_quantum = quantum;
                                        push(&blocked_list, ready_queue.head);
                                        pop(&ready_queue, ready_queue.head);
                                    }

                                    if(ready_queue.head->process->cpu_usage <= 0 && ready_queue.head->process->io_delay <= 0)
                                    {
                                        current_quantum = quantum;
                                        for(int i = 0 ; i < numOfProcesses ; i++){
                                            if(ready_queue.head->process->id == process_array[i].process->id){
                                                process_array[i].process->turnaround_time = tick - process_array[i].process->arrival_time + 1;
                                            }
                                        }
                                        leftProcesses--;
                                        pop(&ready_queue,ready_queue.head);
                                    }
                                }
                            }
                            if(leftProcesses <= 0){
                                break;
                            }
                            // increment system ticks
                            tick++;
                            fprintf(output_file,"\n");
                            fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                                 // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file


        }


//---------------------- Calculations of the outputs ------------------------------------------------------------
        fprintf(output_file,"\n\n");
        fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                             // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file
        fprintf(output_file,"Finishing time: %d\n",tick);
        cpu_utilization = (float)(tick + 1 - not_busy_ticks)/(tick+1);
        fprintf(output_file,"CPU utilization : %.2f\n",cpu_utilization);
        for(int i =0 ; i < numOfProcesses; i++)
        {
            fprintf(output_file,"Turnaround time of Process %d: %d\n",process_array[i].process->id,process_array[i].process->turnaround_time);
            fflush(output_file); //  I just had to call fflush() after the call to fprintf() because my version of the code block program refused to write any thing ..otherwise the output was an empty text file
                                // i found here the same programming issue and its answer: https://stackoverflow.com/questions/33763921/c-program-not-writing-to-text-file
        }

        fclose(input_file);
        fclose(output_file);
        return 0;

}
