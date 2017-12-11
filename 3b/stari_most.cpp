/* 
 * File:   stari_most.cpp
 * Author: soul
 *
 * Created on 2017. prosinca 09, 17:43
 */

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

typedef unsigned long long ull;

int AMOUNT = 100;
ull SINGULARITY = 0x941;    //Proizvoljno ;)
int passed = 0;

pthread_t* ID = (pthread_t*)malloc(AMOUNT * sizeof(pthread_t));     //Preprocesor ne radi s poljima :(

pthread_mutex_t m;
pthread_cond_t side[2];

bool currSide = false;
ull waitingCount[2];
ull bridgeCount = 0;

using namespace std;

struct carProperties
{
    ull ID;
    bool isLeft;
};

void* car(void* carProps)
{
    carProperties props = *((carProperties*)carProps);
    
    ++waitingCount[props.isLeft];
    printf("Auto %llu ceka na prelazak preko mosta.\n", props.ID);
    
    pthread_mutex_lock(&m);
    
    while((props.isLeft != currSide && bridgeCount) || bridgeCount > 2)
        pthread_cond_wait(side + props.isLeft, &m);
    
    printf("\nAuto %llu je ušao na most...\n", props.ID);
    currSide = props.isLeft;
    --waitingCount[props.isLeft];
    ++bridgeCount;
    
    pthread_mutex_unlock(&m);
    
    
    
    for(int i = 0, time = ((rand() % 20000) / 10000) + 1; i < time; ++i)
        sleep(1);
    
    
    
    pthread_mutex_lock(&m);
    
    ++passed;
    printf("Auto %llu je prešao most (%d).\n\n", props.ID, passed);
    
    if(!(--bridgeCount) && waitingCount[props.isLeft] < 1)
    {
        currSide = !currSide;
        pthread_cond_broadcast(side + !props.isLeft);
    }
    else
        pthread_cond_signal(side + props.isLeft);
    
    pthread_mutex_unlock(&m);
    
    pthread_exit(0);
}

bool instantiateCars()
{
    bool toRet = false;
    
    for(int i = 0; i < AMOUNT; ++i)
    {
        carProperties* t = (carProperties*)malloc(sizeof(*t));
        
        t->ID = (ull)i;
        t->isLeft = (t->ID ^ SINGULARITY) % 2;
        
        toRet += pthread_create(ID + i, NULL, car, (void*)t);
    }
    
    return toRet;
}

int main (int argc, char** argv)
{
    srand(time(NULL));
    
    instantiateCars();
    
    for(int i = 0; i < AMOUNT; ++i)
        pthread_join(ID[i], NULL);
    
    return 0;
}