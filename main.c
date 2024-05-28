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

Color chaoPink = (Color){247, 214, 222, 255};
Color chaoBlue = (Color){173, 214, 239, 255};
Color chaoYellow = (Color){247, 239, 82, 255};

Color initCol1, initCol2, initCol3, initCol4;
Color tarCol1, tarCol2, tarCol3, tarCol4;
Color currCol1, currCol2, currCol3, currCol4;

#define LEFT 0
#define CENTER 128
#define RIGHT 255

typedef enum MenuScreen
{
    DEFAULTCHAO,
    GENERATECHAO,
    GENERATECHAOSAVE,
    OPTIONS,
    DELETE,
    COMPLETE,
    FAILURE,
} MenuScreen;



typedef struct IntSelection
{
    Rectangle rect;
    int value;
    bool active;
} IntSelection;

typedef struct DropdownMenu
{
    Rectangle rect;
    const char **items;
    int itemCount;
    int selectedItem;
    bool active;
} DropdownMenu;

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_NET);

const char *JewelMenu[] = {"No Jewel", "Emerald", "Amythest", "Ruby", "Sapphire"};
const int itemCount1 = sizeof(JewelMenu) / sizeof(JewelMenu[0]);

const char *ColorMenu[] = {"Normal Color", "Silver", "Gold", "Black", "Copper", "Onyx", "Jewel+Black"};
const int itemCount2 = sizeof(ColorMenu) / sizeof(ColorMenu[0]);

const char *TypeMenu[] = {"Baby", "Adult", "Swim", "Fly", "Run", "Power", "Chaos"};
const int itemCount3 = sizeof(TypeMenu) / sizeof(TypeMenu[0]);

const char *MedalMenu[] = {"No Medal", "Pearl", "Amethyst", "Sapphire", "Ruby", "Emerald",};
const int itemCount4 = sizeof(MedalMenu) / sizeof(MedalMenu[0]);


//Hex
IntSelection Happiness={
    (Rectangle){100, 220, 200, 30},            // Bounding rectangle
    0,                                        // Value
    false                                    // Active state
};


IntSelection Reincarnations={
    (Rectangle){100, 190, 200, 30},            // Bounding rectangle
    0,                                        // Value
    false                                     // Active state
};


//Dropdown
DropdownMenu dropdownJewel = {
    (Rectangle){100, 65, 200, 30},            // Bounding rectangle
    JewelMenu,                                // Items
    sizeof(JewelMenu) / sizeof(JewelMenu[0]), // Item count
    0,                                        // Selected item
    false                                     // Active state
};

DropdownMenu dropdownColor = {
    (Rectangle){100, 95, 200, 30},            // Bounding rectangle
    ColorMenu,                                // Items
    sizeof(ColorMenu) / sizeof(ColorMenu[0]), // Item count
    0,                                        // Selected item
    false                                     // Active state
};

DropdownMenu dropdownType = {
    (Rectangle){100, 125, 200, 30},            // Bounding rectangle
    TypeMenu,                                // Items
    sizeof(TypeMenu) / sizeof(TypeMenu[0]), // Item count
    0,                                        // Selected item
    false                                     // Active state
};

DropdownMenu dropdownMedal = {
    (Rectangle){100, 155, 200, 30},            // Bounding rectangle
    MedalMenu,                                // Items
    sizeof(MedalMenu) / sizeof(MedalMenu[0]), // Item count
    0,                                        // Selected item
    false                                     // Active state
};





//HEX int to hex converter "uint32_t hex_value = int_to_hex(put_number_to_convert_here);"

//Values for user to pick, to then convert to hex
int ReincarnationValue = 0;
int HappinessValue = 0;


uint32_t int_to_hex(int n) {
    return n;
}

// - - - - Graphics Variables and Functions - - - -

float colTransSpeed = 0.002f;
float colTransProg = 0.0f;

float currentCircleSize = 0.5f;
bool circleAnimate = false;
float currentPolySize = 135.0f;
bool polyAnimate = false;

// Function to linearly interpolate between two colors
Color LerpColor(Color start, Color end, float amount) {
    Color result;
    result.r = (unsigned char)((1 - amount) * start.r + amount * end.r);
    result.g = (unsigned char)((1 - amount) * start.g + amount * end.g);
    result.b = (unsigned char)((1 - amount) * start.b + amount * end.b);
    result.a = (unsigned char)((1 - amount) * start.a + amount * end.a);
    return result;
}

void UpdateGradientColors() {
    // Update the current colors by interpolating towards the target colors
    currCol1 = LerpColor(initCol1, tarCol1, colTransProg);
    currCol2 = LerpColor(initCol2, tarCol2, colTransProg);
    currCol3 = LerpColor(initCol3, tarCol3, colTransProg);
    currCol4 = LerpColor(initCol4, tarCol4, colTransProg);

    // Increment the transition progress
    colTransProg += colTransSpeed;
    if (colTransProg >= 1.0f) {
        colTransProg = 0.0f;
        initCol1 = tarCol1;
        initCol2 = tarCol2;
        initCol3 = tarCol3;
        initCol4 = tarCol4;

        // Set new target colors
        tarCol1 = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
        tarCol2 = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
        tarCol3 = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
        tarCol4 = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
    }
}

void UpdateAnimation() {
    if (circleAnimate) {
        currentCircleSize += 0.5f; // Adjust the increment value for desired animation speed
        if (currentCircleSize >= 9.0f) {
            currentCircleSize = 9.0f;
            circleAnimate = false;
        }
    }
    if (polyAnimate) {
        currentPolySize -= 1.5f;
        if (currentPolySize <= 90.0f) {
            currentPolySize = 90.0f;
            polyAnimate = false;
        }
    }
}

void drawHex(Vector2 position, float rotation, float backwardsrotation, bool highlight, Color polyColor, Color lineColor, Color highlightColor) {
    if (highlight) {
        DrawRectangle(position.x - 5, position.y - 5, 10, 10, RED);
    }

    if (rotation > 0) {
        DrawPoly(position, 10, 80, backwardsrotation, highlightColor);
        DrawPoly(position, 6, 80, rotation, polyColor);
        DrawPolyLines(position, 6, currentPolySize, rotation, lineColor);
        DrawPolyLinesEx(position, 6, 85, rotation, 6, lineColor);
        
    } else {
        DrawPoly(position, 6, 80, 0, polyColor);
        DrawPolyLines(position, 6, 90, 0, lineColor);
        DrawPolyLinesEx(position, 6, 85, 0, 6, lineColor);
    }
}

void DrawMenuGradientCircle(int menuGenX, int menuGenY, int screenWidth) {
    int baseX = (menuGenX == 0) ? 80 : 380;
    int baseY = 80 + (menuGenY * 30);
    if (menuGenY == 12) {
        baseX = screenWidth / 2 - 110;
        baseY = 450;
    }
    DrawCircleGradient(baseX, baseY, currentCircleSize, chaoYellow, chaoPink);
}

void DrawTextOutlined(const char *text, int posX, int posY, int fontSize, Color textColor, Color outlineColor, int outlineThickness) {
    int lineHeight = fontSize + 4; // Adjust the line height as needed

    // Split the text into lines
    char buffer[1024]; // Buffer to hold the current line
    int lineStart = 0;
    int posYLine = posY;

    while (text[lineStart] != '\0') {
        // Find the end of the current line
        int lineEnd = lineStart;
        while (text[lineEnd] != '\n' && text[lineEnd] != '\0') {
            lineEnd++;
        }

        // Copy the current line to the buffer
        int length = lineEnd - lineStart;
        strncpy(buffer, &text[lineStart], length);
        buffer[length] = '\0';
        // Draw outline
        for (int dx = -outlineThickness; dx <= outlineThickness; dx++) {
            for (int dy = -outlineThickness; dy <= outlineThickness; dy++) {
                if (dx != 0 || dy != 0) {
                    DrawText(buffer, posX + dx, posYLine + dy, fontSize, outlineColor);
                }
            }
        }

        // Draw main text
        DrawText(buffer, posX, posYLine, fontSize, textColor);
        lineStart = (text[lineEnd] == '\0') ? lineEnd : lineEnd + 1;
        posYLine += lineHeight;
    }
}

void DrawIntSelection(const char* label, IntSelection *selection, Color textColor) {
    DrawTextOutlined(TextFormat("%s %i", label, selection->value), selection->rect.x + 5, selection->rect.y + 5, 20, textColor, RAYWHITE, 1.3f);
}

void DrawDropDownMenu(DropdownMenu *menu, Color activeColor, Color textColor) {
    // Draw the current selected item
    DrawRectangleRec(menu->rect, LIGHTGRAY);
    DrawText(menu->items[menu->selectedItem], menu->rect.x + 5, menu->rect.y + 5, 20, textColor);

    // Draw the dropdown items if the menu is active
    if (menu->active) {
        for (int i = 0; i < menu->itemCount; i++) {
            Rectangle itemRect = { menu->rect.x, menu->rect.y + (i + 1) * menu->rect.height, menu->rect.width, menu->rect.height };
            if (i == menu->selectedItem) {
                DrawRectangleRec(itemRect, activeColor);
                DrawRectangleRec(itemRect, YELLOW); // Highlight color
            } else {
                DrawRectangleRec(itemRect, activeColor);
            }
            DrawText(menu->items[i], itemRect.x + 5, itemRect.y + 5, 20, textColor);
        }
    }
}

float loadingBarWidth = 0.0f;
float barPosX = 640 / 2 - 100;
float loadingBarSpeed = 200.0f;
bool grow = true;

void GenDelAnimation() {
    if (grow) {
        loadingBarWidth += loadingBarSpeed * GetFrameTime();
        if (loadingBarWidth >= 200.0f) {
            loadingBarWidth = 200.0f;
            grow = false;
        }
    } else {
        loadingBarWidth -= loadingBarSpeed * GetFrameTime();
        barPosX += loadingBarSpeed * GetFrameTime();
        if (loadingBarWidth <= 0.0f) {
            loadingBarWidth = 0.0f;
            barPosX = 640 / 2 - 100;
            grow = true;
        }
    }
    
    DrawRectangle(barPosX, 432, (int)loadingBarWidth, 35, YELLOW);
}


static float fadeSpeed = 2.0f;
static float textPosX = 0.0f;
static bool textMove = false;


Vector2 menuPosition = {100, 75};
float menuWidth = 200;
float menuItemHeight = 30;

int main(int argc, char *argv[])
{
    const int screenWidth = 640;
    const int screenHeight = 480;
    InitWindow(screenWidth, screenHeight, "Debug");

    MenuScreen menuscreen = OPTIONS;
    int gamepad = 0;
    int menuX = 0;
    int menuY = 0;
    /// genmenu ///////////////
    int menuGenX = 0;
    int menuGenY = 0;

    int menuvolume = 250;

    float rotation = 0.0f;
    float backwardsrotation = 0.0f;

    float animationTime = 0.0f;
    const float waveSpeed = 4.0f;
    // Speed of the sinus wave animation

    // VMU SHIT//////////////////////////////////////////////////////////////////////////////////////////////
    //  Initialize VMU filesystem
    vmufs_init();

    // Get the first VMU device
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    // Check if VMU is found
    if (!vmu)
    {
        printf("No VMU found!\n");
        return 1;
    }

    // Open the source file
    FILE *file_vms = fopen("/rd/SONICADV.VMS", "rb");

    // Check if the source file is opened successfully
    if (!file_vms)
    {
        printf("Failed to open source file!\n");
        return 1;
    }

    // Get the file size
    fseek(file_vms, 0, SEEK_END);
    int size_vms = ftell(file_vms);
    rewind(file_vms);

    // Allocate memory for the file buffer
    uint8_t *buffer_vms = (uint8_t *)malloc(size_vms);

    // Read the source file into the buffer
    fread(buffer_vms, 1, size_vms, file_vms);

    // Close the source file
    fclose(file_vms);
    // VMU SHIT//////////////////////////////////////////////////////////////////////////////////////////////

    uint8_t volume = 240;
    snd_init();

    sfxhnd_t beep3 = snd_sfx_load("/rd/audio/button-40.wav");
    sfxhnd_t beep1 = snd_sfx_load("/rd/audio/button-34.wav");
    sfxhnd_t beep2 = snd_sfx_load("/rd/audio/button-41.wav");

    // - - - - Graphics - - - -
    Rectangle rec = { 0, 0, screenWidth * 2, screenHeight * 2 };
    Vector2 position = { 0, 0 };

    float time = 0.0f;
    float speed = 200.0f;
    int phase = 0;
    
    Color chaoBlue = (Color){73, 100, 239, 255};
    Color darkBlue = (Color){10, 80, 255, 255};
    currCol1 = Fade(chaoBlue, 0.6f);  // Almost fully transparent blue
	currCol2 = Fade((Color){0, 100, 50, 255}, 0.5f); // Half transparent green
	currCol3 = Fade((Color){0, 0, 100, 255}, 0.9f);   // Half transparent red
	currCol4 = darkBlue;          // Darker than background blue
	
	tarCol1 = Fade((Color){50, 110, 10, 255}, 0.5f);
	tarCol2 = Fade((Color){100, 0, 0, 255}, 0.3f);
	tarCol3 = Fade((Color){0, 100, 200, 255}, 0.1f);
	tarCol4 = Fade((Color){200, 200, 200, 255}, 0.5f);
	currCol1 = initCol1;
    currCol2 = initCol2;
    currCol3 = initCol3;
    currCol4 = initCol4;

    // - - - Animation - - -


    // Define the dropdown menus
    const char *JewelMenu[] = {"No Jewel", "Emerald", "Amythest", "Ruby", "Sapphire"};
    const char *ColorMenu[] = {"Normal Color", "Silver", "Gold", "Black", "Copper", "Onyx", "Jewel+Black"};
    const char *TypeMenu[] = {"Baby", "Adult", "Swim", "Fly", "Run", "Power", "Chaos"};
    const char *MedalMenu[] = {"No Medal", "Pearl", "Amethyst", "Sapphire", "Ruby", "Emerald"};

    DropdownMenu dropdownJewel = { {100, 65, 200, 30}, JewelMenu, sizeof(JewelMenu) / sizeof(JewelMenu[0]), 0, false };
    DropdownMenu dropdownColor = { {100, 95, 200, 30}, ColorMenu, sizeof(ColorMenu) / sizeof(ColorMenu[0]), 0, false };
    DropdownMenu dropdownType = { {100, 125, 200, 30}, TypeMenu, sizeof(TypeMenu) / sizeof(TypeMenu[0]), 0, false };
    DropdownMenu dropdownMedal = { {100, 155, 200, 30}, MedalMenu, sizeof(MedalMenu) / sizeof(MedalMenu[0]), 0, false };

    IntSelection Happiness = { {100, 220, 200, 30}, 0, false };
    IntSelection Reincarnations = { {100, 190, 200, 30}, 0, false };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        
        // - - - - Background logic - - - -
        time += 30 * GetFrameTime();
        
        switch (phase) {
            case 0:
                position.y -= speed * GetFrameTime();
                if (position.y <= -rec.height + screenHeight) {
                    position.y = -rec.height + screenHeight;
                    phase = 1;
                }
                break;
            case 1:
                position.x -= speed * GetFrameTime();
                if (position.x <= -rec.width + screenWidth) {
                    position.x = -rec.width + screenWidth;
                    phase = 2;
                }
                break;
            case 2:
                position.y += speed * GetFrameTime();
                if (position.y >= 0) {
                    position.y = 0;
                    phase = 3;
                }
                break;
            case 3:
                position.x += speed * GetFrameTime();
                if (position.x >= 0) {
                    position.x = 0;
                    phase = 0;
                    
                }
                UpdateGradientColors();
                break;
        }
        // - - - - - - - -
        static int previousMenuGenX = 1;
        static int previousMenuGenY = 1;
        static int prevMenuX = 1;
        static int prevMenuY = 1;
        if (menuX != prevMenuX || menuY != prevMenuY) {
            currentPolySize = 135.0f;
            polyAnimate = true;
            prevMenuX = menuX;
            prevMenuY = menuY;
        }
        if (menuGenX != previousMenuGenX || menuGenY != previousMenuGenY) {
            currentCircleSize = 1.0f;
            circleAnimate = true;
            previousMenuGenX = menuGenX;
            previousMenuGenY = menuGenY;
        }

        UpdateAnimation();
        // - - - - - - - -

        animationTime += GetFrameTime() * waveSpeed;

        rotation += 0.8f;
        backwardsrotation -= 0.8;

        switch (menuscreen)
        {

        case OPTIONS:
        {
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) || IsKeyDown(KEY_W))
            {
                snd_sfx_play(beep3, menuvolume, CENTER);

                menuY--;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || IsKeyDown(KEY_S))
            {
                snd_sfx_play(beep3, menuvolume, CENTER);
                menuY++;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A))
            {
                snd_sfx_play(beep3, menuvolume, CENTER);
                menuX--;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D))
            {
                snd_sfx_play(beep3, menuvolume, CENTER);
                menuX++;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyReleased(KEY_SPACE))
            {
                snd_sfx_play(beep1, menuvolume, CENTER);

                if (menuX == 0 && menuY == 0)
                {
                    menuX = 0;
                    menuY = 0;
                    menuscreen = DEFAULTCHAO;
                }
                if (menuX == 1 && menuY == 0)
                {
                    menuX = 0;
                    menuY = 0;
                    menuscreen = GENERATECHAO;
                }
                if (menuX == 0 && menuY == 1)
                {
                    menuX = 0;
                    menuY = 0;
                    menuscreen = DELETE;
                }
                if (menuX == 1 && menuY == 1)
                {
                    menuX = 0;
                    menuY = 0;
                    menuscreen = OPTIONS;
                }
            }

            if (menuX < 0)
            {
                menuX = 0;
            }
            if (menuX > 1)
            {
                menuX = 1;
            }
            if (menuY > 1)
            {
                menuY = 1;
            }
            if (menuY < 0)
            {
                menuY = 0;
            }
            break;
        }
        case DEFAULTCHAO:
        {
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A))
            {
                snd_sfx_play(beep3, volume, CENTER);
                menuX--;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D))
            {
                snd_sfx_play(beep3, volume, CENTER);
                menuX++;
            }

            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyReleased(KEY_SPACE))
            {
                snd_sfx_play(beep2, volume, CENTER);

                menuscreen = OPTIONS;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyReleased(KEY_SPACE))
            {
                snd_sfx_play(beep1, menuvolume, CENTER);

                if (menuX == 0 && menuY == 0)
                {
                    menuX = 0;
                    menuY = 0;

                    float delayTime = 0.5f;
                    float currentTime = GetTime();
                    while (GetTime() - currentTime < delayTime)
                    {
                        // Update the screen during the delay
                        BeginDrawing();
                        ClearBackground(chaoBlue);
                        GenDelAnimation();
                        DrawText("Saving chao...", screenWidth / 2 - 100, screenHeight / 2, 22, BLACK);
                        EndDrawing();
                    }

                    int status_vms = vmufs_write(vmu, "SONICADV", buffer_vms, size_vms, VMUFS_OVERWRITE | VMUFS_VMUGAME);
                    free(buffer_vms);

                    if (status_vms == 0)
                    {
                        
                        menuscreen = COMPLETE;
                    }
                    else
                    {
                        menuscreen = FAILURE;
                    }
                }
                if (menuX == 1 && menuY == 0)
                {
                    GenDelAnimation();
                    menuX = 0;
                    menuY = 0;
                    if (size_vms > 0x3000)
                    {
                        buffer_vms[0x3000] = 0x06;
                    }
                    // Close the source file
                    fclose(file_vms);
                    int status_vms = vmufs_write(vmu, "SONICADV", buffer_vms, size_vms, VMUFS_OVERWRITE | VMUFS_VMUGAME);

                    if (status_vms == 0)
                    {
                        menuscreen = COMPLETE;
                    }
                    else
                    {
                        menuscreen = FAILURE;
                    }
                }
            }

            if (menuX < 0)
            {
                menuX = 0;
            }
            if (menuX > 1)
            {
                menuX = 1;
            }
            if (menuY > 1)
            {
                menuY = 1;
            }
            if (menuY < 0)
            {
                menuY = 0;
            }

            break;
        }
        case GENERATECHAO:
        {

            if (!dropdownJewel.active && !dropdownColor.active && !dropdownType.active && !dropdownMedal.active && !Reincarnations.active  && !Happiness.active)
            {
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) || IsKeyDown(KEY_W))
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuGenY--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || IsKeyDown(KEY_S))
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuGenY++;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A))
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuGenX--;
                }
                if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D))
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    menuGenX++;
                }
            }

            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) || IsKeyDown(KEY_W))
            {

                if (dropdownJewel.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    dropdownJewel.selectedItem = (dropdownJewel.selectedItem - 1 + dropdownJewel.itemCount) % dropdownJewel.itemCount;
                }

                if (dropdownColor.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    dropdownColor.selectedItem = (dropdownColor.selectedItem - 1 + dropdownColor.itemCount) % dropdownColor.itemCount;
                }

                if (dropdownType.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    dropdownType.selectedItem = (dropdownType.selectedItem - 1 + dropdownType.itemCount) % dropdownType.itemCount;
                }

                if (dropdownMedal.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    dropdownMedal.selectedItem = (dropdownMedal.selectedItem - 1 + dropdownMedal.itemCount) % dropdownMedal.itemCount;
                }

                if (Reincarnations.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    Reincarnations.value = (Reincarnations.value + 1);
                    if (Reincarnations.value > 255){
                        Reincarnations.value = 255;
                    }
                    
                }
                if (Happiness.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    Happiness.value = (Happiness.value + 1);
                    if (Happiness.value > 100){
                        Happiness.value = 100;
                    }
                    
                }

                
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || IsKeyDown(KEY_S))
            {
                snd_sfx_play(beep3, volume, CENTER);
                if (dropdownJewel.active)
                {
                    dropdownJewel.selectedItem = (dropdownJewel.selectedItem + 1) % dropdownJewel.itemCount;
                }

                if (dropdownColor.active)
                {
                    dropdownColor.selectedItem = (dropdownColor.selectedItem + 1) % dropdownColor.itemCount;
                }

                if (dropdownType.active)
                {
                    dropdownType.selectedItem = (dropdownType.selectedItem + 1) % dropdownType.itemCount;
                }

                 if (dropdownMedal.active)
                {
                    dropdownMedal.selectedItem = (dropdownMedal.selectedItem + 1) % dropdownMedal.itemCount;
                }

                 if (Reincarnations.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    Reincarnations.value = (Reincarnations.value - 1);
                    if (Reincarnations.value < 0){
                        Reincarnations.value = 0;
                    }
                }
                 if (Happiness.active)
                {
                    snd_sfx_play(beep3, volume, CENTER);
                    Happiness.value = (Happiness.value - 1);
                    if (Happiness.value < -100){
                        Happiness.value = -100;
                    }
                }

            }

            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyReleased(KEY_SPACE))
            {
                snd_sfx_play(beep2, volume, CENTER);

                if (menuGenX == 0 && menuGenY == 0)
                {
                    dropdownJewel.active = !dropdownJewel.active;
                }

                if (menuGenX == 0 && menuGenY == 1)
                {
                    dropdownColor.active = !dropdownColor.active;
                }

                if (menuGenX == 0 && menuGenY == 2)
                {
                    dropdownType.active = !dropdownType.active;
                }
                if (menuGenX == 0 && menuGenY == 3)
                {
                     dropdownMedal.active = !dropdownMedal.active;
                }
                if (menuGenX == 0 && menuGenY == 4)
                {
                     Reincarnations.active = !Reincarnations.active;
                }
                 if (menuGenX == 0 && menuGenY == 5)
                {
                     Happiness.active = !Happiness.active;
                }

                if ((menuGenX == 1 && menuGenY == 12) || (menuGenX == 0 && menuGenY == 12))
                {
                    if (strcmp(dropdownJewel.items[dropdownJewel.selectedItem], "Amythest") == 0)
                    {
                        buffer_vms[0x3036] = 0x40;
                        buffer_vms[0x3192] = 0x00;
                    }
                    if (strcmp(dropdownJewel.items[dropdownJewel.selectedItem], "Emerald") == 0)
                    {
                        buffer_vms[0x3036] = 0x40;
                        buffer_vms[0x3192] = 0x01;
                    }
                    if (strcmp(dropdownJewel.items[dropdownJewel.selectedItem], "Ruby") == 0)
                    {
                        buffer_vms[0x3036] = 0x40;
                        buffer_vms[0x3192] = 0x02;
                    }
                    if (strcmp(dropdownJewel.items[dropdownJewel.selectedItem], "Sapphire") == 0)
                    {
                        buffer_vms[0x3036] = 0x40;
                        buffer_vms[0x3192] = 0x03;
                    }

                    
                    if (strcmp(dropdownColor.items[dropdownColor.selectedItem], "Silver") == 0)
                    {
                        buffer_vms[0x3036] = 0x08;
                    }
                    if (strcmp(dropdownColor.items[dropdownColor.selectedItem], "Gold") == 0)
                    {
                        buffer_vms[0x3036] = 0x04;
                    }
                    if (strcmp(dropdownColor.items[dropdownColor.selectedItem], "Black") == 0)
                    {
                        buffer_vms[0x3036] = 0x02;
                    }
                    if (strcmp(dropdownColor.items[dropdownColor.selectedItem], "Copper") == 0)
                    {
                        buffer_vms[0x3036] = 0x06;
                    }
                    if (strcmp(dropdownColor.items[dropdownColor.selectedItem], "Onyx") == 0)
                    {
                        buffer_vms[0x3036] = 0x0A;
                    }
                    if (strcmp(dropdownColor.items[dropdownColor.selectedItem], "Jewel+Black") == 0)
                    {
                        buffer_vms[0x3036] = 0x40;
                        buffer_vms[0x3036] = 0x42;
                    }





                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Baby") == 0)
                    {
                        buffer_vms[0x3000] = 0x00;
                    }
                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Adult") == 0)
                    {
                        buffer_vms[0x3000] = 0x01;
                    }
                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Swim") == 0)
                    {
                        buffer_vms[0x3000] = 0x02;
                    }
                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Fly") == 0)
                    {
                        buffer_vms[0x3000] = 0x03;
                    }
                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Run") == 0)
                    {
                        buffer_vms[0x3000] = 0x04;
                    }
                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Power") == 0)
                    {
                        buffer_vms[0x3000] = 0x05;
                    }
                    if (strcmp(dropdownType.items[dropdownType.selectedItem], "Chaos") == 0)
                    {
                        buffer_vms[0x3000] = 0x06;
                    }



                       if (strcmp(dropdownMedal.items[dropdownMedal.selectedItem], "Pearl") == 0)
                    {
                        buffer_vms[0x3034] = 0x01;
                    }
                    if (strcmp(dropdownMedal.items[dropdownMedal.selectedItem], "Amethyst") == 0)
                    {
                        buffer_vms[0x3034] = 0x02;
                    }
                    if (strcmp(dropdownMedal.items[dropdownMedal.selectedItem], "Sapphire") == 0)
                    {
                        buffer_vms[0x3034] = 0x04;
                    }
                    if (strcmp(dropdownMedal.items[dropdownMedal.selectedItem], "Ruby") == 0)
                    {
                        buffer_vms[0x3034] = 0x08;
                    }
                    if (strcmp(dropdownMedal.items[dropdownMedal.selectedItem], "Emerald") == 0)
                    {
                        buffer_vms[0x3034] = 0x10;
                    }

                    //HEX HEXIDECIMAL

                    if (Happiness.value >= 0){
                    uint32_t HappinessHex = int_to_hex(Happiness.value);
                    buffer_vms[0x3002] = HappinessHex;
                    }
                    if (Happiness.value < 0){
                        Happiness.value = (Happiness.value + 255);
                    uint32_t HappinessHex = int_to_hex(Happiness.value);
                    buffer_vms[0x3002] = HappinessHex;
                    }

                    
                    
                    uint32_t ReincarnationHex = int_to_hex(Reincarnations.value);
                    buffer_vms[0x3178] = ReincarnationHex;


                   
                   
                   


                    fclose(file_vms);
                    int status_vms = vmufs_write(vmu, "SONICADV", buffer_vms, size_vms, VMUFS_OVERWRITE | VMUFS_VMUGAME);
                    menuscreen = COMPLETE;
                }
            }

            

            if (menuGenX < 0)
            {
                menuGenX = 0;
            }
            if (menuGenX > 1)
            {
                menuGenX = 1;
            }
            if (menuGenY > 12)
            {
                menuGenY = 12;
            }
            if (menuGenY < 0)
            {
                menuGenY = 0;
            }

            break;
        }
        case DELETE:
        {

            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyDown(KEY_A))
            {
                snd_sfx_play(beep3, volume, CENTER);
                menuX--;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyDown(KEY_D))
            {
                snd_sfx_play(beep3, volume, CENTER);
                menuX++;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyReleased(KEY_SPACE))
            {
                snd_sfx_play(beep2, volume, CENTER);
                menuscreen = OPTIONS;
            }
            if (IsGamepadButtonReleased(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyReleased(KEY_SPACE))
            {
                snd_sfx_play(beep1, menuvolume, CENTER);

                if (menuX == 1 && menuY == 0)
                {
                    menuX = 0;
                    menuY = 0;

                    // Read the VMU directory
                    vmu_dir_t *dir;
                    int dir_count;
                    if (vmufs_readdir(vmu, &dir, &dir_count) >= 0)
                    {
                        // Delete each file in the directory
                        for (int i = 0; i < dir_count; i++)
                        {
                            vmufs_delete(vmu, dir[i].filename);
                        }
                        free(dir);
                    }

                    float delayTime = 0.5f;
                    float currentTime = GetTime();
                    while (GetTime() - currentTime < delayTime)
                    {
                        // Update the screen during the delay
                        BeginDrawing();
                        ClearBackground(chaoBlue);
                        GenDelAnimation();
                        DrawText("Deleting files...", screenWidth / 2 - 100, screenHeight / 2, 22, BLACK);
                        EndDrawing();
                    }
                }

                if (menuX == 0 && menuY == 0)
                {
                    menuX = 0;
                    menuY = 0;
                    menuscreen = OPTIONS;
                }
            }

            if (menuX < 0)
            {
                menuX = 0;
            }
            if (menuX > 1)
            {
                menuX = 1;
            }
            if (menuY > 1)
            {
                menuY = 1;
            }
            if (menuY < 0)
            {
                menuY = 0;
            }

            break;
        }

        case COMPLETE:
        {

            break;
        }
        case FAILURE:
        {

            break;
        }
        }

        BeginDrawing();
        ClearBackground(chaoBlue);
        // - - Background Rectangle - -
        DrawRectangleGradientEx((Rectangle){position.x, position.y, rec.width, rec.height}, currCol1, currCol2, currCol3, currCol4);

        switch (menuscreen)
        {
        case OPTIONS:
        {

            Vector2 optionPositions[4] = {
                {150, 150},
                {470, 150},
                {470, 375},
                {150, 375}
            };

            // Draw static hexagons
            for (int i = 0; i < 4; i++) {
                drawHex(optionPositions[i], 0, 0, false, chaoYellow, chaoPink, DARKPURPLE);
            }

            // Center hexagon
            drawHex((Vector2){310, 260}, 0, 0, false, chaoYellow, chaoPink, DARKPURPLE);

            // Handle selection and rotation based on menuX and menuY
            if (menuX == 0 && menuY == 0) {
                drawHex(optionPositions[0], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }
            if (menuX == 1 && menuY == 0) {
                drawHex(optionPositions[1], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }
            if (menuX == 1 && menuY == 1) {
                drawHex(optionPositions[2], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }
            if (menuX == 0 && menuY == 1) {
                drawHex(optionPositions[3], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }

            DrawTextOutlined("Default Chao", 100, 170, 22, BLACK, RAYWHITE, 1.2f);
            DrawTextOutlined("Generate Chao", 420, 170, 22, BLACK, RAYWHITE, 1.3f);
            DrawTextOutlined("Options", 420, 395, 22, BLACK, RAYWHITE, 1.3f);
            DrawTextOutlined("DELETE", 100, 395, 22, BLACK, RAYWHITE, 1.3f);

            // Draw FPS counter
            int fps = GetFPS();
            char fpsText[20];
            sprintf(fpsText, "FPS: %d", fps);
            DrawTextOutlined(fpsText, 30, 30, 20, BLACK, RAYWHITE, 1.3f);
            break;
        }

        case DEFAULTCHAO:
        {
            Vector2 defaultChaoPos[2] = {
                {150, 150},
                {470, 150}
            };

            // Draw static hexagons
            for (int i = 0; i < 2; i++) {
                drawHex(defaultChaoPos[i], 0, 0, false, chaoYellow, chaoPink, DARKPURPLE);
            }

            // Center hexagon
            drawHex((Vector2){310, 260}, 0, 0, false, chaoYellow, chaoPink, DARKPURPLE);

            if (menuX == 0 && menuY == 0)
            {
                drawHex(defaultChaoPos[0], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }

            if (menuX == 1 && menuY == 0)
            {
                drawHex(defaultChaoPos[1], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }

            DrawTextOutlined("Save \nDefault Chao", 100, 148, 22, BLACK, RAYWHITE, 1.3f);
            DrawTextOutlined("Save \nChaos Chao", 420, 148, 22, BLACK, RAYWHITE, 1.3f);

            // Draw FPS counter
            int fps = GetFPS();
            char fpsText[20];
            sprintf(fpsText, "FPS: %d", fps);
            DrawTextOutlined(fpsText, 30, 30, 20, BLACK, RAYWHITE, 1.3f);
            break;
        }

        case GENERATECHAO:
        {

            // chaogen // You must order them backwards- so menuGenY 0 should be on BOTTOM ////

            if (Happiness.active) DrawRectangleRec(Happiness.rect, YELLOW);
            if (Reincarnations.active) DrawRectangleRec(Reincarnations.rect, YELLOW);
            DrawIntSelection("Happiness:", &Happiness, BLACK);
            DrawIntSelection("Reincarnations:", &Reincarnations, BLACK);

            DrawDropDownMenu(&dropdownMedal, chaoPink, BLACK);
            DrawDropDownMenu(&dropdownType, chaoPink, BLACK);
            DrawDropDownMenu(&dropdownColor, chaoPink, BLACK);
            DrawDropDownMenu(&dropdownJewel, chaoPink, BLACK);
            
            for (int y = 0; y <= 12; y++) {
                for (int x = 0; x <= 1; x++) {
                    if (menuGenY == y && menuGenX == x) {
                        DrawMenuGradientCircle(x, y, screenWidth);
                    }
                }
            }

            DrawRectangle(screenWidth / 2 - 100, 432, 200, 35, chaoPink);
            if ((menuGenX == 1 && menuGenY == 12) || (menuGenX == 0 && menuGenY == 12)) {
                GenDelAnimation();
            }
            DrawTextOutlined("Generate Chao!", screenWidth / 2 - 85, 440, 22, BLACK, RAYWHITE, 1.3f);
            // Draw FPS counter
            int fps = GetFPS();
            char fpsText[20];
            sprintf(fpsText, "FPS: %d", fps);
            DrawTextOutlined(fpsText, 30, 30, 20, BLACK, RAYWHITE, 1.3f);
            break;
        }
        case DELETE:
        {

            Vector2 deletePos[2] = {
                {150, 150},
                {470, 150}
            };

            // Draw static hexagons
            for (int i = 0; i < 2; i++) {
                drawHex(deletePos[i], 0, 0, false, chaoYellow, chaoPink, DARKPURPLE);
            }

            // Center hexagon
            drawHex((Vector2){310, 260}, 0, 0, false, chaoYellow, chaoPink, DARKPURPLE);

            if (menuX == 0 && menuY == 0)
            {
                 drawHex(deletePos[0], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }

            if (menuX == 1 && menuY == 0)
            {
                 drawHex(deletePos[1], rotation, backwardsrotation, true, chaoYellow, chaoPink, DARKPURPLE);
            }

            DrawTextOutlined("WARNING!! This will delete all of your VMU data", 60, (screenHeight / 2) + 20, 22, BLACK, RED, 1.6f);
            DrawTextOutlined("Back", 100, 170, 22, BLACK, RAYWHITE, 1.3f);
            DrawTextOutlined("Wipe VMU", 420, 170, 22, BLACK, RAYWHITE, 1.3f);

            // Draw FPS counter
            int fps = GetFPS();
            char fpsText[20];
            sprintf(fpsText, "FPS: %d", fps);
            DrawTextOutlined(fpsText, 30, 30, 20, BLACK, RAYWHITE, 1.3f);
            break;
        }

        case COMPLETE:
        {
            textMove = true;
            fadeSpeed -= GetFrameTime() * 0.11f;
            if (fadeSpeed <= 0.0f) {
                fadeSpeed = 0.0f;
            }
            if (textMove) {
                textPosX += GetFrameTime() * 6.0f;
                if (textPosX >= 100.0f) {
                    textPosX = 100.0f;
                    textMove = false;
                }
            }
            DrawTextOutlined("Transfer Complete!", screenWidth / 2 - textPosX, screenHeight / 2, 22, BLACK, RAYWHITE, 1.3f);
            DrawRectangle(0,0, screenWidth, screenHeight, LerpColor(BLANK, RAYWHITE, fadeSpeed));
            

            break;
        }
        case FAILURE:
        {
            DrawTextOutlined("Transfer Failed!", screenWidth / 2, screenHeight / 2, 22, BLACK, RAYWHITE, 1.3f);
            break;
        }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}