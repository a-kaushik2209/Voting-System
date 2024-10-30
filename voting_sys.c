#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CANDIDATES 10
#define MAX_NAME_LENGTH 50
#define ADMIN_PASSWORD "admin123"
#define DATA_FILE "voting_data.txt"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int votes;
} Candidate;

void displayCandidates(Candidate candidates[], int numCandidates);
void addCandidate(Candidate candidates[], int *numCandidates);
void editCandidate(Candidate candidates[], int numCandidates);
void castVote(Candidate candidates[], int numCandidates, int *invalidVotes);
void saveData(Candidate candidates[], int numCandidates);
void loadData(Candidate candidates[], int *numCandidates);
void displayResults(Candidate candidates[], int numCandidates, int totalVotes, int invalidVotes);
int adminAccess();
void clearInputBuffer();

void displayCandidates(Candidate candidates[], int numCandidates) {
    printf("\nCandidates:\n");
    for (int i = 0; i < numCandidates; i++) {
        printf("%d. %s\n", i + 1, candidates[i].name);
    }
}

int isDuplicate(Candidate candidates[], int numCandidates, const char *name) {
    for (int i = 0; i < numCandidates; i++) {
        if (strcmp(candidates[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

void addCandidate(Candidate candidates[], int *numCandidates) {
    if (*numCandidates >= MAX_CANDIDATES) {
        printf("Maximum number of candidates reached.\n");
        return;
    }

    char name[MAX_NAME_LENGTH];
    printf("Enter the name of the new candidate: ");
    clearInputBuffer();
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0;

    if (isDuplicate(candidates, *numCandidates, name)) {
        printf("Candidate with this name already exists.\n");
    } else {
        strcpy(candidates[*numCandidates].name, name);
        candidates[*numCandidates].votes = 0;
        (*numCandidates)++;
        printf("Candidate '%s' added successfully.\n", name);
    }
}

void editCandidate(Candidate candidates[], int numCandidates) {
    int choice;
    displayCandidates(candidates, numCandidates);
    printf("Enter the candidate number to edit (1 to %d): ", numCandidates);
    
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > numCandidates) {
        printf("Invalid candidate number.\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();
    printf("Enter the new name for candidate %d: ", choice);
    fgets(candidates[choice - 1].name, MAX_NAME_LENGTH, stdin);
    candidates[choice - 1].name[strcspn(candidates[choice - 1].name, "\n")] = 0;
    printf("Candidate name updated successfully to '%s'.\n", candidates[choice - 1].name);
}

void castVote(Candidate candidates[], int numCandidates, int *invalidVotes) {
    int choice;
    char confirm;

    displayCandidates(candidates, numCandidates);
    printf("Enter the candidate number to vote (1 to %d): ", numCandidates);

    while (1) {
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > numCandidates) {
            printf("Invalid choice. Please enter a valid candidate number (1 to %d): ", numCandidates);
            while (getchar() != '\n');
            (*invalidVotes)++;
        } else {
            break;
        }
    }

    printf("You selected %s. Confirm vote? (y/n): ", candidates[choice - 1].name);
    scanf(" %c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        candidates[choice - 1].votes++;
        printf("Vote cast for %s\n", candidates[choice - 1].name);
    } else {
        printf("Vote cancelled.\n");
    }
}

void saveData(Candidate candidates[], int numCandidates) {
    FILE *file = fopen(DATA_FILE, "w");
    if (!file) {
        perror("Failed to save data");
        return;
    }

    fprintf(file, "%d\n", numCandidates);
    for (int i = 0; i < numCandidates; i++) {
        fprintf(file, "%s %d\n", candidates[i].name, candidates[i].votes);
    }
    fclose(file);
    printf("Data saved successfully.\n");
}

void loadData(Candidate candidates[], int *numCandidates) {
    FILE *file = fopen(DATA_FILE, "r");
    if (!file) {
        printf("No saved data found. Starting fresh.\n");
        return;
    }

    fscanf(file, "%d", numCandidates);
    for (int i = 0; i < *numCandidates; i++) {
        fscanf(file, "%s %d", candidates[i].name, &candidates[i].votes);
    }
    fclose(file);
    printf("Data loaded successfully.\n");
}

void displayResults(Candidate candidates[], int numCandidates, int totalVotes, int invalidVotes) {
    printf("\nVoting Results:\n");
    for (int i = 0; i < numCandidates; i++) {
        float percentage = (totalVotes > 0) ? ((float)candidates[i].votes / totalVotes) * 100 : 0;
        printf("%s: %d votes (%.2f%%)\n", candidates[i].name, candidates[i].votes, percentage);
    }
    printf("Total votes cast: %d\n", totalVotes);
    printf("Invalid vote attempts: %d\n", invalidVotes);
}

int adminAccess() {
    char password[20];
    printf("Enter admin password: ");
    scanf("%s", password);
    return strcmp(password, ADMIN_PASSWORD) == 0;
}

void clearInputBuffer() {
    while (getchar() != '\n');
}

int main() {
    Candidate candidates[MAX_CANDIDATES];
    int numCandidates = 0, totalVotes = 0, invalidVotes = 0;

    loadData(candidates, &numCandidates);

    char choice;
    do {
        printf("\nMenu:\n");
        printf("1. Cast a vote\n");
        printf("2. Display results (Admin only)\n");
        printf("3. Save results and exit\n");
        printf("4. Add new candidate (Admin only)\n");
        printf("5. Edit candidate name (Admin only)\n");
        printf("Enter your choice (1-5): ");

        while (1) {
            if (scanf(" %c", &choice) != 1 || choice < '1' || choice > '5') {
                printf("Invalid choice. Please enter a valid option (1-5): ");
                clearInputBuffer();
            } else {
                break;
            }
        }

        switch (choice) {
            case '1':
                if (numCandidates > 0) {
                    castVote(candidates, numCandidates, &invalidVotes);
                    totalVotes++;
                } else {
                    printf("No candidates available. Please contact admin to set up candidates.\n");
                }
                break;
            case '2':
                if (adminAccess()) {
                    displayResults(candidates, numCandidates, totalVotes, invalidVotes);
                } else {
                    printf("Incorrect password. Access denied.\n");
                }
                break;
            case '3':
                saveData(candidates, numCandidates);
                printf("Exiting the voting system.\n");
                return 0;
            case '4':
                if (adminAccess()) {
                    addCandidate(candidates, &numCandidates);
                } else {
                    printf("Incorrect password. Access denied.\n");
                }
                break;
            case '5':
                if (adminAccess()) {
                    editCandidate(candidates, numCandidates);
                } else {
                    printf("Incorrect password. Access denied.\n");
                }
                break;
            default:
                break;
        }
    } while (1);

    return 0;
}
