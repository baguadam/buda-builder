#include <iostream>
#include <math.h>
#include "Perlin.hpp"

Perlin::Perlin() {}
Perlin::~Perlin() {}

// hashelés segítségével generálunk egy random egységvektort az ix, iy koordináták alapján
Perlin::vector2 Perlin::randomGradient(int ix, int iy) {
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); 

    vector2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

// egy adott grid pozícióban meghatározzuk az oda tartozó egységvektor és a távolságvektor keresztszorzatát
float Perlin::dotGridGradient(int ix, int iy, float x, float y) {
    // meghatározzuk az irányvektort az adott grid pozícióban
    vector2 gradient = randomGradient(ix, iy);

    // kiszámoljuk a távolságvektort az adott pozíció és a megadott ix, iy alapján
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // a kettõ keresztszorzata
    return (dx * gradient.x + dy * gradient.y);
}

float Perlin::interpolate(float a0, float a1, float w)
{
    // Hermit interpoláció alapján elvégezzük az interpolált, nem lineárisan, hanem négyzetesen tesszük a 
    // szebb eredmény érdekében
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}


// mintavételezzük a zajt az adott pozícióban
float Perlin::perlin(float x, float y) {

    // kiszámoljuk a grid cella sarkainak a koordinátáit 
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // interpoláció súlya
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // meghatározzuk a szükséges értékeket a két felsõ sarokban, majd azokat interpoláljuk
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // ugyanezt tesszük a két alsó sarokban is
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // végül pedig a az alsó és a felsõ kapott értékeket is interpoláljuk, így megkapjuk, hogy
    // az adott koordinátához tarzozó zajt
    float value = interpolate(ix0, ix1, sy);

    return value;
}