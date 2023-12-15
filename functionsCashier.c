#include "local.h"
void generateCashierProcess(int MaxIncome, int ScanTimeMax, int ScanTimeMin, int Cashiers_BehaviorMin,int Cashiers_BehaviorMax, int number)
{
    sleep(1);
    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process (cashier)
        char MaxIncomeStr[10];
        char ScanTimeMaxStr[3];
        char ScanTimeMinStr[3];
        char NumberString[5];
        char Cashiers_BehaviorStr[10];

        int Cashiers_Behavior=getRandomNumber(Cashiers_BehaviorMin,Cashiers_BehaviorMax);

        // Convert integer and double to string
        snprintf(MaxIncomeStr, sizeof(MaxIncomeStr), "%d", MaxIncome);
        snprintf(ScanTimeMaxStr, sizeof(ScanTimeMaxStr), "%d", ScanTimeMax);
        snprintf(ScanTimeMinStr, sizeof(ScanTimeMinStr), "%d", ScanTimeMin);
        snprintf(Cashiers_BehaviorStr, sizeof(Cashiers_BehaviorStr), "%d", Cashiers_Behavior);
        snprintf(NumberString, sizeof(number), "%d", number);

        // Exec cashier.c with arguments
        char *args[] = {"./cashier", MaxIncomeStr, Cashiers_BehaviorStr, ScanTimeMinStr, ScanTimeMaxStr, NumberString, NULL};

        execvp(args[0], args);
        
        // If execvp fails (e.g., file not found), print an error
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
    
}

void increaseLeftCahsiers() {
    int skey=getpid();
    int semid = createSemaphore(1,skey); // Initialize semaphore with a value of 1 to ensure exclusive access

    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_CHECK);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct CheckSharedmemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct CheckSharedmemory *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (struct CheckSharedmemory *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    lockSemaphore(semid); // Lock the semaphore before modifying shared memory
    sharedMemory->leftCahsiers++;
    sharedMemory->totalCashiers--;
    unlockSemaphore(semid); // Unlock the semaphore
    deleteSemaphore(skey);

    // Detach from shared memory
    shmdt(sharedMemory);
}




void scanItems(int cashierID, int scanTimeMin, int scanTimeMax, double MaxSales) {
    int skey = getpid();

    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_CASHIER);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct CashiersSharedmemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct CashiersSharedmemory *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (struct CashiersSharedmemory *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    int semid = createSemaphore(1, skey);

    
    // Simulate scanning items
    int cart = sharedMemory->cart[cashierID][0];
    double cartPrice = sharedMemory->prices[cashierID][0];
    //printf("\n%d\n%lf\n%lf\n",getpid(),sharedMemory->Sales[cashierID],cartPrice);




    if (cart != 0 && sharedMemory->queue[cashierID][0]!=0) {
        lockSemaphore(semid); // Lock the semaphore before accessing the memory
    	//printf("\n%d\n%lf\n%lf\n",getpid(),sharedMemory->Sales[cashierID],cartPrice);

    	//printf("\n%d\n%lf\n%lf\n",getpid(),sharedMemory->Sales[cashierID],cartPrice);
    	sharedMemory->numberofItems[cashierID] = sharedMemory->numberofItems[cashierID] - cart;
    	unlockSemaphore(semid); // Unlock the semaphore after accessing the memory
    	deleteSemaphore(skey);
        for (int i = 0; i < cart; i++) {
            // Simulate random scanning time
            int scanTime = getRandomNumber(scanTimeMin, scanTimeMax);
            usleep(scanTime * 1000); // Convert milliseconds to microseconds (10^-3)
        }
        sharedMemory->Sales[cashierID] = sharedMemory->Sales[cashierID]+cartPrice;
    	sharedMemory->CashierBehavior[cashierID] = sharedMemory->CashierBehavior[cashierID]-cart;
        kill(sharedMemory->queue[cashierID][0], SIGUSR1);
        if (MaxSales <= sharedMemory->Sales[cashierID]) {
            printf("\n=======================\nCashier PID: %d reached the max sales\n=======================\nwith %.2lf sales!\n", cashierID+1, sharedMemory->Sales[cashierID]);
            sleep(1);
            kill(getppid(), SIGINT);
            sleep(5);
        } else {
            printf("Cashier %d finished scanning for Customer. Cart total: $%.2lf\n", cashierID + 1, cartPrice);

        }
        
        if (sharedMemory->CashierBehavior[cashierID] <= 0) {


            increaseLeftCahsiers(); // Typo corrected: increaseLeftCahsiers -> increaseLeftCashiers
            printf("Cashier number #%d left!\n", 1 + cashierID);
            sharedMemory->ID[cashierID]=0;
            for (int i = 1; i < MAX_CUSTOMERS - 1; i++) {
                applyCustomerData(sharedMemory->cart[cashierID][i], sharedMemory->prices[cashierID][i],sharedMemory->queue[cashierID][i]);
            }
            
            exit(0);
        } else {
            shift_queue(cashierID);
        }
	//printf("\n%d\n%lf\n%lf\n",getpid(),sharedMemory->Sales[cashierID],cartPrice);
        
    }

    // Detach from shared memory
    shmdt(sharedMemory);
}



void shift_queue(int cashier_id) {
    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_CASHIER);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct CashiersSharedmemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct CashiersSharedmemory *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (struct CashiersSharedmemory *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    int semid = createSemaphore(1, getpid());

    lockSemaphore(semid);

    // Shift the queue by removing the served customer
    for (int i = 0; i < MAX_CUSTOMERS - 1; ++i) {
        sharedMemory->queue[cashier_id][i] = sharedMemory->queue[cashier_id][i + 1];
        sharedMemory->cart[cashier_id][i] = sharedMemory->cart[cashier_id][i + 1];
    }

    sharedMemory->queue[cashier_id][MAX_CUSTOMERS - 1] = 0;
    sharedMemory->cart[cashier_id][MAX_CUSTOMERS - 1] = 0;

    unlockSemaphore(semid);
    deleteSemaphore(getpid()); // Use semid for deletion, not skey
    shmdt(sharedMemory);
}




void saveCashierData(double MaxIncome, int Cashiers_Behavior, int Number) {
    // Get the key for the KEY_CASHIER memory segment
    key_t key = ftok(".", KEY_CASHIER);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct CashiersSharedmemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        printf("1");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct CashiersSharedmemory *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (struct CashiersSharedmemory *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }


    

    // Save the cashier data into shared memory
    sharedMemory->ID[Number] = getpid();
    sharedMemory->CashierBehavior[Number] = Cashiers_Behavior;
    sharedMemory->Sales[Number] = MaxIncome;

   
    // Detach from shared memory
    shmdt(sharedMemory);
}


