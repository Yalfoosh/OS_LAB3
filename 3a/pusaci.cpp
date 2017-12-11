/* 
 * File:   pusaci.cpp
 * Author: soul
 *
 * Created on 2017. prosinca 09, 16:32
 */

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
 
using namespace std;

pthread_t ID[4];

sem_t uzeto;
sem_t kupac[3];

char izlog = 0;

char otrov[3] = {0, 1, 2};

bool cont = true;

bool prep()     //Vraca bool koji kaze je li doslo do zariba. Ako nije, sve nule daju false, a ako nesto padne, zbrojit ce se 1, tj. vratit ce se true.
{
    bool toRet = sem_init(&uzeto, 0, 1);    //Na pocetku su materijali "uzeti".
    
    for(int i = 0; i < 3; ++i)
        toRet += toRet += sem_init(kupac + i, 0, 0);
    
    return toRet;
}

void* merchant(void* arg) 
{
	sleep(1);
    
    while(cont)
    {
        sem_wait(&uzeto);
        
        sleep(1);
        
        izlog = (rand() % 30000) / 10000;
        string broadcast = "\nTrgovac u izlog stavlja ";
        
        switch(izlog)
        {
            case 0:
                broadcast += "papir i sibice.\n";
                break;
            case 1:
                broadcast += "duhan i sibice.\n";
                break;
            case 2:
                broadcast += "duhan i papir.\n";
                break;
        }
        
        cout << broadcast;
        
        for(int i = 0; i < 3; ++i)
            sem_post(kupac + i);
    }
    
    pthread_exit(0);
}

void* smoker(void* ID)
{
    char type = *((char*)ID);
    
    string declaration = "Ja imam ";
    
    switch(type)
    {
        case 0:
            declaration += "duhan.\n";
            break;
        case 1:
            declaration += "papir.\n";
            break;
        case 2:
            declaration += "sibice.\n";
            break;
    }
    
    cout << declaration;
    
    while(cont)
    {
        sem_wait(kupac + type);
        
        if(type == izlog)
        {
            declaration = "\nUzimam ";
            
            switch(type)
            {
                case 0:
                    declaration += "papir i sibice...\n";
                    break;
                case 1:
                    declaration += "duhan i sibice...\n";
                    break;
                case 2:
                    declaration += "duhan i papir...\n";
                    break;
            }
            
            cout << declaration;
            sem_post(&uzeto);
            
            sleep(1);
            
            printf("Motam i pusim...\n");
            
            for(int i = 0, vrijeme = ((rand() % 30000) / 10000) + 2; i < vrijeme; ++i)
                sleep(1);
        }
    }
    
    pthread_exit(0);
}

bool threadBirth()
{
    bool toRet = pthread_create(&ID[0], NULL, merchant, NULL);
    
    for(int i = 0; i < 3; ++i)
        toRet += pthread_create(&ID[i + 1], NULL, smoker, (void*)(otrov + i));
    
    return toRet;
}

void destroy(int sig)
{
    cont = false;
    
    sem_destroy(&uzeto);
    
    for(int i = 0; i < 3; ++i)
        sem_destroy(kupac + i);
}

int main() 
{
    srand(time(NULL));
    sigset(SIGINT, destroy);
	
    if(prep())
        printf("Inicijalizacija semafora nije uspjela.\n"), exit(1);
    
    if(threadBirth())
        printf("Dretve se nisu ispravno porodile.\n"), exit(1);

	for(int i = 0; i < 4; ++i)
		pthread_join(ID[i], NULL);

	return 0;
}