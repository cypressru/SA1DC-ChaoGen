#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <kos.h>
#include <kos/init.h>
#include <kos/thread.h>
#include <kos/dbgio.h>
#include <dc/biosfont.h>
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <math.h>

#include <stdint.h>
#include <dc/maple.h>
#include <dc/maple/controller.h>
#include <dc/maple/vmu.h>
#include <arch/arch.h>

#define LEFT 0
#define CENTER 128
#define RIGHT 255

typedef enum MenuScreen { SINGLEPLAYER, MULTI, OPTIONS, NETWORK, } MenuScreen;
typedef enum GameScreen { MAINMENU, GAMESCREEN, ENDSCREEN, LOADINGGAMESCREEN, LOADINGENDSCREEN, LOADINGMAINMENU, } GameScreen;

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_NET);


int main(int argc, char *argv[]) {
    const int screenWidth = 640;
    const int screenHeight = 480;
    InitWindow(screenWidth, screenHeight, "Debug");


    MenuScreen menuscreen = OPTIONS;
    int gamepad = 0;
    int menuX = 0;
    int menuY = 0;

    int menuvolume = 250;

    float rotation = 0.0f;
    float backwardsrotation = 0.0f;



    float animationTime = 0.0f;
    const float waveSpeed = 4.0f;  
    // Speed of the sinus wave animation

    //VMU SHIT//////////////////////////////////////////////////////////////////////////////////////////////
        // Initialize VMU filesystem
    vmufs_init();

    // Get the first VMU device
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    // Check if VMU is found
    if (!vmu) {
        printf("No VMU found!\n");
        return 1;
    }

    // Open the source file
    FILE *file_vms = fopen("/rd/SONICADV.VMS", "rb");

    // Check if the source file is opened successfully
    if (!file_vms) {
        printf("Failed to open source file!\n");
        return 1;
    }

    // Get the file size
    fseek(file_vms, 0, SEEK_END);
    int size_vms = ftell(file_vms);
    rewind(file_vms);

    // Allocate memory for the file buffer
    uint8_t* buffer_vms = (uint8_t*)malloc(size_vms);

    // Read the source file into the buffer
    fread(buffer_vms, 1, size_vms, file_vms);

    // Close the source file
    fclose(file_vms);
    //VMU SHIT//////////////////////////////////////////////////////////////////////////////////////////////
   

    
    


    uint8_t volume = 240;
    snd_init();
   

    sfxhnd_t beep3 = snd_sfx_load("/rd/audio/button-40.wav");
    sfxhnd_t beep1 = snd_sfx_load("/rd/audio/button-34.wav");
    sfxhnd_t beep2 = snd_sfx_load("/rd/audio/button-41.wav");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        animationTime += GetFrameTime() * waveSpeed;

        rotation += 0.8f;
        backwardsrotation -=0.8;

        

        switch (menuscreen) {
             
             
            case OPTIONS: {
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) || IsKeyDown(KEY_W)) {
                    snd_sfx_play(beep3, menuvolume, CENTER);

                    menuY--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || IsKeyDown(KEY_S)) {
                    snd_sfx_play(beep3, menuvolume, CENTER);
                    menuY++;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A)) {
                    snd_sfx_play(beep3, menuvolume, CENTER);
                    menuX--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D)) {
                    snd_sfx_play(beep3, menuvolume, CENTER);
                    menuX++;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyReleased(KEY_SPACE)) {
                    snd_sfx_play(beep1, menuvolume, CENTER);

                                
                       if (menuX == 0 && menuY == 0) 
                        {
                         menuX = 0;
                         menuY = 0;
                         menuscreen = SINGLEPLAYER;
                        }
                        if (menuX == 1 && menuY ==0 ) 
                        {
                         menuX = 0;
                         menuY = 0;
                         menuscreen = MULTI;
                        }
                        if (menuX == 0 && menuY == 1 ) 
                        {
                         menuX = 0;
                         menuY = 0;
                         menuscreen = NETWORK;
                        }
                        if (menuX == 1 && menuY ==1 ) 
                        {
                         menuX = 0;
                         menuY = 0;
                         menuscreen = OPTIONS;
                        }
                    }

                if (menuX < 0) {
                    menuX = 0;
                }
                if (menuX > 1) {
                    menuX = 1;
                }
                if (menuY > 1) {
                    menuY = 1;
                }
                if (menuY < 0) {
                    menuY = 0;
                }
                break;
            }
            case SINGLEPLAYER: {
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A)) {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuX--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D)) {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuX++;
                }

                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyReleased(KEY_SPACE)) {
                snd_sfx_play(beep2, volume, CENTER);

                menuscreen = OPTIONS;


                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyReleased(KEY_SPACE)) {
                    snd_sfx_play(beep1, menuvolume, CENTER);

                                
                       if (menuX == 0 && menuY == 0) 
                        {
                         menuX = 0;
                         menuY = 0;
                        int status_vms = vmufs_write(vmu, "SONICADV", buffer_vms, size_vms, VMUFS_OVERWRITE | VMUFS_VMUGAME);
                        free(buffer_vms);



                        if (status_vms == 0) {
                        DrawText("VMU game copied successfully!", 190, 200, 20, BLACK);
                        } else {
                        DrawText("Failed to copy VMU game!", 190, 200, 20, BLACK);
                        }

    
                        }
                        if (menuX == 1 && menuY ==0 ) 
                        {
                         menuX = 0;
                         menuY = 0;

                        }
                       
                    }
                    


        
                if (menuX < 0) {
                    menuX = 0;
                }
                if (menuX > 1) {
                    menuX = 1;
                }
                if (menuY > 1) {
                    menuY = 1;
                }
                if (menuY < 0) {
                    menuY = 0;
                }

                break;
            }
            case MULTI: {
                
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A)) {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuX--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D)) {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuX++;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyReleased(KEY_SPACE)) {
                snd_sfx_play(beep2, volume, CENTER);
                menuscreen = OPTIONS;


                }




                if (menuX < 0) {
                    menuX = 0;
                }
                if (menuX > 1) {
                    menuX = 1;
                }
                if (menuY > 1) {
                    menuY = 1;
                }
                if (menuY < 0) {
                    menuY = 0;
                }

                break;
            }
            case NETWORK: {
                

                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A)) {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuX--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D)) {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuX++;
                }
                 if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyReleased(KEY_SPACE)) {
                    snd_sfx_play(beep2, volume, CENTER);
                    menuscreen = OPTIONS;


                }


                if (menuX < 0) {
                    menuX = 0;
                }
                if (menuX > 1) {
                    menuX = 1;
                }
                if (menuY > 1) {
                    menuY = 1;
                }
                if (menuY < 0) {
                    menuY = 0;
                }

                break;
            }
        }

        BeginDrawing();
        Color backgroundColor = (Color){255, 220, 30, 255};
        ClearBackground(backgroundColor);

       


        switch (menuscreen) {
            case OPTIONS: {

                DrawPoly((Vector2){ 310, 260 }, 6, 80, 0, BLUE);
                DrawPolyLines((Vector2){ 310, 260 }, 6, 90, 0, BLUE);
                DrawPolyLinesEx((Vector2){ 310, 260 }, 6, 85, 0, 6, DARKBLUE);
              
              
                 
                

                // Polygon shapes and lines
                
                if (menuX == 0 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(150, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 150, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, rotation, 6, DARKBLUE);
                    //OPTION 2
                    DrawPoly((Vector2){ 470, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 3
                    DrawPoly((Vector2){ 470, 375 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 375 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 375 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 4
                    DrawPoly((Vector2){ 150, 375 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 375 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 375 }, 6, 85, 0, 6, DARKBLUE);

                }
                if (menuX == 1 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(470, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 2

                    DrawPoly((Vector2){ 470, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 470, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, rotation, 6, DARKBLUE);
                    //OPTION 3
                    DrawPoly((Vector2){ 470, 375 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 375 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 375 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 4
                    DrawPoly((Vector2){ 150, 375 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 375 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 375 }, 6, 85, 0, 6, DARKBLUE);
                }
                if (menuX == 1 && menuY == 1) {
                    //OPTION 1
                    DrawRectangle(470, 375, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 2
                    DrawPoly((Vector2){ 470, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 3
                    DrawPoly((Vector2){ 470, 375 }, 10, 80, backwardsrotation, DARKPURPLE);


                    DrawPoly((Vector2){ 470, 375 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 470, 375 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 375 }, 6, 85, rotation, 6, DARKBLUE);
                    //OPTION 4
                    DrawPoly((Vector2){ 150, 375 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 375 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 375 }, 6, 85, 0, 6, DARKBLUE);
                }
                if (menuX == 0 && menuY == 1) {
                    DrawRectangle(120, 375, 10, 10, RED);
                    //OPTION 1
                    DrawPoly((Vector2){ 150, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 2
                    DrawPoly((Vector2){ 470, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 3
                    DrawPoly((Vector2){ 470, 375 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 375 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 375 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 4
                    DrawPoly((Vector2){ 150, 375 }, 10, 80, backwardsrotation, DARKPURPLE);


                    DrawPoly((Vector2){ 150, 375 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 150, 375 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 375 }, 6, 85, rotation, 6, DARKBLUE);
                }
                
                DrawText( "Single Player", 100, 170, 22, BLACK);
                DrawText( "Multiplayer", 450, 170, 22, BLACK);
                DrawText( "Options", 450, 395, 22, BLACK);
                DrawText( "Network", 100, 395, 22, BLACK);

                // Draw FPS counter
                int fps = GetFPS();
                char fpsText[20];
                sprintf(fpsText, "FPS: %d", fps);
                DrawText(fpsText, 30, 30, 20, BLACK);
                break;
            }

            case SINGLEPLAYER:{
                DrawPoly((Vector2){ 310, 260 }, 6, 80, 0, BLUE);
                DrawPolyLines((Vector2){ 310, 260 }, 6, 90, 0, BLUE);
                DrawPolyLinesEx((Vector2){ 310, 260 }, 6, 85, 0, 6, DARKBLUE);

                if (menuX == 0 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(150, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 150, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, rotation, 6, DARKBLUE);
                    //OPTION 2
                    DrawPoly((Vector2){ 470, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, 0, 6, DARKBLUE);
                
                    

                }



                 if (menuX == 1 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(470, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 2

                    DrawPoly((Vector2){ 470, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 470, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, rotation, 6, DARKBLUE);
                   


                    
                }



                DrawText( "Save Default Chao", 100, 170, 22, BLACK);
                DrawText( "Save Chaos Chao", 450, 170, 22, BLACK);


                // Draw FPS counter
                int fps = GetFPS();
                char fpsText[20];
                sprintf(fpsText, "FPS: %d", fps);
                DrawText(fpsText, 30, 30, 20, BLACK);
                break;
            } 


            case MULTI: {

                DrawPoly((Vector2){ 310, 260 }, 6, 80, 0, BLUE);
                DrawPolyLines((Vector2){ 310, 260 }, 6, 90, 0, BLUE);
                DrawPolyLinesEx((Vector2){ 310, 260 }, 6, 85, 0, 6, DARKBLUE);

                if (menuX == 0 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(150, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 150, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, rotation, 6, DARKBLUE);
                    //OPTION 2
                    DrawPoly((Vector2){ 470, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, 0, 6, DARKBLUE);
                   

                }



                 if (menuX == 1 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(470, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 2

                    DrawPoly((Vector2){ 470, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 470, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, rotation, 6, DARKBLUE);
                  


                    
                    
                }


                DrawText( "Co-op", 100, 170, 22, BLACK);
                DrawText( "PvP", 450, 170, 22, BLACK);

                // Draw FPS counter
                int fps = GetFPS();
                char fpsText[20];
                sprintf(fpsText, "FPS: %d", fps);
                DrawText(fpsText, 30, 30, 20, BLACK);
                break;
            }
            case NETWORK: {

                DrawPoly((Vector2){ 310, 260 }, 6, 80, 0, BLUE);
                DrawPolyLines((Vector2){ 310, 260 }, 6, 90, 0, BLUE);
                DrawPolyLinesEx((Vector2){ 310, 260 }, 6, 85, 0, 6, DARKBLUE);
                
                if (menuX == 0 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(150, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 150, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, rotation, 6, DARKBLUE);
                    //OPTION 2
                    DrawPoly((Vector2){ 470, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, 0, 6, DARKBLUE);
                

                }



                 if (menuX == 1 && menuY == 0) {
                    //OPTION 1
                    DrawRectangle(470, 150, 10, 10, RED);
                    DrawPoly((Vector2){ 150, 150 }, 6, 80, 0, BLUE);
                    DrawPolyLines((Vector2){ 150, 150 }, 6, 90, 0, BLUE);
                    DrawPolyLinesEx((Vector2){ 150, 150 }, 6, 85, 0, 6, DARKBLUE);
                    //OPTION 2

                    DrawPoly((Vector2){ 470, 150 }, 10, 80, backwardsrotation, DARKPURPLE);

                    DrawPoly((Vector2){ 470, 150 }, 6, 80, rotation, BLUE);
                    DrawPolyLines((Vector2){ 470, 150 }, 6, 90, rotation, BLUE);
                    DrawPolyLinesEx((Vector2){ 470, 150 }, 6, 85, rotation, 6, DARKBLUE);
                 


                    
                }


                DrawText( "Online Play", 100, 170, 22, BLACK);
                DrawText( "Website", 450, 170, 22, BLACK);
               

                // Draw FPS counter
                int fps = GetFPS();
                char fpsText[20];
                sprintf(fpsText, "FPS: %d", fps);
                DrawText(fpsText, 30, 30, 20, BLACK);
                break;
            }






        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}