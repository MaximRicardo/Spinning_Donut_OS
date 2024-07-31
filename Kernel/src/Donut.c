#include <stdint.h>

#include "Print.h"
#include "MemFuncs.h"
#include "Donut.h"

#define m_DONUT_HOLE_RADIUS (3.f)
#define m_DONUT_TUBE_RADIUS (1.5f)

#define m_PIT_FREQUENCY 1193181 /* PIT clock frequency in hZ */

#define m_PI 3.14159265358979f

#define TRIG_N 10

float fmodf(float x, float div) {
	if (div != div || x != x) {
		return x;
	}
	return x - (int) (x / div) * div;
}

float sinf(float x) {

    float n = 0.f;

    float dn;

    int i;

    x = fmodf(x, 2.f * m_PI);
    dn = x;

    for (i = 1; i < 2*TRIG_N+2; i += 2) {
        n += dn;
        dn *= -x*x / (((float)i + 1.f) * ((float)i + 2.f));
    }

    return n;

}

float cosf(float x) {

    float n = 0.f;

    float dn;

    int i;
    
    x = fmodf(x, 2.f * m_PI);
    dn = x*x/2.f;

    for (i = 2; i < 2*TRIG_N+2; i += 2) {
        n += dn;
        dn *= -x*x / (((float)i + 1.f) * ((float)i + 2.f));
    }

    return 1.f-n;

}

float fmaxf(float a, float b) {

    if (a > b) return a;
    else return b;

}

void Donut() {

    extern uint32_t pitIntCount;

    float t, i, j, A = m_PI/2.f, B = 0.f, z[70*22];
    int x, y;
    unsigned int cursorStartX, cursorStartY;

    char b[70*22];

    float secCount = 0.f;   /* Seconds since the first frame began */

    float s1, s2;

    GetCursorPos(&cursorStartX, &cursorStartY);

    /* Compute the delta time using the difference of pit counts */
    s1 = (float)pitIntCount/1000.f;
    s2 = s1;

    for (;;) {

        float deltaTime = fmaxf(s2 - s1, 0.00001f);
        float fps = 1.f / deltaTime;

        float sinA = sinf(A);
        float cosA = cosf(A);
        float sinB = sinf(B);
        float cosB = cosf(B);

        secCount += deltaTime;

        memset(z, 0, 70*22*sizeof(float));
        memset(b, 32, 70*22);

        for (i = 0; i < 6.28; i+=0.04f) {
            float sini = sinf(i);
            float cosi = cosf(i);
            for (j = 0; j < 6.28; j+=0.07f) {
                
                float sinj = sinf(j);
                float cosj = cosf(j);

                float rX, rY, pX, pY, pZ, nX, nY, nZ;
                int qX, qY, o, n;

                rX = cosj * m_DONUT_TUBE_RADIUS;
                rY = sinj * m_DONUT_TUBE_RADIUS;

                pX = (rX + m_DONUT_HOLE_RADIUS + m_DONUT_TUBE_RADIUS) * cosi;
                pY = rY;
                pZ = (rX + m_DONUT_HOLE_RADIUS + m_DONUT_TUBE_RADIUS) * sini;

                t = pY;
                pY = t * cosA - pZ * sinA;
                pZ = t * sinA + pZ * cosA;
                t = pX;
                pX = t * cosB - pY * sinB;
                pY = t * sinB + pY * cosB;
               
                pZ += 8.f;

                qX = (1.f + pX / pZ / (80.f/22.f)) * 35.f;
                qY = (1.f + pY / pZ) * 11.f;

                o = 70 * qY + qX;

                if (pZ < 0.01f || qX < 0 || qX >= 70 || qY < 0 || qY >= 22 || 1.f/pZ < z[o]) continue;

                z[o] = 1.f/pZ;

                nX = rX * cosi;
                nY = rY;
                nZ = rX * sini;
                
                t = nY;
                nY = t * cosA - nZ * sinA;
                nZ = t * sinA + nZ * cosA;
                t = nX;
                nX = t * cosB - nY * sinB;
                nY = t * sinB + nY * cosB;

                /* [0, -0.894427, -0.447214] is the normalized version of [0, -2, -1] */
                n = (1.f + 0.f*nX + (-0.894427f)*nY + (-0.447214f)*nZ) / 2.f * 11.f;

                if (n < 0) n = 0;
                else if (n > 11) n = 11;

                b[o] = ".,-~:;=!*#$@"[n];

            }
        }

        SetCursorPos(cursorStartX, cursorStartY);
        for (y = 0; y < 22; y++) {
            for (x = 0; x < 70; x++) kPrintf("%c", b[70*y+x]);
            kPrintf("\n");
        }

        kPuts("Donut");
        if (fmodf(secCount, 1.f) < 0.01f) kPrintf("fps = %d        \n", (int)fps);   /* Only updates once every second (Assuming your fps isn't absolutely horrible) */
        else kPrintf("\n");
        kPrintf("fps currently = %d        \n", (int)fps);                           /* Updates after every frame */

        A += 1.4f * deltaTime;
        B += 0.7f * deltaTime;

        s1 = s2;
        s2 = (float)pitIntCount / 1000.f;

    }

}
