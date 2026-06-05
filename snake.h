#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <unordered_map>
#include <vector>

using namespace std;

#ifdef _WIN32
    #include <conio.h>
    #include <Windows.h>
    inline void refresh_screen(int snake_speed) {
        Sleep(snake_speed);
        system("cls");
    }
    inline char get_input() {
        return getch();
    }

#elif __linux__
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    inline int kbhit() {
        static const int STDIN = 0;
        static bool initialized = false;
        if (! initialized) {
            termios term;       // Use termios to turn off line buffering
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }
        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }
    inline void refresh_screen(int snake_speed) {
        usleep(snake_speed * 1000);
        system("clear");
    }
    inline char get_input() {
        return getchar();
    }

#else
    #error "Unknown OS used!!"
#endif

struct Node {
    int x, y;
    Node *next, *prev;
    Node(int x, int y, Node *next, Node *prev) {
        this->x = x;
        this->y = y;
        this->next = next;
        this->prev = prev;
    }
};

inline uint64_t pack(int32_t x, int32_t y) {
    return ((uint64_t)(uint32_t)x << 32) | (uint32_t)y;
}

class game {
    public:
    int f_x, f_y, size_x = 35, size_y = 100;
    int snake_length = 3, snake_speed = 300;
    char last_char;
    bool fruit_eaten = false;
    unordered_map<uint64_t, char> snake_pos;
    Node *root, *tail;
    void init();
    void fruit();
    bool snake(char);
    int snake_game();
};

inline void game::fruit() {
    while(1) {
        srand(time(NULL));
        int x = rand() % size_x;
        int y = rand() % size_y;
        uint64_t xy = pack(x, y);
        for(auto it : snake_pos) {
            if(xy == it.first)
                continue;
        }
        if(x == 0 || x == size_x-1 || y == 0 || y == size_y-1)
            continue;
        f_x = x;
        f_y = y;
        break;
    }
}

inline void game::init() {
    srand(time(NULL));
    int x = rand() % 4, head_x = size_x / 2, head_y = size_y / 2;
    char c = vector<char>({'>', '<', '^', 'v'})[x];
    char d = vector<char>({'-', '-', '|', '|'})[x];
    last_char = vector<char>({'d', 'a', 'w', 's'})[x];
    root = new Node(head_x, head_y, NULL, NULL);
    cout << head_x << " " << head_y;
    snake_pos[pack(head_x, head_y)] = c;
    Node *head = root;
    for(int i = 1; i < snake_length; i++) {
        switch(x) {
            case 0: head_y--; break;
            case 1: head_y++; break;
            case 2: head_x++; break;
            case 3: head_x--; break;
        }
        Node *node = new Node(head_x, head_y, NULL, head);
        snake_pos[pack(head_x, head_y)] = d;
        head->next = node;
        head = node;
    }
    tail = head;
    this->fruit();
}

inline bool game::snake(char ch) {
    if(!fruit_eaten) {
        Node *ptr = tail;
        snake_pos.erase(pack(tail->x, tail->y));
        tail = tail->prev;
        tail->next = NULL;
        delete ptr;
    }
    fruit_eaten = false;
    if(ch == 'l')
        ch = last_char;
    if(ch == 'd') {
        Node *node = new Node(root->x, root->y + 1, root, NULL);
        snake_pos[pack(node->x, node->y)] = '>';
        snake_pos[pack(root->x, root->y)] = '-';
        root->prev = node;
        root = node;
    }
    if(ch == 'a') {
        Node *node = new Node(root->x, root->y - 1, root, NULL);
        snake_pos[pack(node->x, node->y)] = '<';
        snake_pos[pack(root->x, root->y)] = '-';
        root->prev = node;
        root = node;
    }
    if(ch == 'w') {
        Node *node = new Node(root->x - 1, root->y, root, NULL);
        snake_pos[pack(node->x, node->y)] = '^';
        snake_pos[pack(root->x, root->y)] = '|';
        root->prev = node;
        root = node;
    }
    if(ch == 's') {
        Node *node = new Node(root->x + 1, root->y, root, NULL);
        snake_pos[pack(node->x, node->y)] = 'v';
        snake_pos[pack(root->x, root->y)] = '|';
        root->prev = node;
        root = node;
    }
    last_char = ch;
    if(root->x == f_x && root->y == f_y) {
        this->fruit();
        snake_length++;
        fruit_eaten = true;
    }
    if(root->x == 0 || root->x == size_x-1 || root->y == 0 || root->y == size_y-1) {
        return false;
    }
    Node *head = root->next;
    while(head != NULL) {
        if(head->x == root->x && head->y == root->y) {
            cout << "shit" << root->x << " " << root->y << " " << snake_pos[pack(head->x, head->y)] << endl;
            return false;
        }
        head = head->next;
    }
    return true;   
}

inline int game::snake_game() {
    FILE *fptr;
    fptr = fopen("snake_game_leaderboard.txt","a+");
    fseek(fptr, 0, SEEK_SET);
    int c = fgetc(fptr);
    if (c == EOF)
        fprintf(fptr,"Highscore none 0\n");
    else
        ungetc(c, fptr);
    fseek(fptr,0,SEEK_SET);
    int highscore, score;
    char user[100], tuser[100];
    fscanf(fptr,"%*s %s %d",user,&highscore);
    fseek(fptr,0,SEEK_SET);
    bool flag = true;
    this->init();
    while(flag) {
        while(!kbhit() && flag) {
            for(int i = 0; i < size_x; i++) {
                for(int j = 0; j < size_y; j++) {
                    if(i == 0 || i == size_x - 1 || j == 0 || j == size_y - 1)
                        printf("*");
                    else if(i == f_x && j == f_y)
                        printf("@");
                    else if(auto it = snake_pos.find(pack(i, j)); it != snake_pos.end())
                        printf("%c", it->second);
                    else
                        printf(" ");
                }
                if(i == 16)
                    printf("              Current Score :    ");
                else if(i == 17)
                    printf("                    %d", snake_length - 3);
                else if(i == 25)
                    printf("                 !!Help!! :      ");
                else if(i == 26)
                    printf("       1) Move UP : w / W / ^ arrow");
                else if(i == 27)
                    printf("       2) Move Left : a / A / v arrow ");
                else if(i == 28)
                    printf("       3) Move Down : s / S / < arrow ");
                else if(i == 29)
                    printf("       4) Move Right : d / D / > arrow ");
                else if(i == 30)
                    printf("       5) Increase snake speed : f ");
                else if(i == 31)
                    printf("       6) Decrease snake speed : g ");
                else if(i == 32)
                    printf("       7) Quit : q / Q");
                else if(i == 33)
                    printf("       8) Change GRID_SIZE(%ix%i) : x ", size_x, size_y);
                printf("\n");
            }
            printf("                                       Last Highscore: %d by %s \n",highscore,user);
            flag = this->snake('l');
            refresh_screen(snake_speed);
        }
        if(flag) {
            char ch = get_input();
            int k = 0;
            if(ch == 27) {
                scanf("%c", &ch);
                k++;
            }
            if(k && ch == 91) {
                scanf("%c", &ch);
                k++;
            }
            if(k == 2) {
                if(ch == 65)
                    flag = this->snake('w');
                else if(ch == 68)
                    flag = this->snake('a');
                else if(ch == 66)
                    flag = this->snake('s');
                else if(ch == 67)
                    flag = this->snake('d');
            }
            else {
                if(ch == 'w' || ch == 'W')
                    flag = this->snake('w');
                if(ch == 'a' || ch == 'A')
                    flag = this->snake('a');
                if(ch == 's' || ch == 'S')
                    flag = this->snake('s');
                if(ch == 'd' || ch == 'D')
                    flag = this->snake('d');
                if(ch == 'f')
                    snake_speed /= 2;
                if(ch == 'g')
                    snake_speed *= 2;
                if(ch == 'x') {
                    printf("Warning: Large sizes can distort and break the grid!!\n");
                    printf("Enter new GRID_SIZE in the format AxB (Ex: 40x40)\n");
                    int copy_x = size_x, copy_y = size_y;
                    scanf("%ix%i", &size_x, &size_y);
                    if(copy_x > size_x || copy_y > size_y)
                        fruit();
                }
                if(ch == 'q' || ch == 'Q')
                    flag = false;
            }
        }
    }
    score = snake_length - 3;
    printf("                                                      ****************************************\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *            --------------            *\n");
    printf("                                                      *            |            |            *\n");
    printf("                                                      *            |            |            *\n");
    printf("                                                      *            ------------>|            *\n");
    printf("                                                      *                         |            *\n");
    printf("                                                      *                         |            *\n");
    printf("                                                      *                       ---            *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *          You Failed!!!!!!            *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *              Your Score :            *\n");
    printf("                                                      *%20d                  *\n",score);
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      *                                      *\n");
    printf("                                                      ****************************************\n");
    printf("                                                      ****************************************\n");
    char d = 'y';
    if(score > highscore) {
        printf("\n                                                      Congratulations on beating the highscore!!\n");
        printf("                                                             Your name : ");
        scanf("%s",tuser);
        printf("                                                      Thank You for playing the game!!\n");
        int line_count = 0;
        FILE *tfp = fopen("temp.txt","w");
        fprintf(tfp,"Highscore %s %d",tuser,score);
        while(d != EOF) {
            d = getc(fptr);
            if(line_count != 0) {
                if(d != '\n')
                    fprintf(tfp,"%c",d);
            }
            if(d == '\n') {
                fprintf(tfp,"%c",d);
                line_count++;
            }
        }
        fprintf(tfp,"%s %d\n",tuser,score);
        fclose(fptr);
        fclose(tfp);
        remove("snake_game_leaderboard.txt");
        rename("temp.txt","snake_game_leaderboard.txt");
    }
    else {
        printf("\n                                                      Would you like to save your result ? (y/N) : ");
        scanf("%c",&d);
        if(d == 'y') {
            printf("\n                                                      Enter the player name : ");
            scanf("%s",tuser);
            printf("\n                                                      Saved successfully !!");
            fseek(fptr,0,SEEK_END);
            fprintf(fptr,"%s %d\n",tuser,score);
        }
        printf("\n                                                      Thanks for playing the game!!\n");
    }
    char e;
    printf("\n                                                      Would you like to play again? (y/N) : ");
    e = getchar();
    if(e == 'y' || e == 'Y')
        return 1;
    return 0;
}
