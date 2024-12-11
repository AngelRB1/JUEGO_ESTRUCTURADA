
#include "raylib.h"
#include "raymath.h"

#define G 1200
#define PLAYER_JUMP_SPD 600.0f
#define PLAYER_HOR_SPD 300.0f

typedef struct Jugador
{
    Vector2 posicion;
    float velocidad;
    bool puedeSaltar;
} Jugador;

typedef struct ObjetoEntorno
{
    Rectangle rect;
    int bloqueante;
    Color color;
} ObjetoEntorno;

// Declaración de funciones del módulo
void ActualizarJugador(Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta);
void ActualizarCamaraCentro(Camera2D *camara, Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta, int ancho, int alto);

void ActualizarJugador(Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta)
{
    if (IsKeyDown(KEY_LEFT))
        jugador->posicion.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT))
        jugador->posicion.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && jugador->puedeSaltar)
    {
        jugador->velocidad = -PLAYER_JUMP_SPD;
        jugador->puedeSaltar = false;
    }

    bool golpeaObstaculo = false;
    for (int i = 0; i < longitudObjetosEntorno; i++)
    {
        ObjetoEntorno *oe = objetosEntorno + i;
        Vector2 *p = &(jugador->posicion);
        if (oe->bloqueante &&
            oe->rect.x <= p->x &&
            oe->rect.x + oe->rect.width >= p->x &&
            oe->rect.y >= p->y &&
            oe->rect.y <= p->y + jugador->velocidad * delta)
        {
            golpeaObstaculo = true;
            jugador->velocidad = 0.0f;
            p->y = oe->rect.y;
            break;
        }
    }

    if (!golpeaObstaculo)
    {
        jugador->posicion.y += jugador->velocidad * delta;
        jugador->velocidad += G * delta;
        jugador->puedeSaltar = false;
    }
    else
    {
        jugador->puedeSaltar = true;
    }
}

void ActualizarCamaraCentro(Camera2D *camara, Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta, int ancho, int alto)
{
    camara->offset = (Vector2){ancho / 2.0f, alto / 2.0f};
    camara->target = jugador->posicion;
}

=======
#include "raylib.h"
#include "raymath.h"

#define G 1200
#define PLAYER_JUMP_SPD 600.0f
#define PLAYER_HOR_SPD 300.0f

typedef struct Jugador
{
    Vector2 posicion;
    float velocidad;
    bool puedeSaltar;
} Jugador;

typedef struct ObjetoEntorno
{
    Rectangle rect;
    int bloqueante;
    Color color;
} ObjetoEntorno;

// Declaración de funciones del módulo
void ActualizarJugador(Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta);
void ActualizarCamaraCentro(Camera2D *camara, Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta, int ancho, int alto);

void ActualizarJugador(Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta)
{
    if (IsKeyDown(KEY_LEFT))
        jugador->posicion.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT))
        jugador->posicion.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && jugador->puedeSaltar)
    {
        jugador->velocidad = -PLAYER_JUMP_SPD;
        jugador->puedeSaltar = false;
    }

    bool golpeaObstaculo = false;
    for (int i = 0; i < longitudObjetosEntorno; i++)
    {
        ObjetoEntorno *oe = objetosEntorno + i;
        Vector2 *p = &(jugador->posicion);
        if (oe->bloqueante &&
            oe->rect.x <= p->x &&
            oe->rect.x + oe->rect.width >= p->x &&
            oe->rect.y >= p->y &&
            oe->rect.y <= p->y + jugador->velocidad * delta)
        {
            golpeaObstaculo = true;
            jugador->velocidad = 0.0f;
            p->y = oe->rect.y;
            break;
        }
    }

    if (!golpeaObstaculo)
    {
        jugador->posicion.y += jugador->velocidad * delta;
        jugador->velocidad += G * delta;
        jugador->puedeSaltar = false;
    }
    else
    {
        jugador->puedeSaltar = true;
    }
}

void ActualizarCamaraCentro(Camera2D *camara, Jugador *jugador, ObjetoEntorno *objetosEntorno, int longitudObjetosEntorno, float delta, int ancho, int alto)
{
    camara->offset = (Vector2){ancho / 2.0f, alto / 2.0f};
    camara->target = jugador->posicion;
}

>>>>>>> 1d05b59d84fd9d4872e34505c137df75a89d1f72
