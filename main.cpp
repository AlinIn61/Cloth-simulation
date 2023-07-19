#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>

struct Particle{
    float x;
    float y;
    float prevx;
    float prevy;
    float mass;
    bool isPinned = false;
    bool isSelected = false;

    Particle(float thisX, float thisY, float thisMass) {
        x = thisX;
        y = thisY;
        prevx = thisX;
        prevy = thisY;
        mass = thisMass;
    }
};
class Stick {

public:
    Particle &particle1;
    Particle &particle2;
    float length;
    float getDistance(){
        float dx = particle1.x - particle2.x;
        float dy = particle1.y - particle2.y;

        return sqrt(dx*dx + dy*dy);
    }

    Vector2 getDifference(){
        return {particle1.x - particle2.x, particle2.y - particle2.y};
    }

    Stick(Particle &p1, Particle &p2) : particle1(p1), particle2(p2) {
        length = getDistance();
    };

    void updateStick(){
        Vector2 diff = getDifference();
        float dx = particle1.x - particle2.x;
        float dy = particle1.y - particle2.y;

        float distance = sqrt(dx*dx + dy*dy);
        float diffFactor = ((length - distance)/distance) /2;
        Vector2 offset;
        offset.x = dx * diffFactor;
        offset.y = dy * diffFactor;

        if(particle1.isPinned == false) {
            particle1.x += offset.x;
            particle1.y += offset.y;
            particle2.x -= offset.x;
            particle2.y -= offset.y;
        }
        else{
            particle1.x = particle1.prevx;
            particle1.y = particle1.prevy;
            particle2.x -= offset.x;
            particle2.y -= offset.y;
        }

    }
};

class Mouse {
private:
    inline static Vector2 pos = {0,0};
    inline static Vector2 prevPos = {0,0};

    inline static float cursorSize = 20;
    inline static float maxCursorSize = 100;
    inline static float minCursorSize = 20;

    inline static bool buttonClicked = false;

public:
    static void initMousePos() {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !buttonClicked) {
            pos = GetMousePosition();
            prevPos = pos;
            buttonClicked = true;
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && buttonClicked){
            pos = GetMousePosition();
        }
        else{
            buttonClicked = false;
        }
    }
    static void UpdatePosition(float x, float y)
    {
        prevPos.x = pos.x;
        prevPos.y = pos.y;
        pos.x = x;
        pos.y = y;
    }

    static void IncreaseCursorSize(float increment)
    {
        if(cursorSize + increment > maxCursorSize || cursorSize + increment < minCursorSize)
            return;

        cursorSize += increment;
    }
    static Vector2 getPosition(){
        return pos;
    }
    static Vector2  getprevPosition(){
        return prevPos;
    }
    static float GetCursorSize(){
        return cursorSize;
    }
    static bool getStatus(){
        return buttonClicked;
    }
};

void keepInsideView(Particle &particle, float height, float width) {
    float vel_x = particle.x - particle.prevx;
    float vel_y = particle.y - particle.prevy;
    if (particle.y >= height){ particle.y = height; particle.prevy = particle.y + vel_y; }
    if (particle.x >= width){ particle.x = width; particle.prevx = particle.x + vel_x;}
    if (particle.y < 0){ particle.y = 0; particle.prevy = particle.y + vel_y;}
    if (particle.x < 0){ particle.x = 0; particle.prevx = particle.x + vel_x;}
}
void update(std::vector<Particle> &p, std::vector<Stick> &s, float deltaTime, float height, float width){
    Vector2 force = {0, 20.1f};
    Mouse::initMousePos();
    float cursorSize = Mouse::GetCursorSize();
    Vector2 mousepos = Mouse::getPosition();
    Vector2 mouseprev = Mouse::getprevPosition();
    Mouse::UpdatePosition(mousepos.x, mousepos.y);
    std::cout<<mousepos.x;

    for (auto &particle : p)
    {
        Vector2 cursorToPosDir;
        cursorToPosDir.x = particle.x - mousepos.x;
        cursorToPosDir.y = particle.y - mousepos.y;

        float cursorToPosDist = cursorToPosDir.x * cursorToPosDir.x + cursorToPosDir.y * cursorToPosDir.y;
        bool isSelected = cursorToPosDist < cursorSize * cursorSize;

        particle.isSelected = isSelected;

        if(Mouse::getStatus() && particle.isSelected){
            Vector2 difference;
            float elasticity = 10;
            difference.x = mousepos.x - mouseprev.x;
            difference.y = mousepos.y - mouseprev.y;
            if (difference.x > elasticity) difference.x = elasticity;
            if (difference.y > elasticity) difference.y = elasticity;
            if (difference.x < -elasticity) difference.x = -elasticity;
            if (difference.y < -elasticity) difference.y = -elasticity;

            particle.prevx = particle.x - difference.y;
            particle.prevy = particle.y - difference.y;
        }
        if(particle.isPinned == false) {

            Vector2 acceleration = {force.x / particle.mass, force.y / particle.mass};
            Vector2 prevPosition = {particle.x, particle.y};

            particle.x = 2 * particle.x - particle.prevx + acceleration.x * (deltaTime * deltaTime);
            particle.y = 2 * particle.y - particle.prevy + acceleration.y * (deltaTime * deltaTime);

            particle.prevx = prevPosition.x;
            particle.prevy = prevPosition.y;
            keepInsideView(particle, height, width);
        }
    }
    for (auto &stick : s){
        stick.updateStick();
    }
}

int TwoDtoOneD(int i, int j, int iMax){
    int rez;
    rez = iMax * j + i;
    return rez;
}

int main() {
    std::cout << "Hello, World!";
    const int screenWidth = 800;
    const int screenHeight = 500;
    std::vector<Particle> p;
    std::vector<Stick> sticks;
    float distance = 10;

    for(float i = 0; i<49; i++){
        for(float j = 0; j<79; j++){
            if(i==0 || i ==1)
                p.emplace_back(j*distance, i*distance, 10000).isPinned = true;
            else{
                p.emplace_back(j*distance, i*distance, 10000);
            }
        }
    }

    for(int i = 0; i<49; i++){
        for(int j = 0; j<79; j++){
            int index = TwoDtoOneD(j, i, 79);

            if(j != 78){
                sticks.emplace_back(p[index], p[index+1]);
            }
        }
    }

    for(int i = 0; i<79; i++){
        for(int j = 0; j<49; j++){
            int index = TwoDtoOneD(i, j, 79);

            if(j != 48){
                sticks.emplace_back(p[index], p[index+79]);
            }
        }
    }

    
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    float old_time = GetTime();
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   float new_time = GetTime() - old_time;
        old_time = new_time;
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        update(p,sticks, new_time, screenHeight, screenWidth);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for(auto particle : p) {
            if(particle.isSelected){
                DrawCircle(particle.x, particle.y, 1, RED);
            }
            else{
                DrawCircle(particle.x, particle.y, 1, BLUE);
            }
        }
        for(auto stick : sticks){
            DrawLine(stick.particle1.x, stick.particle1.y, stick.particle2.x, stick.particle2.y, RED);
        }

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
