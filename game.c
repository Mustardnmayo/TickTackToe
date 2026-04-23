#define WIN32_LEAN_AND_MEAN
#include<windows.h> //try to remove in the future


typedef unsigned int Board;
typedef unsigned int uint;

//Ascii Numbers Ordinal Offset
#define NumberOffset 48

#define CELL 0b11U
#define FULL -1U

#define UNCLAIMED 0U
#define X_CLAIMED 1U
#define O_CLAIMED 2U

#define FAILURE -1U

static void puts(const char* in) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    char size = 0;
    while (in[size++] != '\0'); //pre or post?...
    WriteFile(out, in, size, NULL, NULL);
}

static uint readNumber() {
    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    char square;
    WINBOOL success = ReadFile(in, &square, 1, NULL, NULL);
    //clear the buffer 
    //not actually safe, but protects from the \n\r at the end of the input 
    char junk[8];
    ReadFile(in, &junk, 8, NULL, NULL);
    if (success) {
        uint ret = square - NumberOffset;
        if (ret <= 9) {
            return ret;
        }
        return FAILURE;
    }
    else {
        return FAILURE;
    }
}

static void didgit_toStr(char* buf, uint num) {
    buf[0] = (char)num + NumberOffset;
    buf[1] = '\0';
}
static void prDidgit(uint num) {
    char buf[2];
    didgit_toStr(buf, num);
    puts(buf);
}

/*
//debugging
static void prBinInt(uint num) {
    puts("0b ");
    for (uint i = (sizeof(uint) * 8) - 1; i >= 0; i--) {
        uint bit = (num >> i) & 1;
        prDidgit(bit);
    }
}
*/

//assumes input is valid
uint boardShift(uint square) {
    return (square - 1) * 2;
}

uint boundsCheckBoard(uint square) {
    if (0 < square && square <= 9) return 0;
    return FAILURE;
}

/*
    board is 18 bits in 4 bytes
    000_0000 000_0099 8877_6655 4433_2211
*/

uint readBoard(const Board* board, uint square) {
    uint shiftAmount = boardShift(square);
    return (*board & (CELL << shiftAmount)) >> shiftAmount;
}

uint writeBoard(Board* board, uint square, uint data) {
    if (boundsCheckBoard(square) == FAILURE) return FAILURE;
    uint shiftAmount = boardShift(square);

    Board x = (data << shiftAmount);
    *(board) |= x;

    return data;
}

void drawBoard(Board* board) {
    puts("\n");
    for (uint y = 0; y < 3; y++) {
        for (uint x = 1; x <= 3; x++) {
            uint cellState = readBoard(board, 3 * y + x);
            char drawChar = 0x0;
            switch (cellState) {
            case UNCLAIMED:
                drawChar = '.';
                break;
            case X_CLAIMED:
                drawChar = 'X';
                break;
            case O_CLAIMED:
                drawChar = 'O';
                break;
            }

            char str[4];
            str[0] = ' ';
            str[1] = drawChar;
            str[2] = ' ';
            str[3] = '\0';
            puts(str);
        }

        puts("  |  ");
        for (uint x = 1; x <= 3; x++) {
            uint idx = 3 * y + x;
            char drawChar = ' ';
            if (readBoard(board, idx) == UNCLAIMED) {
                drawChar = idx + NumberOffset;
            }

            char str[4];
            str[0] = ' ';
            str[1] = drawChar;
            str[2] = ' ';
            str[3] = '\0';
            puts(str);
        }
        puts("\n");
    }
}

uint process(Board* board, uint inputSquare, uint player) {
    if (boundsCheckBoard(inputSquare) == FAILURE) {
        return FAILURE;
    }

    uint cell = readBoard(board, inputSquare);

    if (cell != 0) {
        puts("That square has already been taken\n");
        return FAILURE;
    }
    uint success = writeBoard(board, inputSquare, player);
    return success;
}

uint checkWin(Board* board, uint player) {
    //magic square method may be smaller
    for (uint offset = 0; offset < 3; offset++) {
        //verticals
        uint stor = CELL;
        for (uint i = 0; i < 3; i++) {
            uint square = 3 * i + (offset + 1);
            stor &= readBoard(board, square);
        }
        if (stor != UNCLAIMED) {
            return stor;
        }
        //horizontals
        stor = CELL;
        for (uint i = 1; i <= 3; i++) {
            uint square = offset * 3 + i;
            stor &= readBoard(board, square);
        }
        if (stor != UNCLAIMED) {
            return stor;
        }
    }
    //diagonals
    uint stor = CELL;
    stor &= readBoard(board, 1);
    stor &= readBoard(board, 5);
    stor &= readBoard(board, 9);
    if (stor != UNCLAIMED) {
        return stor;
    }
    stor = CELL;
    stor &= readBoard(board, 3);
    stor &= readBoard(board, 5);
    stor &= readBoard(board, 7);
    if (stor != UNCLAIMED) {
        return stor;
    }
    return UNCLAIMED;
}


void gameLoop() {
    Board gameBoard = 0U;

    uint player_has_won = UNCLAIMED;
    uint counter = 0U;

    while (player_has_won == UNCLAIMED && counter++ < 9) {
        drawBoard(&gameBoard);

        uint player = (~counter & 1) + 1;
        uint inputSquare;

        do {
            puts("Player : [");
            prDidgit(player);
            puts("] enter what square to place on: ");
            inputSquare = readNumber();
        } while (process(&gameBoard, inputSquare, player) == FAILURE);
        puts("\n");
        player_has_won = checkWin(&gameBoard, player);
    }
    drawBoard(&gameBoard);

    if (player_has_won != UNCLAIMED) {
        puts("Player : [");
        prDidgit(player_has_won);
        puts(player_has_won == X_CLAIMED ? " \"X\"" : " \"O\"");
        puts("] has won!\n");
    }
    else {
        puts("It appears the cat has won...\n");
    }
}

void nomain() {

    gameLoop();

    ExitProcess(0);

    /*
    asm(
        "xorl % ecx, % ecx\n\t"
        "call * __imp_ExitProcess(% rip)\n\t"
    );
    */
}
