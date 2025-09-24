#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * This code automatically collects game data in an infinite loop.
 * It uses the standard input to place data into the game variables such as x and y.
 * YOU DO NOT NEED TO MODIFY THE INITIALIZATION OF THE GAME VARIABLES.
 **/

int main()
{

    // game loop
    while (1)
    {
        int x;                 // x position of your pod
        int y;                 // y position of your pod
        int next_checkpoint_x; // x position of the next check point
        int next_checkpoint_y; // y position of the next check point
        cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y;
        cin.ignore();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        // Edit this line to output the target position
        // and thrust (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        cout << next_checkpoint_x << " " << next_checkpoint_y << " 100" << endl;
    }
}