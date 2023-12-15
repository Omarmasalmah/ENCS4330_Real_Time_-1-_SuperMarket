#ifndef __LOCAL_H_
#define __LOCAL_H_

/*
 * Common header file: parent, producer and consumer
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>
#include <signal.h>
#include <time.h>
#include <GL/glut.h>

#define MAX_LINES 100
#define KEY_ITEM 1111
#define KEY_CASHIER 2222
#define KEY_CHECK 3333
#define MaxItems 300
#define MAXCASHIERS 20
#define MAX_CUSTOMERS 100
void readAndPrintCashierSharedMemory();
void checkCustomersAndCashiers(int MaxLeaveCashiers, int MaxCustomersLeave, volatile sig_atomic_t *childSignaled);
void saveCashierData(double MaxIncome, int Cashiers_Behavior, int Number);
void generateCustomerProcess(int ShoppingTimeMin, int ShoppingTimeMax,int MaxWaitTime);
void generateCashierProcess(int MaxIncome, int ScanTimeMax, int ScanTimeMin, int Cashiers_BehaviorMin,int Cashiers_BehaviorMax, int number);
void scanItems(int cashierID, int scanTimeMin, int scanTimeMax, double MaxSales);
void shift_queue(int cashier_id);
void terminateChildProcesses();
int getRandomNumber(int min, int max);
void generateCustomerArrivalProcess(int Customers_Arrival_Max, int Customers_Arrival_Min, int ShoppingTimeMin, int ShoppingTimeMax,int MaxWaitTime);
void readFromFile(const char *filename, int *array, int *numLines);
void readItemsAndStoreInItemsSharedMemory(const char *filename);
void initializeCheckSharedMemory(int CahsiersNumber);
void readAndPrintItemSharedMemory();
int createSemaphore(int initialValue, int SEM_KEY);
void lockSemaphore(int semid);
void unlockSemaphore(int semid);
void printCheckSharedMemory();
void increaseLeftCustomers();
void increaseLeftCahsiers();
void DeleteSharedMemories();
void getRandomItemsAndCalculateCart(double *cartPrice, int *cart);
void applyCustomerData(int cart, double cartPrice, int ID);
void deleteSemaphore(int skey);
void createCashiersSharedMemory();
int readDataFromCashiersSharedMemory();
int readDataFromCheckSharedMemory();
void readDataFromCashiersSharedMemorySales(double * Sales);
void OptimizedgetRandomItemsAndCalculateCart(double *cartPrice, int *cart);
void increaseCustomers();
void decreaseCustomers();
void GetNumbers(int *CahsiersNumber, int *CustomersNumber, int *leftCahsiersNumber,int *leftCustomersNumber);
void generateOpenGl();
void readCashierSharedMemory(int behavior[],double income[],int customers[],int ids[]);
struct ItemSharedmemory {
    char Name[MaxItems][30];   // Assuming 10 items, adjust as needed
    int ID[MaxItems];
    int quantity[MaxItems];
    double price[MaxItems];
};

struct CashiersSharedmemory {
    pid_t ID[MAXCASHIERS];  // Assuming 5 cashiers, adjust as needed
    int CashierBehavior[MAXCASHIERS];
    double Sales[MAXCASHIERS];
    pid_t queue[MAXCASHIERS][MAX_CUSTOMERS];
    int cart[MAXCASHIERS][MAX_CUSTOMERS];
    double prices[MAXCASHIERS][MAX_CUSTOMERS]; 
    int numberofItems[MAXCASHIERS];
};

struct CheckSharedmemory {
    int leftCahsiers;
    int leftCustomers;
    int totalCashiers;
    int totalCustomers;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#endif



