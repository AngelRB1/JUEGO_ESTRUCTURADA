#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

// CONSTANTE PARA EL JUEGO
#define GRAVEDAD 1200
#define SALTO_VELOCIDAD 600.0f
#define VELOCIDAD_HORIZONTAL 300.0f
#define AlTURA_SUELO 50
#define CAMERA_SPEED 0.05f

typedef struct Jugador
{
    Rectangle rect;
    Vector2 velocidad;
    bool enSuelo;
    int estado; // 0: Descansando, 1: Saltando, 2: Caminando, 3: Muerte;
    bool mirandoDerecha;
    Texture2D descanso[4];
    Texture2D caminar[4];
    Texture2D salto[5];
    Texture2D muerte[4];
} Jugador;

typedef struct
{
    Texture2D *textures;
    int frameCount;
    int currentFrame;
    float frameDuration;
    float frameTime;
} TextureAnimation;

typedef enum PantallaJuego
{
    MENU = 0,
    NIVELES,
    SALIR,
    NIVELEASY,
    NIVELNORMAL,
    PERSONALIZACION
} PantallaJuego;

typedef struct
{
    float x;   // Posición x
    float y;   // Posición y
    int type;  // Tipo de plataforma (1 = plataforma de suelo, 2 = plataforma horizontal, 3 = plataforma sola, 4 = plataforma vertical)
    char part; // Parte de la plataforma ('b' = base, 'i' = esquina izquierda, 'd' = esquina derecha, 'c' = centro, 'l' = centro izquierda, 'r' = centro derecha, 's' = suelo base, 'h' = suelo izquierda, 'm' = suelo derecha)
} Plataforma;

typedef struct EnvItem
{
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;

float volumenActual = 0.5f; // Valor inicial de volumen (por ejemplo, la mitad del volumen máximo)
Texture2D descanso[3][4];
Texture2D caminar[3][4];
Texture2D salto[3][5];
Texture2D muerte[3][4];

Color cafeOscuro = {139, 69, 19, 255};
Color cafeOscuroB = {121, 55, 8, 255};
Color Rojos = {192, 6, 4, 255};
Color Verde = {38, 76, 13, 0.8};

// Declaraciones de funciones
void DibujarMenu(PantallaJuego *pantallaActual, Font font, Sound sound2, Sound *soundmenu);
void DibujarPreguntas(Font font);
void DibujarNiveles(PantallaJuego *pantallaActual, Font font, Sound sound2, Sound *soudnmenu);
void DibujarNE(PantallaJuego *pantallaActual, Font font, Sound sound2);
void DibujarNN(PantallaJuego *pantallaActual, Font font, Sound sound2);
void DibujarNH(PantallaJuego *pantallaActual, Font font, Sound sound2);
void DibujarVolumen(Font font, Sound sound2, Sound soundmenu);
void ActualizarJugador(Jugador *jugador, EnvItem *envItems, int envItemsLength, float delta);
void DibujarPersonalizacion(PantallaJuego *pantallaActual, Jugador *jugador, Texture2D descanso[][4], Texture2D caminar[][4], Texture2D salto[][5], Texture2D muerte[][4], int numAvatares, Font font, Texture2D fondo);
// void sonidoSalto(Sound *sonidoSalto);
void VerificarCargaTexture(Texture2D texture, const char *nombreArchivo);
void sonido2(Sound *sound2);
void sonidomenu(Sound *soundmenu);
void limpiarRecursos(Texture2D texture, Texture2D texture2, Texture2D texture3, Font font, Sound sound, Sound sound2, Sound soundmenu);

// Funcion para plataformas:
void DrawPlatform(float posX, float posY, int platformType, char platformPart,
                  Texture2D baseTexture, Texture2D esquinaIzqTexture, Texture2D esquinaDerTexture,
                  Texture2D centroBaseTexture, Texture2D centroIzqTexture, Texture2D centroDerTexture,
                  Texture2D sueloBaseTexture, Texture2D sueloIzqTexture, Texture2D sueloDerTexture,
                  Texture2D baseTexture2, Texture2D esquinaIzqTexture2, Texture2D esquinaDerTexture2,
                  Texture2D baseTexture3, Texture2D baseTexture4, Texture2D centroTexture, Texture2D abajoTexture);
void InvertVerticalPlatformBase(float posX, float posY, Texture2D baseTexture4);

// Add this near the top of the file, with other global declarations
TextureAnimation animaciones[4];

int main()
{
    const int screenWidth = 1000;
    const int screenHeight = 500;

    InitWindow(screenWidth, screenHeight, "SHADOW DASH");

    // Variables del programa
    Image image = LoadImage("logo2.png");
    Font font = LoadFont("The_Last_Shuriken.otf");

    // Escalar la imagen al tamaño de la ventana
    const float scaleFactor = 0.5f;
    ImageResize(&image, (int)(image.width * scaleFactor), (int)(image.height * scaleFactor));

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    float alpha = 0.0f;
    const float fadeSpeed = 1.0f / (12.0f * 60.0f); // 12 segundos de desvanecimiento a 60 FPS

    InitAudioDevice();

    Sound sound = LoadSound("imu.wav");

    PlaySound(sound);

    PantallaJuego pantallaActual = MENU;

    // Animación de desvanecimiento al inicio de la aplicación
    while (alpha < 1.0f && !WindowShouldClose())
    {
        alpha += fadeSpeed;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
        DrawTexturePro(texture, (Rectangle){0.0f, 0.0f, (float)texture.width, (float)texture.height},
                       (Rectangle){screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2,
                                   (float)texture.width, (float)texture.height},
                       (Vector2){0, 0}, 0.0f, Fade(WHITE, alpha));
        EndDrawing();
    }

    // Esperar 2 segundos antes de hacer que la imagen desaparezca
    WaitTime(2.0f);

    // Animación de desvanecimiento para hacer que la imagen desaparezca
    while (alpha > 0.0f && !WindowShouldClose())
    {
        alpha -= fadeSpeed;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
        DrawTexturePro(texture, (Rectangle){0.0f, 0.0f, (float)texture.width, (float)texture.height},
                       (Rectangle){screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2,
                                   (float)texture.width, (float)texture.height},
                       (Vector2){0, 0}, 0.0f, Fade(WHITE, alpha));
        EndDrawing();
    }

    // Reproducir sonido de carga
    Sound sound2;
    sonido2(&sound2);

    Sound soundmenu;
    soundmenu = LoadSound("soundmenu.wav");

    // Mostrar "Cargando..." en un fondo negro
    float loadingTime = 6.0f; // 6 segundos de pantalla de carga
    double startTime = GetTime();
    while ((GetTime() - startTime < loadingTime) && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(font, "C a r g a n d o . . .", (Vector2){screenWidth / 2 - MeasureTextEx(font, "C a r g a n d o . . .", 50, 0).x / 2, screenHeight / 2 - 10}, 50, 0, WHITE);
        EndDrawing();
    }

    // Cargar la textura para "menu.png"
    Image image2 = LoadImage("menu.png");
    Texture2D texture2 = LoadTextureFromImage(image2);
    UnloadImage(image2);

    Image image4 = LoadImage("personalizacion.png");
    Texture2D fondo = LoadTextureFromImage(image4);
    UnloadImage(image4);

    // Carga la textura para "Niveles"
    Image image3 = LoadImage("Niveles.png");
    int nuevaAncho = 1000, nuevoAltura = 500;
    ImageResize(&image3, nuevaAncho, nuevoAltura);
    Texture2D texture3 = LoadTextureFromImage(image3);
    UnloadImage(image3);

    // Variables para NIVELEASY
    Texture2D baseTexture = LoadTexture("Sprite-p_base.png");
    Texture2D esquinaIzqTexture = LoadTexture("Sprite-p_esquinaIZQ.png");
    Texture2D esquinaDerTexture = LoadTexture("Sprite-p_esquinaDER.png");
    Texture2D centroBaseTexture = LoadTexture("Sprite-p_centrobase.png");
    Texture2D centroIzqTexture = LoadTexture("Sprite-p_centroIZQ.png");
    Texture2D centroDerTexture = LoadTexture("Sprite-p_centroDER.png");
    Texture2D sueloBaseTexture = LoadTexture("Sprite-p_suelobase.png");
    Texture2D sueloIzqTexture = LoadTexture("Sprite-p_sueloIZQ.png");
    Texture2D sueloDerTexture = LoadTexture("Sprite-p_sueloDER.png");
    Texture2D baseTexture2 = LoadTexture("Sprite-p2_base.png");
    Texture2D esquinaIzqTexture2 = LoadTexture("Sprite-p2_esquinaIZQ.png");
    Texture2D esquinaDerTexture2 = LoadTexture("Sprite-p2_esquinaDER.png");
    Texture2D baseTexture3 = LoadTexture("Sprite-p3_base.png");
    Texture2D baseTexture4 = LoadTexture("Sprite-p4_base.png");
    Texture2D centroTexture = LoadTexture("Sprite-p4_centro.png");
    Texture2D abajoTexture = LoadTexture("Sprite-p4_abajo.png");

    // Cargar las texturas de los avatares
    descanso[0][0] = LoadTexture("Sprite-descanso01.png");
    descanso[0][1] = LoadTexture("Sprite-descanso02.png");
    descanso[0][2] = LoadTexture("Sprite-descanso03.png");
    descanso[0][3] = LoadTexture("Sprite-descanso04.png");
    caminar[0][0] = LoadTexture("Sprite-caminar01.png");
    caminar[0][1] = LoadTexture("Sprite-caminar02.png");
    caminar[0][2] = LoadTexture("Sprite-caminar01.png");
    caminar[0][3] = LoadTexture("Sprite-caminar04.png");
    salto[0][0] = LoadTexture("Sprite-salto01.png");
    salto[0][1] = LoadTexture("Sprite-salto02.png");
    salto[0][2] = LoadTexture("Sprite-salto03.png");
    salto[0][3] = LoadTexture("Sprite-salto04.png");
    salto[0][4] = LoadTexture("Sprite-salto05.png");
    muerte[0][0] = LoadTexture("Sprite-muerte01.png");
    muerte[0][1] = LoadTexture("Sprite-muerte02.png");
    muerte[0][2] = LoadTexture("Sprite-muerte03.png");
    muerte[0][3] = LoadTexture("Sprite-muerte04.png");

    descanso[1][0] = LoadTexture("SPC1.png");
    descanso[1][1] = LoadTexture("SPD2.png");
    descanso[1][2] = LoadTexture("SPD3.png");
    descanso[1][3] = LoadTexture("SPC1.png");
    caminar[1][0] = LoadTexture("SPC1.png");
    caminar[1][1] = LoadTexture("SPC2.png");
    caminar[1][2] = LoadTexture("SPC1.png");
    caminar[1][3] = LoadTexture("SPC4.png");
    salto[1][0] = LoadTexture("SPC1.png");
    salto[1][1] = LoadTexture("SPS2.png");
    salto[1][2] = LoadTexture("SPS3.png");
    salto[1][3] = LoadTexture("SPS4.png");
    salto[1][4] = LoadTexture("SPS5.png");

    descanso[2][0] = LoadTexture("SPRC1.png");
    descanso[2][1] = LoadTexture("SPRD2.png");
    descanso[2][2] = LoadTexture("SPRD3.png");
    descanso[2][3] = LoadTexture("SPRC1.png");
    caminar[2][0] = LoadTexture("SPRC1.png");
    caminar[2][1] = LoadTexture("SPRC2.png");
    caminar[2][2] = LoadTexture("SPRC1.png");
    caminar[2][3] = LoadTexture("SPRC4.png");
    salto[2][0] = LoadTexture("SPRC1.png");
    salto[2][1] = LoadTexture("SPRS2.png");
    salto[2][2] = LoadTexture("SPRS3.png");
    salto[2][3] = LoadTexture("SPRS4.png");
    salto[2][4] = LoadTexture("SPRS5.png");

    VerificarCargaTexture(descanso[0][0], "Sprite-descanso01.png");

    // Initialize animaciones
    animaciones[0] = (TextureAnimation){descanso[0], 4, 0, 0.25f, 0.0f}; // Animación de descanso
    animaciones[1] = (TextureAnimation){salto[0], 5, 0, 0.2f, 0.0f};     // Animación de salto
    animaciones[2] = (TextureAnimation){caminar[0], 4, 0, 0.1f, 0.0f};   // Animación de caminar
    animaciones[3] = (TextureAnimation){muerte[0], 4, 0, 0.3f, 0.0f};    // Animación de muerte

    // Definir las plataformas
    EnvItem envItems[] = {
        {{0, 400, 1000, 200}, 1, GRAY},
        {{300, 200, 400, 10}, 1, GRAY},
        {{250, 300, 100, 10}, 1, GRAY},
        {{650, 300, 100, 10}, 1, GRAY}};
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    // Inicializar el jugador
    // Inicializar el jugador
    Jugador jugador;
    for (int i = 0; i < 4; i++)
    {
        jugador.descanso[i] = descanso[0][i];
        jugador.caminar[i] = caminar[0][i];
        jugador.muerte[i] = muerte[0][i];
    }
    for (int i = 0; i < 5; i++)
    {
        jugador.salto[i] = salto[0][i];
    }
    jugador.velocidad = (Vector2){0, 0};
    jugador.enSuelo = true;
    jugador.estado = 0;            // Inicialmente descansando
    jugador.mirandoDerecha = true; // Inicialmente mirando a la derecha
    jugador.rect = (Rectangle){screenWidth / 2 - descanso[0][0].width * 3 / 2, 400 - descanso[0][0].height * 3, descanso[0][0].width * 3, descanso[0][0].height * 3};

    // Establecer objetivo de frames por segundo
    SetTargetFPS(60);

    // Bucle principal del juego
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch (pantallaActual)
        {
        case MENU:
            DrawTexture(texture2, 0, 0, WHITE);
            DibujarMenu(&pantallaActual, font, sound2, &soundmenu);
            DibujarVolumen(font, sound2, soundmenu);
            break;

        case NIVELES:
            DrawTexture(texture3, 0, 0, WHITE);
            DibujarNiveles(&pantallaActual, font, sound2, &soundmenu);
            DibujarVolumen(font, sound2, soundmenu);
            break;

        case NIVELEASY:
            StopSound(soundmenu);

            // Lógica de actualización de animación y movimiento
            float deltaTime = GetFrameTime();

            Camera2D camera = {0};
            camera.target = (Vector2){jugador.rect.x + jugador.rect.width / 2, jugador.rect.y + jugador.rect.height / 2};

            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
            camera.rotation = 0.0f;
            camera.zoom = 1.0f;
            // Actualizar la cámara para seguir al jugador
            camera.target = (Vector2){jugador.rect.x + jugador.rect.width / 2, jugador.rect.y + jugador.rect.height / 2};

            BeginMode2D(camera);
            if (jugador.estado == 3)
            {
                TextureAnimation *animacionActual = &animaciones[jugador.estado];
                animacionActual->frameTime += deltaTime;
                if (animacionActual->frameTime >= animacionActual->frameDuration)
                {
                    animacionActual->frameTime = 0.0f;
                    animacionActual->currentFrame++;
                    if (animacionActual->currentFrame >= animacionActual->frameCount)
                    {
                        // Reiniciar jugador después de la animación de muerte
                        int personajeActual = 0; // Cambia este índice según el personaje activo
                        jugador.rect.x = screenWidth / 2 - descanso[personajeActual][0].width * 3 / 2;
                        jugador.rect.y = 400 - descanso[personajeActual][0].height * 3;

                        jugador.velocidad = (Vector2){0, 0};
                        jugador.enSuelo = true;
                        jugador.estado = 0;                // Volver a descansar
                        jugador.mirandoDerecha = true;     // Volver a mirar a la derecha
                        animacionActual->currentFrame = 0; // Reiniciar la animación de muerte
                    }
                }
            }
            else
            {
                ActualizarJugador(&jugador, envItems, envItemsLength, deltaTime);
            }

            // Limitar movimiento dentro de la ventana
            if (jugador.rect.x < 0)
                jugador.rect.x = 0;
            if (jugador.rect.x + jugador.rect.width > screenWidth)
                jugador.rect.x = screenWidth - jugador.rect.width;

            // Actualizar animación
            if (jugador.estado != 3)
            {
                TextureAnimation *animacionActual = &animaciones[jugador.estado];
                animacionActual->frameTime += deltaTime;
                if (animacionActual->frameTime >= animacionActual->frameDuration)
                {
                    animacionActual->frameTime = 0.0f;
                    animacionActual->currentFrame++;
                    if (animacionActual->currentFrame >= animacionActual->frameCount)
                    {
                        animacionActual->currentFrame = 0;
                    }
                }
            }

            for (int i = 0; i < envItemsLength; i++)
            {
                DrawRectangleRec(envItems[i].rect, envItems[i].color);
            }

            // Dibujar la imagen usando la textura y escalando
            TextureAnimation *animacionActual = &animaciones[jugador.estado];
            Rectangle source = {
                0.0f,
                0.0f,
                (float)animacionActual->textures[animacionActual->currentFrame].width,
                (float)animacionActual->textures[animacionActual->currentFrame].height};
            Rectangle dest = {
                jugador.rect.x,
                jugador.rect.y,
                jugador.rect.width,
                jugador.rect.height};
            Vector2 origin = {0.0f, 0.0f};

            if (!jugador.mirandoDerecha)
            {
                source.width *= -1; // Invertir la imagen horizontalmente
            }

            DrawTexturePro(
                animacionActual->textures[animacionActual->currentFrame],
                source,
                dest,
                origin,
                0.0f,
                WHITE);

            DrawTextEx(font, "NIVEL 1", (Vector2){670, 10}, 25, 5, cafeOscuro);

            // Dibujar botón para regresar a NIVELES
            Rectangle botonRegresar = {10, 10, 150, 40};
            DrawRectangleRec(botonRegresar, LIGHTGRAY);
            DrawTextEx(font, "Regresar", (Vector2){420, 465}, 20, 2, BLACK);

            // Verificar si se ha hecho clic en el botón
            if (CheckCollisionPointRec(GetMousePosition(), botonRegresar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                pantallaActual = NIVELES;
            }
            break;

        case NIVELNORMAL:
            StopSound(soundmenu);
            DibujarNN(&pantallaActual, font, sound2);
            break;

        case PERSONALIZACION:
            DrawTexture(fondo, 0, 0, WHITE);
            DibujarPersonalizacion(&pantallaActual, &jugador, descanso, caminar, salto, muerte, 3, font, fondo);
            break;

        case SALIR:
            // Handle exit logic here
            CloseWindow();
            return 0;
        }
        EndMode2D();
        EndDrawing();
    }

    // Liberar recursos
    UnloadTexture(baseTexture);
    UnloadTexture(esquinaIzqTexture);
    UnloadTexture(esquinaDerTexture);
    UnloadTexture(centroBaseTexture);
    UnloadTexture(centroIzqTexture);
    UnloadTexture(centroDerTexture);
    UnloadTexture(sueloBaseTexture);
    UnloadTexture(sueloIzqTexture);
    UnloadTexture(sueloDerTexture);
    UnloadTexture(baseTexture2);
    UnloadTexture(esquinaIzqTexture2);
    UnloadTexture(esquinaDerTexture2);
    UnloadTexture(baseTexture3);
    UnloadTexture(baseTexture4);
    UnloadTexture(centroTexture);
    UnloadTexture(abajoTexture);
    UnloadTexture(fondo);
    UnloadFont(font);

    // Liberar recursos
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            UnloadTexture(descanso[i][j]);
            UnloadTexture(caminar[i][j]);
            UnloadTexture(muerte[i][j]);
        }
        for (int j = 0; j < 5; j++)
        {
            UnloadTexture(salto[i][j]);
        }
    }

    limpiarRecursos(texture, texture2, texture3, font, sound, sound2, soundmenu);

    return 0;
}

void DibujarMenu(PantallaJuego *pantallaActual, Font font, Sound sound2, Sound *soundmenu)
{
    // volumenG(font, &volumen);
    Vector2 mousePoint = GetMousePosition();
    Rectangle botonNIVELES = {400, 220, 200, 50};
    Rectangle botonSalir = {400, 300, 200, 50};

    if (CheckCollisionPointRec(mousePoint, botonNIVELES) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = NIVELES; // Cambia al estado de juego
    }
    if (CheckCollisionPointRec(mousePoint, botonSalir) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = SALIR; // Esto ahora cerrará el programa
    }

    // Añadir la siguiente condición para reproducir el sonido del menú
    if (*pantallaActual == MENU)
    {
        sonidomenu(soundmenu);
    }

    int outlineThickness = 1;
    DrawTextEx(font, "         S H A D O W   D A S H", (Vector2){230, 100}, 35, 0, BLACK);

    // Dibujar botón "JUGAR"
    for (int i = -outlineThickness; i <= outlineThickness; i++)
    {
        for (int j = -outlineThickness; j <= outlineThickness; j++)
        {
            if (i != 0 || j != 0)
            {
                DrawRectangle(400 + i, 220 + j, 200 + i, 50 + j, BLACK);
            }
        }
    }
    DrawRectangle(400, 220, 200, 50, LIGHTGRAY);

    // Dibujar texto con contorno
    for (int i = -outlineThickness; i <= outlineThickness; i++)
    {
        for (int j = -outlineThickness; j <= outlineThickness; j++)
        {
            if (i != 0 || j != 0)
            {
                DrawTextEx(font, "J U G A R", (Vector2){455 + i, 235 + j}, 20, 0, BLACK);
            }
        }
    }
    DrawTextEx(font, "J U G A R", (Vector2){455, 235}, 20, 0, GRAY);

    // Dibujar botón "Salir"
    for (int i = -outlineThickness; i <= outlineThickness; i++)
    {
        for (int j = -outlineThickness; j <= outlineThickness; j++)
        {
            if (i != 0 || j != 0)
            {
                DrawRectangle(400 + i, 300 + j, 200 + i, 50 + j, BLACK);
            }
        }
    }
    DrawRectangle(400, 300, 200, 50, LIGHTGRAY);

    // Dibujar texto con contorno
    for (int i = -outlineThickness; i <= outlineThickness; i++)
    {
        for (int j = -outlineThickness; j <= outlineThickness; j++)
        {
            if (i != 0 || j != 0)
            {
                DrawTextEx(font, "S A L I R", (Vector2){455 + i, 318 + j}, 20, 0, BLACK);
            }
        }
    }
    // Dibujar botón "Salir"
    DrawTextEx(font, "S A L I R", (Vector2){455, 318}, 20, 0, GRAY);
}

void DibujarNiveles(PantallaJuego *pantallaActual, Font font, Sound sound2, Sound *soundmenu)
{
    // volumenG(font, &volumen);
    sonidomenu(soundmenu);
    DrawTextEx(font, "NIVEL 1", (Vector2){670, 70}, 30, 5, cafeOscuro);
    DrawTextEx(font, "  PROXIMAMENTE", (Vector2){570, 175}, 30, 5, cafeOscuro);
    DrawTextEx(font, "PERSONALIZACION", (Vector2){590, 280}, 30, 5, cafeOscuroB);
    DrawTextEx(font, "REGRESAR", (Vector2){645, 375}, 30, 5, Rojos);

    Vector2 mousePosicion = GetMousePosition();
    Rectangle botonEASY = {700, 70, 100, 30};
    Rectangle botonNORMAL = {670, 175, 160, 30};
    Rectangle botonHARD = {690, 280, 110, 30};
    Rectangle botonREGRESAR = {645, 375, 225, 30};

    if (CheckCollisionPointRec(mousePosicion, botonEASY) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = NIVELEASY; // Cambia al estado de juego
    }
    if (CheckCollisionPointRec(mousePosicion, botonNORMAL) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = NIVELNORMAL; // Cambia al estado de juego
    }
    if (CheckCollisionPointRec(mousePosicion, botonHARD) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = PERSONALIZACION; // Cambia al estado de juego
    }
    if (CheckCollisionPointRec(mousePosicion, botonREGRESAR) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = MENU; // Cambia al estado de juego
    }
}

void DibujarNE(PantallaJuego *pantallaActual, Font font, Sound sound2)
{
    Vector2 mousePosicion = GetMousePosition();
    DrawTextEx(font, "REGRESAR", (Vector2){645, 375}, 30, 5, Rojos);
    Rectangle botonREGRESAR = {645, 375, 225, 30};
    if (CheckCollisionPointRec(mousePosicion, botonREGRESAR) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = NIVELES; // Cambia al estado de juego
    }
}

void DibujarNN(PantallaJuego *pantallaActual, Font font, Sound sound2)
{
    Vector2 mousePosicion = GetMousePosition();
    DrawTextEx(font, "REGRESAR", (Vector2){645, 375}, 30, 5, Rojos);
    Rectangle botonREGRESAR = {645, 375, 225, 30};
    if (CheckCollisionPointRec(mousePosicion, botonREGRESAR) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = NIVELES; // Cambia al estado de juego
    }
}

void DibujarNH(PantallaJuego *pantallaActual, Font font, Sound sound2)
{
    Vector2 mousePosicion = GetMousePosition();
    DrawTextEx(font, "REGRESAR", (Vector2){645, 375}, 30, 5, Rojos);
    Rectangle botonREGRESAR = {645, 375, 225, 30};
    if (CheckCollisionPointRec(mousePosicion, botonREGRESAR) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        sonido2(&sound2);
        *pantallaActual = NIVELES; // Cambia al estado de juego
    }
}

void DibujarVolumen(Font font, Sound sound2, Sound soundmenu)
{
    Vector2 mousePosicion = GetMousePosition();

    // Botones para subir y bajar volumen
    Rectangle botonVS = {20, 15, 20, 20};
    Rectangle botonVB = {70, 15, 20, 20};
    DrawTextEx(font, "+", (Vector2){20, 15}, 40, 5, Rojos);
    DrawTextEx(font, "-", (Vector2){70, 15}, 40, 5, Rojos);

    if (CheckCollisionPointRec(mousePosicion, botonVS) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        volumenActual += 0.1f; // Incrementar el volumen
        if (volumenActual > 1.0f)
        {
            volumenActual = 1.0f; // Asegurar que el volumen no supere el máximo
        }
        SetSoundVolume(sound2, volumenActual);    // Aplicar el nuevo volumen al sonido
        SetSoundVolume(soundmenu, volumenActual); // Ajusta el volumen del sonido del menú
    }

    if (CheckCollisionPointRec(mousePosicion, botonVB) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        volumenActual -= 0.1f; // Decrementar el volumen
        if (volumenActual < 0.0f)
        {
            volumenActual = 0.0f; // Asegurar que el volumen no sea negativo
        }
        SetSoundVolume(sound2, volumenActual);    // Aplicar el nuevo volumen al sonido
        SetSoundVolume(soundmenu, volumenActual); // Ajusta el volumen del sonido del menú
    }

    // Dibujar el valor del volumen actual en la pantalla
    char volumenTexto[20];
    sprintf(volumenTexto, "Volumen: %.0f", volumenActual * 10);
    DrawTextEx(font, volumenTexto, (Vector2){0, 0}, 20, 2, BLACK);
}
/*void sonidoSalto(Sound *sonidoSalto)
{
    *soundSalto = LoadSound("salto.wav");
    PlaySound(*soundSalto);
}
*/
void sonido2(Sound *sound2)
{
    *sound2 = LoadSound("carga.wav");
    PlaySound(*sound2);
}

void sonidomenu(Sound *soundmenu)
{
    if (!IsSoundPlaying(*soundmenu))
    { // Verifica si el sonido del menú no se está reproduciendo actualmente
        PlaySound(*soundmenu);
    }
}
void DibujarPreguntas(Font font)
{
    // Dibujar Preguntas
    //.-1
    DrawText("  ¿Cuál es el plural de gato? ", 400, -575, 25, Rojos);
    DrawText(" Gatos", 200, -725, 25, Rojos);
    DrawText(" Gatas ", 900, -725, 25, Rojos);
    //.-2
    DrawText("  ¿Cuál es la tilde correcta para la palabra -camion- ?", -950, -1625, 25, Rojos);
    DrawText(" Camión", -1000, -1775, 25, Rojos);
    DrawText(" Camíon", -300, -1775, 25, Rojos);
    ///.-3
    DrawText("  ¿Cuál es el sinónimo de la palabra -bonito-?", -1600, -2675, 25, Rojos);
    DrawText(" Feo", -1000, -2825, 25, Rojos);
    DrawText(" Lindo", -1700, -2825, 25, Rojos);
    // Final
    DrawText("FELICIDADES!!! GANASTE!!!", -10, -4000, 50, Rojos);
}

void limpiarRecursos(Texture2D texture, Texture2D texture2, Texture2D texture3, Font font, Sound sound, Sound sound2, Sound soundmenu)
{
    UnloadSound(sound);
    UnloadSound(sound2);
    UnloadSound(soundmenu);
    UnloadTexture(texture);
    UnloadTexture(texture2);
    UnloadTexture(texture3);
    // UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();
}

void ActualizarJugador(Jugador *jugador, EnvItem *envItems, int envItemsLength, float delta)
{
    bool moviendoHorizontal = false;

    // Movimiento horizontal
    if (IsKeyDown(KEY_LEFT))
    {
        jugador->rect.x -= VELOCIDAD_HORIZONTAL * delta;
        jugador->mirandoDerecha = false;
        moviendoHorizontal = true;
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        jugador->rect.x += VELOCIDAD_HORIZONTAL * delta;
        jugador->mirandoDerecha = true;
        moviendoHorizontal = true;
    }

    if (moviendoHorizontal && jugador->enSuelo)
    {
        jugador->estado = 2; // Caminando
    }
    else if (!moviendoHorizontal && jugador->enSuelo)
    {
        jugador->estado = 0; // Descansando
    }

    // Aplicar gravedad
    jugador->velocidad.y += GRAVEDAD * delta;

    // Salto
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && jugador->enSuelo)
    {
        jugador->velocidad.y = -SALTO_VELOCIDAD;
        jugador->enSuelo = false;
        jugador->estado = 1; // Saltando
    }

    // Actualizar posición vertical
    jugador->rect.y += jugador->velocidad.y * delta;

    // Comprobar colisiones con plataformas
    jugador->enSuelo = false;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;

        // Verificar colisiones en la dirección 'y'
        if (ei->blocking &&
            ei->rect.x <= jugador->rect.x + jugador->rect.width &&
            ei->rect.x + ei->rect.width >= jugador->rect.x &&
            ei->rect.y >= jugador->rect.y &&
            ei->rect.y <= jugador->rect.y + jugador->velocidad.y * delta)
        {
            if (jugador->velocidad.y > 0)
            {
                jugador->rect.y = ei->rect.y; // Ajustar posición del jugador al tope de la plataforma
                jugador->velocidad.y = 0;
                jugador->enSuelo = true;
            }
            break;
        }
    }

    // Activar animación de muerte y reiniciar jugador
    if (IsKeyPressed(KEY_X))
    {
        jugador->estado = 3; // Muerte
        jugador->velocidad = (Vector2){0, 0};
    }
}

// Función para dibujar una plataforma
void DrawPlatform(float posX, float posY, int platformType, char platformPart,
                  Texture2D baseTexture, Texture2D esquinaIzqTexture, Texture2D esquinaDerTexture,
                  Texture2D centroBaseTexture, Texture2D centroIzqTexture, Texture2D centroDerTexture,
                  Texture2D sueloBaseTexture, Texture2D sueloIzqTexture, Texture2D sueloDerTexture,
                  Texture2D baseTexture2, Texture2D esquinaIzqTexture2, Texture2D esquinaDerTexture2,
                  Texture2D baseTexture3, Texture2D baseTexture4, Texture2D centroTexture, Texture2D abajoTexture)
{
    Rectangle destRect = {posX, posY, 0, 0}; // Rectángulo destino (posición x, posición y, ancho, alto)
    Texture2D texture;                       // Textura a dibujar
    float scale = 1.0f;                      // Escala para aumentar el tamaño de la textura

    // Seleccionar la textura correspondiente según el tipo de plataforma y la parte deseada
    switch (platformType)
    {
    case 1:
        // Plataforma de suelo
        switch (platformPart)
        {
        case 'b':
            texture = baseTexture;
            break;
        case 'i':
            texture = esquinaIzqTexture;
            break;
        case 'd':
            texture = esquinaDerTexture;
            break;
        case 'c':
            texture = centroBaseTexture;
            break;
        case 'l':
            texture = centroIzqTexture;
            break;
        case 'r':
            texture = centroDerTexture;
            break;
        case 's':
            texture = sueloBaseTexture;
            break;
        case 'h':
            texture = sueloIzqTexture;
            break;
        case 'm':
            texture = sueloDerTexture;
            break;
        default:
            // Parte de plataforma no reconocida
            return;
        }
        break;
    case 2:
        // Plataforma horizontal
        switch (platformPart)
        {
        case 'b':
            texture = baseTexture2;
            break;
        case 'i':
            texture = esquinaIzqTexture2;
            break;
        case 'd':
            texture = esquinaDerTexture2;
            break;
        default:
            // Parte de plataforma no reconocida
            return;
        }
        break;
    case 3:
        // Plataforma sola
        texture = baseTexture3;
        break;
    case 4:
        // Plataforma vertical
        switch (platformPart)
        {
        case 'b':
            texture = baseTexture4;
            break;
        case 'c':
            texture = centroTexture;
            break;
        case 'a':
            texture = abajoTexture;
            break;
        default:
            // Parte de plataforma no reconocida
            return;
        }
        break;
    default:
        // Tipo de plataforma no reconocido
        return;
    }

    // Dibujar la plataforma
    DrawTextureEx(texture, (Vector2){destRect.x, destRect.y}, 0.0f, scale, WHITE);
}

// Función para invertir la base de una plataforma vertical
void InvertVerticalPlatformBase(float posX, float posY, Texture2D baseTexture4)
{
    float scale = 1.5f; // Escala para aumentar el tamaño de la textura
    // Dibujar la plataforma invertida
    DrawTextureEx(baseTexture4, (Vector2){posX, posY}, 180.0f, scale, WHITE);
}

void DibujarPersonalizacion(PantallaJuego *pantallaActual, Jugador *jugador, Texture2D descanso[][4], Texture2D caminar[][4], Texture2D salto[][5], Texture2D muerte[][4], int numAvatares, Font font, Texture2D fondo)
{
    Vector2 mousePosicion = GetMousePosition();
    int avatarsPerRow = 3;
    int avatarSize = 150;
    int spacing = 50;
    static int selectedAvatar = -1;

    // Dibujar fondo que abarque toda la pantalla
    DrawTexturePro(fondo,
                   (Rectangle){0, 0, fondo.width, fondo.height},
                   (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                   (Vector2){0, 0},
                   0,
                   WHITE);

    for (int i = 0; i < numAvatares; i++)
    {
        int row = i / avatarsPerRow;
        int col = i % avatarsPerRow;
        Rectangle avatarRect = {
            100 + col * (avatarSize + spacing),
            150 + row * (avatarSize + spacing),
            avatarSize,
            avatarSize};

        // Dibujar avatar ampliado
        DrawTexturePro(descanso[i][0],
                       (Rectangle){0, 0, descanso[i][0].width, descanso[i][0].height},
                       avatarRect,
                       (Vector2){0, 0},
                       0,
                       WHITE);

        // Dibujar marco si este avatar está seleccionado
        if (i == selectedAvatar)
        {
            DrawRectangleLinesEx(avatarRect, 3, RED);
        }

        if (CheckCollisionPointRec(mousePosicion, avatarRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectedAvatar = i;
            animaciones[0] = (TextureAnimation){descanso[i], 4, 0, 0.25f, 0.0f}; // Animación de descanso
            animaciones[1] = (TextureAnimation){salto[i], 5, 0, 0.2f, 0.0f};     // Animación de salto
            animaciones[2] = (TextureAnimation){caminar[i], 4, 0, 0.1f, 0.0f};   // Animación de caminar
            animaciones[3] = (TextureAnimation){muerte[i], 4, 0, 0.3f, 0.0f};    // Animación de muerte

            // Update the player's textures
            for (int j = 0; j < 4; j++)
            {
                jugador->descanso[j] = descanso[i][j];
                jugador->caminar[j] = caminar[i][j];
                jugador->muerte[j] = muerte[i][j];
            }
            for (int j = 0; j < 5; j++)
            {
                jugador->salto[j] = salto[i][j];
            }
        }
    }

    // Dibujar textos
    DrawTextEx(font, "Selecciona tu avatar", (Vector2){250, 50}, 30, 2, BLACK);

    // Botón para regresar a NIVELEASY
    Rectangle botonRegresar = {400, 450, 200, 50};
    DrawRectangleRec(botonRegresar, LIGHTGRAY);
    DrawTextEx(font, "Regresar", (Vector2){420, 465}, 20, 2, BLACK);

    if (CheckCollisionPointRec(mousePosicion, botonRegresar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *pantallaActual = NIVELES;
    }
}

// Funciones para validación de texturas
void VerificarCargaTexture(Texture2D texture, const char *nombreArchivo)
{
    if (texture.id == 0)
    {
        printf("Error al cargar la textura: %s\n", nombreArchivo);
        exit(1);
    }
}
