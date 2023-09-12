#include<ncurses.h>
#include<random>
#include<ctime>
#include<string>

using namespace std;

//searches array for key via linear search
bool arrSearch(int arr[],int key,int arrSize){
    for(int i=0;i<arrSize;i++){
        if(arr[i] == key) return true;
    }
    return false;
}

//counts adjacent mines
int countAdjacent(int tile,string board[],int boardsize,int boardheight,int mines,int mine[]){
    // places to check
    // tile - boardsize -1      tile - boardsize - 0        tile - boardsize + 1
    // tile-1                   tile                        tile +1
    // tile + boardsize -1      tile + boardsize - 0        tile + boardsize + 1
    int count = 0;
    for (int i = -1; i<2; i++) {
        //top row case
        if(i == -1 && static_cast<int>(tile/boardsize) == 0) continue;
        //bottom row case
        if(i == 1 && static_cast<int>(tile/boardsize) == boardheight-1) continue;
        for (int j = -1; j<2;j++){
            //left edge case
            if(j == -1 && tile%boardsize == 0) continue;
            //right edge case
            if(j == 1 && (tile+1)%boardsize == 0) continue;
            if(arrSearch(mine,tile + (boardsize*i) + j,mines)){
                //mine found
                count++;
            }
        }
    }
    return count;
}

//TODO: make win screen
void gameWon(){
    mvwprintw(stdscr,10, 10, "Won!!");
}

//TODO: make gameover screen
void gameLost(bool &gameover){
    gameover = true;
}

//flags/unflags a space
void flagSpace(int tile,string board[]){
    if(board[tile] == "X"){
        //already flagged; unflag
        board[tile] = "-";
    }else board[tile] = "X";
}

//clears a tile
void demolish(int tile,string board[],int boardsize,int boardheight,int mines,int mine[],bool &gameover){
    //check if tile is flagged
    if(board[tile] == "X")return;

    //check if tile is a mine
    if(arrSearch(mine, tile , mines)){
        //game over
        gameLost(gameover);
    }
    int adj = countAdjacent(tile,board,boardsize,boardheight,mines,mine);
    board[tile] = adj>0?to_string(adj):" ";
    //clear adjacent tiles if safe tile
    if(adj != 0)return;
    for (int i = -1; i<2; i++) {
        if(i == -1 && static_cast<int>(tile/boardsize) == 0) continue;
        if(i == 1 && static_cast<int>(tile/boardsize) == boardheight-1) continue;
        for (int j = -1; j<2;j++){
            if(j == -1 && tile%boardsize == 0) continue;
            if(j == 1 && (tile+1)%boardsize == 0) continue;
            if(board[tile + (boardsize*i) + j] == "-"){
                //undiscovered space found
                demolish(tile + (boardsize*i) + j,board,boardsize, boardheight, mines, mine,gameover);
            }
        }
    }
}

int main(int argc, char **argv){

    //init ncurses
    initscr();
    noecho();

    //variables
    //BEGINNER:     8x8,10 mines
    //INTERMEDIATE: 16x16,40 mines
    //EXPERT:       30x16,99 mines
    int sizey = 8,sizex = 8,mines = 10;
    /* int sizey = 16,sizex = 16,mines = 40; */
    /* int sizey = 16,sizex = 30,mines = 99; */
    int cursx = 0,cursy = 0;
    int height,width;
    getmaxyx(stdscr,height,width);
    WINDOW *boardwin = newwin(sizey+2,(sizex*2)+3,(height/2) - (sizey+2)/2,width/2 - (sizex*2+3)/2);
    int boardsize = sizey*sizex;
    //gamestate variable
    //0 is menu, 1 is game
    //TODO: make/handle gamestate change
    int gamestate = 0;

    //enable colors and set color pairs
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    init_pair(3,COLOR_BLUE,COLOR_BLACK);
    init_pair(4,COLOR_YELLOW,COLOR_BLACK);
    init_pair(5,COLOR_CYAN,COLOR_BLACK);

    
    //init random engine
    default_random_engine randomGen(time(0));
    uniform_int_distribution<int> minegen(0,boardsize-1);

    //array to hold board states
    //- is undiscovered space
    //X is flagged space
    //a number denotes adjacent mines
    //empty is discovered unadjacent tile
    int mine[mines];
    bool gameover = false;
    //bad idea but using a 1d array for the board
    string board[boardsize];
    //init board
    for(int i =0; i<boardsize;i++){
        board[i] = '-';
    }
    //generate mines
    for (int i = 0; i< mines;i++){
        mine[i] = minegen(randomGen);
        /* reveals mines; for debug */
        /* board[mine[i]] = "o"; */
    }

    //enable keypad
    keypad(boardwin,true);
    
    while (1){
        //check for lose
        if(gameover)break;

        //check for win
        for (int i = 0; i < mines; i++) {
            if(board[mine[i]] != "X")break;
            if(i == mines-1){
                //all mines flagged
                gameWon();
            }
        }

        //draw ui
        box(stdscr,0,0);
        wattron(boardwin,COLOR_PAIR(1));
        box(boardwin,0,0);
        wattroff(boardwin,COLOR_PAIR(1));
        //draw board
        for(int i = 0; i<boardsize;i++){
            mvwprintw(boardwin,static_cast<int>(i/sizex)+1,((i%sizex)*2)+2,board[i].c_str());
            if(board[i] == "-"){
                wattron(boardwin,COLOR_PAIR(3));
                mvwprintw(boardwin,static_cast<int>(i/sizex)+1,((i%sizex)*2)+2,board[i].c_str());
                wattroff(boardwin,COLOR_PAIR(3));
            } else if (board[i] == "X"){
                wattron(boardwin,COLOR_PAIR(5));
                mvwprintw(boardwin,static_cast<int>(i/sizex)+1,((i%sizex)*2)+2,board[i].c_str());
                wattroff(boardwin,COLOR_PAIR(4));
            }else if (board[i] == "1"){
                wattron(boardwin,COLOR_PAIR(1));
                mvwprintw(boardwin,static_cast<int>(i/sizex)+1,((i%sizex)*2)+2,board[i].c_str());
                wattroff(boardwin,COLOR_PAIR(1));
            }else if (board[i] == "2"){
                wattron(boardwin,COLOR_PAIR(4));
                mvwprintw(boardwin,static_cast<int>(i/sizex)+1,((i%sizex)*2)+2,board[i].c_str());
                wattroff(boardwin,COLOR_PAIR(4));
            }else if (board[i] == "3"){
                wattron(boardwin,COLOR_PAIR(2));
                mvwprintw(boardwin,static_cast<int>(i/sizex)+1,((i%sizex)*2)+2,board[i].c_str());
                wattroff(boardwin,COLOR_PAIR(2));
            }
        }

        //move cursor back to cursor location
        wmove(boardwin,cursy + 1,(cursx*2) +2);

        //refresh to render
        refresh();
        wrefresh(boardwin);

        //get and process input
        int key = wgetch(boardwin);
        if (key == 'w' || key == KEY_UP){
            //got up
            cursy = cursy-1<0?0:cursy-1;
        }else if (key == 's' || key == KEY_DOWN){
            //got down
            cursy = cursy+1>sizey-1?sizey-1:cursy+1;
        }else if (key == 'a' || key == KEY_LEFT){
            //got left
            cursx = cursx-1>=0?cursx-1:0;
        }else if (key == 'd' || key == KEY_RIGHT){
            //got right
            cursx = cursx+1>sizex-1?sizex-1:cursx+1;
        }else if (key == 'f' || key == KEY_BACKSPACE){
            //flag space
            flagSpace((cursy*sizex) + cursx,board);
        }else if (key == 'e' || key == 10){
            //clear space
            demolish((cursy*sizex) + cursx, board, sizex,sizey,mines, mine,gameover);
        }else if (key == 'q') break;
    }

    //end ncurses
    endwin();
}
