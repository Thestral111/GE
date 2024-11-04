#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header
#include <fstream>

using namespace std;


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


class tile {
    GamesEngineeringBase::Image sprite;
public:
    tile() {}
    void load(string filename) {
        sprite.load(filename);
    }
    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (unsigned int i = 0; i < sprite.height; i++)
            // bounds checking goes here
            if (y + i > 0 && (y + i) < (canvas.getHeight()))
                for (unsigned int n = 0; n < sprite.width; n++)
                    canvas.draw(x + n, y + i, sprite.atUnchecked(n, i));
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
    srand(static_cast<unsigned int>(time(nullptr)));
    // Create a canvas window with dimensions 1024x768 and title "Example"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "Tiles");
    bool running = true; // Variable to control the main loop's running state.

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;

    //world w;
    world w("order.txt");
    hero h(canvas.getWidth() / 2, canvas.getHeight()/2);

    int x = 0;
    int y = 0;
    while (running)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();
        // Clear the window for the next frame rendering
        canvas.clear();

        bool alpha = false;

        if (canvas.keyPressed(VK_ESCAPE)) break;
        if (canvas.keyPressed('W')) y += 2;
        if (canvas.keyPressed('S')) y -= 2;
        //if (canvas.keyPressed('A')) h.update(-2, 0);
        //if (canvas.keyPressed('D')) h.update(2, 0);
        if (canvas.keyPressed('A')) x += 2;
        if (canvas.keyPressed('D')) x -= 2;
        //if (canvas.keyPressed('Q')) alpha = !alpha;
        // scroll vertically all the time

        //t.draw(canvas, y++);
        //y += 2;
        // w.draw(canvas, y);
        /*if (alpha)
            w.drawAlpha(canvas, y);
        else*/
        w.draw(canvas, x, y);
        h.draw(canvas);
        //w.collision(canvas, h, y);


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;

}
