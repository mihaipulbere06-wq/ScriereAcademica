#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MILIARD 1000000000
#define MILION 1000000
#define SUTAMII 100000

typedef struct Vector {
    int *v,n,start;
} Vector;

int comp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void gnomesort(int *v ,int n){
    for(int i=0;i<n;i++)
        if(!(i==0 || v[i] >= v[i-1]))
        {
            int tmp = v[i];
            v[i] = v[i-1];
            v[i-1] = tmp;
            i-=2;
        }
}

void* Tgnomesort(void* arg){
    Vector* v = (Vector*)arg;
    for(int i=v->start;i<v->n;i+=6)
        if(!(i==v->start || v->v[i] >= v->v[i-6]))
        {
            int tmp = v->v[i];
            v->v[i] = v->v[i-6];
            v->v[i-6] = tmp;
            i-=12;
        }
    return NULL;
}

void bubblesort(int *v, int n){
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(v[i]>v[j]){
                int tmp = v[i];
                v[i] = v[j];
                v[j] = tmp;
            }
}

void countingsort(int *v, int n){
    unsigned int maxi =v[0],k=0;
    for(int i=0;i<n;i++){
        if(v[i]>maxi)
            maxi = v[i];
    }
    maxi++;
    int *a = calloc(maxi,sizeof(int));
    for(int i=0;i<n;i++){
        a[v[i]]++;
    }
    for(int i=0;i<maxi;i++)
    {
        for(int j=0;j<a[i];j++)
        {
            v[k] = i;
            k++;
        }
    }
    free(a);
}

void multicoresort(int *v, int n){
    pthread_t t[6];
    Vector A[6];

    for(int i = 0; i < 6; i++){
        A[i].v = v;
        A[i].n = n;
        A[i].start = i;
        pthread_create(&t[i], NULL, Tgnomesort, &A[i]);
    }

    for(int i = 0; i < 6; i++)
        pthread_join(t[i], NULL);

    for(int i = 0; i < n; i++){
        int tmp = v[i];
        int j = i - 1;
        while(j >= 0 && v[j] > tmp){
            v[j+1] = v[j];
            j--;
        }
        v[j+1] = tmp;
    }
}

void merge(int *v, int st, int mij, int dr){
    int n1 = mij - st + 1;
    int n2 = dr - mij;

    int* S = malloc(n1 * sizeof(int));
    int* D = malloc(n2 * sizeof(int));

    for(int i = 0; i < n1; i++) S[i] = v[st + i];
    for(int i = 0; i < n2; i++) D[i] = v[mij + 1 + i];

    int i = 0, j = 0, k = st;
    while(i < n1 && j < n2){
        if(S[i] <= D[j])
            v[k++] = S[i++];
        else
            v[k++] = D[j++];
    }

    while(i < n1) v[k++] = S[i++];
    while(j < n2) v[k++] = D[j++];

    free(S);
    free(D);
}

void mergesort(int *v, int st, int dr){
    if(st < dr){
        int mij = st + (dr - st) / 2;
        mergesort(v, st, mij);
        mergesort(v, mij + 1, dr);
        merge(v, st, mij, dr);
    }
}

void insertionsort(int *v, int n){
    for(int i = 0; i < n; i++){
        int tmp = v[i];
        int j = i - 1;
        while(j >= 0 && v[j] > tmp){
            v[j+1] = v[j];
            j--;
        }
        v[j+1] = tmp;
    }
}

int main()
{
    int serial_port = open("/dev/ttyACM0", O_RDWR);

    if (serial_port < 0) {
        printf("Error");
        return 1;
    }

    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0) {
        return 1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB; 
    tty.c_cflag &= ~CSTOPB; 
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; 

    tcsetattr(serial_port, TCSANOW, &tty);

    unsigned char send[] = { 's' };
    unsigned char end[] = { 'f' };
    unsigned char clear[] = { 'c' };
    


    int n=SUTAMII;
    clock_t x;


    x = clock();
    int *v = malloc(n*sizeof(int));
    x = clock() - x;
    printf("allocation took %f seconds / %d clocks\n",(double)x/CLOCKS_PER_SEC,x);


    srand(time(NULL));
    x = clock();
    for(int i=0;i<n;i++)
        //v[i] = rand();
        //v[i] = i;
        //if(i%10 == 0) v[i] = rand(); else v[i] = i;
    x = clock() - x;
    printf("generation took %f seconds / %d clocks\n",(double)x/CLOCKS_PER_SEC,x);


    x = clock();
    write(serial_port, send, sizeof(send));
    //qsort(v, n, sizeof(int), comp);
    //mergesort(v,0,n-1);
    //countingsort(v,n);s
    x = clock() - x;
    write(serial_port, end, sizeof(end));
    printf("sorting took %f seconds / %d clocks\n",x,(double)x/CLOCKS_PER_SEC,x);


    for(int i=0;i<n-1;i++)
        if(v[i]>v[i+1])
            printf("Eroare at %d\n",i);
    printf("Check complete\n");

    free(v);

    close(serial_port);
    return 0;
}