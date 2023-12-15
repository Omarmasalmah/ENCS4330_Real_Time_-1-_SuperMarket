#include "local.h"

// Flag to indicate if a child process has sent a SIGINT signal
volatile sig_atomic_t childSignaled = 0;

// Signal handler function to handle signals from child processes
void ParentSigintHandler(int signum) {
    childSignaled = 1;
    //terminateChildProcesses();
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <UserDefinedNumbersFile> <ItmesFile>\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (signal(SIGINT, ParentSigintHandler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    const char *UserDefinedNumbers = argv[1];
    const char *ItemsFile = argv[2];
    
    
    int numberArray[MAX_LINES];
    int numLines;

    readFromFile(UserDefinedNumbers, numberArray, &numLines);
    readItemsAndStoreInItemsSharedMemory(ItemsFile);
    //readAndPrintItemSharedMemory();
    createCashiersSharedMemory();
    int Customers_Arrival_Max = numberArray[0];
    int Customers_Arrival_Min = numberArray[1];
    int Cashiers_Number = numberArray[2];
    int Cashiers_BehaviorMin = numberArray[3];
    int Cashiers_BehaviorMax = numberArray[4];
    int ShoppingTimeMax = numberArray[5];
    int ShoppingTimeMin = numberArray[6];
    int ScanTimeMax = numberArray[7];
    int ScanTimeMin = numberArray[8];
    int MaxWaitTime = numberArray[9];
    //int MaxLeaveCashiers = numberArray[10];
    //int MaxCustomersLeave = numberArray[11];
    initializeCheckSharedMemory(Cashiers_Number);
    double IncomeMax = numberArray[12];
    /* Create the opengl */
    
    for ( int number = 0 ; number < Cashiers_Number ; number++ ){
        usleep(50000);
        generateCashierProcess(IncomeMax, ScanTimeMax, ScanTimeMin, Cashiers_BehaviorMin, Cashiers_BehaviorMax, number);
        
    }
    //readAndPrintCashierSharedMemory();

    generateCustomerArrivalProcess(Customers_Arrival_Max, Customers_Arrival_Min, ShoppingTimeMin, ShoppingTimeMax, MaxWaitTime);
    generateOpenGl();
    while (!childSignaled) {
        // Sleep or perform other tasks here
        usleep(15000); // Sleep for 100 milliseconds (adjust as needed)
        checkCustomersAndCashiers(numberArray[10],numberArray[11],&childSignaled);
	//readAndPrintCashierSharedMemory();
    }

    printf("Received SIGINT signal from a customer. Terminating all processes...\n");
    //sleep(1);
    //checkCustomersAndCashiers(numberArray[10],numberArray[11],&childSignaled);
    DeleteSharedMemories();
    printf("\n================\nDone\n================\n");
    terminateChildProcesses();

    // Wait for all child processes to exit
    int status;
    while (wait(&status) > 0) {
    
        // Continue waiting until all children have exited
    }
    return 0;
}


