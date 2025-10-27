// Unidad 3 MAVI 2025
//Mendez Caldeira Triana
#include "raylib.h"


//=================================================================
//                     CLASE PLAYER
//=================================================================
class Player {
public:
    Vector2 position{ 200.0f, 400.0f };
    Vector2 velocity{ 0.0f, 0.0f };
	float   scale{ 1.0f };           //escala del sprite
    Color   color{ WHITE };         // WHITE es sin tinte
    int     facing{ 1 };            // 1 = derecha, -1 = izquierda
    Texture2D* sprite{ nullptr };   // Apuntar a textura compartida (evitar duplicar cargas y descargas)

    void Update(float dt, int screenW, int screenH) {
        // Integración de movimiento
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        // Actualizar “facing” según dirección X
        if (velocity.x < 0) facing = -1;
        else if (velocity.x > 0) facing = 1;

        // Limitar dentro de pantalla considerando el tamaño escalado
        if (sprite && sprite->id != 0) {
            float margin = 60.0f; // px que se restan a cada lado
            float w = (sprite->width - margin) * scale;
            float h = (sprite->height - margin) * scale;

            // Corregir posición si se sale por la izquierda/arriba
            if (position.x < 0) position.x = 0;
            if (position.y < 0) position.y = 0;

            // Corregir posición si se sale por la derecha/abajo
            if (position.x > screenW - w) position.x = screenW - w;
            if (position.y > screenH - h) position.y = screenH - h;
        }
    }

    void Draw(bool highlight = false) const {
    if (!sprite || sprite->id == 0) return; // Salir si no hay textura válida

    // si está resaltado: +20% de escala
    float appliedScale = scale * (highlight ? 1.2f : 1.0f);

    // color base o tinte rojo semitransparente
    Color appliedColor;
    if (highlight)
        appliedColor = color;  // jugador activo, color normal
    else
        appliedColor = Color{ 255, 80, 80, 200 };  // jugador inactivo, tinte rojo

    // Definir rectángulo fuente (src): usar ancho negativo para espejar según 'facing'
    Rectangle src{
        0.0f, 0.0f,
        (float)sprite->width * (float)facing,
        (float)sprite->height
    };

    // Calcular rectángulo destino (dst): mantener centrado al cambiar escala (compensar delta
    Rectangle dst{
        position.x - (sprite->width * appliedScale - sprite->width * scale) * 0.5f,
        position.y - (sprite->height * appliedScale - sprite->height * scale) * 0.5f,
        sprite->width * appliedScale,
        sprite->height * appliedScale
    };

    // Definir origen en (0,0) para no rotar alrededor del centro
    Vector2 origin{ 0.0f, 0.0f };

    // Dibujar textura con transformación y tinte
    DrawTexturePro(*sprite, src, dst, origin, 0.0f, appliedColor);
}
};
// ----------------------------------------------------------------------

int main(void)
{
    const int W = 2080, H = 2080;
    InitWindow(W, H, "Proyecto MAVI - Unidad 3");
    SetTargetFPS(60);

    // Cargar la textura UNA sola vez 
    Texture2D tex = LoadTexture("../assets/personaje1.png");
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR); // Aplicar filtro bilineal (suavizar al escalar)
    if (tex.id == 0) {
        TraceLog(LOG_ERROR, "No se pudo cargar la textura. CWD=%s", GetWorkingDirectory()); // Loguear error si falla otra vez
    }


    // Propiedades para DrawTextureEx()
    Vector2 position = { 200.0f, 200.0f };
    float rotation = 0.0f;
    float scale = 1.2f;



    // Crear dos jugadores con posiciones distintas y compartir la misma textura
    Player p1, p2;
    p1.position = { 200.0f, 400.0f };
    p2.position = { 600.0f, 400.0f };
    p1.scale = p2.scale = 0.6f;
    p1.sprite = p2.sprite = &tex;   // los dos usan la MISMA textura compartida

   

    // jugador activo (0 = p1, 1 = p2)
    int activeIndex = 0;


    //=================================================================
    //                      INICIA BUCLE RINCIPAL
    //=================================================================

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();      // delta time del frame actual

        // Alternar el control con ESPACIO
        if (IsKeyPressed(KEY_SPACE)) {
            activeIndex ^= 1;   // 0->1, 1->0
        }

        // Input solo para el activo
        Player& active = (activeIndex == 0) ? p1 : p2;
        Player& other = (activeIndex == 0) ? p2 : p1;


        // Movimiento con teclado y  velocidad 
        active.velocity = { 0.0f, 0.0f };
        if (IsKeyDown(KEY_RIGHT)) active.velocity.x = 600.0f;
        if (IsKeyDown(KEY_LEFT))  active.velocity.x = -600.0f;
        if (IsKeyDown(KEY_DOWN))  active.velocity.y = 600.0f;
        if (IsKeyDown(KEY_UP))    active.velocity.y = -600.0f;

        // el otro se queda quieto 
        other.velocity = { 0.0f, 0.0f };

        // Actualizar ambos
        p1.Update(dt, W, H);
        p2.Update(dt, W, H);

        // 7) Dibujar los dos; resaltar el activo
        BeginDrawing();
        ClearBackground(BLACK);


        // Dibujar jugadores (pasar 'highlight' = true sólo al activo)
        p1.Draw(activeIndex == 0);
        p2.Draw(activeIndex == 1);

        // Ayuda visual UI
        DrawText("ESPACIO: cambiar jugador activo | FLECHAS: mover", 20, 40, 52, RAYWHITE);

		
        // Mostrar info de ambos jugadores en esquina superior derecha
        {
            const int fontSize = 20;
            const int margin = 16;
            int y = 16; // empieza arriba

            auto drawRight = [&](const char* txt, Color col)
                {
                    int w = MeasureText(txt, fontSize);
                    int x = GetScreenWidth() - margin - w; // alinea a la derecha
                    DrawText(txt, x, y, fontSize, col);
                    y += fontSize + 4; // avanza a la siguiente línea
                };

            // Para p1
            drawRight(TextFormat("Jugador 1"), YELLOW);
            drawRight(TextFormat("Pos: (%.0f, %.0f)", p1.position.x, p1.position.y), RAYWHITE);
            drawRight(TextFormat("Scale: %.2f", p1.scale * (activeIndex == 0 ? 1.2f : 1.0f)), RAYWHITE);
            drawRight(TextFormat("Color: R%d G%d B%d A%d", p1.color.r, p1.color.g, p1.color.b, p1.color.a), RAYWHITE);

            y += 10; //  espacio entre bloques

            // Para p2
            drawRight(TextFormat("Jugador 2"), SKYBLUE);
            drawRight(TextFormat("Pos: (%.0f, %.0f)", p2.position.x, p2.position.y), RAYWHITE);
            drawRight(TextFormat("Scale: %.2f", p2.scale * (activeIndex == 1 ? 1.2f : 1.0f)), RAYWHITE);
            drawRight(TextFormat("Color: R%d G%d B%d A%d", p2.color.r, p2.color.g, p2.color.b, p2.color.a), RAYWHITE);
        }



        EndDrawing();
    }

    // 8) Descargar la textura UNA sola vez 
    if (tex.id != 0) UnloadTexture(tex);
    CloseWindow();
    return 0;
}