#include "local.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <MaxIncome> <Cashiers_Behavior> <ScanTimeMin> <ScanTimeMax> <Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Parse command line arguments
    double MaxIncome = atof(argv[1]);
    int Cashiers_Behavior = atoi(argv[2]);
    int ScanTimeMin = atoi(argv[3]);
    int ScanTimeMax = atoi(argv[4]);
    int Number = atoi(argv[5]);
    saveCashierData(0, Cashiers_Behavior, Number);
    printf("Cashier Process #%d\n",Number+1);
    //printf("MaxIncome: %lf\n", MaxIncome);
    printf("Cashiers_Behavior: %d\n", Cashiers_Behavior);
    //printf("ScanTimeMin: %d\n", ScanTimeMin);
    //printf("ScanTimeMax: %d\n", ScanTimeMax);
    while (1){
        scanItems(Number, ScanTimeMin, ScanTimeMax, MaxIncome);
        
    
    
    }
    return 0;
}

