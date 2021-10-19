#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#define KEY 34856
#define MAT_SIZE 3

// type:
// 		0 = see "messageType"
// 		1 = player 1 has started.
// 		2 = player 2 has started.
// 	messageType:
// 		0 = wait for the other player.
// 		1 = other player's turn.
// 		2 = winner player 1.
// 		3 = winner player 2.
// 		4 = Board is full.

// Messages
typedef struct Message {
    long type;
	int matRow;
	int matCol;
	int messageType;
} message;

char matrix[MAT_SIZE][MAT_SIZE];
int row, col;
bool endGame = false;

char getValidChar(char square);
bool checkBoundaries(int row, int col);
void fillBoard(char (*matrix)[MAT_SIZE]);
void showBoard(char (*matrix)[MAT_SIZE]);
bool isBoardFull(char (*matrix)[MAT_SIZE]);
bool isThereAWinner(char (*matrix)[MAT_SIZE], char symbol);
bool isSquareTaken(int row, int col, char (*matrix)[MAT_SIZE]);
bool checkVerticalMatch(char (*matrix)[MAT_SIZE], char symbol);
bool checkHorizontalMatch(char (*matrix)[MAT_SIZE], char symbol);
bool checkLeftDiagonalMatch(char (*matrix)[MAT_SIZE], char symbol);
bool checkRightDiagonalMatch(char (*matrix)[MAT_SIZE], char symbol);
void takeSquare(int row, int col, char (*matrix)[MAT_SIZE], char symbol);

int main() {
    // Mandatory variables
    int playerTwoId;
    int messageSize;

    // Messages network
    message send;
    message recieve;

    // Initilize board
    fillBoard(matrix);

    // Message size
    messageSize = sizeof(message) - sizeof(long);

    // Initializing message stack
    playerTwoId = msgget(KEY, 0600 | IPC_CREAT);

    // Wait until player 1 has started
    printf("Waiting for player 1...\n");

    // Recieve player 1 confirmation
    msgrcv(playerTwoId, &recieve, messageSize, 1, 0);

    // Exit if connection between player 1 and 2 fails
    if(recieve.messageType != 0) {
        printf("ERROR: Connection between the two players failed, try again.\n");

        // Deleting message stack
        msgctl(playerTwoId, IPC_RMID, 0);

        return 0;
    }

    // Indicates player 2 has started 
    send.type = 2;
    send.matRow = 0;
    send.matCol = 0;
    send.messageType = 0;

    // Send message
    msgsnd(playerTwoId, &send, messageSize, 0);

    // Show empty board
    showBoard(matrix);

    // Player 1's turn
    printf("Waiting for player 1...\n");
     
	// Recieve other player's move
    msgrcv(playerTwoId, &recieve, messageSize, 0, 0);

    // Reflect other player's move on board
    takeSquare(recieve.matRow - 1, recieve.matCol - 1, matrix, 'X');

    showBoard(matrix);

    // Game has started
    while (1) {
        if(isBoardFull(matrix)) {
            printf("No winners...\n");
					
            send.matRow = 0;
            send.matCol = 0;
            send.messageType = 4;

            msgsnd(playerTwoId, &send, messageSize, 0);
            return 0;
        }
        
        printf("Enter coordinates:\n");
		printf("Row: \n");
		scanf("%i", &row);		
		printf("Column: \n");
		scanf("%i", &col);

        if(checkBoundaries(row, col)) {
            if(isSquareTaken(row - 1, col - 1, matrix))
                printf("Square is already taken!\n");
            else {
                // Player takes a square from the board
                takeSquare(row - 1, col - 1, matrix, '0');

                showBoard(matrix);

                // Check if player 2 has won
                endGame = isThereAWinner(matrix, '0');

                if(endGame) {
                    printf("The winner is player 2!!\n");
					
                    send.matRow = row;
					send.matCol = col;
					send.messageType = 3;

					msgsnd(playerTwoId, &send, messageSize, 0);
					return 0;
                }

                // Send player's move to the other
                send.type = 2;
				send.matRow = row;
				send.matCol = col;
				send.messageType = 1;

                printf("Sending move to player 1...\n");
				msgsnd(playerTwoId, &send, messageSize, 0);

                printf("Waiting for player 1...\n");
				
                // Recieve other player's move
                printf("Recieving player 1's move...\n");
                msgrcv(playerTwoId, &recieve, messageSize, 0, 0);

                // Reflect other player's move on board
                takeSquare(recieve.matRow - 1, recieve.matCol - 1, matrix, 'X');

                showBoard(matrix);

                if(recieve.messageType == 2) {
                    printf("The winner is player 1!!\n");

					msgctl(playerTwoId, IPC_RMID, 0);
					return 0;
                }
                else if(recieve.messageType == 4) {
                    printf("No winners...\n");

					msgctl(playerTwoId, IPC_RMID, 0);
					return 0;
                }
            }
        }
        else
            printf("The coordinates (%i,%i) are invalid! Please enter numbers from 1 to 3.\n", row, col);
    }

    return 0;
}

// Initializes de board
void fillBoard(char (*matrix)[MAT_SIZE]) {
    for(int i = 0; i < MAT_SIZE; i++)
        for(int j = 0; j < MAT_SIZE; j++)
            matrix[i][j] = 'T';
}

// Shows the TicTacToe board
void showBoard(char (*matrix)[MAT_SIZE]) {
    int middleSquare = MAT_SIZE - 2;
    int bottomBoard = MAT_SIZE - 1;

    printf("\n");

    for(int i = 0; i < MAT_SIZE; i++) {
        for(int j = 0; j < MAT_SIZE; j++) {
            char square = getValidChar(matrix[i][j]);

            if(j == middleSquare) {
                printf("|  %c  |", square);
            }
            else {
                printf("   %c   ", square);
            }
        }
        
        if(i != bottomBoard)
            printf("\n---------------------\n");
    }

    printf("\n");
}

// Returns which char should be showed inside the square: Empty space, X or 0
char getValidChar(char square) {
    if(square == 'T')
        return ' ';
    else
        return square;
}

// Checks if the player has entered valid coordinates
bool checkBoundaries(int row, int col) {
    return (row >= 1 && row <= 3) && (col >= 1 && col <= 3);
}

// Checks if the square is already used
bool isSquareTaken(int row, int col, char (*matrix)[MAT_SIZE]) {
    return matrix[row][col] == 'X' || matrix[row][col] == '0';
}

// Take a square from the board
void takeSquare(int row, int col, char (*matrix)[MAT_SIZE], char symbol) {
    matrix[row][col] = symbol;
}

// Check if there is a horizontal match on the board
bool checkHorizontalMatch(char (*matrix)[MAT_SIZE], char symbol) {
    int count = 0;
    
    for(int i = 0; i < MAT_SIZE; i++) {
        for(int j = 0; j < MAT_SIZE; j++) {
            // Check every column to find three contiguous symbols 
            if(matrix[i][j] == symbol) {
                count++;
            }
        }

        // If there are three, it's a win, else, game continues
        if(count == 3)
            break;
        else
            count = 0;
    }

    return count == 3;
}

// Check if there is a vertical match on the board
bool checkVerticalMatch(char (*matrix)[MAT_SIZE], char symbol) {
    int count = 0;
    
    for(int i = 0; i < MAT_SIZE; i++) {
        for(int j = 0; j < MAT_SIZE; j++) {
            // Check every row to find three contiguous symbols 
            if(matrix[j][i] == symbol) {
                count++;
            }
        }

        // If there are three, it's a win, else, game continues
        if(count == 3)
            break;
        else
            count = 0;
    }

    return count == 3;
}

// Check if there is a left diagonal match on the board
bool checkLeftDiagonalMatch(char (*matrix)[MAT_SIZE], char symbol) {
    int count = 0;
    
    for(int i = 0; i < MAT_SIZE; i++) {
        if(matrix[i][i] == symbol) {
            count++;
        }
    }

    return count == 3;
}

// Check if there is a right diagonal match on the board
bool checkRightDiagonalMatch(char (*matrix)[MAT_SIZE], char symbol) {
    int count = 0;
    
    for(int i = 0, j = MAT_SIZE - 1; i < MAT_SIZE; i++, j--) {
        if(matrix[i][j] == symbol) {
            count++;
        }
    }

    return count == 3;
}

// Check if there is a winner
bool isThereAWinner(char (*matrix)[MAT_SIZE], char symbol) {
    return checkHorizontalMatch(matrix, symbol) || checkVerticalMatch(matrix, symbol)
        || checkLeftDiagonalMatch(matrix, symbol) || checkRightDiagonalMatch(matrix, symbol);
}

// Check no moves can be done
bool isBoardFull(char (*matrix)[MAT_SIZE]) {
    int count = 0;

    for(int i = 0; i < MAT_SIZE; i++) 
        for(int j = 0; j < MAT_SIZE; j++) 
            if(matrix[i][j] == 'X' || matrix[i][j] == '0')
                count++;

    return count == MAT_SIZE * MAT_SIZE;
}