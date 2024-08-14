#define _CRT_SECURE_NO_WARNINGS

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct process
{
    int pid;
    //int priority;
    int arrivalTime;
    int burstTime;
    int initialRunningTime;
    int notRunningSince;
    int remainingBurstTime;
} process;

// process node for the queue
typedef struct processNode
{
    process* process;
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

// context or states of scheduling
typedef struct context
{
    processQueue jobQueue;
    processQueue readyQueue;
    process* currentProcess;
    int elapsedTime;
    int currentProcessStartTime;
} context;

// scheduler to save context states
typedef struct scheduler
{
    process* (*whatToStart)(const context* ctx, void* data);
    int (*whetherToStop)(const context* ctx, void* data);
    void* data;
} scheduler;

// dispathcer for context switching
typedef struct dispatcher
{
    context context;
    const scheduler* scheduler;
} dispatcher;

//Struct of calculating output data
typedef struct stats
{
    double averageCpuUsage;
    double  finishingTime;
    double averageTurnaroundTime;
} stats;

// push back a node to queue
void push(processQueue* queue, process* process)
{
    processNode* node = malloc(sizeof(processNode)); // create new node and input node
    node->process = process;
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
process* pop(processQueue* queue, processNode* node)
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

    process* process = node->process;
    free(node);
    queue->size -= 1;
    return process; // return popped node
}

// find where the node exist
processNode* find(processQueue* queue, process* process)
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

//to create and set new context
context makeContext(size_t jobCount, const process jobs[])
{
    processQueue jobQueue = { NULL, NULL, 0 };
    processQueue readyQueue = { NULL, NULL, 0 };
    for (size_t i = 0; i < jobCount; ++i) // Filling the jobs in the array jobs into the job queue
    {
        process* job = malloc(sizeof(process));
        memcpy(job, &jobs[i], sizeof(process)); // copy the memory of process to array of jobs
        push(&jobQueue, job);
    }
    context context = { jobQueue, readyQueue, NULL, 0 };
    return context;
}

// to set the dispatcher
dispatcher makeDispatcher(context context, const scheduler* scheduler)
{
    const dispatcher dispatcher = { context, scheduler };
    return dispatcher;
}

// to print current time to output FILE*/
void printCurrentTime(const context* ctx, FILE* output_file)
{
    fprintf(output_file, "<time %d \t", ctx->elapsedTime);
}

// to run process and print some data in single time
int step(dispatcher* dispatcher, processQueue* finishedProcesses, FILE* output_file)
{
    context* ctx = &dispatcher->context;
    const scheduler* scheduler = dispatcher->scheduler;

    if (ctx->currentProcess != NULL && ctx->elapsedTime - ctx->currentProcessStartTime >= ctx->currentProcess->remainingBurstTime) //if the current process is finished
    {
        process* process = pop(&ctx->readyQueue, find(&ctx->readyQueue, ctx->currentProcess)); // take the process from ready queue

        process->notRunningSince = ctx->elapsedTime;
        process->remainingBurstTime = process->remainingBurstTime - ctx->elapsedTime + ctx->currentProcessStartTime;
        push(finishedProcesses, process); // push finishedProcesses queue to calculate output data

        printCurrentTime(ctx, output_file);
        fprintf(output_file, "[FINISHED] Process %" PRIu8 "\n", process->pid);
        fprintf(output_file, "-------------------------------------------\n");
        ctx->currentProcess = NULL;
    }

    if (ctx->jobQueue.size == 0 && ctx->readyQueue.size == 0) // if all processes are finished
    {
        printCurrentTime(ctx, output_file);
        fprintf(output_file, "[All FINISHED]\n");
        return 0; // return 0 to finish funtion run's while loop
    }

    if (ctx->currentProcess != NULL && scheduler->whetherToStop(ctx, scheduler->data)) // Stop process if the scheduler system call, save the data in context variable ctx
    {
        ctx->currentProcess->notRunningSince = ctx->elapsedTime;
        ctx->currentProcess->remainingBurstTime -= ctx->elapsedTime - ctx->currentProcessStartTime;
        printCurrentTime(ctx, output_file);
        fprintf(output_file, "[STOP] Process %" PRIu8 "\n", ctx->currentProcess->pid);
        fprintf(output_file, "-------------------------------------------\n");
        ctx->currentProcess = NULL;
    }

    processNode* node = ctx->jobQueue.head; // Push arrived processes into ready queue.
    while (node != NULL)
    {
        if (node->process->arrivalTime <= ctx->elapsedTime) //if new processes arrived the time, pop the processes from job queue to push ready queue
        {
            processNode* next_node = node->next;
            process* process = pop(&ctx->jobQueue, node);
            process->notRunningSince = ctx->elapsedTime;
            push(&ctx->readyQueue, process);
            printCurrentTime(ctx, output_file);
            fprintf(output_file, "[READY] Process %" PRIu8 "\n", process->pid);
            node = next_node;
        }
        else
        {
            node = node->next;
        }
    }

    if (ctx->currentProcess == NULL && ctx->readyQueue.size > 0) // Start process the scheduler wants if no process is running.
    {
        ctx->currentProcess = scheduler->whatToStart(ctx, scheduler->data);
        ctx->currentProcessStartTime = ctx->elapsedTime;

        printCurrentTime(ctx, output_file);
        fprintf(output_file, "[START] Process %" PRIu8 "\n", ctx->currentProcess->pid);
    }

    printCurrentTime(ctx, output_file); // Print current process

    if (ctx->currentProcess == NULL)
    {
        fprintf(output_file, "[Idle]\n");
    }
    else
    {
        fprintf(output_file, "[RUNNING] Process %" PRIu8 "\n", ctx->currentProcess->pid);
    }

    ctx->elapsedTime += 1; //count elapsed time
    return 1;
}

//to run process and return finished process through step function
processQueue run(dispatcher* dispatcher, FILE* output_file)
{
    processQueue finishedProcesses = { NULL, NULL, 0 };
    while (step(dispatcher, &finishedProcesses, output_file)) // if all processes are finished
    {}
    return finishedProcesses; //return finished processes queue. They have data.
}

// to find first process in scheduling FCFS
process* fcfsWhatToStart(const context* ctx, void* data)
{
    return ctx->readyQueue.head->process;
}

// to set stop in scheduling FCFS
int fcfsWhetherToStop(const context* ctx, void* data)
{
    return 0;
}

// to set start point and stop point in scheduling FCFS because FCFS do not interrupt
const scheduler getFcfs()
{
    scheduler scheduler = { fcfsWhatToStart, fcfsWhetherToStop, NULL };
    return scheduler;
}



// Funtion PrintStats to print output data to output FILE
void printStats(const stats* stats, FILE* output_file)
{
    fprintf(output_file, "Finishing time: %lf %% \n", stats->finishingTime);
    fprintf(output_file, "Average CPU utilization : %lf %% \n", stats->averageCpuUsage);
    fprintf(output_file, "Average turnaround time : %lf ms\n", stats->averageTurnaroundTime);
}

// to parse from inputed FILE data to process
int parseInput(FILE* input_file, process** jobs)
{
    *jobs = malloc(sizeof(process) * 10);
    int count = 0;
    char singleline[100];
    while (!feof(input_file)) //while file not finished because the feof fun in c tests the end-of-file indicator
    {
        process* process = &(*jobs)[count++];
        fgets(singleline, 50, input_file);
        sscanf(singleline,"%d %d %d %d" , &process->pid, &process->arrivalTime, &process->burstTime);//CPU time==burst time
        process->initialRunningTime = UINT64_MAX;
        process->notRunningSince = 0;
        process->remainingBurstTime = process->burstTime;
    }
    return count; // return the process array job's size
}

/*Function simulateScheduling to semi-main function for single scheduling */
void simulateScheduling(const char* title, const scheduler* scheduler, int jobCount, const process jobs[], FILE* output_file)
{
    fprintf(output_file, "===========================================\n");
    fprintf(output_file, "Scheduling: %s\n", title);
    fprintf(output_file, "-------------------------------------------\n");

    dispatcher dispatcher = makeDispatcher(makeContext(jobCount, jobs), scheduler); // make dispatcher variable to context switching, save scheduler data and size of jobs

    processQueue finishedProcesses = run(&dispatcher, output_file); //run all processess and take finished processes queue and they have datas.

    stats stats = { 0, 0,0, 0 };
    double totalExecutionTime = 0;

    for (processNode* node = finishedProcesses.head; node != NULL;node = node->next) // until all processes in the finishedProcesses queue are finished
    {
        process* process = node->process;
        if (totalExecutionTime < process->notRunningSince) // update totalExecution time according to the progress
        {
            totalExecutionTime = process->notRunningSince;
        }

        double turnaroundTime = process->notRunningSince + process->burstTime;

        stats.averageCpuUsage += process->burstTime;
        stats.averageTurnaroundTime += turnaroundTime;
        free(node->process);
    }
    clear(&finishedProcesses); //clear finishedProcesses queue for recycle

    stats.averageCpuUsage = stats.averageCpuUsage * 100 / totalExecutionTime;
    stats.finishingTime = totalExecutionTime;
    stats.averageTurnaroundTime /= jobCount;

    fprintf(output_file, "-------------------------------------------\n");
    printStats(&stats, output_file); //printf all output data to output FILE
    fprintf(output_file, "===========================================\n\n");
}


int main(int argc, char* argv[])
{

        FILE *input_file;
        FILE *output_file;

        input_file = fopen("InputFile.txt", "r");
        output_file = fopen("output_file.txt", "w");

        scheduler fcfs = getFcfs();

        process* jobs;
        int jobCount = parseInput(input_file, &jobs); // call parseInput function to take process size

        simulateScheduling("FCFS", &fcfs, jobCount, jobs, output_file);


        free(jobs);

        fclose(input_file);
        fclose(output_file);

}


