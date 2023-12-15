#include "local.h"
void generateCustomerProcess(int ShoppingTimeMin, int ShoppingTimeMax,int MaxWaitTime) {
    pid_t pid = fork();



    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process (customer)
        char ShoppingTimeStr[10]; // Adjust the size as needed
        int ShoppingTime=getRandomNumber(ShoppingTimeMin, ShoppingTimeMax);
        char MaxWaitTimeStr[10];   // Adjust the size as needed
        // Convert integers to strings
        snprintf(ShoppingTimeStr, sizeof(ShoppingTimeStr), "%d", ShoppingTime);
        snprintf(MaxWaitTimeStr, sizeof(MaxWaitTimeStr), "%d", MaxWaitTime);

        // Exec customer.c with arguments
        char *args[] = {"./customer", ShoppingTimeStr, MaxWaitTimeStr, NULL};

        execvp(args[0], args);

        // If execvp fails (e.g., file not found), print an error
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
}






void getRandomItemsAndCalculateCart(double *cartPrice, int *cart) {
    // Initialize cart variables
    *cart = 0;
    *cartPrice = 0.0;

    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_ITEM);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct ItemSharedmemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct ItemSharedmemory *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (struct ItemSharedmemory *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    int skey = getpid();
    int semid = createSemaphore(1, skey); // Initialize semaphore with a value of 1 to ensure exclusive access
    int RandomNumberOfItems = getRandomNumber(1, MaxItems);
    //printf("\n%d\n",getpid());
    while (*cart < RandomNumberOfItems) {
        int randomItemIndex = getRandomNumber(0, MaxItems - 1); // Corrected range
        int quantity = sharedMemory->quantity[randomItemIndex];
            //printf("\n%d\n\nbbb\n",getpid());
        if (quantity == 0) {
            continue;
        }

        int randomQuantity = getRandomNumber(1, quantity); // Ensure quantity is within bounds

        lockSemaphore(semid);
        *cartPrice += (randomQuantity * sharedMemory->price[randomItemIndex]);
        *cart += randomQuantity;
        sharedMemory->quantity[randomItemIndex] -= randomQuantity;
        unlockSemaphore(semid); // Unlock the semaphore
    }

    //deleteSemaphore(skey);

    // Detach from shared memory
    shmdt(sharedMemory);
}
void OptimizedgetRandomItemsAndCalculateCart(double *cartPrice, int *cart) {
    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_ITEM);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct ItemSharedmemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct ItemSharedmemory *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (struct ItemSharedmemory *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    int skey = getpid();
    int semid = createSemaphore(1, skey); // Initialize semaphore with a value of 1 to ensure exclusive access
    int RandomNumberOfItems = getRandomNumber(1, 500);

    int cartCount = 0;       // Track the cart items
    double cartTotal = 0.0; // Track the cart total price
    int randomItemIndex = getRandomNumber(1000,5000);
    while (cartCount < RandomNumberOfItems) {
        int Index = randomItemIndex%MaxItems;
        int quantity = sharedMemory->quantity[Index];
        randomItemIndex/=13;
        randomItemIndex*=17;
        if (quantity == 0) {
            continue;
        }

        int randomQuantity = getRandomNumber(1, quantity); // Ensure quantity is within bounds

        lockSemaphore(semid);
        cartTotal += (randomQuantity * sharedMemory->price[Index]);
        cartCount += randomQuantity;
        sharedMemory->quantity[Index] -= randomQuantity;
        unlockSemaphore(semid);
    }

    // Update the cart variables after the loop
    *cartPrice = cartTotal;
    *cart = cartCount;

    // Detach from shared memory
    shmdt(sharedMemory);
}


void applyCustomerData(int cart, double cartPrice, int ID) {
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



    // Find the best cashier based on CashierBehavior - numberofItems
    int bestCashier = -1;
    int bestScore = 0;
    for (int i = 0; i < MAXCASHIERS; i++) {
        if (sharedMemory->ID[i] != 0 && sharedMemory->CashierBehavior[i]>0) { // Check if cashier is available
            int score = sharedMemory->CashierBehavior[i] - sharedMemory->numberofItems[i];
            if (score > bestScore) {
                bestScore = score;
                bestCashier = i;
            }
        }
    }
    int i=0;
    // If a valid cashier is found, add the customer to their queue
    while (sharedMemory->queue[bestCashier][i] != 0) {
        i++;
    }

    int semid = createSemaphore(1,getpid());
    lockSemaphore(semid);
    sharedMemory->queue[bestCashier][i] = ID;
    sharedMemory->cart[bestCashier][i] = cart;
    sharedMemory->prices[bestCashier][i] = cartPrice;
    sharedMemory->numberofItems[bestCashier]+=cart;
    unlockSemaphore(semid);
    deleteSemaphore(getpid());

    //printf("Customer %d joined Cashier %d's queue\n", getpid(), bestCashier+1);

    
    shmdt(sharedMemory);
}


void generateCustomerArrivalProcess(int Customers_Arrival_Max, int Customers_Arrival_Min, int ShoppingTimeMin, int ShoppingTimeMax,int MaxWaitTime) {
    pid_t pid = fork();



    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char ShoppingTimeMaxStr[10]; // Adjust the size as needed
        char ShoppingTimeMinStr[10]; // Adjust the size as needed
        char MaxWaitTimeStr[10];   // Adjust the size as needed
        char Customers_Arrival_MaxStr[10];
        char Customers_Arrival_MinStr[10];
        
        snprintf(ShoppingTimeMinStr, sizeof(ShoppingTimeMinStr), "%d", ShoppingTimeMin);
        snprintf(ShoppingTimeMaxStr, sizeof(ShoppingTimeMaxStr), "%d", ShoppingTimeMax);
        snprintf(MaxWaitTimeStr, sizeof(MaxWaitTimeStr), "%d", MaxWaitTime);
        snprintf(Customers_Arrival_MaxStr, sizeof(Customers_Arrival_MaxStr), "%d", Customers_Arrival_Max);
        snprintf(Customers_Arrival_MinStr, sizeof(Customers_Arrival_MinStr), "%d", Customers_Arrival_Min);
        
        char *args[] = {"./customerArrival", Customers_Arrival_MinStr, Customers_Arrival_MaxStr, ShoppingTimeMinStr, ShoppingTimeMaxStr, MaxWaitTimeStr, NULL};

        execvp(args[0], args);

        // If execvp fails (e.g., file not found), print an error
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
}


// Function to increase leftCustomers by one
void increaseLeftCustomers() {
    int skey=getpid();
    int semid = createSemaphore(1, skey); // Initialize semaphore with a value of 1 to ensure exclusive access

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
    sharedMemory->leftCustomers++;
    sharedMemory->totalCustomers--;
    unlockSemaphore(semid); // Unlock the semaphore
    deleteSemaphore(skey);

    // Detach from shared memory
    shmdt(sharedMemory);
}


void increaseCustomers() {
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
    sharedMemory->totalCustomers++;
    unlockSemaphore(semid); // Unlock the semaphore
    deleteSemaphore(skey);

    // Detach from shared memory
    shmdt(sharedMemory);
}
void decreaseCustomers() {
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
    sharedMemory->totalCustomers--;
    unlockSemaphore(semid); // Unlock the semaphore
    deleteSemaphore(skey);

    // Detach from shared memory
    shmdt(sharedMemory);
}
