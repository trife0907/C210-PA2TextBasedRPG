### 2. For every function you wrote (TODO 1 to 19), explain in your own words what it does and give its time complexity in terms of n, the number of scenes. One short paragraph each.
TODO 1: Constructs the scene object. Fills it with the data and allocates and assigns its array of children O(1) because
only dependent on the numChoices, which is max 3.

TODO 2: Destroys each scene and everything it owns, including its children scenes, which takes place recursively, so O(n).

TODO 3: Checks if the current scene is an ending, O(1).

TODO 4: Returns the scene pointer in the valid slot index, which is a child scene of the current scene. O(1).

TODO 5: Inserts a child scene into the array of choices, assuming the index is valid. O(1).

TODO 6: Keeps track of a counter and recursively goes down the tree to count the number of scenes by accessing child scenes. 
O(n).

TODO 7: Similar to TODO 6, but this time only keeps a tracker of the number of endings. Still O(n) because it must reach
the leaf scenes to find endings, which means traversing through the entire tree.

TODO 8: Finds the height of the tree, so it recursively searches the tree for the longest path. O(n).

TODO 9: Deletes the entire story and tree. Running this calls the destructor from the root scene, so this takes O(n).

TODO 10: Reads in the file, handles errors, and if everything succeeds, it loads the entire tree starting from the root,
so O(n).

TODO 11: Reads in the entire scene's data and creates the `Moment` and `Scene` objects. Does this recursively, O(n).

TODO 12: Reads in the user's input when selecting a new scene. Time complexity does not depend on number of scenes, so O(1).

TODO 13: Plays through the entire game and traverses the tree, picking up items and HP changes across the way. O(n^2)  because
we may search the bag for a worst-case of O(n) times on each scene we get to.

TODO 14: Prints the stats of the story, O(n) because it calls `countScenes()` and `countEndings()` on each run.

TODO 15: Prints the spoiler paths and endings of the story. Does this by calling `printEndings()`, so O(n).

TODO 16: Traverses the tree to every ending and records the path along the way, and prints out the results. O(n).

TODO 17: Takes the path that gives you the most loot always, no matter the HP cost. Because it traverses to an ending, worst case
is O(n).

TODO 18: Returns the best amount of loot possible from an ending where you stay alive. Because it traverses the tree to an ending,
O(n).

TODO 19: Gives the path of the optimal run, and calls `bestLoot()`, meaning this takes O(n).

### 2. Why does each scene allocate exactly numChoices slots instead of declaring Scene<T>* choices[3]? What does an ending hold in choices_, and why?
Each scene allocates exactly numChoices slots because these slots aren't always guaranteed to be 3 choices, only max allowed is 3.
Some scenes can have 2 choices or 1. An ending holds 0 in `choices_` because it is an ending, and you cannot progress further than that.

### 3. Walk through your Scene destructor. What gets deleted, and in what order? What would go wrong if choices_ were deleted first?
The Scene destructor deletes the slots in the array of choices first, then it deletes the array itself and the data. If we deleted
the array first, then we would no longer have access to the child scenes inside the array, and we would lose access to those, causing memory
leaks.

### 4. Show the path greedy took and the path optimal took on your story, with the loot for each. Explain why greedy lost. If greedy did not lose, change your story until it does, and explain what you changed.
Greedy took the path, 1-1 with 18 loot, and optimal took the path 2-2-1-1-1 with 11 loot. Greedy lost because it went
for gun skins only instead of trying to win the round, and got killed.

### 5. bestLoot tries every path, yet it runs in O(n) time. Explain why it is not exponential, using what you know about paths and leaves in a tree.
`bestLoot()` is not exponential because it only traverses through the tree once. It does not need to revisit any scenes, and it
only keeps track of the path with the best loot, so its O(n).

### 6. Which List methods does your game use, and why? What does one search cost as the bag grows?
My game uses `addFront()` to add to the list, `search()` to search for a specific item, `size()` to check the number of items, and `print()` to
display the current bag. One search will cost O(n) with n being the number of items, as the bag grows.