#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500

struct Projectile {
    Vector2 pos;
    float radius;
    Vector2 speed;
    float lifetime;
    bool active;
};

struct Jet {
    Vector2 points[3];
    Vector2 center;
    float rotation;
    float rotation_delay;
    float speed;
    float angle_speed;
    KeyboardKey shoot_ctrl;
    KeyboardKey left_ctrl;
    KeyboardKey right_ctrl;
    Projectile projectile;
    int result;
    Color color;
};

void updateJet(Jet *jet);

void checkForHit(Jet *jet1, Jet *jet2);

int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib");

    char rot_text1[50];
    char rot_text2[50];

    char res1[10];
    char res2[10];

    Jet jet1 = {};
    jet1.points[0] = {SCREEN_WIDTH / 2 + 30, SCREEN_HEIGHT / 2 - 30};
    jet1.points[1] = {SCREEN_WIDTH / 2 - 15 + 30, SCREEN_HEIGHT / 2 + 15};
    jet1.points[2] = {SCREEN_WIDTH / 2 + 15 + 30, SCREEN_HEIGHT / 2 + 15};
    jet1.center = { (jet1.points[0].x + jet1.points[1].x + jet1.points[2].x) / 3,
                    (jet1.points[0].y + jet1.points[1].y + jet1.points[2].y) / 3};
    jet1.rotation = 0;
    jet1.rotation_delay = 0;
    jet1.speed = 100.0f;
    jet1.angle_speed = 2;
    jet1.shoot_ctrl = KEY_W;
    jet1.left_ctrl =  KEY_A;
    jet1.right_ctrl = KEY_D;
    jet1.projectile = {};
    jet1.result = 0;
    jet1.color = WHITE;

    Jet jet2 = {};
    jet2.points[0] = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 30};
    jet2.points[1] = {SCREEN_WIDTH / 2 - 15 - 30, SCREEN_HEIGHT / 2 + 15};
    jet2.points[2] = {SCREEN_WIDTH / 2 + 15 - 30, SCREEN_HEIGHT / 2 + 15};
    jet2.center = { (jet2.points[0].x + jet2.points[1].x + jet2.points[2].x) / 3,
                    (jet2.points[0].y + jet2.points[1].y + jet2.points[2].y) / 3};
    jet2.rotation = 0;
    jet2.rotation_delay = 0;
    jet2.speed = 100.0f;
    jet2.angle_speed = 2;
    jet2.shoot_ctrl = KEY_UP;
    jet2.left_ctrl =  KEY_LEFT;
    jet2.right_ctrl = KEY_RIGHT;
    jet2.projectile = {};
    jet2.result = 0;
    jet2.color = BLACK;//

    //Shader shader = LoadShader(0, "res/pixelizer.fs");

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        updateJet(&jet1);
        updateJet(&jet2);

        checkForHit(&jet1, &jet2);

        //sprintf(rot_text1, "%10.10f", jet1.rotation);
        //sprintf(rot_text2, "%10.10f", jet1.rotation_delay);

        sprintf(res1, "%d", jet1.result);
        sprintf(res2, "%d", jet2.result);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GRAY);
            //BeginShaderMode(shader); 

            DrawTriangle(jet1.points[0], jet1.points[1], jet1.points[2], jet1.color);
            DrawCircle(jet1.center.x, jet1.center.y, 4, BLACK);
            if (jet1.projectile.active)
                DrawCircle(jet1.projectile.pos.x, jet1.projectile.pos.y, jet1.projectile.radius, jet1.color);

            DrawTriangle(jet2.points[0], jet2.points[1], jet2.points[2], jet2.color);
            DrawCircle(jet2.center.x, jet2.center.y, 4, WHITE);
            if (jet2.projectile.active)
                DrawCircle(jet2.projectile.pos.x, jet2.projectile.pos.y, jet2.projectile.radius, jet2.color);

            //DrawText(rot_text1, 10, 30, 30, RED);
            //DrawText(rot_text2, 10, 60, 30, GREEN);

            DrawText(res1, SCREEN_WIDTH / 2 + 20, 10, 30, WHITE);
            DrawText(res2, SCREEN_WIDTH / 2 - 20, 10, 30, BLACK);

            DrawFPS(10, 10);

            

            //EndShaderMode();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void updateJet(Jet* jet) {
    //if there is right or left contorl pressed(but not both!)
    if (IsKeyDown(jet->right_ctrl) != IsKeyDown(jet->left_ctrl)) {

        float sgn = 0;
        if (IsKeyDown(jet->right_ctrl)) sgn = + 1;  //if it is right rotation is positive
        else                            sgn = - 1;  // left = negative

        
        Vector2 vec1, vec2;
        //transalte vectors to the coordinate start, rotate it, translate it back, all using the center vector
        for (int i = 0; i < 3; i++) {   
            vec1 = Vector2Subtract(jet->points[i], jet->center);
            vec2 = Vector2Rotate(vec1, sgn * jet->angle_speed * GetFrameTime());
            jet->points[i] = Vector2Add(vec2, jet->center);
        }
        jet->rotation += sgn * jet->angle_speed * GetFrameTime();
            
    }


    if (jet->rotation_delay < jet->rotation) {
        jet->rotation_delay += 1.0f * GetFrameTime();
    }

    if (jet->rotation_delay > jet->rotation) {
        jet->rotation_delay -= 1.0f * GetFrameTime();
    }

    //simple forward movement based on the angle
    //if (IsKeyDown(jet->up_ctrl)) {
        jet->center.x += jet->speed * GetFrameTime() * sinf(jet->rotation_delay);
        jet->center.y -= jet->speed * GetFrameTime() * cosf(jet->rotation_delay);
        for (int i = 0; i < 3; i++) {
            jet->points[i].x += jet->speed * GetFrameTime() * sinf(jet->rotation_delay);
            jet->points[i].y -= jet->speed * GetFrameTime() * cosf(jet->rotation_delay);
        }
    //}

    if (IsKeyPressed(jet->shoot_ctrl) && !jet->projectile.active) {
        jet->projectile.pos = jet->points[0];
        jet->projectile.speed.x =   1.2 * jet->speed * sinf(jet->rotation);
        jet->projectile.speed.y = - 1.2 * jet->speed * cosf(jet->rotation);
        jet->projectile.radius = 3;
        jet->projectile.lifetime = 5.0f;
        jet->projectile.active = true;
    }
    //update projectile if necesary
    if (jet->projectile.active) {
        jet->projectile.pos.x += jet->projectile.speed.x * GetFrameTime();
        jet->projectile.pos.y += jet->projectile.speed.y * GetFrameTime();
        jet->projectile.lifetime -= GetFrameTime();
        if (jet->projectile.lifetime < 0) jet->projectile.active = false;
    }


    //walls collision
    //west
    if (jet->center.x < 0 - 30) {
        jet->center.x += SCREEN_WIDTH + 60;
        for (int i = 0; i < 3; i++) {
            jet->points[i].x += SCREEN_WIDTH + 60;
        }
    }
    //east
    if (jet->center.x > SCREEN_WIDTH + 30) {
        jet->center.x -= SCREEN_WIDTH + 60;
        for (int i = 0; i < 3; i++) {
            jet->points[i].x -= SCREEN_WIDTH + 60;
        }
    }
    //north
    if (jet->center.y < 0 - 30) {
        jet->center.y += SCREEN_HEIGHT + 60;
        for (int i = 0; i < 3; i++) {
            jet->points[i].y += SCREEN_HEIGHT + 60;
        }
    }
    //south
    if (jet->center.y > SCREEN_HEIGHT + 30) {
        jet->center.y -= SCREEN_HEIGHT + 60;
        for (int i = 0; i < 3; i++) {
            jet->points[i].y -= SCREEN_HEIGHT + 60;
        }
    }
}


void checkForHit(Jet *jet1, Jet *jet2) {
    if (jet1->projectile.active  && CheckCollisionPointTriangle(jet1->projectile.pos, jet2->points[0],  jet2->points[1],  jet2->points[2])) {
        jet1->result++;
        jet1->projectile.active = false;
    }

    if (jet2->projectile.active && CheckCollisionPointTriangle(jet2->projectile.pos, jet1->points[0],  jet1->points[1],  jet1->points[2])) {
        jet2->result++;
        jet2->projectile.active = false;
    }
}