#include <iostream>
#include <math.h>
#include "Perlin.hpp"

Perlin::Perlin() {}
Perlin::~Perlin() {}

// hashel�s seg�ts�g�vel gener�lunk egy random egys�gvektort az ix, iy koordin�t�k alapj�n
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

// egy adott grid poz�ci�ban meghat�rozzuk az oda tartoz� egys�gvektor �s a t�vols�gvektor keresztszorzat�t
float Perlin::dotGridGradient(int ix, int iy, float x, float y) {
    // meghat�rozzuk az ir�nyvektort az adott grid poz�ci�ban
    vector2 gradient = randomGradient(ix, iy);

    // kisz�moljuk a t�vols�gvektort az adott poz�ci� �s a megadott ix, iy alapj�n
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // a kett� keresztszorzata
    return (dx * gradient.x + dy * gradient.y);
}

float Perlin::interpolate(float a0, float a1, float w)
{
    // Hermit interpol�ci� alapj�n elv�gezz�k az interpol�lt, nem line�risan, hanem n�gyzetesen tessz�k a 
    // szebb eredm�ny �rdek�ben
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}


// mintav�telezz�k a zajt az adott poz�ci�ban
float Perlin::perlin(float x, float y) {

    // kisz�moljuk a grid cella sarkainak a koordin�t�it 
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // interpol�ci� s�lya
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // meghat�rozzuk a sz�ks�ges �rt�keket a k�t fels� sarokban, majd azokat interpol�ljuk
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // ugyanezt tessz�k a k�t als� sarokban is
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // v�g�l pedig a az als� �s a fels� kapott �rt�keket is interpol�ljuk, �gy megkapjuk, hogy
    // az adott koordin�t�hoz tarzoz� zajt
    float value = interpolate(ix0, ix1, sy);

    return value;
}