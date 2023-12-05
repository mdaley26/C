#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t lock;

typedef struct {
    char aname[10];
    int balance;
} Account;

typedef struct {
    char cname[10];
    char transactions[100];
} Client;

typedef struct {
    Client *client;
    int numAccounts;
    Account *accountList;
} ClientThreadArgs;

void performTransaction(Account *accountList, int numAccounts, char *accountName, int amount, bool isDeposit) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accountList[i].aname, accountName) == 0) {
            if (isDeposit) {
                accountList[i].balance += amount;
            } else {
                if (accountList[i].balance >= amount) {
                    accountList[i].balance -= amount;
                }
            }
            break;
        }
    }

    pthread_mutex_unlock(&lock);
}

// thread function for each client
void *clientThread(void *arg) {
    // extract parameters from the argument
    ClientThreadArgs *args = (ClientThreadArgs *)arg;
    Client *client = args->client;
    int numAccounts = args->numAccounts;
    Account *accountList = args->accountList;

    // parse and perform transactions
    char *token = strtok(client->transactions, " ");
    while (token != NULL) {
        if (strcmp(token, "deposit") == 0) {
            token = strtok(NULL, " ");
            char accountName[10];
            strcpy(accountName, token);
            token = strtok(NULL, " ");
            int amount = atoi(token);
            performTransaction(accountList, numAccounts, accountName, amount, true);
        } else if (strcmp(token, "withdraw") == 0) {
            token = strtok(NULL, " ");
            char accountName[10];
            strcpy(accountName, token);
            token = strtok(NULL, " ");
            int amount = atoi(token);
            performTransaction(accountList, numAccounts, accountName, amount, false);
        }

        token = strtok(NULL, " ");
    }

    pthread_exit(NULL);
}



int main() {
    FILE *file;
    file = fopen("assignment_5_input.txt", "r"); // open file
    char line[100];

    long originalPos = ftell(file);
    int numAccounts = 0;
    int numClients = 0;
    char buffer[1024];  // adjust the buffer size as needed
    
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[0] == 'A') {
            numAccounts++; // counting number lines
        } else {
            numClients++;
        }
    }

    Account accountList[numAccounts];
    Client clientList[numClients];
    int i = 0;
    int j = 0;
    if (fseek(file, originalPos, SEEK_SET) != 0) {
        fclose(file); // reset pointer position to start of file
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == 'A') { // file account info
            if (sscanf(line, "%9s balance %d", accountList[i].aname, &accountList[i].balance) == 2) {
                i++;
            }
        } else { 
            sscanf(line, "%9s", clientList[j].cname);
            char *transactionsPos = strchr(line, ' ');
            if (transactionsPos != NULL) {
                strcpy(clientList[j].transactions, transactionsPos + 1);
                clientList[j].transactions[strlen(clientList[j].transactions)] = '\0';
            }
            j++;
        }
    }

    printf("No. of Accounts: %d\n", numAccounts);
    printf("No. of Clients: %d\n", numClients);

    pthread_t threads[numClients];
    ClientThreadArgs threadArgs[numClients];


    // create threads for each client
    for (int i = 0; i < numClients; i++) {

        // populate thread arguments
        threadArgs[i].client = &clientList[i];
        threadArgs[i].numAccounts = numAccounts;
        threadArgs[i].accountList = accountList;

        // pass the arguments to the clientThread function
        if (pthread_create(&threads[i], NULL, clientThread, (void *)&threadArgs[i]) != 0) {
            fprintf(stderr, "Error creating thread for client %d: %s\n", i, strerror(errno));
            return 1;
        }
    }

    // wait for all threads to finish
    for (int i = 0; i < numClients; i++) {
        pthread_join(threads[i], NULL);
    }

    // output the balances of each bank account
    for (int i = 0; i < numAccounts; i++) {
        printf("%s balance %d\n", accountList[i].aname, accountList[i].balance);
    }

    return 0;
}