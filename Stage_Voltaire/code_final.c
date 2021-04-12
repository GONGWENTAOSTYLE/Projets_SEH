#include<stdio.h>
#include<math.h>
/*
V_top: la vitesse limite dans la zone en km/h
N_feu: le nombre de feu
distance: une tableau représente la distance entre le feu et le début de la zone en mètres
duree: une tableau représente la durée en seconde pendant laquelle le feu reste dans un état (rouge ou vert) en secondes
*/
int test(double V_top, int N_feu, double distance[], double duree[]){

    double v, t[N_feu], i = V_top;
    int falg[N_feu]; //Créez un tableau de longueur N pour stocker les résultats du jugement. Si il ne s'arrête pas à cause du feu rouge, on le met 1, sinon 0
    for(int m = 0 ; i < N_feu ; m++) falg[m] = 0;

    while(i){
        v = (double)(i*10/36); // la vitesse en m/s

        for ( int j=0; j<N_feu; j++){
            t[j] = distance[j]/v; // Le temps d'arriver (j-1)ème feu
            if(fmod (t[j],2*duree[j]) <duree[j]){//fmod () est utilisé pour moduler les nombres à virgule flottante
                falg[j] = 1; // Juger si le vélo s'arrête ou pas à cause du (j-1)ème feu rouge.
            }
            else{
                falg[j] = 0;
                break;
            }
        }
        if(falg[N_feu-1]==1){ //Si pour tous les feux, le vélo ne s'arrête pas, alors saute la boucle et renvoie la vitesse.
            break;
        }
        else i--;
    }
    return (int)i;
}

int main()
{
    double V_top = 90;
    int N_feu = 3;
    double distance[] = {300, 1500, 3000};
    double duree[] = {30, 20, 10};
    int V_max = test(V_top, N_feu, distance, duree);
    printf("V_max = %d km/h\n", V_max);
}
