//
//  main.cpp
//  Tetris
//
//  Created by Tejash Datta on 06/08/24.
//

#include <ncurses.h>
#include <iostream>
#include <thread>
#include <locale>
using namespace std;

int nScreenWidth = 80;
int nScreenHeight = 24;

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;

int rotate(int py, int px, int r) {
    switch (r % 4) {
        case 0: return py * 4 + px;
        case 1: return 12 + py - (px * 4);
        case 2: return 15 - (py * 4) - px;
        case 3: return 3 - py + (px * 4);
    }
    return 0;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
    for (int py = 0; py < 4; py++)
        for (int px = 0; px < 4; px++) {
            int piece_index = rotate(py, px, nRotation);
            
            int field_index = (nPosY + py) * nFieldWidth + (nPosX + px);
            
            if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
                    if (tetromino[nTetromino][piece_index] == L'X' && pField[field_index] != 0)
                        return false;
        }
    
    return true;
}

int main(int argc, const char * argv[]) {
    setlocale(LC_ALL, "");
    
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
    
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    
    // Tetromino shapes
    {
        tetromino[0].append(L"..X.");
        tetromino[0].append(L"..X.");
        tetromino[0].append(L"..X.");
        tetromino[0].append(L"..X.");
        
        tetromino[1].append(L"..X.");
        tetromino[1].append(L".XX.");
        tetromino[1].append(L".X..");
        tetromino[1].append(L"....");
        
        tetromino[2].append(L".X..");
        tetromino[2].append(L".XX.");
        tetromino[2].append(L"..X.");
        tetromino[2].append(L"....");
        
        tetromino[3].append(L"....");
        tetromino[3].append(L".XX.");
        tetromino[3].append(L".XX.");
        tetromino[3].append(L"....");
        
        tetromino[4].append(L"..X.");
        tetromino[4].append(L".XX.");
        tetromino[4].append(L"..X.");
        tetromino[4].append(L"....");
        
        tetromino[5].append(L"....");
        tetromino[5].append(L".XX.");
        tetromino[5].append(L"..X.");
        tetromino[5].append(L"..X.");
        
        tetromino[6].append(L"....");
        tetromino[6].append(L".XX.");
        tetromino[6].append(L".X..");
        tetromino[6].append(L".X..");
    }
    
    pField = new unsigned char[nFieldWidth * nFieldHeight];
    for (int y = 0; y < nFieldHeight; y++)
        for (int x = 0; x < nFieldWidth; x++)
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
    
    bool bGameOver = false;
    
    int nCurrentPiece = rand() % 7;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;
    
    bool bKey[4];
    for (auto &k : bKey) k = false;
    
    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nPieceCount = 0;
    int nScore = 0;
    
    vector<int> vLines;
    
    while (!bGameOver) {
        // Timing ====
        this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);
        
        // Input ====
        switch (getch()) {
            case 'a':
                bKey[0] = true;
                break;
            case 'd':
                bKey[1] = true;
                break;
            case 's':
                bKey[2] = true;
                break;
            case 'w':
                bKey[3] = true;
                break;
        }
            
        // Game Logic ====
        if (bKey[0] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
            nCurrentX--;
        if (bKey[1] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
            nCurrentX++;
        if (bKey[2] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
            nCurrentY++;
        if (bKey[3] && doesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
            nCurrentRotation++;
        
        for (auto &k : bKey) k = false;
        
        if (bForceDown) {
            if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++;
            else {
                // Lock piece
                for (int py = 0; py < 4; py++)
                    for (int px = 0; px < 4; px++)
                        if (tetromino[nCurrentPiece][rotate(py, px, nCurrentRotation)] == L'X')
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
                
                nPieceCount++;
                if (nPieceCount % 10 == 0)
                    if (nSpeed >= 10) nSpeed--;
                
                // Check for lines
                for (int py = 0; py < 4; py++)
                    if (nCurrentY + py < nFieldHeight - 1) {
                        bool bLine = true;
                        
                        for (int x = 1; x < nFieldWidth - 1; x++)
                            bLine &= pField[(nCurrentY + py) * nFieldWidth + x] != 0;
                        
                        if (bLine) {
                            for (int x = 1; x < nFieldWidth - 1; x++)
                                pField[(nCurrentY + py) * nFieldWidth + x] = 8;
                            vLines.push_back(nCurrentY + py);
                        }
                    }
                
                nScore += 25;
                if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;
    
                // Choose next piece
                nCurrentPiece = rand() % 7;
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                
                // If next piece can't fit - Game Over
                bGameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }
            
            nSpeedCounter = 0;
        }
        
        // Display ====
        
        // Draw field
        for (int y = 0; y < nFieldHeight; y++)
            for (int x = 0; x < nFieldWidth; x++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
        
        // Draw piece
        for (int py = 0; py < 4; py++)
            for(int px = 0; px < 4; px++)
                if (tetromino[nCurrentPiece][rotate(py, px, nCurrentRotation)] == L'X')
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
        
        // Draw score
        swprintf(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"Score: %8d", nScore);

        if (!vLines.empty()) {
            // Draw Frame (to show lines)
            for (int y = 0; y < nScreenHeight; y++) {
                for (int x = 0; x < nScreenWidth; x++) {
                    mvaddch(y, x, screen[y * nScreenWidth + x]);
                }
            }
            refresh();
            
            this_thread::sleep_for(400ms);
            
            for (auto &v : vLines)
                for (int x = 1; x < nFieldWidth - 1; x++) {
                    for (int y = v; y > 0; y--)
                        pField[y * nFieldWidth + x] = pField[(y - 1) * nFieldWidth + x];
                    pField[x] = 0;
                }
            
            vLines.clear();
        }
        
        // Draw Frame
        for (int y = 0; y < nScreenHeight; y++) {
            for (int x = 0; x < nScreenWidth; x++) {
                mvaddch(y, x, screen[y * nScreenWidth + x]);
            }
        }
        refresh();

    }
    
    delete[] pField;
    
    endwin();
    delete[] screen;
    
    // Game over
    cout<<"Game Over!! Score: "<<nScore<<endl;
    cin.get();
    
    return 0;
}
