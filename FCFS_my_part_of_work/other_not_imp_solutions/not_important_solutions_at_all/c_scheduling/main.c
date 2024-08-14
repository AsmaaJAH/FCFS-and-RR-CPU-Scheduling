#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<stdbool.h>




/// ---------------------------------  data types ----------------------------
#define MAX_NUM_OF_PROCESSES 100
int ROUNDROBIN = 1;
int FCFS = 0;
int DEBUG =0;

struct process{
    unsigned int id;
    unsigned int cpu_usage;
    unsigned int io_delay;
    unsigned int arrival_time;
    unsigned int turnaround_time;
    unsigned int front, rear, size;


    //bool operand = id < processObj.id;
};
typedef struct process Process;
Process process_array[MAX_NUM_OF_PROCESSES]; //array of STRUCTs


//------------------------- Queue operations --------------------------------
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct process* createQueue(unsigned capacity)
{
    struct process* queue = (struct process*)malloc(sizeof(struct process));
    queue->front=queue->size = 0;

    // This is important, see the enqueue
    queue->rear = MAX_NUM_OF_PROCESSES - 1;
    return queue;
}



// Function to add an item to the queue.
// It changes rear and size
void push_back(struct process* queue, Process item)
{

    queue->rear = (queue->rear + 1);
    process_array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue.
// It changes front and size
Process dequeue(struct process* queue)
{

    Process item = process_array[queue->front];
    queue->front = (queue->front + 1);
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
Process front(struct process queue)
{

    return process_array[queue.front];
}


// Function to get rear of queue
Process rear(struct process queue)
{

    return process_array[queue.rear];
}
// push back a node to queue
void push(processQueue* queue, Process* process)
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

int main()
{
    //  not correct linked list :
    Process ready_queue;
    Process blocked_list;
    Process ready_list;
    /// ----------------------------- process array init from file ----------------------
    int numOfProcesses = 0;
    unsigned int tick = 0;
    float cpu_utilization = 0;
    int not_busy_ticks = 0;
    bool scheduler = 0;
    int quantum = 0,current_quantum = 0;
    FILE *input_file;
    FILE *output_file;
    input_file = fopen("InputFile.txt", "r");
    output_file = fopen("output_file.txt","w");
    if(input_file == NULL){
        printf("fuckin' failed,dude\n");
        return -1;
    }
    int i = 0;
    char singleline[50];
    while (!feof(input_file))
    {
        fgets(singleline, 50, input_file);
        sscanf(singleline,"%d %d %d %d",&process_array[i].id,&process_array[i].cpu_usage,&process_array[i].io_delay,&process_array[i].arrival_time);
        process_array[i].turnaround_time = 0;
        numOfProcesses++;
        i++;
    }
    int leftProcesses = numOfProcesses;
    fclose(input_file);
    #if DEBUG == 1
        printf("lines: %d",numOfProcesses);
        for(int i= 0 ; i < numOfProcesses ; i++){
            printf("id: %d, cpu_usage: %d, io: %d, arrival: %d\n",process_array[i].id,
                                                                process_array[i].cpu_usage,
                                                                process_array[i].io_delay,
                                                                process_array[i].arrival_time);
        }
    #endif
    printf("Choose Scheduler: \n0: FCFS\n1:Round-Robin\n");
    scanf("%d",&scheduler);
    system("cls");

    if(scheduler == ROUNDROBIN){
        printf("Enter length of quantum: \n");
        scanf("%d",&quantum);
        current_quantum = quantum;
        system("cls");
    }
    while(1){
        fprintf(output_file,"%d: ",tick);
        /// traverse process_array to check if process as arrived and add them to queue
        for(i = 0 ; i < numOfProcesses ; i++){
            /// check for arrival
            if(process_array[i].arrival_time == tick){
                /// set the state
                /// push in ready queue
                fprintf(output_file,"process %d: ready\t",process_array[i].id);
                push_back(&ready_list,process_array[i]);
            }
        }
        if(blocked_list.size> 0){
                //it==i==iteration
            for(Process  it = front(blocked_list); it !=rear(blocked_list);++it){
                /// decrement io_block
                /// check if io request handled
                if(it->io_delay <= 0){
                    for(int i = 0 ; i < numOfProcesses ; i++){
                        if(process_array[i].id == it->id){
                            it->cpu_usage = process_array[i].cpu_usage;
                        }
                    }
                    ///it->cpu_usage++;
                    ready_list.push_back(*it);
                    it = blocked_list.erase(it);
                    --it;
                }else{
                    fprintf(output_file,"process %d: blocked\t",it->id);
                    it->io_delay--;
                }
            }
        }
        ready_list.sort();
        for(auto it = ready_list.begin(); it != ready_list.end(); ++it){
            ready_queue.push_back(*it);
            it = ready_list.erase(it);
            it--;
        }
        if(ready_queue.size() <= 0 && leftProcesses > 0){
            not_busy_ticks++;

        }
        if(scheduler == FCFS){
            /// running
            /// check if there is something to run
            if(ready_queue.size() > 0){
                /// check if there is cpu time left
                if(ready_queue.front().cpu_usage > 0){
                    fprintf(output_file,"process %d: running \t",ready_queue.front().id);
                    ready_queue.front().cpu_usage--;
                }
                if(ready_queue.front().cpu_usage <= 0 && ready_queue.front().io_delay > 0){
                    blocked_list.push_back(ready_queue.front());
                    ready_queue.pop_front();
                }
                if(ready_queue.front().cpu_usage <= 0 && ready_queue.front().io_delay <= 0){
                    for(int i = 0 ; i < numOfProcesses ; i++){
                        if(ready_queue.front().id == process_array[i].id){
                            process_array[i].turnaround_time = tick - process_array[i].arrival_time + 1;
                        }
                    }
                    leftProcesses--;
                    ready_queue.pop_front();
                }
            }
        }else if(scheduler == ROUNDROBIN){
            if(ready_queue.size() > 0){
                /// check if there is cpu time left
                if(ready_queue.front().cpu_usage > 0 && current_quantum > 0){
                    current_quantum--;
                    fprintf(output_file,"process %d: running \t",ready_queue.front().id);
                    ready_queue.front().cpu_usage--;
                }
                if(current_quantum <= 0 && ready_queue.front().cpu_usage > 0){
                    current_quantum = quantum;
                    Process temp = ready_queue.front();
                    ready_queue.pop_front();
                    ready_queue.push_back(temp);
                }
                if(ready_queue.front().cpu_usage <= 0 && ready_queue.front().io_delay > 0){
                    current_quantum = quantum;
                    blocked_list.push_back(ready_queue.front());
                    ready_queue.pop_front();
                }
                if(ready_queue.front().cpu_usage <= 0 && ready_queue.front().io_delay <= 0){
                    current_quantum = quantum;
                    for(int i = 0 ; i < numOfProcesses ; i++){
                        if(ready_queue.front().id == process_array[i].id){
                            process_array[i].turnaround_time = tick - process_array[i].arrival_time + 1;
                        }
                    }
                    leftProcesses--;
                    ready_queue.pop_front();
                }
            }
        }
        if(leftProcesses <= 0){
            break;
        }
        /// increment system ticks
        tick++;
        fprintf(output_file,"\n");
    }
    /// Calculations
    fprintf(output_file,"\n\n");
    /**
        calculations
    **/
    fprintf(output_file,"Finishing time: %d\n",tick);
    cpu_utilization = (float)(tick + 1 - not_busy_ticks)/(tick+1);
    fprintf(output_file,"CPU utilization : %.2f\n",cpu_utilization);
    for(int i =0 ; i < numOfProcesses; i++){
        fprintf(output_file,"Turnaround time of Process %d: %d\n",process_array[i].id,process_array[i].turnaround_time);
    }
    fclose(output_file);
    return 0;
}
