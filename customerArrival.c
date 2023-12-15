#include "local.h"
int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s Customers_Arrival_Min Customers_Arrival_Max ShoppingTimeMin ShoppingTimeMax MaxWaitTime\n", argv[0]);
        return 1;
    }

    int Customers_Arrival_Min = atoi(argv[1]);
    int Customers_Arrival_Max = atoi(argv[2]);
    int ShoppingTimeMin = atoi(argv[3]);
    int ShoppingTimeMax = atoi(argv[4]);
    int MaxWaitTime = atoi(argv[5]);

    if (Customers_Arrival_Min > Customers_Arrival_Max) {
        fprintf(stderr, "Error: Customers_Arrival_Min should be less than or equal to Customers_Arrival_Max.\n");
        return 1;
    }

    if (ShoppingTimeMin > ShoppingTimeMax) {
        fprintf(stderr, "Error: ShoppingTimeMin should be less than or equal to ShoppingTimeMax.\n");
        return 1;
    }

    if (MaxWaitTime <= 0) {
        fprintf(stderr, "Error: MaxWaitTime should be greater than 0.\n");
        return 1;
    }

    while (1) {
        // Generate a random arrival time between Customers_Arrival_Min and Customers_Arrival_Max
        int arrivalTime = getRandomNumber(Customers_Arrival_Min, Customers_Arrival_Max);

        usleep(arrivalTime*1000); // Wait for the random arrival time

        generateCustomerProcess(ShoppingTimeMin,ShoppingTimeMax,MaxWaitTime);
    }

    return 0;
}

