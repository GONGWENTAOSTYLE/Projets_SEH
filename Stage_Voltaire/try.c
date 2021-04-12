#include<stdio.h>
#include<math.h>
#define V_top  90 //la vitesse limite dans la zone en km/h
#define D1 300 //la distance entre le 1er feu et le début de la zone en mètres
#define T1 30 // la durée en seconde pendant laquelle le 1er feu reste dans un état
#define D2 1500 //la distance entre le 2èrm feu et le début de la zone en mètres
#define T2 20 // la durée en seconde pendant laquelle le 2èrm feu reste dans un état
#define D3 3000 //la distance entre le 3èrm feu et le début de la zone en mètres
#define T3 10 // la durée en seconde pendant laquelle le 3èrm feu reste dans un état


int main()
{
    double v, t1, t2, t3, i = V_top;

    while (i)
    {
        v = i*10/36; // la vitesse en m/s
        t1 = D1/v; // le temps d'arriver le 1er feu
        t2 = D2/v; // le temps d'arriver le 2èrm feu
        t3 = D3/v; // le temps d'arriver le 3èrm feu

        if((fmod (t1,2*T1) <T1) && (fmod (t2,2*T2) <T2) && (fmod (t3,2*T3) <T3))
        {
            printf("V max = %f km/h\n", i);
            printf(" = %f m/s\n",v);
            break;
        }
        else i--;
    }
}
