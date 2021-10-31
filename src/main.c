#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int vertices;
    int grau_m;
    int **arestas;
    int **pesos;
    int *grau;
}Grafo;
typedef struct{
    char nome[100];
    int tamanho;
    int vi;
    int vf;
}Ruas;
typedef struct{
    char nome[100];
    int v_proximo;
    int dist_v;
}Pontos;

Grafo *CriaGrafo(int vertice, int grau_m);
void LiberaMemoria(Grafo *cajuru);
void InserirRua(Grafo *cajuru,int origem,int destino,int tam,int parar);
void MenorCaminho(Grafo *cajuru, int ini, int *ant, int *dist);
int MenorDistancia(int *dist, int *visitado, int NV);
void AdicionaRua(Ruas rua[], FILE *R, int ra);
void AdicionaReferencias(Pontos referencia[], FILE *P, int pt);

int main(void){
    char navegar[30];
    printf("Onde voce quer navegar:");
    gets(navegar);
    fflush(stdin);
    int v, r, p, D; char csvr[10], csvp[12];
    if(!strcmp(navegar,"CICLOVIAS")||!strcmp(navegar,"PARQUES")){
        v = 350, D = 1, r = 363, p = 74, strcpy(csvr,"ruasp.csv"), strcpy(csvp,"pontosp.csv");
    } else if(!strcmp(navegar,"CAJURU")){
        v = 1488, D = 1, r = 2229, p = 75, strcpy(csvr,"ruasc.csv"), strcpy(csvp,"pontosc.csv");
    } else exit(1);

    Grafo *cajuru = CriaGrafo(v,6);
    Ruas rua[r];
    Pontos referencia[p];
    int ant[v], dist[v];
    char org[100], dest[100];
    int i, origem = -1, destino = -1;
    FILE *R = fopen(csvr,"r");
    FILE *P = fopen(csvp,"r");
    if (R == NULL || P == NULL)exit(1);

    AdicionaRua(rua,R,r);
    AdicionaReferencias(referencia,P,p);
    fclose(P); fclose(R);
    P = NULL, R = NULL;
    //for(i=0;i<r;i++){
        InserirRua(cajuru,rua[i].vi,rua[i].vf,rua[i].tamanho,D);
    //}

    printf("Onde Voce Esta:");
    gets(org);
    fflush(stdin);
    printf("\nOnde Voce Quer Ir:");
    gets(dest);
    fflush(stdin);

    for (i=0;i<r;i++){
        if(!(strcmp(org,referencia[i].nome))) origem = i;
        if(!(strcmp(dest,referencia[i].nome))) destino = i;
    }
    if(origem==-1 || destino==-1)exit(1);

    int o = referencia[destino].v_proximo, d = referencia[origem].v_proximo, d2 = d;
    MenorCaminho(cajuru, o, ant, dist);
    if(ant[d] == -1){
        printf("Nao ha um caminho possivel!");
        exit(1);
    }
    printf("\n\nSaindo de %s, siga %dm, apos:\n", referencia[origem].nome, referencia[origem].dist_v);

    while(d!=o){
        for(i=0;i<r;i++){
            if(rua[i].vi == d && rua[i].vf == ant[d] || rua[i].vi == ant[d2] && rua[i].vf == d2){
                    printf("-> %dm pela %s\n", rua[i].tamanho, rua[i].nome);
            }
        }
        d = ant[d], d2 = d;
    }
    printf("\nSiga mais %dm\nVoce Chegou em %s!\nDistancia total percorrida:%dm\n\n\n",referencia[destino].dist_v,referencia[destino].nome,(referencia[origem].dist_v + referencia[destino].dist_v + dist[referencia[origem].v_proximo]));
    LiberaMemoria(cajuru);
    return 0;
}

Grafo *CriaGrafo(int vertice, int grau_m){
    Grafo *cajuru = (Grafo *)malloc(sizeof(Grafo));
    if ( cajuru != NULL){
        int i;
        cajuru->vertices = vertice;
        cajuru->grau_m = grau_m;
        cajuru->grau = (int *)calloc(vertice,sizeof(int));
        cajuru->arestas = (int **)malloc(vertice*sizeof(int *));
        cajuru->pesos = (int **)malloc(vertice*sizeof(int *));

        for ( i = 0; i < vertice; i++){
            cajuru->arestas[i] = (int *)malloc(grau_m*sizeof(int));
            cajuru->pesos[i] = (int *)malloc(grau_m*sizeof(int));
        }
        return cajuru;
    }
}
void InserirRua(Grafo *cajuru,int origem,int destino,int tam, int parar){
    cajuru->arestas[origem][cajuru->grau[origem]] = destino;
    cajuru->pesos[origem][cajuru->grau[origem]] = tam;
    cajuru->grau[origem]++;
    cajuru->arestas[destino][cajuru->grau[destino]] = origem;
    cajuru->pesos[destino][cajuru->grau[destino]] = tam;
    cajuru->grau[destino]++;

    //if (parar){
      //  InserirRua(cajuru,destino,origem,tam,0);
    //}
}
void LiberaMemoria(Grafo *cajuru){
    int i;
    for ( i = 0; i < cajuru->vertices; i++){
        free(cajuru->arestas[i]);
        free(cajuru->pesos[i]);
    }
    free(cajuru->arestas);
    free(cajuru->pesos);
    free(cajuru->grau);
    free(cajuru);
}
void MenorCaminho(Grafo *cajuru, int ini, int *ant, int *dist){
    int i, cont, NV, ind, *visitado, u;
    cont = NV = cajuru->vertices;
    visitado = (int *)malloc(NV*sizeof(int));
    for ( i = 0; i < NV; i++){
        ant[i] = -1;
        dist[i] = -1;
        visitado[i] = 0;
    }
    dist[ini] = 0;
    while(cont > 0){
        u = MenorDistancia(dist,visitado,NV);
        if (u == -1) break;
        visitado[u] = 1;
        cont--;
        for ( i = 0; i < cajuru->grau[u]; i++){
            ind = cajuru -> arestas[u][i];
            if (dist[ind] < 0){
                dist[ind] = dist[u] + cajuru->pesos[u][i];
                ant[ind] = u;
            }else{
                if(dist[ind] > dist[u] + cajuru->pesos[u][i]){
                    dist[ind] = dist[u] + cajuru->pesos[u][i];
                    ant[ind] = u;
                }
            }
        }
    }
    free(visitado);
}
int MenorDistancia(int *dist, int *visitado, int vertice){
    int i, menor = -1, primeiro = 1;
    for ( i = 0; i < vertice; i++){
        if (dist[i] >= 0 && visitado[i] == 0){
            if(primeiro){
                menor = i;
                primeiro = 0;
            }else{
                if(dist[menor] > dist[i]) menor = i;
            }
        }
    }
    return menor;
}
void AdicionaRua(Ruas rua[],FILE *R,int ra){
    char *temp, dados_t[200], s[2] = ";";
    int i, j;
    for ( i = 0; i < ra; i++){
        fgets(dados_t,200,R);
        temp = strtok(dados_t,s);
        temp = strtok(NULL,s);
        strcpy(rua[i].nome,temp);
        for (j=0;j<5;j++)temp = strtok(NULL,s);
        rua[i].tamanho = atoi(temp);
        temp = strtok(NULL,s);
        rua[i].vi = atoi(temp);
        temp = strtok(NULL,s);
        rua[i].vf = atoi(temp);
    }
}
void AdicionaReferencias(Pontos referencia[], FILE *P,int pt){
    char *temp, dados_t[200], s[2] = ";";
    int i;
    for ( i = 0; i < pt; i++){
        fgets(dados_t,200,P);
        temp = strtok(dados_t,s);
        strcpy(referencia[i].nome,temp);
        temp = strtok(NULL,s);temp = strtok(NULL,s);temp = strtok(NULL,s);
        referencia[i].v_proximo = atoi(temp);
        temp = strtok(NULL,s);
        referencia[i].dist_v = atoi(temp);
    }
}




