#pragma once
#include "Scene.h"

// The whole story. It knows the first scene and nothing else.
//
// RULES (do not change):
//   - Keep every name and signature exactly as written.
//   - The private helpers must be recursive. No loops that walk down the tree.
//   - This class must work for ANY T. Never mention Moment in here.
template <typename T>
class StoryTree {
private:
    Scene<T>* root_;

    // TODO 6: how many scenes are in the story that starts at scene?
    // An empty story has 0 scenes.
    int countScenes(Scene<T>* scene) const {
        if (scene == nullptr) // Scene tree is empty
            return 0;

        int count = 1; // Start at 1 to include current scene
        for (int i = 0; i < scene->getNumChoices(); i++)
            count += countScenes(scene->getChoice(i)); // Add recursive call to count

        return count;
    }

    // TODO 7: how many endings are in the story that starts at scene?
    int countEndings(Scene<T>* scene) const {
        if (scene == nullptr) // Scene tree is empty
            return 0;

        int count = 0; // Count starts at 0 endings
        if (scene->isEnding()) {
            count++;
        } else {
            for (int i = 0; i < scene->getNumChoices(); i++)
                count += countEndings(scene->getChoice(i));
        }
        return count;
    }

    // TODO 8: the most choices a player can make on the way to an ending,
    // starting at scene. A single ending scene has height 0.
    // An empty story has height -1.
    int height(Scene<T>* scene) const {
        if (scene == nullptr) // Empty story
            return -1;

        if (scene->isEnding()) { // Single scene
            return 0;
        }

        int maxHeight = -1; // Initial value
        for (int i = 0; i < scene->getNumChoices(); i++) {
            int height = height(scene->getChoice(i)); // Recursive case

            if (height > maxHeight) // Keep only the greatest height so far
                maxHeight = height;
        }

        return maxHeight + 1; // +1 to account for the initial -1
    }

public:
    StoryTree() : root_(nullptr) {}

    ~StoryTree() {
        clear();
    }

    void setRoot(Scene<T>* root) {
        clear();
        root_ = root;
    }

    Scene<T>* getRoot() const {
        return root_;
    }

    bool isEmpty() const {
        return root_ == nullptr;
    }

    // TODO 9: free every scene in the story and leave the tree empty.
    // Hint: what does deleting a Scene already do for you?
    void clear() {
        delete root_;
        root_ = nullptr;
    }

    int countScenes() const {
        return countScenes(root_);
    }

    int countEndings() const {
        return countEndings(root_);
    }

    int height() const {
        return height(root_);
    }
};
