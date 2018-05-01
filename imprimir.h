#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

void imprimir_plantilla();
void imprimir_hipodromo();
void recorrer_pista_i(int num_pista, int longitud, float espera);
