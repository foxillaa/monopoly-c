/*
 * I recommend to run the program in full screen console mode and ideally
 * with a small console font size, so that the game field fits correctly
 * into the console window and the game does not crash with an error.
 *
 * There are 3 modes of play in the game:
 * 1. Computer vs. computer - this is a simulation mode of computer moves only
 * 2. Computer vs. Player - this is a mode where a single player P1 plays against the other computers
 * 3. Player vs. Player - this is a mode with complete player decisions. You can play by rolling a dice in real life. No simulations.
 *
 * In the menu, control to the UP/DOWN arrows.
 * In the game, input is made with the digits 1-6.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <time.h>

#include <curses.h>

#include "monopoly.h"

#define COL_BROWN 1
#define COL_SKYBLUE 2
#define COL_PURPLE 3
#define COL_ORANGE 4
#define COL_RED 5
#define COL_YELLOW 6
#define COL_GREEN 7
#define COL_BLUE 8
#define COL_NOPROPERTY 9

#define COL_PLAYER1 10
#define COL_PLAYER2 11
#define COL_PLAYER3 12
#define COL_PLAYER4 13

#define HEIGHT_CARD 6
#define WIDTH_CARD 18

// Array of players
PLAYER players[MAX_PLAYERS];

// Ssettings
int num_players = 2; // Default number of players
int game_mode = 0; // Game mode: 1. Computer vs Computer, 2. Computer vs Player, 3. Player vs Player

// Game variables
int step_counter = 1; // Current step of the game
int game_active = 1; // Flag if game is active now

void init_custom_color() 
{
    start_color();

    // Init color pairs
    init_pair(COL_BROWN, COLOR_WHITE + 8, COLOR_YELLOW); // Brown
    init_pair(COL_SKYBLUE, COLOR_WHITE + 8, COLOR_CYAN); // Skyblue
    init_pair(COL_PURPLE, COLOR_WHITE + 8, COLOR_MAGENTA); // Purple
    init_pair(COL_ORANGE, COLOR_WHITE + 8, COLOR_RED + 8); // Orange (high intensity red)
    init_pair(COL_RED, COLOR_WHITE + 8, COLOR_RED); // Red
    init_pair(COL_YELLOW, COLOR_BLACK, COLOR_YELLOW + 8); // Yellow (high intensity yellow)
    init_pair(COL_GREEN, COLOR_WHITE + 8, COLOR_GREEN); // Green
    init_pair(COL_BLUE, COLOR_WHITE + 8, COLOR_BLUE); // Blue
    init_pair(COL_NOPROPERTY, COLOR_BLACK, COLOR_WHITE + 8); // Other (high intensity white)

    // Init color pairs for players
    init_pair(COL_PLAYER1, COLOR_RED, COLOR_BLACK);
    init_pair(COL_PLAYER2, COLOR_BLUE, COLOR_BLACK);
    init_pair(COL_PLAYER3, COLOR_GREEN, COLOR_BLACK);
    init_pair(COL_PLAYER4, COLOR_YELLOW, COLOR_BLACK);
}

void show_start_screen() 
{
    // Two-dimensional character array
    char *art[] = { 
    "                 ...:+xx+.    .;++;;:.             ",
    "                .;X;...x&$.   .x;..:..             ",
    "              .;&&&x...:$&$....;;x..               ",
    "             .;x$&&&x;:..+$$xX+.:+;+..             ",
    "       .:.    .;Xx$X&$Xx;:::+X:. .:+++.            ",
    "   ...:x:;      .XX;xx.:..  .;;:.  .;+X$X+..       ",
    "..;xx++:.x+;:. .X++;:. .+..::..x..  .+:+;.::.      ",
    "+;;+....;xx&&&$+$+$+.    .:;+:+X$:  .x:X+;:x.      ",
    "$+Xx:..:x&&&&&&&&&x+:.x:x. ....+;. .xx+$;x:X+;.    ",
    "         ..;X&&&&&&&&&;+;:;+.++...;X&&&Xx:..;+x:.  ",
    "             .;$&&&&&&X:;;;;+&&&&&&&&&&&x..  .;x+:.",
    "               .X&&&&&&xX+:xx&&&&&&&&&X..      .;+x",
    "        ..      :&&&&&&$:  .X&&&&&&X:.           ..",
    "        .+     .$X$&&&&&X. .X&&X..                 ",
    "        .Xx:..+&&&&&&&$:.&..x&&X.                  ",
    "         .X&&&&&&&&&&&:  :&&&&&&x.:+.              ",
    "          .:xX&&&&&&&&  .x&&&&&&$x..               ",
    "             .+&&&x&&+..;&&&&&&&;.                 ",
    "             ..X&&$X$..x$+&&&&&&&.                 ",
    "               ..xX;;.:;x.X&&&&&&:                 ",
    "                     ...;X&&&&&&x.                 ",
    "                    :$x&&&&&&&x:.                  ",
    "                   .$..X&&&&;.                     ",
    "                   .X..x&&x.                       ",
    "                    X...;:.                        ",
    "                    x.                             ",
    "                    .X+:.                          ",
    "                                                   "
    };

    initscr();  // Initializing the screen
    clear();   // clearing the screen of past content
    noecho();  // deactivate input echo
    cbreak();  // immediate input transfer mode, without buffering

    init_custom_color();

    // Get the screen dimensions
    int max_y = 0;
    int max_x = 0;
    getmaxyx(stdscr, max_y, max_x);  

    // Number of lines in the image
    int n_lines = sizeof(art) / sizeof(art[0]);

    // Calculate vertical start position
    int y_start = (max_y - n_lines) / 2;  
    
    // Print art
    attron(COLOR_PAIR(12));
    for (int i = 0; i < n_lines; ++i) 
    {
        int line_len = strlen(art[i]);
        mvprintw(y_start + i, (max_x - line_len) / 2, "%s", art[i]);  // Center each line
    }
    attroff(COLOR_PAIR(12));
    
    mvprintw(max_y - 1, (max_x - strlen("Press any key to continue...")) / 2, "Press any key to continue...");

    // Refreshing the screen and adding all new content
    refresh();
 
    // Waiting for any key to be pressed
    getch(); 

    endwin();
}

void show_mode_selection() 
{

    // Number of items in the menu
    int n_choices = 3;
    char *choices[] = {
        " 1. Computer vs Computer ",
        " 2. Computer vs Player ",
        " 3. Player vs Player "
    };

    
    initscr();  // Initializing the screen
    clear();   // clearing the screen of past content
    noecho();  // deactivate input echo
    cbreak();  // immediate input transfer mode, without buffering

    // Set the dimensions of the future window
    int height = 10;
    int width = 50;
    // LINES and COLS are global variables that store the current number of rows (LINES) and columns (COLS) of the terminal
    int starty = (LINES - height) / 2;  // vertical start position calculation
    int startx = (COLS - width) / 2;    // calculating the initial horizontal position
    // creating a new window for the menu
    WINDOW *menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);  // allow processing of function keys in the window

    // Frame drawing
    box(menu_win, 0, 0);
    // Header rendering
    mvwprintw(menu_win, 0, (width-strlen(" Select game mode "))/2, " Select game mode ");

    int highlight = 1;   // current highlighted item

    // Display all menu items and draw according to the selection
    for (int i = 0; i < n_choices; ++i) 
    {
        if (highlight == i + 1)     // if this is the currently selected item
        {
            wattron(menu_win, A_REVERSE);                       // enable selection mode for the current item, This causes the item text to appear in inverted colors.
            mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } 
        else 
        {
            mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
        }
    }

    // updates the part of the terminal corresponding to the specified window
    wrefresh(menu_win);

    // Processing user input
    while (1) 
    {
        int c = wgetch(menu_win);
        switch (c) 
        {
            case KEY_UP:
                if (highlight > 1) 
                {
                    highlight -= 1;  // upward movement
                }
                break;
            case KEY_DOWN:
                if (highlight < n_choices) 
                {
                    highlight += 1;  // downward movement
                }
                break;
            case 10:  // Enter key code
                game_mode = highlight;  // item selection
                break;
            default:
                break;
        }

        if (c == 10)  // Exiting the cycle after selection
            break;

        // Redraw menu with updated selection
        for (int i = 0; i < n_choices; ++i) 
        {
            if (highlight == i + 1) 
            {
                wattron(menu_win, A_REVERSE);
            }

            mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);

            if (highlight == i + 1) 
            {
                wattroff(menu_win, A_REVERSE);
            }
        }

        wrefresh(menu_win);
    }

    // Releasing resources and terminating ncurses
    delwin(menu_win);
    endwin();
}

void show_players_selection() 
{
    int n_choices = 3;
    char *choices[] = {
        " 2 Players ",
        " 3 Players ",
        " 4 Players "
    };

    initscr();              // Initializing the screen
    clear();                // Clearing previous content
    noecho();               // Disabling the display of input characters
    cbreak();               // Disabling line buffering

    int height = 10;
    int width = 50;
    int starty = (LINES - height) / 2;  // Vertical centering
    int startx = (COLS - width) / 2;    // Horizontal centering

    WINDOW *menu_win = newwin(height, width, starty, startx);
     // Enabling keyboard mode
    keypad(menu_win, TRUE);  


    // First we draw the frame
    box(menu_win, 0, 0); 
    // Then add text to the frame
    mvwprintw(menu_win, 0, (width-strlen(" Select number of players "))/2, " Select number of players "); 

    int highlight = 1;

    for (int i = 0; i < n_choices; ++i) 
    {
        if (highlight == i + 1) 
        {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } 
        else 
        {
            mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
        }
    }
    
    wrefresh(menu_win);

    while (1) 
    {
        int c = wgetch(menu_win);
        switch (c) 
        {
            case KEY_UP:
                if (highlight > 1) 
                {
                    highlight-=1;
                }
                break;
            case KEY_DOWN:
                if (highlight < n_choices) 
                {
                    highlight+=1;
                }
                break;
            case 10: 
                num_players = highlight + 1;
                break;
            default:
                refresh();
                break;
        }
        
        if (c == 10) 
            break;  // Exiting the cycle after selection

        for (int i = 0; i < n_choices; ++i) 
        {
            if (highlight == i + 1) 
            {
                wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
                wattroff(menu_win, A_REVERSE);
            } 
            else 
            {
                mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
            }
        }
        wrefresh(menu_win);
    }

    delwin(menu_win);
    endwin(); 
}

WINDOW *create_property_card(int starty, int startx, SPACE game_space, int space_index) 
{
    init_custom_color();

    WINDOW *local_win = newwin(HEIGHT_CARD, WIDTH_CARD, starty, startx);
    box(local_win, 0, 0);

    // Determining the color pair for the card background
    int property_color_pair = game_space.type == Property ? game_space.property->color + 1 : COL_NOPROPERTY;

    // Determine the centering position for the property name/type
    const char* name = game_space.type == Property ? game_space.property->name : space_types[game_space.type];
    int name_length = strlen(name);
    int name_pos = (WIDTH_CARD - name_length) / 2;

    // Printing property name or type in the center
    wattron(local_win, COLOR_PAIR(property_color_pair));  // Turn on the color pair attribute
    mvwprintw(local_win, 1, 1, "                ");
    mvwprintw(local_win, 1, name_pos, "%s", name);
    wattroff(local_win, COLOR_PAIR(property_color_pair)); // Turn off the color pair attribute

    mvwhline(local_win, 2, 1, ACS_HLINE, WIDTH_CARD - 2);
    
    // The position for withdrawing players' chips starts from the left edge
    int player_pos = 1;

    // Display of player chips
    for (int i = 0; i < num_players; i++) 
    {
        if (players[i].space_number - 1 == space_index) 
        {
            int player_color = COL_PLAYER1 + i; // Selecting a player's color
            wattron(local_win, COLOR_PAIR(player_color));
            mvwprintw(local_win, 3, player_pos, "P%d", i + 1); // Display the symbol of a player's chip with his number
            wattroff(local_win, COLOR_PAIR(player_color));
            player_pos += 3; // Move to the right 3 positions for the next chip so that there is some space between them
        }
    }

    // Identification and display of the owner, if any
    int owner_found = -1;
    for (int i = 0; i < num_players; i++) 
    {
        for (int j = 0; j < players[i].num_properties; j++) 
        {
            if (players[i].owned_properties[j] == game_space.property) 
            {
                owner_found = i + 1;
                break;
            }
        }
        if (owner_found != -1) break;
    }

    if (owner_found != -1) 
    {
        mvwprintw(local_win, 4, 4, "OWNER: P%d", owner_found);
    } 

    // Price, if it's real estate
    if (game_space.type == Property) 
    {
        char price[16]; // Buffer for formatted price string
        sprintf(price, " %d M ", game_space.property->price); // Format price
        int price_length = strlen(price);
        int price_pos = (WIDTH_CARD - price_length) / 2;
        mvwprintw(local_win, 5, price_pos, "%s", price); // Print price centered
    }

    wrefresh(local_win);
    return local_win;
}

void print_game_info() 
{
    init_custom_color();

    // Display the current turn number
    mvprintw(0, 0, "Turn: %d", step_counter);

    // Display player list
    mvprintw(2, 0, "Players:");

    for (int i = 0; i < num_players; i++) 
    {
        int player_color = COL_PLAYER1 + i; // Selecting a player's color
        attron(COLOR_PAIR(player_color));
        mvprintw(i + 3, 0, "P%d", i + 1); // Display the symbol of a player's chip with his number
        attroff(COLOR_PAIR(player_color));

        // Display information about money, jail passes, and jail status
        attron(COLOR_PAIR(0)); // White color for text
        mvprintw(i + 3, 3, "C: %d M, JP: %d, IJ: %s",
                 players[i].cash,                // Credits
                 players[i].num_jail_pass,       // Jail passes
                 players[i].is_in_jail ? "yes" : "no"); // Jail or no jail
        attroff(COLOR_PAIR(0));
    }

    refresh();
}

int roll_dice() 
{
    return rand() % 6 + 1; 
}

int print_dice(int player_index) 
{
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);  // Get screen dimensions

    // Display the player on turn
    mvprintw(maxy - 2, 0, "Player on turn: P%d", player_index + 1);

    int dice = 0;
    // Handle dice roll
    if (game_mode == 1 || (game_mode == 2 && player_index != 0)) 
    {
        dice = roll_dice();  // Computer's turn

        mvprintw(maxy - 1, 0, "Computer rolls: %d", dice);

        refresh();  // output is displayed before pausing

        napms(2000);  // Pause for 2000 milliseconds (2 seconds)
    } 
    else 
    {
        flushinp(); // Clearing the input buffer before reading data

        echo();     // Enabling the display of input characters

        mvprintw(maxy - 1, 0, "Enter dice roll (1-6): ");

        refresh();  // Refreshing the screen before inputting
        
        char input[10]; // Input buffer
        getnstr(input, sizeof(input)); // Reading a string
        sscanf(input, "%d", &dice); // Converting a string to a number
        
        noecho();   // Turns off the display of input characters
    }
    refresh();

    return dice;

}

void print_game_board() 
{
    initscr();  // Initialize the screen
    clear();    // Clear previous contents

    print_game_info();

    WINDOW *win[NUM_SPACES];
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);  // Get the maximum screen dimensions

    for (int i = 0; i < NUM_SPACES; i++) 
    {
        int x = 0, y = 0;

        // Place the first 7 elements along the bottom row from right to left
        if (i < 7) 
        {
            x = maxx - WIDTH_CARD * (i + 1);
            y = maxy - HEIGHT_CARD;
        }
        // Next 7 elements along the left column from bottom to top
        else if (i >= 7 && i < 13) 
        {
            x = maxx - WIDTH_CARD * 7;
            y = maxy - HEIGHT_CARD * (i - 5);
        }
        // Next 7 elements along the top row from left to right
        else if (i >= 13 && i < 19) 
        {
            x = (maxx - WIDTH_CARD * 5) + WIDTH_CARD * (i - 14);
            y = maxy - HEIGHT_CARD * 7;
        }
        // Final 3 elements along the right column from top to bottom
        else if (i >= 19 && i < 24)
        {
            x = maxx - WIDTH_CARD;
            y = (maxy - HEIGHT_CARD * 6) + HEIGHT_CARD * (i - 19);
        }

        win[i] = create_property_card(y, x, game_board[i], i);
    }

    
}


void init_players()
{
    for (int i = 0; i < num_players; i++) 
    {
        players[i].number = i+1; // Assign a player number
        players[i].space_number = 1; // Player starts on the first square

        if (num_players == 2)   // Initial amount of money
        {
            players[i].cash = 20; 
        }
        else if (num_players == 3)
        {
            players[i].cash = 18; 
        }
        else if (num_players == 4)
        {
            players[i].cash = 16; 
        }

        players[i].num_jail_pass = 0; // The player initially has no cards
        players[i].is_in_jail = 0; // The player is not initially in jail
        players[i].num_properties = 0; // The player initially has no properties purchased
    }
}

int check_monopoly(PLAYER player, COLOR color)
{
    int properties_of_color = 0;

    for (int i = 0; i < player.num_properties; ++i)
    {
        if (player.owned_properties[i]->color == color)
        {
            properties_of_color+=1;
        }
    }

    return properties_of_color == 2;
}

int get_winner()
{
    int max_cash = -1;
    int winning_player = -1;
    int draw = 0;

    // Search for the player with the maximum amount of money
    for (int i = 0; i < num_players; i++)
    {
        if (players[i].cash > max_cash)
        {
            max_cash = players[i].cash;
            winning_player = i;
            draw = 0; // Flag reset in case a new leader has been found
        }
        else if (players[i].cash == max_cash)
        {
            draw = 1; // If two players with the same money are found
        }
    }

    // If it's not a tie, we'll draw the winner.
    if (!draw)
    {
        return winning_player + 1;
    }

    // Else:

    // We remove those users who are not in the draw
    for (int i = 0; i < num_players; i++)
    {
        if (players[i].cash != max_cash)
        {
            players[i].cash = 0;
            players[i].num_properties = 0;
        }
    }

    max_cash = -1; // Drop the maximum amount of money
    draw = 0;

    for (int i = 0; i < num_players; i++)
    {
        int total_cash = -1;

        total_cash = players[i].cash; // The initial value is the amount of money of the player

        for (int j = 0; j < players[i].num_properties; j++)
        {
            total_cash += players[i].owned_properties[j]->price; // Adding real estate value
        }

        if (total_cash > max_cash)
        {
            max_cash = total_cash;
            winning_player = i;
            draw = 0; // Found a new leader
        }
        else if (total_cash == max_cash)
        {
            draw = 1; // Found another player with the same total amount of money
        }
    }

    if (!draw)
    {
        return winning_player + 1;
    }
    else
    {
        return -1;
    }
}
//show winner
void show_final_screen() 
{
    char *art[] = {
        "         ,-.-.  .=-.-..-._        .-._           ,----.               ",
        ",-..-.-./  \\==\\/==/_ /==/ \\  .-._/==/ \\  .-._ ,-.--` , \\  .-.,.---.   ",
        "|, \\=/\\=|- |==|==|, ||==|, \\/ /, /==|, \\/ /, /==|-  _.-` /==/  `   \\  ",
        "|- |/ |/ , /==/==|  ||==|-  \\|  ||==|-  \\|  ||==|   `.-.|==|-, .=., | ",
        " \\, ,     _|==|==|- ||==| ,  | -||==| ,  | -/==/_ ,    /|==|   '='  / ",
        " | -  -  , |==|==| ,||==| -   _ ||==| -   _ |==|    .-' |==|- ,   .'  ",
        "  \\  ,  - /==/|==|- ||==|  /\\ , ||==|  /\\ , |==|_  ,`-._|==|_  . ,'.  ",
        "  |-  /\\ /==/ /==/. //==/, | |- |/==/, | |- /==/ ,     //==/  /\\ ,  ) ",
        "  `--`  `--`  `--`-` `--`./  `--``--`./  `--`--`-----`` `--`-`--`--'  "
    };

    initscr();  // Initializing the screen
    clear();    // clearing the screen of past content
    noecho();   // deactivate input echo
    cbreak();   // immediate input transfer mode, without buffering

    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);  // Get the screen dimensions

    int n_lines = sizeof(art) / sizeof(art[0]);
    int y_start = (max_y - n_lines - 3) / 2;  // Calculate the vertical starting position

    // Art output
    attron(COLOR_PAIR(13));
    for (int i = 0; i < n_lines; ++i) 
    {
        mvprintw(y_start + i, (max_x - strlen(art[i])) / 2, "%s", art[i]);  // Center each line
    }
    attroff(COLOR_PAIR(13));

    // We get the winner's number
    int winner = get_winner();  // Function to get the number of the winner
    char winner_text[50];       //Preparing text for output

    if (winner != -1) 
    {
        sprintf(winner_text, "--- P%d ---", winner);
    } 
    else 
    {
        strcpy(winner_text, "--- DRAW ---");
    }

    // Display the number of the winning player or the draw message below the artwork
    mvprintw(y_start + n_lines + 1, (max_x - strlen(winner_text)) / 2, "%s", winner_text);

    // Instructions for proceeding
    mvprintw(max_y - 1, (max_x - strlen("Press any key to exit...")) / 2, "Press any key to exit...");

    refresh();  // Screen update
    // Waiting for any key press to complete
    getch(); 

    endwin();  // Completing work with ncurses
}

void property_action(int player_number, SPACE *space) {

    PROPERTY *property = space->property;

    // Searching for a property owner
    int owner_number = -1;

    for (int i = 0; i < num_players; i++)
    {
        for (int j = 0; j < players[i].num_properties; j++)
        {
            if (players[i].owned_properties[j] == property)
            {
                owner_number = i;
                break;
            }
        }
        if (owner_number != -1) {
            break;
        }
    }

    // If the property belongs to the current player, we do nothing
    if (owner_number == player_number)
    {
        return;
    }

    int rent = property->price;

    // If the property is not owned by anyone
    if (owner_number == -1)
    {
        // Bankruptcy check
        if (players[player_number].cash <  property->price) 
        {
            game_active = 0;
            return;
        }

        players[player_number].cash -= rent;
        players[player_number].owned_properties[players[player_number].num_properties] = property;
        players[player_number].num_properties+=1;

        return;
    }

    // If the property is owned by someone
    if (check_monopoly(players[owner_number], property->color)) 
    {
        rent *= 2; // Double the rent
    }

    // Bankruptcy check
    if (players[player_number].cash < rent) 
    {
        game_active = 0;
        return;
    }

    // Subtract the rent from the current player's balance and add it to the owner's balance
    players[player_number].cash -= rent;
    players[owner_number].cash += rent;
}

void go_to_jail_action(int player_number) 
{
    if (players[player_number].num_jail_pass > 0)
    {
        players[player_number].num_jail_pass -= 1;
        return;
    }

    // The player is going to prison
    players[player_number].space_number = 7;
    players[player_number].is_in_jail = 1;
}

int main() 
{
    setlocale(LC_ALL, "");
    srand(time(NULL));
    
    // Initializing the screen
    initscr();  

    // Displaying the start window
    show_start_screen();

    // Starter customisations
    show_mode_selection();
    show_players_selection();

    // Initialisation of players
    init_players();

    // Listing fields
    print_game_board();

    // Game cycle
    while(game_active)
    {
        for (int i = 0; i < num_players; ++i)
        {
            // Kocka
            int dice = print_dice(i);
        
            // If you're in prison
            if(players[i].is_in_jail)
            {
                if (players[i].cash < 1)
                {
                    game_active = 0;
                    break;
                }

                players[i].cash -= 1;
                players[i].is_in_jail = 0;
            }

            // Calculating a new field
            int new_space_number = players[i].space_number + dice;

            // Processing of the start pass
            if (new_space_number > NUM_SPACES)
            {
                players[i].cash += 2;
                new_space_number -= NUM_SPACES;
            }

            SPACE *new_space =  &game_board[new_space_number - 1];

            switch (new_space->type)
            {
                case Property:
                    property_action(i, new_space);
                    break;
                case Go_to_jail:
                    go_to_jail_action(i);
                    break;
                case Jail_pass:
                    players[i].num_jail_pass+=1;
                    break;
                case In_jail:
                case Start:
                case Free_parking:
                    break;
                default:
                    break;
            }

            // Updating the player's position
            if (!players[i].is_in_jail)
            {
                players[i].space_number = new_space_number;
            }

            // Completion of the cycle if there is a bankruptcy
            if (!game_active)
            {
                break;
            }

            print_game_board();

            step_counter +=1;
        }
    }

    // Displaying the end window
    show_final_screen();

    clear();
    refresh();

    endwin();

    return 0;
}
