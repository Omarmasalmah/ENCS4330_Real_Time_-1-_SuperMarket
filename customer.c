#include "local.h"
void sigintHandler(int signal) {
    // Handle the SIGINT signal here
    printf("Customer finished shopping and left happy\n");
    decreaseCustomers();
    // Add your custom handling logic here

    // Optionally, exit the program gracefully
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ShoppingTime> <MaxWaitTime>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //printf("Customer %d went inside the supermarket!\n",getpid());
    int ShoppingTime = atoi(argv[1]);
    int MaxWaitTime = atoi(argv[2]);
    increaseCustomers();
    //printf("Customer Process %d :\n",getpid());
    //printf("Shopping Time: %d\n", ShoppingTime);
    //printf("Max Wait Time: %d\n", MaxWaitTime);


    // Simulate max wait time by sleeping for MaxWaitTime seconds
    if (signal(SIGUSR1, sigintHandler) == SIG_ERR) {
        perror("Unable to register SIGINT handler");
        exit(EXIT_FAILURE);
    }
    double cartPrice;
    int cart;
    sleep(ShoppingTime);
    
    //printf("Customer has finished shopping %d!\n",getpid());
    //getRandomItemsAndCalculateCart(&cartPrice,&cart);
    OptimizedgetRandomItemsAndCalculateCart(&cartPrice, &cart);
    //printf("Customer %d with cart price: $%.2lf and with %d items\n",getpid(), cartPrice,cart);
    applyCustomerData(cart, cartPrice, getpid());

    sleep(MaxWaitTime);
    //printf("Customer has finished shopping and waiting!\nS/He left!\n=====\n");
    increaseLeftCustomers();
    
    
    return 0;
    
    
    
    



    return 0;
}

