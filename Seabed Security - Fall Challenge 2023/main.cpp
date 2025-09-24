#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>

using namespace std;

struct Point
{
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Creature
{
    int id;
    int color;
    int type;
    Point pos;
    Point velocity;
};

struct Drone
{
    int id;
    Point pos;
    bool emergency;
    int battery;
};

class Game
{
public:
    int my_score;
    int foe_score;
    map<int, Creature> creatures;
    set<int> my_scans;
    set<int> foe_scans;
    vector<Drone> my_drones;
    vector<Drone> foe_drones;
    map<int, set<int>> drone_scans;
    map<int, Point> visible_creatures;
    map<pair<int, int>, string> radar_blips;

    // Приоритеты типов рыб (чем выше значение, тем выше приоритет)
    map<int, int> type_priority = {{0, 1}, {1, 2}, {2, 3}};

    void clear()
    {
        my_scans.clear();
        foe_scans.clear();
        drone_scans.clear();
        visible_creatures.clear();
        radar_blips.clear();
        my_drones.clear();
        foe_drones.clear();
    }
};

double distance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool is_valid_point(int x, int y)
{
    return x >= 0 && x <= 10000 && y >= 0 && y <= 10000;
}

Point calculate_move_towards(const Point &from, const Point &to, int max_distance)
{
    double dist = distance(from.x, from.y, to.x, to.y);
    if (dist <= max_distance)
    {
        return to;
    }

    double ratio = max_distance / dist;
    int new_x = from.x + (to.x - from.x) * ratio;
    int new_y = from.y + (to.y - from.y) * ratio;

    // Ensure we stay within bounds
    new_x = max(0, min(10000, new_x));
    new_y = max(0, min(10000, new_y));

    return Point(new_x, new_y);
}

int main()
{
    Game game;

    int creature_count;
    cin >> creature_count;
    cin.ignore();

    // Read all creatures
    for (int i = 0; i < creature_count; i++)
    {
        int creature_id, color, type;
        cin >> creature_id >> color >> type;
        cin.ignore();
        game.creatures[creature_id] = {creature_id, color, type};
    }

    // Game loop
    while (1)
    {
        game.clear();

        cin >> game.my_score;
        cin.ignore();
        cin >> game.foe_score;
        cin.ignore();

        // My scans
        int my_scan_count;
        cin >> my_scan_count;
        cin.ignore();
        for (int i = 0; i < my_scan_count; i++)
        {
            int creature_id;
            cin >> creature_id;
            cin.ignore();
            game.my_scans.insert(creature_id);
        }

        // Foe scans
        int foe_scan_count;
        cin >> foe_scan_count;
        cin.ignore();
        for (int i = 0; i < foe_scan_count; i++)
        {
            int creature_id;
            cin >> creature_id;
            cin.ignore();
            game.foe_scans.insert(creature_id);
        }

        // My drones
        int my_drone_count;
        cin >> my_drone_count;
        cin.ignore();
        for (int i = 0; i < my_drone_count; i++)
        {
            Drone drone;
            cin >> drone.id >> drone.pos.x >> drone.pos.y >> drone.emergency >> drone.battery;
            cin.ignore();
            game.my_drones.push_back(drone);
        }

        // Foe drones
        int foe_drone_count;
        cin >> foe_drone_count;
        cin.ignore();
        for (int i = 0; i < foe_drone_count; i++)
        {
            Drone drone;
            cin >> drone.id >> drone.pos.x >> drone.pos.y >> drone.emergency >> drone.battery;
            cin.ignore();
            game.foe_drones.push_back(drone);
        }

        // Drone scans
        int drone_scan_count;
        cin >> drone_scan_count;
        cin.ignore();
        for (int i = 0; i < drone_scan_count; i++)
        {
            int drone_id, creature_id;
            cin >> drone_id >> creature_id;
            cin.ignore();
            game.drone_scans[drone_id].insert(creature_id);
        }

        // Visible creatures
        int visible_creature_count;
        cin >> visible_creature_count;
        cin.ignore();
        for (int i = 0; i < visible_creature_count; i++)
        {
            int creature_id, x, y, vx, vy;
            cin >> creature_id >> x >> y >> vx >> vy;
            cin.ignore();
            game.visible_creatures[creature_id] = Point(x, y);
            if (game.creatures.find(creature_id) != game.creatures.end())
            {
                game.creatures[creature_id].pos = Point(x, y);
                game.creatures[creature_id].velocity = Point(vx, vy);
            }
        }

        // Radar blips
        int radar_blip_count;
        cin >> radar_blip_count;
        cin.ignore();
        for (int i = 0; i < radar_blip_count; i++)
        {
            int drone_id, creature_id;
            string radar;
            cin >> drone_id >> creature_id >> radar;
            cin.ignore();
            game.radar_blips[{drone_id, creature_id}] = radar;
        }

        // For each of my drones
        for (const Drone &drone : game.my_drones)
        {
            // Emergency: move to surface
            if (drone.emergency)
            {
                cout << "MOVE " << drone.pos.x << " 0 0" << endl;
                continue;
            }

            // Combine scans already saved and carried by this drone
            set<int> already_scanned = game.my_scans;
            if (game.drone_scans.find(drone.id) != game.drone_scans.end())
            {
                already_scanned.insert(game.drone_scans[drone.id].begin(),
                                       game.drone_scans[drone.id].end());
            }

            // Find best target among visible creatures
            int best_target_id = -1;
            double best_score = -1;
            double best_distance = 100000;

            for (const auto &vc : game.visible_creatures)
            {
                int creature_id = vc.first;
                Point creature_pos = vc.second;

                // Skip if already scanned
                if (already_scanned.find(creature_id) != already_scanned.end())
                {
                    continue;
                }

                // Calculate score based on type and distance
                double dist = distance(drone.pos.x, drone.pos.y, creature_pos.x, creature_pos.y);
                int type = game.creatures[creature_id].type;
                double score = game.type_priority[type] * 1000 / (dist + 1);

                if (score > best_score || (score == best_score && dist < best_distance))
                {
                    best_score = score;
                    best_distance = dist;
                    best_target_id = creature_id;
                }
            }

            Point target_point;
            bool use_light = false;

            if (best_target_id != -1)
            {
                // Move towards visible creature
                target_point = game.visible_creatures[best_target_id];

                // Use light if battery is sufficient and creature is not too close
                if (drone.battery > 10 && best_distance > 500)
                {
                    use_light = true;
                }
            }
            else
            {
                // No visible targets, use strategic movement
                if (drone.pos.y < 2500)
                {
                    // We're high up, explore horizontally
                    if (drone.pos.x < 5000)
                    {
                        target_point = Point(min(10000, drone.pos.x + 2000), drone.pos.y + 1000);
                    }
                    else
                    {
                        target_point = Point(max(0, drone.pos.x - 2000), drone.pos.y + 1000);
                    }
                }
                else
                {
                    // We're deep, move towards surface while exploring
                    target_point = Point(drone.pos.x, max(0, drone.pos.y - 1000));
                }
            }

            // Ensure target is valid and within movement limits
            Point move_to = calculate_move_towards(drone.pos, target_point, 600);

            // Avoid getting stuck at boundaries
            if (move_to.x <= 100)
                move_to.x = 1000;
            if (move_to.x >= 9900)
                move_to.x = 9000;
            if (move_to.y <= 100)
                move_to.y = 1000;
            if (move_to.y >= 9900)
                move_to.y = 9000;

            // Output command
            cout << "MOVE " << move_to.x << " " << move_to.y << " " << (use_light ? 1 : 0) << endl;
        }
    }
}