#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header
#include <fstream>
#include <sstream>
//#include <cmath>

using namespace std;

const int canvasHeight = 768;
const int canvasWidth = 1024;

template <typename T>
class node {
public:
    T data;
    node* next;
    node(T& d) {
        data = d;
    }
};

template <typename T>
class myVector {
    //template <typename T>
    
    T* data;
    int size;
public:

    myVector() {
        size = 0;
        data = new T[10];
        
    }

    ~myVector() {
        delete[] data;
    }

    void push() {
        
    }

};

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

    void takeDamage() {
        health -= 1;
        cout << "taking damage\n";
        if (health <= 0) {
            cout << "dead\n";
        }
    }

};

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
                //if ((x + n) >= 0 && (x + n) < canvas.getWidth() && (y + i) >= 0 && (y + i) < canvas.getHeight()) 
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
class enemy : public entity{
    
public:
    float speed = 200.f;
    int move;
    float elapsedTime = 0.f;
    float timeThreshold = 1.0f;
    enemy(int cx, int cy) {
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
            speed = 200.f;
            break;
        case 3:
            // slime
            sprite.load("Resources/slimeSmall.png");
            health = 2;
            speed = 120.f;
            break;
        case 4:
            // bat
            sprite.load("Resources/bat.png");
            health = 1;
            speed = 250.f;
            break;
        default:
            sprite.load("Resources/skeleton.png");
            health = 2;
            speed = 150.f;
            break;
        }
        //sprite.load("Resources/bat.png");
        do {
            x = rand() % 2688 - 1344;
            y = rand() % 2688 - 1344;
        } while ((x>cx && x<cx+canvasWidth) && (y>cy && y<cy+canvasHeight));
        
        //cout << "pos: " << x << " " << y << endl;
    }
    void update(GamesEngineeringBase::Window& canvas, float dt, int px, int py, hero& h) {
        // towards player
        int xOffset = px - x;
        int yOffset = py - y;
        float dist = sqrt(xOffset * xOffset + yOffset * yOffset);
        //cout << dist << endl;
        //move = static_cast<int>((speed * dt));
        //if (dist > 50.f) {
        //    x += xOffset / dist * move;
        //    y += yOffset / dist * move;
        //    //x -= (sprite.width / 2);
        //    //y -= (sprite.height / 2);
        //}

        move = static_cast<int>((speed * dt));
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
        //x += _x;
        //y += _y;
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

class enemyList {
public:
    enemy* sarray[emaxsize];
    int currentSize = 0;
    float timeElapsed = 0.f;
    float timeThreshold = 2.f;
    
    enemyList() {

    }

    void generateEnemy(GamesEngineeringBase::Window& canvas, float dt, int cx, int cy) {
        timeElapsed += dt;
        if (currentSize < emaxsize) {
            if (timeElapsed > timeThreshold) { // create new plane
                enemy* p = new enemy(cx, cy);
                //  cout << "Created: " << currentSize << '\t' << timeThreshold << '\t' << dt << endl;
                cout << "Created: " << currentSize << endl;

                sarray[currentSize++] = p;
                timeElapsed = 0.f;
                //timeThreshold -= 0.2f;
                //timeThreshold = max(0.2f, timeThreshold);
            }
        }
    }

    void update(GamesEngineeringBase::Window& canvas, float dt, int cx, int cy, hero& h) {
        generateEnemy(canvas, dt, cx, cy);
        int playerX = cx + canvas.getWidth() / 2;
        int playerY = cy + canvas.getHeight() / 2;
        for (unsigned int i = 0; i < currentSize; i++) {
            if (sarray[i] != nullptr) {
                sarray[i]->update(canvas, dt, playerX, playerY, h);
                if (sarray[i]->health <= 0) {
                    enemy* temp = sarray[i];
                    sarray[i] = nullptr;
                    delete temp;
                }
            }
        }
        
        // update every enemy

    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (unsigned int i = 0; i < currentSize; i++)
        {
            if (sarray[i] != nullptr)
                sarray[i]->draw(canvas, x, y);
        }
    }

};

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
        //int ex, ey;
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

    void generateProjectile(GamesEngineeringBase::Window& canvas, enemyList el) {

    }

    bool collide(entity& entity) {
        int dx = x - entity.x;
        int dy = y - entity.y;

        float radius = 20.f;
        float d = sqrtf(dx * dx + dy * dy); // can do without sqrtf also
        return d < 2.f * radius;
    }
    
    void update(GamesEngineeringBase::Window& canvas, float dt) {

        int xOffset = e->x - x;
        int yOffset = e->y - y;
        float dist = sqrt(xOffset * xOffset + yOffset * yOffset);
        

        int move = static_cast<int>((600.f * dt));
        if (!collide(*e)) {
            
            x += xOffset / dist * move;
            y += yOffset / dist * move;
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

const unsigned int tileNum = 6;
class tileSet {
    tile tiles[tileNum];
    unsigned int size = tileNum;
public:
    tileSet() {}
    void load(string s = "") {
        for (unsigned int i = 0; i < size; i++) {
            string filename;
            filename = "Resources/" + s + to_string(i) + ".png";
            tiles[i].load(filename);
        }
    }
    tile& operator[](unsigned int index) { return tiles[index]; }
};

class world1 {
public:
    tile tiles[42][42];
    world1(string filename) {
        ifstream infile(filename);
        int tilenum;
        string map[42][42];
        string line;
        string tilename;
        //tile tiles[42][42];
        int i = 0;
        
        while (getline(infile, line)) {
            stringstream ss(line);
            string number;
            int j = 0;
            //cout << "i = " << i << endl;

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

class camera {
public:
    GamesEngineeringBase::Image sprite;
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
    float timeThreshold = 3.f;
    int currentSize = 0;
    

    camera() {
        
    }

    void getPlayerPos(GamesEngineeringBase::Window& canvas, hero& h) {
        px = -x + canvas.getWidth() / 2;
        py = -y + canvas.getHeight() / 2;
    }

    void updatePos(GamesEngineeringBase::Window& canvas, float dt, enemyList& el, hero& h) {
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
         
        // bound checking
        // mapSize + player offset (half canvas size)
        if (x > 1856) x = 1856;
        if (x < -832) x = -832;
        if (y > 1728) y = 1728;
        if (y < -960) y = -960;
        //cout << "x = " << x << " ";
        //cout << "y = " << y << endl;
        timeElapsed += dt;
        if (currentSize < emaxsize) {
            if (timeElapsed > timeThreshold) { // create new plane
                projectile* p = new projectile(px, py , el);
                //  cout << "Created: " << currentSize << '\t' << timeThreshold << '\t' << dt << endl;
                cout << " projectile created: " << currentSize << endl;

                parray[currentSize++] = p;
                timeElapsed = 0.f;
                //timeThreshold -= 0.2f;
                //timeThreshold = max(0.2f, timeThreshold);
            }
        }
        for (int i = 0; i < currentSize; i++) {
            if (parray[i] != nullptr) {
                parray[i]->update(canvas, dt);
                if (parray[i]->collide(*(parray[i]->e))) {
                    parray[i]->e->takeDamage();
                    entity* p = parray[i];
                    parray[i] = nullptr;
                    delete p;
                }
            }
            
        }
        
    }

    

    void draw(GamesEngineeringBase::Window& canvas, world1& w1, hero& h, enemyList& el) {
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

    void drawGameOver(GamesEngineeringBase::Window& canvas) {
        sprite.load("Resources/game_over_sprite.png");
        int centerX = canvas.getWidth() / 2 - sprite.width/2;
        int centerY = canvas.getHeight() / 2 - sprite.height/2;
        for (unsigned int i = 0; i < sprite.height; i++) {
            // bounds checking goes here
            if (centerY + i > 0 && (centerY + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < sprite.width; n++) {
                    if (centerX + n > 0 && (centerX + n) < (canvas.getWidth()))
                        if (sprite.alphaAtUnchecked(n, i) > 10)
                            canvas.draw(centerX + n, centerY + i, sprite.atUnchecked(n, i));

                }
            }

        }
    }

};

const int maxSize = 100;
class world {
    tileSet tiles;
    tileSet alphas;
    //unsigned int a[maxSize];
    unsigned int size;
    unsigned int* a;
public:
    world() {
        size = maxSize;
        a = new unsigned int[size];
        tiles.load();
        //alphas.load("alpha"); // change
        for (unsigned int i = 0; i < maxSize; i++) {
            a[i] = rand() % tileNum; // randomly choose a tile
            // cout << a[i] << endl; 
        }
    }
    world(string filename) { // load from file name
        tiles.load();
        //alphas.load("alpha");
        ifstream infile(filename);
        int tilenum, i;
        string s;

        infile >> tilenum;
        cout << tilenum << endl;
        a = new unsigned int[tilenum];
        size = tilenum;

        for (i = 0; i < tilenum; i++)
            infile >> a[i];

        for (i = 0; i < tilenum; i++)
            cout << a[i] << '-';
        cout << endl;
        infile.close();
    }
    /*~world() {
        delete[] a;
    }*/
    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        int height = 384; // should not be fixed 
        int width = 384;
        int Y = wy / height;
        int r = wy % height;
        int X = wx / height;
        int xr = wx % height;
        tiles[a[Y % size]].draw(canvas, xr, (canvas.getHeight() / 2) + r);
        tiles[a[(Y + 1) % size]].draw(canvas, xr, r);
        tiles[a[(Y + 2) % size]].draw(canvas, xr, -height + r);
    }
//
//    void drawAlpha(GamesEngineeringBase::Window& canvas, int wy) {
//        int height = 384; // should not be fixed 
//        int Y = wy / height;
//        int r = wy % height;
//        alphas[a[Y % size]].draw(canvas, (canvas.getHeight() / 2) + r);
//        alphas[a[(Y + 1) % size]].draw(canvas, r);
//        alphas[a[(Y + 2) % size]].draw(canvas, -height + r);
//    }
//
//    bool collision(GamesEngineeringBase::Window& canvas, hero& h, unsigned int wy) {
//        bool b1 = testline(canvas, h.getX(), h.getWidth(), wy, h.getHeight() / 3);
//        bool b2 = testline(canvas, h.getX() + 19, 44, wy, (h.getHeight() / 2) - 1);
//        return b1 && b2;
//
//    }
//private:
//    bool testline(GamesEngineeringBase::Window& canvas, unsigned int hx, unsigned int h2, unsigned int wy, unsigned int offset)
//    {
//        bool col = false;
//        int Y = (wy + offset) / 384; // divide by height of tile to get tile - use first one as all tiles the same height
//
//        tile& t = alphas[a[Y % size]]; // get the tile at the right position
//
//        unsigned int y = t.getHeight() - ((offset + wy) % t.getHeight()); // get y position
//
//        for (unsigned int i = hx; i < hx + h2; i++) {// width of plane 
//            // draw the bounding line for testing
//            if (i > canvas.getWidth()) continue;
//
//            if (t.getSprite().at(i, y, 0) < 100) {
//                canvas.draw(i, canvas.getHeight() - offset, 255, 0, 0); // draw line around plane
//                col = true;
//            }
//            else {
//                canvas.draw(i, canvas.getHeight() - offset, 0, 255, 0); // draw line around plane
//
//            }
//        }
//        return col;
//    }
};


int main() {
    //enemy e;
    //node<enemy> newnode(e);
    int windowNo = 1;
    srand(static_cast<unsigned int>(time(nullptr)));
    // Create a canvas window with dimensions 1024x768 and title "Example"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "Tiles");
    //canvas.create(1920, 1080, "Tiles");
    bool running = true; // Variable to control the main loop's running state.

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;

    //world w;
    //world w("order.txt");
    hero h(canvas.getWidth() / 2, canvas.getHeight()/2);
    world1 w1("Resources/tiles1.txt");
    enemyList el;
    camera c;
    //projectile p(0 , 0, el);

    int x = 0;
    int y = 0;
    while (running)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();
        // Clear the window for the next frame rendering
        canvas.clear();

        bool alpha = false;
        float dt = timer.dt();
        //int move = static_cast<int>((500.f * dt));

        switch (windowNo)
        {
        case 1:
            c.updatePos(canvas, dt, el, h);
            el.update(canvas, dt, -(c.x), -(c.y), h);

            c.draw(canvas, w1, h, el);
            canvas.present();
            if (h.health <= 0) {
                windowNo = 2;
            }
            break;

        case 2:
            c.drawGameOver(canvas);
            break;
        case 3:

        default:
            break;
        }

        if (canvas.keyPressed(VK_ESCAPE)) break;
        
        
        
        //c.draw(canvas, w1, h, el);
        //p.draw(canvas);


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;

}
