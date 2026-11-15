#include "Adventure.h"

#include <filesystem>
#include <iostream>

// ===================== GIVEN HELPERS (do not change) =====================

// Reads the next line that is not blank.
// Spaces at the start of the line are removed.
// Returns false when the file has no more lines.
bool Adventure::nextLine(std::ifstream& in, std::string& line) {
    std::string raw;
    while (std::getline(in, raw)) {
        int start = 0;
        while (start < (int)raw.size() && (raw[start] == ' ' || raw[start] == '\t')) {
            start++;
        }
        line = raw.substr(start);
        // Files saved on Windows can end each line with an extra '\r'.
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line = line.substr(0, line.size() - 1);
        }
        if (!line.empty()) {
            return true;
        }
    }
    return false;
}

// Reads the next line and checks that it starts with keyword.
// Example: for the line "HP -2" and keyword "HP", value becomes "-2".
// Prints an error and returns false if the line is missing or wrong.
bool Adventure::readField(std::ifstream& in, const std::string& keyword, std::string& value) {
    std::string line;
    if (!nextLine(in, line)) {
        std::cout << "Error: expected a " << keyword << " line but the file ended." << std::endl;
        return false;
    }
    std::string prefix = keyword + " ";
    if (line.substr(0, prefix.size()) != prefix) {
        std::cout << "Error: expected a " << keyword << " line but found: " << line << std::endl;
        return false;
    }
    value = line.substr(prefix.size());
    return true;
}

// Adds one choice number to a path.
// extendPath("", 2) gives "2". extendPath("2-1", 3) gives "2-1-3".
std::string Adventure::extendPath(const std::string& path, int choiceNumber) const {
    if (path.empty()) {
        return std::to_string(choiceNumber);
    }
    return path + "-" + std::to_string(choiceNumber);
}

// ===================== LOADING =====================

// TODO 10: open the story file and build the tree.
//   - If the file will not open, print exactly:
//       Error: could not open file <fileName>
//     and return false.
//   - Read the START_HP line and save the number in startHp_.
//     (std::stoi turns "10" into 10.)
//   - Build the whole story with readScene. The first scene's label is "START".
//   - If anything failed, return false. Otherwise give the story to tree_.
bool Adventure::load(const std::string& fileName) {
    std::ifstream in(fileName);

    if (!in.is_open()) { // File will not open
        std::cout << "Error: could not open file " << fileName << std::endl;
        return false;
    }

    std::string value; // Initialize value
    if (!readField(in, "START_HP", value)) // Return false if readField fails
        return false;
    startHp_ = std::stoi(value); // Save starting HP

    Scene<Moment>* root = readScene(in, "START");
    if (root == nullptr) { // Nothing loaded
        return false;
    }

    tree_.setRoot(root); // Give the story to tree
    return true;
}

// TODO 11: read ONE scene and everything below it. Return it.
//   - Read SCENE, HP, LOOT, ITEM, and CHOICES with readField, in that order.
//   - If CHOICES is below 0 or above Scene<Moment>::MAX_CHOICES, print exactly:
//       Error: scene "<scene text>" has <n> choices. The limit is 3.
//     and return nullptr.
//   - Each choice is a LABEL line followed by a whole scene.
//   - If anything goes wrong at any depth, return nullptr
//     and make sure nothing you already built is leaked.
Scene<Moment>* Adventure::readScene(std::ifstream& in, const std::string& label) {
    std::string scene;
    std::string hpString;
    std::string lootString;
    std::string item;
    std::string choicesString;

    // Read in the fields
    if (!readField(in, "SCENE", scene))
        return nullptr;
    if (!readField(in, "HP", hpString))
        return nullptr;
    int hp = std::stoi(hpString);
    if (!readField(in, "LOOT", lootString))
        return nullptr;
    int loot = std::stoi(lootString);
    if (!readField(in, "ITEM", item))
        return nullptr;
    if (!readField(in, "CHOICES", choicesString))
        return nullptr;
    int choices = std::stoi(choicesString);
    if (choices < 0 || choices > Scene<Moment>::MAX_CHOICES) {
        std::cout << "Error: scene \"" << scene << "\" has" << choices << "choices. The limit is 3." << std::endl;
        return nullptr;
    }

    Moment* moment = new Moment(scene, label, hp, loot, item); // Create data of the scene
    Scene<Moment>* current = new Scene<Moment>(moment, choices); // Create the scene

    for (int i = 0; i < choices; i++) { // Build each child scene recursively
        std::string choiceLabel;

        if (!readField(in, "CHOICE", choiceLabel)) {
            delete current;
            return nullptr;
        }

        Scene<Moment>* child = readScene(in, choiceLabel);

        if (child == nullptr) {
            delete current;
            return nullptr;
        }

        current->setChoice(i, child); // Build the array of choices
    }
    return current;
}

// ===================== PLAYING =====================

// TODO 12: ask the player to pick a choice from 1 to numChoices.
//   - Print "Your choice: " (no newline) and read a whole line.
//   - If the line is not a valid number in range, print exactly:
//       Please type a number from 1 to <numChoices>.
//     and ask again.
//   - If the input ends (getline fails), return 0.
int Adventure::askChoice(int numChoices) {
    std::string input;

    while (true) {
        std::cout << "Your choice: ";
        if (!std::getline(std::cin, input)) // Get user input and store in input
            return 0;

        for (int i = 1; i <= numChoices; i++) { // Check input across numChoices
            if (input == std::to_string(i))
                return i;
        }

        std::cout << "Please type a number from 1 to " << numChoices << "." << std::endl; // Handle invalid input
    }
}

// TODO 13: play the story once from the start.
// Every time the player ENTERS a scene, in this order:
//   1. Add its HP change to hp and its loot to loot.
//   2. Print a line of 40 dashes, then the scene text.
//   3. If the scene has an item:
//        if the bag already has it, print "You already have: <item>"
//        otherwise add it to the bag and print "You picked up: <item>"
//   4. Print "HP: <hp>  Loot: <loot>" (two spaces before Loot).
//   5. If hp is 0 or less, print "Your HP hit 0. GAME OVER." and stop.
//      If the scene is an ending, print "THE END." and stop.
//   6. Print each choice as "[1] <label>" and use askChoice.
//      If askChoice returns 0, leave play right away.
// When the game stops, print:
//   Final HP: <hp>  Final loot: <loot>
//   Inventory (<count> items):
// and then print the bag.
// The bag is a List<Item>. Start with HP = startHp_ and loot = 0.
void Adventure::play() {
    int HP = startHp_;
    int loot = 0;
    List<Item> bag;

    Scene<Moment>* current = tree_.getRoot(); // Start at root scene
    while (current != nullptr) {
        Moment* moment = current->getData(); // Get data from scene

        // Add HP change and loot
        HP += moment->getHpChange();
        loot += moment->getLoot();

        // Print dashes and scene text
        std::cout << std::string(40, '-') << std::endl;
        std::cout << moment->getText() << std::endl;

        // Add items
        if (moment->hasItem()) {
            Item item(moment->getItem()); // Create item object

            if (bag.search(item)) { // Item is already in bag
                std::cout << "You already have: " << moment->getItem() << std::endl;
            } else { // Item is not in bag
                bag.addFront(new Item(moment->getItem()));
                std::cout << "You picked up: " << moment->getItem() << std::endl;
            }
        }

        // Print HP and loot
        std::cout << "HP: " << HP << "  Loot: " << loot << std::endl;

        // Endings
        if (HP <= 0) {
            std::cout << "Your HP hit 0. GAME OVER." << std::endl;
            break;
        }

        if (current->isEnding()) {
            std::cout << "THE END." << std::endl;
            break;
        }

        // Display choices and get new choice
        for (int i = 0; i < current->getNumChoices(); i++) {
            Scene<Moment>* child = current->getChoice(i);
            Moment* childMoment = child->getData();
            std::cout << "[" << i + 1 << "] " << childMoment->getLabel() << std::endl;
        }

        int choice = askChoice(current->getNumChoices());
        if (choice == 0)
            return;

        current = current->getChoice(choice - 1); // Set new chosen scene
    }

    // Print ending stats
    std::cout << "Final HP: " << HP << "  Final loot: " << loot << std::endl;
    std::cout << "Inventory (" << bag.size() << " items):" << std::endl;
    bag.print();
}

// ===================== ANALYZING =====================

// TODO 14: print exactly three lines:
//   Scenes: <count>
//   Endings: <count>
//   Longest playthrough: <height> choices
void Adventure::printStats() const {
    std::cout << "Scenes: " << tree_.countScenes() << std::endl;
    std::cout << "Endings: " << tree_.countEndings() << std::endl;
    std::cout << "Longest playthrough: " << tree_.height() << " choices" << std::endl;
}

// TODO 15: print every ending in the story, one per line.
void Adventure::printSpoilers() const {
    printEndings(tree_.getRoot(), "");
}

// TODO 16: print every ending below scene.
// path holds the choice numbers taken to reach scene, like "2-1".
// Each ending prints as:
//   Path <path>: <scene text>
// If the very first scene is an ending, print "Path (start): <scene text>".
// Endings print in order: choice 1 before choice 2 before choice 3.
void Adventure::printEndings(Scene<Moment>* scene, const std::string& path) const {
    if (scene == nullptr) // No story
        return;

    if (scene->isEnding()) {
        std::cout << "Path ";

        if (path.empty()) { // First scene is an ending
            std::cout << "(start)";
        } else { // Print path
            std::cout << path;
        }

        std::cout << ": " << scene->getData()->getText() << std::endl; // Print scene text
        return;
    }

    for (int i = 0; i < scene->getNumChoices(); i++) { // Traverse down the tree in order
        printEndings(scene->getChoice(i), extendPath(path, i + 1));
    }
}

// TODO 17: the greedy advisor.
// Start at the first scene with startHp_ and 0 loot.
// Enter scenes the same way play does (add HP and loot).
// Stop when hp is 0 or less, or you reach an ending.
// Otherwise always move to the choice with the MOST loot right now.
// On a tie, pick the lower choice number.
// Print:
//   Greedy advisor
//     Path: <path>
//     Loot: <loot>
//     Result: died               (or "Result: reached an ending")
void Adventure::greedyRun() const {
    int HP = startHp_;
    int loot = 0;
    std::string path;

    Scene<Moment>* current = tree_.getRoot();
    while (true) {
        HP += current->getData()->getHpChange();
        loot += current->getData()->getLoot();

        // HP is 0 or reach an ending
        if (HP <= 0 || current->isEnding())
            break;

        // Assume the first choice is the best for greedy
        int bestChoice = 0;
        int mostLoot = current->getChoice(0)->getData()->getLoot();

        // Compare with the other choices
        for (int i = 1; i < current->getNumChoices(); i++) {
            int childLoot = current->getChoice(i)->getData()->getLoot();

            if (childLoot > mostLoot) { // Save best loot and index
                mostLoot = childLoot;
                bestChoice = i;
            }
        }

        path = extendPath(path, bestChoice + 1); // Update path
        current = current->getChoice(bestChoice); // Move to best choice
    }

    // Printing
    std::cout << "Greedy advisor" << std::endl;
    std::cout << "  Path: " << (path.empty() ? "(start)" : path) << std::endl;
    std::cout << "  Loot: " << loot << std::endl;
    if (HP <= 0) {
        std::cout << "  Result: died" << std::endl;
    } else {
        std::cout << "  Result: reached an ending" << std::endl;
    }
}

// TODO 18: the best total loot you can collect from scene down to an ending,
// arriving at scene with hp health and having taken path to get here.
//   - Return -1 if every way down from here kills you.
//   - Put the path of the best run into bestPath.
//   - Must be recursive and look at every choice.
int Adventure::bestLoot(Scene<Moment>* scene, int hp, const std::string& path, std::string& bestPath) const {
    Moment* moment = scene->getData();
    hp += moment->getHpChange();

    if (hp <= 0) // Dead
        return -1;

    if (scene->isEnding()) { // Alive
        bestPath = path;
        return moment->getLoot();
    }

    int mostLoot = -1;
    for (int i = 0; i < scene->getNumChoices(); i++) {
        std::string childBestPath;

        int childLoot = bestLoot(scene->getChoice(i), hp, extendPath(path, i + 1), childBestPath);
        if (childLoot > mostLoot) {
            mostLoot = childLoot;
            bestPath = childBestPath;
        }
    }

    if (mostLoot == -1) // No child survived
        return -1;

    return moment->getLoot() + mostLoot; // Original loot + the loot down the path
}

// TODO 19: the optimal advisor. Use bestLoot from the first scene.
// Print:
//   Optimal advisor
//     Path: <path>
//     Loot: <loot>
//     Result: reached an ending
// If no path survives, print only:
//   Optimal advisor
//     Result: no path survives this story
void Adventure::optimalRun() const {
    std::string bestPath;
    int loot = bestLoot(tree_.getRoot(), startHp_, "", bestPath);

    std::cout << "Optimal advisor" << std::endl;
    if (loot != -1) {
        std::cout << "  Path: " << (bestPath.empty() ? "(start)" : bestPath) << std::endl;
        std::cout << "  Loot: " << loot << std::endl;
        std::cout << "  Result: reached an ending" << std::endl;
    } else {
        std::cout << "  Result: no path survives this story" << std::endl;
    }
}
