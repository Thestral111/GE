#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header
#include <fstream>
#include <sstream>


using namespace std;

const int canvasHeight = 768;
const int canvasWidth = 1024;

int score = 0;

// score increases after eliminating any enemy
void scoreInc() {
    score++;
}



// Entity class includes all the characters and projectile.
class entity {

public:
    int health = 10;
    GamesEngineeringBase::Image sprite;
    int x;
    int y;
    void update(int inc, int yinc) {
        x += inc;
        y += yinc;
    }
    entity() {

    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (unsigned int i = 0; i < sprite.height; i++) {
            // bounds checking goes here
            if (y + i > 0 && (y + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < sprite.width; n++) {
                    if (x + n > 0 && (x + n) < (canvas.getWidth()))
                        if (sprite.alphaAtUnchecked(n, i) > 10)
                            canvas.draw(x + n, y + i, sprite.atUnchecked(n, i));

                }
            }

        }

    }

    // check collision
    bool collide(entity& p) {
        int dx = x - p.x;
        int dy = y - p.y;

        float radius = static_cast<float>(sprite.width / 3);
        float d = sqrtf(dx * dx + dy * dy); // can do without sqrtf also
        return d < 2.f * radius;
    }

    bool collide(int px, int py) {
        int dx = x - px;
        int dy = y - py;

        float radius = static_cast<float>(sprite.width / 3) + 2.f;
        float d = sqrtf(dx * dx + dy * dy); // can do without sqrtf also
        return d < 2.f * radius;
    }

    // damage after collision
    void takeDamage() {
        health -= 1;
        //cout << "taking damage\n";
        if (health <= 0) {
            cout << "dead\n";
            scoreInc();
        }
    }

    int getHealth() {
        return health;
    }

};

// hero class is the player
class hero : public entity {
    GamesEngineeringBase::Image sprite;
    int x;
    int y;
    int worldX, worldY;
    float elapsedTime = 0.f;
    float timeThreshold = 1.0f;
    //int health;
public:
    void update(int inc, int yinc) {
        x += inc;
        y += yinc;
    }
    hero(int _x, int _y) {
        sprite.load("Resources/mage2.png");
        x = _x - sprite.width / 2; // centre
        y = _y - sprite.height / 2;
        health = 3;
    }
    void updateWorldPos(int px, int py, float dt) {
        worldX = px;
        worldY = py;
        elapsedTime += dt;
    }
    void takeDamage() {
        if (elapsedTime > timeThreshold) {
            health--;
            cout << "hero damage\n";
            elapsedTime = 0.f;
            if (health <= 0) {
                cout << "hero dead\n";
            }
        }
        
    }

    

    void draw(GamesEngineeringBase::Window& canvas) {
        //unsigned int y = (canvas.getHeight()) - (sprite.height); // fixed height
        for (unsigned int i = 0; i < sprite.height; i++)
        {
            for (unsigned int n = 0; n < sprite.width; n++)
            {
                
                if ((x + n) >= 0 && (x + n) < canvas.getWidth()) // y is not needed to be checked because it is constant - in theory we never go out of bounds to check x either
                {
                    if (sprite.alphaAtUnchecked(n, i) > 210)
                    {
                        canvas.draw(x + n, y + i, sprite.atUnchecked(n, i));
                    }
                }
            }
        }
        unsigned char red[3] = { 255,0,0 };
        canvas.draw(canvas.getWidth() / 2, canvas.getHeight() / 2, red); // draw red pixel in centre
    }
    int getX() { return x; }
    unsigned int getHeight() { return sprite.height; }
    unsigned int getWidth() { return sprite.width; }

};

const int emaxsize = 200;

// enemy class manages the four types of NPCs
class enemy : public entity{
    
public:
    float speed = 200.f;
    
    int move;
    float elapsedTime = 0.f;
    float timeThreshold = 1.0f;

    enemy(int cx, int cy, int mapNo) {
        // enemy is initialised randomly among four types
        int enemyNo = rand() % 4 + 1;
        switch (enemyNo)
        {
        case 1:
            // skeleton
            sprite.load("Resources/skeleton.png");
            health = 2;
            speed = 150.f;
            break;
        case 2:
            // spirit
            sprite.load("Resources/spirit.png");
            health = 1;
            speed = 180.f;
            break;
        case 3:
            // slime
            sprite.load("Resources/slimeSmall.png");
            health = 2;
            speed = 100.f;
            break;
        case 4:
            // bat
            sprite.load("Resources/bat.png");
            health = 1;
            speed = 220.f;
            break;
        default:
            sprite.load("Resources/skeleton.png");
            health = 2;
            speed = 150.f;
            break;
        }

        // randomly generated outside camera view
        // mapNo indicates finite or infinite map which leads to different position
        do {
            if (mapNo == 1) {
                x = rand() % 2688 - 1344;
                y = rand() % 2688 - 1344;
            }
            else {
                x = rand() % 2688 - 1344 + cx;
                y = rand() % 2688 - 1344 + cy;
            }
            
        } while ((x>cx && x<cx+canvasWidth) && (y>cy && y<cy+canvasHeight));
        
        //cout << "pos: " << x << " " << y << endl;
    }

    void update(GamesEngineeringBase::Window& canvas, float dt, int px, int py, hero& h) {
        // towards player
        int xOffset = px - x;
        int yOffset = py - y;
        float dist = sqrt(xOffset * xOffset + yOffset * yOffset);
        
        move = static_cast<int>((speed * dt));

        // collision between hero and enemy
        // leading to a damage to hero health (has cool down to avoid hero die in one sec)
        elapsedTime += dt;
        if (collide(px, py)) {
            if (elapsedTime > timeThreshold) {
                h.takeDamage();
            }

        }
        else {
            x += xOffset / dist * move;
            y += yOffset / dist * move;
        }
        

    }

    

    void draw(GamesEngineeringBase::Window& canvas, int _x, int _y) {
        // offset to let enemy stay on the map
        _x += x;
        _y += y;
        _x -= (sprite.width / 2);
        _y -= (sprite.height / 2);
        for (unsigned int i = 0; i < sprite.height; i++) {
            // bounds checking goes here
            if (_y + i > 0 && (_y + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < sprite.width; n++) {
                    if (_x + n > 0 && (_x + n) < (canvas.getWidth())) {
                        if (sprite.alphaAtUnchecked(n, i) > 210)
                            canvas.draw(_x + n, _y + i, sprite.atUnchecked(n, i));
                    }
                }
            }

        }
    }

    
    
};

// Use an array of enemy pointer to manage all the enemies
class enemyList {
public:
    enemy* sarray[emaxsize];
    int currentSize = 0;
    float timeElapsed = 0.f;
    float totalTime = 0.f;
    float timeThreshold = 2.f;
    
    enemyList() {

    }

    void generateEnemy(GamesEngineeringBase::Window& canvas, float dt, int cx, int cy, int mapNo) {
        // frequency increase over time
        timeElapsed += dt;
        totalTime += dt;
        if (totalTime <= 30.f) {

        }
        else if (totalTime >= 30.f && totalTime <= 120.f) {
            timeThreshold = 1.5f;
        }
        else {
            timeThreshold = 1.0f;
        }

        if (currentSize < emaxsize) {
            if (timeElapsed > timeThreshold) { // create new enemy
                enemy* p = new enemy(cx, cy, mapNo);
                //  cout << "Created: " << currentSize << '\t' << timeThreshold << '\t' << dt << endl;
                //cout << "Created: " << currentSize << endl;

                sarray[currentSize++] = p;
                timeElapsed = 0.f;
                
            }
        }
    }

    void update(GamesEngineeringBase::Window& canvas, float dt, int cx, int cy, hero& h, int mapNo) {
        generateEnemy(canvas, dt, cx, cy, mapNo);
        int playerX = cx + canvas.getWidth() / 2;
        int playerY = cy + canvas.getHeight() / 2;
        // update all enemies towards player
        for (unsigned int i = 0; i < currentSize; i++) {
            if (sarray[i] != nullptr) {
                sarray[i]->update(canvas, dt, playerX, playerY, h);
                // check for dead enemy
                if (sarray[i]->health <= 0) {
                    enemy* temp = sarray[i];
                    sarray[i] = nullptr;
                    delete temp;
                }
            }
        }
        
        

    }

    // AOE attack of hero goes here
    void takeAOE() {
        // max 5 enemy
        entity* topN[5];
        int count = 0;
        // find the top 5 health enemy
        for (unsigned int i = 0; i < currentSize; i++) {
            if (sarray[i] != nullptr) {
                if (count < 5) {
                    topN[count] = sarray[i];
                    count++;
                }
                else {
                    for (unsigned int j = 0; j < 5; j++) {
                        if (topN[j] != nullptr) {
                            if ((sarray[i]->health) > (topN[j]->health)) {
                                topN[j] = sarray[i];
                                break;
                            }
                        }
                        
                    }
                    
                }
            }
        }
        cout << "count = " << count << endl;
        // deal 2 point of damage to them
        for (unsigned int i = 0; i < count; i++) {
            if (topN[i] != nullptr)
                topN[i]->takeDamage();
                topN[i]->takeDamage();
        }
        cout << "aoe\n";
    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (unsigned int i = 0; i < currentSize; i++)
        {
            if (sarray[i] != nullptr)
                sarray[i]->draw(canvas, x, y);
        }
    }

};

// projectiles shoot by hero
class projectile : public entity {
public:
    int x = 500;
    int y = 500;
    int ex = 0;
    int ey = 0;
    entity *e;

    projectile(int px, int py, enemyList el) {
        sprite.load("Resources/round_bullet_sprite.png");
        x = px;
        y = py;
        // find nearest enemy
        int minDist = 10000;
        for (unsigned int i = 0; i < el.currentSize; i++) {
            if (el.sarray[i] != nullptr) {
                int currentX, currentY;
                int currentDist;
                currentX = el.sarray[i]->x;
                currentY = el.sarray[i]->y;
                int xOffset = currentX - x;
                int yOffset = currentY - y;
                currentDist = sqrt(xOffset * xOffset + yOffset * yOffset);
                if (currentDist < minDist) {
                    minDist = currentDist;
                    ex = currentX;
                    ey = currentY;
                    e = (el.sarray[i]);

                }
            }
        }
    }

    
    // overwrite collide() to take pointer as parameter
    bool collide(entity* entity) {
        if (entity == nullptr) {
            return true;
        }
        int dx = x - entity->x;
        int dy = y - entity->y;

        float radius = 20.f;
        float d = sqrtf(dx * dx + dy * dy); // can do without sqrtf also
        return d < 2.f * radius;
    }
    
    // projectile move towards enemy 
    void update(GamesEngineeringBase::Window& canvas, float dt) {
        if (e != nullptr) {
            int xOffset = e->x - x;
            int yOffset = e->y - y;
            float dist = sqrt(xOffset * xOffset + yOffset * yOffset);


            int move = static_cast<int>((600.f * dt));
            if (!collide(e)) {

                x += xOffset / dist * move;
                y += yOffset / dist * move;
            }
        }
        
        
    }

    void draw(GamesEngineeringBase::Window& canvas, int _x, int _y) {
        _x += x;
        _y += y;
        _x -= (sprite.width / 2);
        _y -= (sprite.height / 2);
        for (unsigned int i = 0; i < sprite.height; i++) {
            // bounds checking goes here
            if (_y + i > 0 && (_y + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < sprite.width; n++) {
                    if (_x + n > 0 && (_x + n) < (canvas.getWidth())) {
                        if (sprite.alphaAtUnchecked(n, i) > 210)
                            canvas.draw(_x + n, _y + i, sprite.atUnchecked(n, i));
                    }
                }
            }

        }
    }
};

// tile based method to load map
class tile {
    GamesEngineeringBase::Image sprite;
public:
    tile() {}
    void load(string filename) {
        sprite.load(filename);
    }
    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (unsigned int i = 0; i < sprite.height; i++) {
            // bounds checking goes here
            if (y + i > 0 && (y + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < sprite.width; n++) {
                    if (x + n > 0 && (x + n) < (canvas.getWidth()))
                        canvas.draw(x + n, y + i, sprite.atUnchecked(n, i));

                }
            }

        }
    
    }
    unsigned int getHeight() { return sprite.height; }
    unsigned int getWidth() { return sprite.width; }
    GamesEngineeringBase::Image& getSprite() { return sprite; }
};


// use a 2d array to organize the tiles to form the map
class world1 {
public:
    tile tiles[42][42];
    world1(string filename) {
        ifstream infile(filename);
        int tilenum;
        string map[42][42];
        string line;
        string tilename;
        
        int i = 0;
        // read line from file
        while (getline(infile, line)) {
            stringstream ss(line);
            string number;
            int j = 0;
            
            // use sstream to handle the string and convert into array
            while (getline(ss, number, ',')) {
                //cout << "j = " << j << endl;
                map[i][j] = number;
                //cout << map[i][j] << " ";
                tilename = "Resources/" + number + ".png";
                tiles[i][j].load(tilename);
                j++;
                
            }
            i++;
        }
        //cout << map;
        

    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (int i = 0; i < 42; i++) {
            for (int j = 0; j < 42; j++) {
                tiles[i][j].draw(canvas, (0 + x + j * 32), (0 + y + i * 32));
            }
        }
    }
};

// the virtual camera area with hero in the center
// camera class manages all the things generated and drawed inside the camera view.
class camera {
public:
    GamesEngineeringBase::Image spriteOver;
    //GamesEngineeringBase::Image sprite1;
    int leftbound = 1856;
    int rightbound = -832;
    int upperbound = 1728;
    int lowerbound = -960;
    
    int x = 0;
    int y = 0;
    int px = 0;
    int py = 0;
    GamesEngineeringBase::Timer timer;
    //float dt;
    int move;
    //hero h;
    projectile* parray[emaxsize];
    float timeElapsed = 0.f;
    float timeThreshold = 2.5f;
    int currentSize = 0;
    

    camera() {
        
    }

    // calculate the player position inside the map
    void getPlayerPos(GamesEngineeringBase::Window& canvas, hero& h) {
        px = -x + canvas.getWidth() / 2;
        py = -y + canvas.getHeight() / 2;
    }

    // update projectiles emitted from the center of view (player)
    void updateProjectile(GamesEngineeringBase::Window& canvas, float dt, enemyList& el) {
        // emit projectile every 2.5s
        timeElapsed += dt;
        if (currentSize < emaxsize) {
            if (timeElapsed > timeThreshold) { 
                projectile* p = new projectile(px, py, el);
                //  cout << "Created: " << currentSize << '\t' << timeThreshold << '\t' << dt << endl;
                //cout << " projectile created: " << currentSize << endl;

                parray[currentSize++] = p;
                timeElapsed = 0.f;
                //timeThreshold -= 0.2f;
                //timeThreshold = max(0.2f, timeThreshold);
            }
        }

        // deal damage to enemy uppon collision
        // enemy dies when health to 0
        for (int i = 0; i < currentSize; i++) {
            if (parray[i] != nullptr) {
                parray[i]->update(canvas, dt);
                if (parray[i]->collide(parray[i]->e)) {
                    if (parray[i]->e != nullptr)
                        parray[i]->e->takeDamage();
                    entity* p = parray[i];
                    parray[i] = nullptr;
                    delete p;
                }
            }

        }
    }

    // update camera
    void updatePos(GamesEngineeringBase::Window& canvas, float dt, enemyList& el, hero& h) {
        
        getPlayerPos(canvas, h);
        h.updateWorldPos(px, py, dt);
        move = static_cast<int>((500.f * dt));
        
        // x, y of camera is used to draw the position of map
        if (canvas.keyPressed('W')) y += move;
        if (canvas.keyPressed('S')) y -= move;
        if (canvas.keyPressed('A')) x += move;
        if (canvas.keyPressed('D')) x -= move;
         
        // bound checking
        // mapSize + player offset (half canvas size)
        if (x > 1856) x = 1856;
        if (x < -832) x = -832;
        if (y > 1728) y = 1728;
        if (y < -960) y = -960;
        //cout << "x = " << x << " ";
        //cout << "y = " << y << endl;
        updateProjectile(canvas, dt, el);
        
    }

    // infinite map version of camera, whithout bound checking
    void updateInfinite(GamesEngineeringBase::Window& canvas, float dt, enemyList& el, hero& h) {
        /*if (canvas.keyPressed(VK_ESCAPE)) break;*/
        //dt = timer.dt();
        getPlayerPos(canvas, h);
        h.updateWorldPos(px, py, dt);
        move = static_cast<int>((500.f * dt));
        //cout << "move = " << move << endl;

        if (canvas.keyPressed('W')) y += move;
        if (canvas.keyPressed('S')) y -= move;
        if (canvas.keyPressed('A')) x += move;
        if (canvas.keyPressed('D')) x -= move;

        // no bound checking
        
        updateProjectile(canvas, dt, el);

    }

    
    // draw all the things including map and characters
    void draw(GamesEngineeringBase::Window& canvas, world1& w1, hero& h, enemyList& el) {
        // form a larger map
        w1.draw(canvas, x, y);
        w1.draw(canvas, x - 1344, y); // splicing larger map 
        w1.draw(canvas, x, y - 1344);
        w1.draw(canvas, x - 1344, y - 1344);
        h.draw(canvas);
        el.draw(canvas, x, y);
        for (int i = 0; i < currentSize; i++) {
            if (parray[i] != nullptr)
                parray[i]->draw(canvas, x, y);
        }
    }

    // infinite version of draw
    // always draw the 4 pieces of map around player, so map is infinite
    void drawInfinite(GamesEngineeringBase::Window& canvas, world1& w1, hero& h, enemyList& el) {
        int xOffset = round((float)px / 1344.f);
        int yOffset = round((float)py / 1344.f);
        //cout << "xoffset = " << xOffset << endl;
        w1.draw(canvas, x + xOffset * 1344, y + yOffset * 1344);
        w1.draw(canvas, x - 1344 + xOffset * 1344, y + yOffset * 1344); // splicing larger map 
        w1.draw(canvas, x + xOffset * 1344, y - 1344 + yOffset * 1344);
        w1.draw(canvas, x - 1344 + xOffset * 1344, y - 1344 + yOffset * 1344);
        h.draw(canvas);
        el.draw(canvas, x, y);
        for (int i = 0; i < currentSize; i++) {
            if (parray[i] != nullptr)
                parray[i]->draw(canvas, x, y);
        }
    }

    
    // game over screen when hero dead
    void drawGameOver(GamesEngineeringBase::Window& canvas) {
        spriteOver.load("Resources/game_over_sprite.png");
        int centerX = canvas.getWidth() / 2 - spriteOver.width/2;
        int centerY = canvas.getHeight() / 2 - spriteOver.height/2;
        for (unsigned int i = 0; i < spriteOver.height; i++) {
            // bounds checking goes here
            if (centerY + i > 0 && (centerY + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < spriteOver.width; n++) {
                    if (centerX + n > 0 && (centerX + n) < (canvas.getWidth()))
                        if (spriteOver.alphaAtUnchecked(n, i) > 10)
                            canvas.draw(centerX + n, centerY + i, spriteOver.atUnchecked(n, i));

                }
            }

        }
    }

    

};




int main() {
    // first choose from 2 maps using terminal input
    int windowNo = 1;
    int input = 0;
    cout << "GE survivor game\n";
    cout << "Input 1 for finite map, 2 for infinite map.\n";
    while (input != 1 && input != 2) {
        cin >> input;
    }
    //cin >> input;
    windowNo = input;
    srand(static_cast<unsigned int>(time(nullptr)));
    // Create a canvas window with dimensions 1024x768 and title "Example"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "Tiles");
    
    bool running = true; // Variable to control the main loop's running state.

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;

    // initialize classes
    hero h(canvas.getWidth() / 2, canvas.getHeight()/2);
    world1 w1("Resources/tiles1.txt");
    enemyList el;
    camera c;
    //projectile p(0 , 0, el);

    int x = 0;
    int y = 0;
    int currentFps = 0;
    int frameCount = 0;
    int totalFps = 0;
    int averFps = 0;
    float timeElapsed = 0.f;
    float totalTime = 0.f;
    float cd = 0.f;
    while (running)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();
        // Clear the window for the next frame rendering
        canvas.clear();

        bool alpha = false;
        float dt = timer.dt();
        cd += dt;
        
        // switch to determine which window/map is now
        switch (windowNo)
        {
        case 1:
            // finite map
            c.updatePos(canvas, dt, el, h);
            el.update(canvas, dt, -(c.x), -(c.y), h, windowNo);
            if (canvas.keyPressed('E') && cd > 3.f) {
                el.takeAOE();
                cd = 0.f;
            }
            c.draw(canvas, w1, h, el);
            canvas.present();
            // game over when hero health 0
            if (h.health <= 0) {
                windowNo = 3;
            }
            
            // calculate FPS
            timeElapsed += dt;
            totalTime += dt;
            frameCount++;
            if (timeElapsed >= 1.0f) {
                timeElapsed = 0.f;
                currentFps = frameCount;
                frameCount = 0;
                cout << "current FPS: " << currentFps << endl;
                totalFps += currentFps;
            }

            break;

        case 3:
            // game over screen
            c.drawGameOver(canvas);
            break;
        case 2:
            // infinite map
            c.updateInfinite(canvas, dt, el, h);
            el.update(canvas, dt, -(c.x), -(c.y), h, windowNo);
            if (canvas.keyPressed('E') && cd > 3.f) {
                el.takeAOE();
                cd = 0.f;
            }
            c.drawInfinite(canvas, w1, h, el);
            canvas.present();
            if (h.health <= 0) {
                windowNo = 3;
            }

            timeElapsed += dt;
            totalTime += dt;
            frameCount++;
            if (timeElapsed >= 1.0f) {
                timeElapsed = 0.f;
                currentFps = frameCount;
                frameCount = 0;
                cout << "current FPS: " << currentFps << endl;
                totalFps += currentFps;
            }

            break;
        
        default:
            break;
        }

        // exit the game
        if (canvas.keyPressed(VK_ESCAPE)) {
            averFps = totalFps / (int)totalTime;
            /*cout << "score = " << score << endl;*/
            break;
        }
            
        
       


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    cout << "score = " << score << endl;
    cout << "average FPS = " << averFps << endl;
    return 0;

}
