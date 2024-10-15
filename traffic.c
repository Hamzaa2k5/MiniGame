#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>

#define MAX_MAP_ROW 25
#define MAX_MAP_COL 25

// Car struct to represent player and  cars
typedef struct {
    int row;
    int col;
    char direction;
} Car;

// Function to clear the terminal screen
void clearScreen() {
    printf("\033[2J\033[1;1H");
}

// Function declaration
int checkGameStatus(char **map, int map_row, int map_col);

// printing the map
void printMap(char **map, int map_row, int map_col) {
    for (int i = 0; i < map_row; i++) {
        for (int j = 0; j < map_col; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

//  Update the car positions and check for collisions
int updateCars(char **map, int map_row, int map_col, Car *cars, Car *playerCar, char input) {
    // Update player car position
    map[playerCar->row][playerCar->col] = '.';  // Clear current position

    // Update player position
    int newRow = playerCar->row;
    int newCol = playerCar->col;

    switch (input) {
        case 'w': 
            newRow = playerCar->row - 1;
            break;
        case 's': 
            newRow = playerCar->row + 1;
            break;
        case 'a': 
            newCol = playerCar->col - 1;
            break;
        case 'd': 
            newCol = playerCar->col + 1;
            break;
    }

    // Check if the new position is within bounds and empty
    if (newRow >= 1 && newRow < map_row - 1 && newCol >= 1 && newCol < map_col - 1 && map[newRow][newCol] == '.') {
        playerCar->row = newRow;
        playerCar->col = newCol;
    }

    // Check for collisions
    for (int i = 0; i < map_row / 2 - 1; i++) {
        if (newRow == cars[i].row && newCol == cars[i].col) {
            if (cars[i].direction == '<' || cars[i].direction == '>') {
                printf("Oops! You collided with a car. Game over!\n");
                return -1;  // Player collided
            }
        }
    }


    // Update player position on the map
    map[playerCar->row][playerCar->col] = 'P';

    // Update  car positions
    for (int i = 0; i < map_row / 2 - 1; i++) {
        int currentRow = cars[i].row;
        int currentCol = cars[i].col;
        char currentDirection = cars[i].direction;

        map[currentRow][currentCol] = '.';  // Clear current position

        // Move the car one step in its current direction
        if (currentDirection == '<') {
            // Move left if the cell to the left is empty
            if (currentCol > 1 && map[currentRow][currentCol - 1] == '.') {
                cars[i].col--;
            } else {
                cars[i].direction = '>';
            }
        } else {
            // Move right if the cell to the right is empty
            if (currentCol < map_col - 2 && map[currentRow][currentCol + 1] == '.') {
                cars[i].col++;
            } else {
                cars[i].direction = '<';  // Change direction to left if reached end
            }
        }

        // Update map with the new car position
        map[currentRow][cars[i].col] = currentDirection;
    }

    return checkGameStatus(map, map_row, map_col);  // Continue playing
}

// Function to check if the player has won
int checkGameStatus(char **map, int map_row, int map_col) {
    // Check if the player has reached the goal
    if (map[1][map_col - 2] == 'P') {
        return 1;  // Player won
    }

    return 0;  // Continue playing
}

// Function to free allocated memory
void freeMemory(char **map, int map_row) {
    for (int i = 0; i < map_row; i++) {
        free(map[i]);
    }
    free(map);
}

// Function to set terminal to non-canonical mode
void setNonCanonicalMode() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
}

// Function to restore terminal to canonical mode
void restoreCanonicalMode() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

int main(int argc, char *argv[]) {
    // Set terminal to non-canonical mode
    setNonCanonicalMode();

    // Check if the correct number of arguments is provided
    if (argc != 3) {
        printf("Usage: %s <map_row> <map_col>\n", argv[0]);
        return 1;  // Exit the program
    }

    // Parse and validate row
    int map_row = atoi(argv[1]);
    if (map_row < 5 || map_row % 2 == 0) {
        printf("Invalid map_row. It should be an odd number greater than or equal to 5.\n");
        return 1;  // Exit the program
    }

    // Parse and validate column
    int map_col = atoi(argv[2]);
    if (map_col < 5) {
        printf("Invalid map_col. It should be greater than or equal to 5.\n");
        return 1;  // Exit the program
    }

    // Allocate memory for the 2D array
    char **map = (char **)malloc(map_row * sizeof(char *));
    for (int i = 0; i < map_row; i++) {
        map[i] = (char *)malloc(map_col * sizeof(char));
    }

    // Initialize the map
    for (int i = 0; i < map_row; i++) {
        for (int j = 0; j < map_col; j++) {
            if (i == 0 || i == map_row - 1 || j == 0 || j == map_col - 1) {
                map[i][j] = '*';
            } else if (i == map_row - 2 && j == map_col - 2) {
                map[i][j] = 'G';  // Goal at the bottom right
            } else if (i == map_row - 2 && j == 1) {
                map[i][j] = ' ';  
            } else if (i % 2 == 0) {
                map[i][j] = '.';  // Dotted line only in even rows
            } else {
                map[i][j] = ' ';  // Blank in odd rows
            }
        }
    }

    // Set initial player car position
    Car playerCar;
    playerCar.row = 1;
    playerCar.col = 1;
    playerCar.direction = '>';

    // Set initial  car positions
    Car *cars = (Car *)malloc((map_row / 2 - 1) * sizeof(Car));
    srand(time(NULL));
    for (int i = 0; i < map_row / 2 - 1; i++) {
        cars[i].row = (i + 1) * 2;
        cars[i].col = (rand() % (map_col - 2)) + 1;  // Random initial column within the map borders
        cars[i].direction = rand() % 2 == 0 ? '<' : '>';
        map[cars[i].row][cars[i].col] = cars[i].direction;
    }

    // Main game loop
    while (1) {
        clearScreen();  // Clear the terminal screen

        // Print the map with updated positions
        printMap(map, map_row, map_col);

        // Read user input
        char input;
        if (read(0, &input, sizeof(input)) > 0) {
            // Update car positions and check for collisions
            int gameStatus = updateCars(map, map_row, map_col, cars, &playerCar, input);

            // Check for winning/losing conditions
            if (gameStatus == 1) {
                printf("Congratulations! You won!\n");
                break;  /
            } else if (gameStatus == -1) {
                printf("Oops! You collided with a car. Game over!\n");
                break;  
            }
        }
    }

    // Free allocated memory
    freeMemory(map, map_row);
    free(cars);

    // Restore terminal to canonical mode
    restoreCanonicalMode();

    return 0;  // Exit normally
    }
