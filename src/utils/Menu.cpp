#include "Menu.h"
#include "utils.h"
#include <iostream>

Menu::MenuOption Menu::displayMainMenu() {
    while (true) {
        displayMenuOptions();
        MenuOption choice = getUserChoice();

        if (choice == MenuOption::INSTRUCTIONS) {
            displayInstructions();
            continue;
        }

        return choice;
    }
}

void Menu::displayMenuOptions() {
    if (COLORS_ENABLED)
        print_room_by_txtName("menu_options.txt");
    else
        print_room_by_txtName("menu_options_off.txt");
    gotoxy(19, 40);
}

Menu::MenuOption Menu::getUserChoice() {
    const int raw = readKey();
    if (raw < 0) // stdin closed - leave instead of redrawing the menu forever
        return MenuOption::EXIT;
    char input = static_cast<char>(raw);

    if (input >= 'a' && input <= 'z') {
        input = input - 'a' + 'A';
    }

    switch (input) {
    case '1':
        return MenuOption::START_NEW_GAME;
    case '3':
        COLORS_ENABLED = !COLORS_ENABLED;
        return MenuOption::INVALID;
    case '8':
        return MenuOption::INSTRUCTIONS;
    case '9':
        return MenuOption::EXIT;
    default:
        return MenuOption::INVALID;
    }
}

void Menu::displayInstructions() {
    int currentPage = 0;
    const int totalPages = 3;
    bool exitInstructions = false;

    while (!exitInstructions) {
        clearScreen();
        if (currentPage == 0)
            print_room_by_txtName("display_instructions_1.txt");
        else if (currentPage == 1)
            print_room_by_txtName("display_instructions_2.txt");
        else
            print_room_by_txtName("display_instructions_3.txt");

        char key = static_cast<char>(readKey());

        if (key == 0 || key == -32) {
            key = static_cast<char>(readKey());
            if (key == 75) { // Left arrow
                if (currentPage > 0) currentPage--;
            } else if (key == 77) { // Right arrow
                if (currentPage < totalPages - 1) currentPage++;
            }
        } else if (key == 27) { // ESC
            exitInstructions = true;
        }
    }
}

void Menu::clearScreen() {
    cls();
}
