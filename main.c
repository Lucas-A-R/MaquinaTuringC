#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES 10
#define MAX_SYMBOLS 10
#define TAPE_SIZE 100
#define MAX_TRANSITIONS 20
#define MAX_INPUTS 10

#define INITIAL_STATE 1
#define ACCEPT_STATE 5
#define REJECT_STATE 6
#define INPUT_FILE "input.txt"

typedef struct {
    int currentState;
    char readSymbol;
    char writeSymbol;
    char direction;
    int nextState;
} Transition;

typedef struct {
    int numStates;
    int numSymbols;
    char symbols[MAX_SYMBOLS];
    Transition transitions[MAX_TRANSITIONS];
    int initialState;
    int acceptState;
    int rejectState;
} TuringMachine;

void initializeTape(char *tape, const char *input) {
    strcpy(tape, input);
    tape[strlen(input)] = ' '; // Adiciona um espaço em branco no final da fita
}

int findSymbolIndex(TuringMachine *tm, char symbol) {
    for (int i = 0; i < tm->numSymbols; i++) {
        if (tm->symbols[i] == symbol) {
            return i;
        }
    }
    return -1;
}

void runTuringMachine(TuringMachine *tm, char *tape, int *result) {
    int currentState = tm->initialState;
    int headPosition = 0;

    while (currentState != tm->acceptState && currentState != tm->rejectState) {
        char currentSymbol = tape[headPosition];
        int symbolIndex = findSymbolIndex(tm, currentSymbol);

        if (symbolIndex == -1) {
            printf("Erro: Símbolo desconhecido '%c'\n", currentSymbol);
            *result = 0; // Rejeitado
            return;
        }

        int found = 0;
        for (int i = 0; i < MAX_TRANSITIONS; i++) {
            Transition t = tm->transitions[i];
            if (t.currentState == currentState && t.readSymbol == currentSymbol) {
                printf("Transicao: (%d, %c) -> (%c, %c, %d)\n", currentState, currentSymbol, t.writeSymbol, t.direction, t.nextState);
                tape[headPosition] = t.writeSymbol;
                headPosition += (t.direction == 'D') ? 1 : -1;
                currentState = t.nextState;
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Nenhuma transicao encontrada para o estado %d e simbolo '%c'\n", currentState, currentSymbol);
            *result = 0; // Rejeitado
            return;
        }

        if (headPosition < 0 || headPosition >= TAPE_SIZE) {
            printf("Erro: Head position fora dos limites\n");
            *result = 0; // Rejeitado
            return;
        }

        if (tape[headPosition] == ' ') {
            *result = 1; // Aceito automaticamente
            return;
        }
    }

    *result = (currentState == tm->acceptState) ? 1 : 0; // Aceito ou Rejeitado
}

void loadTuringMachine(const char *filename, TuringMachine *tm, char inputs[MAX_INPUTS][TAPE_SIZE], int *numInputs) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%s", tm->symbols);
    tm->numSymbols = strlen(tm->symbols);

    tm->symbols[tm->numSymbols] = ' '; // Adiciona o símbolo de espaço em branco aos símbolos permitidos
    tm->numSymbols++;

    fscanf(file, "%d", &tm->numStates);

    int numTransitions;
    fscanf(file, "%d", &numTransitions);

    for (int i = 0; i < numTransitions; i++) {
        int currentState, nextState;
        char readSymbol, writeSymbol, direction;
        fscanf(file, "%d %c %c %c %d", &currentState, &readSymbol, &writeSymbol, &direction, &nextState);
        tm->transitions[i] = (Transition){currentState, readSymbol, writeSymbol, direction, nextState};
    }

    fscanf(file, "%d", numInputs);

    for (int i = 0; i < *numInputs; i++) {
        fscanf(file, "%s", inputs[i]);
    }

    fclose(file);

    tm->initialState = INITIAL_STATE;
    tm->acceptState = ACCEPT_STATE;
    tm->rejectState = REJECT_STATE;
}

int main() {
    TuringMachine tm;
    char inputs[MAX_INPUTS][TAPE_SIZE];
    int numInputs;

    loadTuringMachine(INPUT_FILE, &tm, inputs, &numInputs);

    for (int i = 0; i < numInputs; i++) {
        char tape[TAPE_SIZE] = {0};
        initializeTape(tape, inputs[i]);

        int result;
        runTuringMachine(&tm, tape, &result);

        printf("%d: %s %s\n", i + 1, inputs[i], result ? "OK" : "NAO OK!!");
    }

    return 0;
}
