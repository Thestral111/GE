#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

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
    GamesEngineeringBase::Image sprite;
    int x;
    int y;
    void update(int inc, int yinc) {
        x += inc;
        y += yinc;
    }
    entity() {

    }

    void draw(GamesEngineeringBase::Window& canvas) {
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

};

class hero {
    GamesEngineeringBase::Image sprite;
    int x;
    int y;
public:
    void update(int inc, int yinc) {
        x += inc;
        y += yinc;
    }
    hero(int _x, int _y) {
        sprite.load("Resources/L.png");
        x = _x - sprite.width / 2; // centre
        y = _y - sprite.height / 2;
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

const int emaxsize = 100;
class enemy : public entity{
    
public:
    float speed = 300.f;
    int move;
    enemy() {
        sprite.load("Resources/L2.png");
        x = rand() % 2688 - 1344;
        y = rand() % 2688 - 1344;
        cout << "pos: " << x << " " << y << endl;
    }
    void update(GamesEngineeringBase::Window& canvas, float dt, int px, int py) {
        // towards player
        int xOffset = px - x;
        int yOffset = py - y;
        float dist = sqrt(xOffset * xOffset + yOffset * yOffset);
        //cout << dist << endl;
        move = static_cast<int>((speed * dt));
        x += xOffset / dist * move;
        y += yOffset / dist * move;

    }

    void draw(GamesEngineeringBase::Window& canvas, int _x, int _y) {
        //x += _x;
        //y += _y;
        _x += x;
        _y += y;
        for (unsigned int i = 0; i < sprite.height; i++) {
            // bounds checking goes here
            if (_y + i > 0 && (_y + i) < (canvas.getHeight())) {
                for (unsigned int n = 0; n < sprite.width; n++) {
                    if (_x + n > 0 && (_x + n) < (canvas.getWidth()))
                        canvas.draw(_x + n, _y + i, sprite.atUnchecked(n, i));

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

    void generateEnemy(GamesEngineeringBase::Window& canvas, float dt) {
        timeElapsed += dt;
        if (currentSize < emaxsize) {
            if (timeElapsed > timeThreshold) { // create new plane
                enemy* p = new enemy();
                //  cout << "Created: " << currentSize << '\t' << timeThreshold << '\t' << dt << endl;
                cout << "Created: " << currentSize << endl;

                sarray[currentSize++] = p;
                timeElapsed = 0.f;
                //timeThreshold -= 0.2f;
                //timeThreshold = max(0.2f, timeThreshold);
            }
        }
    }

    void update(GamesEngineeringBase::Window& canvas, float dt, int cx, int cy) {
        generateEnemy(canvas, dt);
        int playerX = cx + canvas.getWidth() / 2;
        int playerY = cy + canvas.getHeight() / 2;
        for (unsigned int i = 0; i < currentSize; i++) {
            if (sarray[i] != nullptr) {
                sarray[i]->update(canvas, dt, playerX, playerY);
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
    int leftbound = 1856;
    int rightbound = -832;
    int upperbound = 1728;
    int lowerbound = -960;
    
    int x = 0;
    int y = 0;
    GamesEngineeringBase::Timer timer;
    //float dt;
    int move;
    

    camera() {
        
    }

    void updatePos(GamesEngineeringBase::Window& canvas, float dt) {
        /*if (canvas.keyPressed(VK_ESCAPE)) break;*/
        //dt = timer.dt();
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
        
    }

    void draw(GamesEngineeringBase::Window& canvas, world1& w1, hero& h, enemyList& el) {
        w1.draw(canvas, x, y);
        w1.draw(canvas, x - 1344, y); // splicing larger map 
        w1.draw(canvas, x, y - 1344);
        w1.draw(canvas, x - 1344, y - 1344);
        h.draw(canvas);
        el.draw(canvas, x, y);
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

        if (canvas.keyPressed(VK_ESCAPE)) break;
        c.updatePos(canvas, dt);
        el.update(canvas, dt, -(c.x), -(c.y));
        //if (canvas.keyPressed('W')) y += move;
        //if (canvas.keyPressed('S')) y -= move;
        //if (canvas.keyPressed('A')) h.update(-2, 0);
        //if (canvas.keyPressed('D')) h.update(2, 0);
        /*if (canvas.keyPressed('A')) x += move;
        if (canvas.keyPressed('D')) x -= move;*/
        //if (canvas.keyPressed('Q')) alpha = !alpha;
        // scroll vertically all the time

        //t.draw(canvas, y++);
        //y += 2;
        // w.draw(canvas, y);
        /*if (alpha)
            w.drawAlpha(canvas, y);
        else*/
        //w.draw(canvas, x, y);
        //w1.draw(canvas, x, y);
        //w1.draw(canvas, x - 1344, y); // splicing larger map 
        //h.draw(canvas);
        //w.collision(canvas, h, y);
        c.draw(canvas, w1, h, el);


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;

}
