#include "local.h"
int getRandomNumber(int min, int max) {
    if (min > max) {
        //printf("Error: min should be less than or equal to max.\n");
        return -1; // Return an error code
    }

    // Seed the random number generator with the current time
    srand(time(NULL));

    // Calculate the range and generate a random number within that range
    int range = max - min + 1;
    int randomNumber = rand() % range + min;

    return randomNumber;
}

void readFromFile(const char *filename, int *array, int *numLines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *numLines = 0;
    while (fscanf(file, "%*[^0-9]%d", &array[*numLines]) == 1) {
        (*numLines)++;
        if (*numLines >= MAX_LINES) {
            printf("Too many lines in the file. Increase MAX_LINES if needed.\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

void readItemsAndStoreInItemsSharedMemory(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Create or attach to the shared memory segment
    key_t key = ftok(".", KEY_ITEM);
    int shmid = shmget(key, sizeof(struct ItemSharedmemory), IPC_CREAT | 0666);
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

    // Read items from the file and store them in shared memory
    int i = 0;
    while (fscanf(file, "%29[^,],%d,%d,%lf\n", sharedMemory->Name[i], &sharedMemory->ID[i], &sharedMemory->quantity[i], &sharedMemory->price[i]) == 4) {
        i++;
        if (i >= MaxItems) {
            printf("Warning: Maximum number of items reached .\n");
            break;
        }
    }

    // Detach from shared memory
    shmdt(sharedMemory);

    // Close the file
    fclose(file);
}
void readAndPrintItemSharedMemory() {
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

    // Print the contents of shared memory
    for (int i = 0; i < MaxItems; i++) { // Assuming you have up to 200 items
        if (sharedMemory->ID[i] == 0) {
            break; // Stop if the ID is 0 (assuming 0 indicates an empty item)
        }
        printf("Item %d:\n", i + 1);
        printf("Name: %s\n", sharedMemory->Name[i]);
        printf("ID: %d\n", sharedMemory->ID[i]);
        printf("Quantity: %d\n", sharedMemory->quantity[i]);
        printf("Price: %.2lf\n", sharedMemory->price[i]);
        printf("----------------\n");
    }

    // Detach from shared memory
    shmdt(sharedMemory);
}
void initializeCheckSharedMemory(int CahsiersNumber) {
    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_CHECK);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct CheckSharedmemory), IPC_CREAT | 0666);
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

    // Initialize values to zero
    sharedMemory->leftCahsiers = 0;
    sharedMemory->leftCustomers = 0;
    sharedMemory->totalCashiers = CahsiersNumber;
    sharedMemory->totalCustomers = 0;
    // Detach from shared memory
    shmdt(sharedMemory);
}
int createSemaphore(int initialValue, int SEM_KEY) {
    int semid = semget(ftok(".", SEM_KEY), 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        
        printf("DelteCrr\n");
        exit(EXIT_FAILURE);
    }

    union semun arg;
    arg.val = initialValue;

    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return semid;
}
void lockSemaphore(int semid) {
    struct sembuf sops = {0, -1, 0};
    if (semop(semid, &sops, 1) == -1) {
        perror("semop (lock)");
        exit(EXIT_FAILURE);
    }
}

void unlockSemaphore(int semid) {
    struct sembuf sops = {0, 1, 0};
    if (semop(semid, &sops, 1) == -1) {
        perror("semop (unlock)");
        exit(EXIT_FAILURE);
    }
}
void printCheckSharedMemory() {
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

    lockSemaphore(semid); // Lock the semaphore before accessing shared memory

    printf("leftCahsiers: %d\n", sharedMemory->leftCahsiers);
    printf("leftCustomers: %d\n", sharedMemory->leftCustomers);

    unlockSemaphore(semid); // Unlock the semaphore
    deleteSemaphore(skey);
    // Detach from shared memory
    shmdt(sharedMemory);
}

void DeleteSharedMemories(){
    int array[3];
    array[0]=KEY_ITEM;
    array[1]=KEY_CASHIER;
    array[2]=KEY_CHECK;
    for (int i=0 ; i<3 ; i++){
	    key_t shm_key = ftok(".", array[i]);  // Use the same key used to create the shared memory
	    int shm_id = shmget(shm_key, 0, 0);

	    if (shm_id == -1) {
		perror("shmget");
		exit(1);
	    }

	    // Remove the shared memory segment
	    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(1);
	    }

	    //printf("Shared memory segment removed.\n");
    }
}
void deleteSemaphore(int skey) {
    // Get the semaphore ID
    int semid = semget(ftok(".", skey), 1, 0);
    if (semid == -1) {
        perror("semget");
        printf("DelteEr\n");
        exit(EXIT_FAILURE);
    }

    // Delete the semaphore
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl (IPC_RMID)");
        exit(EXIT_FAILURE);
    }

    //printf("Semaphore deleted successfully.\n");
}
// Function to handle termination of child processes
void terminateChildProcesses() {
    // Send a termination signal (e.g., SIGTERM) to all child processes
    // You can also use SIGKILL (kill -9) for forceful termination
    // Here, we use SIGTERM as a graceful termination signal
    kill(0, SIGTERM);
}
void checkCustomersAndCashiers(int MaxLeaveCashiers, int MaxCustomersLeave, volatile sig_atomic_t *childSignaled) {
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
    int leftCustomers = sharedMemory->leftCustomers;
    int leftCashiers = sharedMemory->leftCahsiers;

    //printf("Left Customers: %d, Left Cashiers: %d\n", leftCustomers, leftCashiers);

	// Check for exit conditions (e.g., when no customers and cashiers are left)
    if (leftCustomers == 0 && leftCashiers == 0) {
        //printf("No customers or cashiers left. Exiting...\n");
    }
    if (leftCustomers >= MaxCustomersLeave || leftCashiers >= MaxLeaveCashiers){
    	*childSignaled=1;
    	
    	printf("Left Customers: %d, Left Cashiers: %d\n", leftCustomers, leftCashiers);

    }

    // Detach from shared memory
    shmdt(sharedMemory);
}
void createCashiersSharedMemory() {
    // Create the shared memory segment
    int shmid = shmget(ftok(".", KEY_CASHIER), sizeof(struct CashiersSharedmemory), IPC_CREAT | 0666);
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

    // Initialize CashiersSharedmemory values
    for (int i = 0; i < MAXCASHIERS; i++) {
        sharedMemory->ID[i] = 0;
        sharedMemory->CashierBehavior[i] = 0;
        sharedMemory->Sales[i] = 0.0;
        sharedMemory->numberofItems[i] = 0;
        for (int j = 0; j < MAX_CUSTOMERS; j++) {
            sharedMemory->queue[i][j] = 0;
            sharedMemory->cart[i][j] = 0;
            sharedMemory->prices[i][j] = 0.0;
        }
    }

    // Detach from shared memory
    shmdt(sharedMemory);

    }
void readAndPrintCashierSharedMemory() {
    // Replace SHM_KEY with the actual key for your cashier shared memory segment
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

    // Print the contents of the cashier shared memory
    printf("Cashier Shared Memory Contents:\n");
    for (int i = 0; i < MAXCASHIERS; i++) {
        printf("Cashier #%d:\n", i + 1);
        printf("ID: %d\n", sharedMemory->ID[i]);
        printf("Cashier Behavior: %d\n", sharedMemory->CashierBehavior[i]);
        printf("Sales: %.2lf\n", sharedMemory->Sales[i]);
        printf("Number of Items: %d\n", sharedMemory->numberofItems[i]);

        // You can print other cashier-related data as needed

        printf("\n");
    }

    // Detach from shared memory
    shmdt(sharedMemory);
}


void GetNumbers(int *CahsiersNumber, int *CustomersNumber, int *leftCahsiersNumber,int *leftCustomersNumber) {
    // Get the key for the shared memory segment
    key_t key = ftok(".", KEY_CHECK);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment ID
    int shmid = shmget(key, sizeof(struct CheckSharedmemory), IPC_CREAT | 0666);
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

    // Initialize values to zero
    *CahsiersNumber = sharedMemory->totalCashiers;
    *CustomersNumber = sharedMemory->totalCustomers;
    *leftCahsiersNumber = sharedMemory->leftCahsiers;
    *leftCustomersNumber = sharedMemory->leftCustomers;
    // Detach from shared memory
    shmdt(sharedMemory);
}
void generateOpenGl(){
    pid_t childPid = fork(); // Create a new child process
    char *args[] = {"./myprogram.elf", NULL};
    if (childPid == -1) {
        perror("fork"); // Failed to create child process
        exit(EXIT_FAILURE);
    }

    if (childPid == 0) {
        // This code runs in the child process
        execvp(args[0],args); // Replace the child process with a new program
        perror("execvp"); // Execvp failed only if it returns, so print an error
        exit(EXIT_FAILURE); // Terminate the child process on failure
    } 
}
void readCashierSharedMemory(int behavior[],double income[],int customers[],int ids[]) {
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

    // Loop through each cashier
    for (int cashierID = 0; cashierID < MAXCASHIERS; cashierID++) {
        // Check if the cashier exists (has a valid ID)
        if (sharedMemory->ID[cashierID] != 0) {
            int numOfCustomersInQueue = 0;
            double totalSales = 0.0;
            int cashierBehavior = sharedMemory->CashierBehavior[cashierID];
            double cashierIncome = sharedMemory->Sales[cashierID];

            // Loop through each customer in the cashier's queue
            for (int customerIndex = 0; customerIndex < MAX_CUSTOMERS; customerIndex++) {
                pid_t customerID = sharedMemory->queue[cashierID][customerIndex];
                double customerPrice = sharedMemory->prices[cashierID][customerIndex];

                // Check if the customer exists (has a valid ID)
                if (customerID != 0) {
                    numOfCustomersInQueue++;
                    totalSales += customerPrice;
                }
            }

            behavior[cashierID] = cashierBehavior;
            income[cashierID] = cashierIncome;
            customers[cashierID] = numOfCustomersInQueue;
            ids[cashierID] = sharedMemory->ID[cashierID];
            
        }
    }

    // Detach from shared memory
    shmdt(sharedMemory);
}






