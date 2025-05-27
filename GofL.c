#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINII 1000
#define MAX_COLOANE 1000

// Structura pentru o celula modificata 
typedef struct Celula 
{
    int linie, coloana;           // pozitia celulei in matrice
    struct Celula* urmator;           // pointer urmatoarea celula din lista
} Celula;

// Lista de celule (retine modificarile)
typedef struct Lista 
{
    Celula* inceput;              // pointer catre inceputul listei
} Lista;

// Stiva de liste (folosita la task 2 pentru undo)
typedef struct NodStiva 
{
    Lista* lista;                 // pointer catre lista de modificari
    struct NodStiva* urmator;         // pointer catre urmatorul nod din stiva
} NodStiva;

// Arbore binar pentru Task 3 (fiecare nod are o matrice si pointeri la stanga/dreapta)
typedef struct NodArbore 
{
    Lista* lista;                 // pointer catre lista de modificari (nefolosit la task 3)
    char matrice[MAX_LINII][MAX_COLOANE]; // matricea asociata nodului
    struct NodArbore *stanga, *dreapta; // pointeri catre copii
} NodArbore;

// Vector cu directiile pentru vecinii unei celule (8 directii)
int directii[8][2] = 
{
    {-1,-1}, {-1,0}, {-1,1},
    { 0,-1},         { 0,1},
    { 1,-1}, { 1,0}, { 1,1}
};


// Functie care numara vecinii vii ai unei celule
int numara_vecini_vii(const char matrice[MAX_LINII][MAX_COLOANE], int l, int c, int n, int m) 
{
    int vecini = 0;
    for (int d = 0; d < 8; d++) 
    {
        int nl = l + directii[d][0];
        int nc = c + directii[d][1];
    
        // verifica daca vecinul este in interiorul matricei si este viu
        if (nl >= 0 && nl < n && nc >= 0 && nc < m && matrice[nl][nc] == 'X') 
        {
            vecini++;
        }
    }
    return vecini;
}

// Copiaza o matrice in alta matrice
void copie_matrice(char sursa[MAX_LINII][MAX_COLOANE], char destinatie[MAX_LINII][MAX_COLOANE], int n, int m) 
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < m; j++) 
        {
            destinatie[i][j] = sursa[i][j];
        }
    }
}

// Afiseaza o matrice in fisierul out
void afiseaza_matrice(FILE* out, const char matrice[MAX_LINII][MAX_COLOANE], int n, int m) 
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < m; j++) 
        {
            fputc(matrice[i][j], out);
        }
        fputc('\n', out);
    }
    fputc('\n', out);
}

// Adauga o celula in lista de modificari (in ordine crescatoare dupa linie si coloana)
void adauga_celula(Lista* lista, int l, int c) 
{
    Celula* nou = malloc(sizeof(Celula));
    nou->linie = l;
    nou->coloana = c;
    nou->urmator = NULL;

    // daca lista e goala sau celula trebuie pusa la inceput
    if (!lista->inceput || l < lista->inceput->linie || (l == lista->inceput->linie && c < lista->inceput->coloana)) 
    {
        nou->urmator = lista->inceput;
        lista->inceput = nou;
        return;
    }

    // altfel, cauta pozitia corecta in lista
    Celula* curent = lista->inceput;
    while (curent->urmator && (curent->urmator->linie < l || (curent->urmator->linie == l && curent->urmator->coloana < c))) 
    {
        curent = curent->urmator;
    }
    nou->urmator = curent->urmator;
    curent->urmator = nou;
}

// Returneaza lista de celule care difera intre doua matrici
Lista* diferenta_matrici(const char m1[MAX_LINII][MAX_COLOANE], const char m2[MAX_LINII][MAX_COLOANE], int n, int m) // m1 = matrice 1 m2 = matrice 2, pentru a face diferenta intre doua matrici
{
    Lista* lista = malloc(sizeof(Lista));

    lista->inceput = NULL;

    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < m; j++) 
        {
            if (m1[i][j] != m2[i][j]) 
            {
                adauga_celula(lista, i, j);
            }
        }
    }
    return lista;
}

// Pune o lista pe stiva (folosita la task 2)
void push_stiva(NodStiva** stiva, Lista* lista) 
{
    NodStiva* nod = malloc(sizeof(NodStiva));
    nod->lista = lista;
    nod->urmator = *stiva;
    *stiva = nod;
}

// TASK 1: Simuleaza K generatii si afiseaza matricea dupa fiecare generatie
void task1(FILE* in, FILE* out) 
{
    int n, m, k, t;
    fscanf(in, "%d", &t); // citeste tipul taskului (nu se foloseste aici)
    fscanf(in, "%d %d", &n, &m); // citeste dimensiunile matricei
    fscanf(in, "%d", &k); // citeste numarul de generatii
    char matrice[MAX_LINII][MAX_COLOANE], urmatoarea_matrice[MAX_LINII][MAX_COLOANE];
    for (int i = 0; i < n; i++) fscanf(in, "%s", matrice[i]); // citeste matricea initiala
    for (int g = 0; g < k; g++) 
    { // pentru fiecare generatie
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) 
            {
                int v = numara_vecini_vii(matrice, i, j, n, m); // numara vecinii vii
                // aplica regulile
                urmatoarea_matrice[i][j] = (matrice[i][j] == 'X' ? (v < 2 || v > 3 ? '+' : 'X') : (v == 3 ? 'X' : '+'));
            }
        copie_matrice(urmatoarea_matrice, matrice, n, m); // copiaza matricea noua in matricea curenta
        afiseaza_matrice(out, matrice, n, m); // afiseaza matricea
    }
}

// TASK 2: Simuleaza K generatii si afiseaza modificarile pentru fiecare generatie (folosind stiva)
void task2(FILE* in, FILE* out) 
{
    int n, m, k, t;
    fscanf(in, "%d", &t);
    fscanf(in, "%d %d", &n, &m);
    fscanf(in, "%d", &k);
    char matrice[MAX_LINII][MAX_COLOANE], urmatoarea_matrice[MAX_LINII][MAX_COLOANE];
    for (int i = 0; i < n; i++) fscanf(in, "%s", matrice[i]);
    NodStiva* stiva = NULL;

    for (int g = 0; g < k; g++) 
    {
        for (int i = 0; i < n; i++) 
            for (int j = 0; j < m; j++) 
            {
                int v = numara_vecini_vii(matrice, i, j, n, m);
                urmatoarea_matrice[i][j] = (matrice[i][j] == 'X' ? (v < 2 || v > 3 ? '+' : 'X') : (v == 3 ? 'X' : '+'));
            }
        Lista* schimbari = diferenta_matrici(matrice, urmatoarea_matrice, n, m); // retine modificarile
        push_stiva(&stiva, schimbari); // pune modificarile pe stiva
        copie_matrice(urmatoarea_matrice, matrice, n, m); // actualizeaza matricea
    }
    NodStiva* curent = stiva; int nr = k;
    while (curent) 
    { // afiseaza modificarile de la ultima la prima generatie
        for (Celula* c = curent->lista->inceput; c; c = c->urmator)
            fprintf(out, "%d %d %d\n", nr, c->linie, c->coloana);
        curent = curent->urmator; nr--;
    }
}

// TASK 3: Construieste un arbore binar cu regula B (stanga) si regula standard (dreapta), afiseaza in preordine
NodArbore* creeaza_nod(char matrice[MAX_LINII][MAX_COLOANE], int n, int m) 
{
    NodArbore* nod = malloc(sizeof(NodArbore));
    copie_matrice(matrice, nod->matrice, n, m); // copiaza matricea in nod
    nod->lista = NULL;
    nod->stanga = nod->dreapta = NULL;
    return nod;
}

// Aplica o regula pe o matrice si pune rezultatul in noua matrice
void aplica_regula(char matrice[MAX_LINII][MAX_COLOANE], char noua[MAX_LINII][MAX_COLOANE], int n, int m, int regula_b) 
{
    for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) 
    {
        int v = numara_vecini_vii(matrice, i, j, n, m);
        // daca regula_b==1 aplica regula B, altfel regula standard
        noua[i][j] = (regula_b ? (v == 2 ? 'X' : '+') : (matrice[i][j] == 'X' ? (v < 2 || v > 3 ? '+' : 'X') : (v == 3 ? 'X' : '+')));
    }
}

// Construieste recursiv arborele binar pentru task 3
void construire_arbore(NodArbore* rad, int k, int curent, int n, int m) 
{
    if (curent >= k) return; // daca am ajuns la nivelul maxim, opreste recursivitatea
    char stanga[MAX_LINII][MAX_COLOANE], dreapta[MAX_LINII][MAX_COLOANE];
    aplica_regula(rad->matrice, stanga, n, m, 1); // aplica regula B pentru copilul stang
    aplica_regula(rad->matrice, dreapta, n, m, 0); // aplica regula standard pentru copilul drept
    rad->stanga = creeaza_nod(stanga, n, m); // creeaza copilul stang
    rad->dreapta = creeaza_nod(dreapta, n, m); // creeaza copilul drept
    construire_arbore(rad->stanga, k, curent + 1, n, m); // recursiv pentru stanga
    construire_arbore(rad->dreapta, k, curent + 1, n, m); // recursiv pentru dreapta
}

// Parcurge arborele in preordine si afiseaza fiecare matrice
void afiseaza_arbore_preordine(FILE* out, NodArbore* nod, int n, int m) 
{
    if (!nod) return;
    afiseaza_matrice(out, nod->matrice, n, m); // afiseaza grila nodului curent
    afiseaza_arbore_preordine(out, nod->stanga, n, m); // recursiv pe stanga
    afiseaza_arbore_preordine(out, nod->dreapta, n, m); // recursiv pe dreapta
}

// Functia pentru task 3 (citeste datele, construieste arborele si afiseaza in preordine)
void task3(FILE* in, FILE* out) 
{
    int n, m, k, t;
    fscanf(in, "%d", &t);
    fscanf(in, "%d %d", &n, &m);
    fscanf(in, "%d", &k);
    char matrice[MAX_LINII][MAX_COLOANE];
    for (int i = 0; i < n; i++) fscanf(in, "%s", matrice[i]);
    NodArbore* radacina = creeaza_nod(matrice, n, m); // creeaza radacina arborelui
    construire_arbore(radacina, k, 0, n, m); // construieste arborele
    afiseaza_arbore_preordine(out, radacina, n, m); // afiseaza in preordine
}

// Structura pentru un varf din drum (folosita la task 4)
typedef struct Varf 
{
    int linie, coloana;
} Varf;

// Vectori pentru directiile de deplasare (sus, dreapta, jos, stanga)
int dx4[4] = {-1, 0, 1, 0};
int dy4[4] = {0, 1, 0, -1};

int vizitat[100][100]; // matrice pentru marcarea celulelor vizitate
Varf drum_maxim[10000]; // retine drumul maxim gasit
Varf drum_temp[10000];  // retine drumul curent in DFS
int lungime_maxima;     // lungimea maxima gasita
int n_global, m_global; // dimensiunile grilei (globale pentru DFS)

// Functie DFS pentru gasirea lantului Hamiltonian maxim
void dfs_hamiltonian(char matrice[MAX_LINII][MAX_COLOANE], int l, int c, int lungime, int total, int index) 
{
    vizitat[l][c] = 1; // marcheaza celula ca vizitata
    drum_temp[index].linie = l; // salveaza pozitia in drumul curent
    drum_temp[index].coloana = c;

    if (lungime > lungime_maxima) 
    { // daca am gasit un drum mai lung
        lungime_maxima = lungime;
        for (int i = 0; i <= lungime; i++) 
        {
            drum_maxim[i] = drum_temp[i]; // salveaza drumul maxim
        }
    }

    // incearca sa continue drumul in toate cele 4 directii
    for (int d = 0; d < 4; d++) 
    {
        int nl = l + dx4[d], nc = c + dy4[d];
        if (nl >= 0 && nl < n_global && nc >= 0 && nc < m_global &&
            matrice[nl][nc] == 'X' && !vizitat[nl][nc]) {
            dfs_hamiltonian(matrice, nl, nc, lungime + 1, total, index + 1);
        }
    }
    vizitat[l][c] = 0; // demarcheaza celula la intoarcerea din recursivitate
}

// Determina si afiseaza cel mai lung lant Hamiltonian dintr-o matrice
void determina_lant_hamiltonian(FILE* out, char grila[MAX_LINII][MAX_COLOANE], int n, int m) 
{
    n_global = n;
    m_global = m;
    lungime_maxima = -1;

    // incearca sa porneasca DFS din fiecare celula vie
    for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) 
    {
        if (grila[i][j] == 'X') 
        {
            memset(vizitat, 0, sizeof(vizitat));
            dfs_hamiltonian(grila, i, j, 0, 0, 0);
        }
    }

    fprintf(out, "%d\n", lungime_maxima); // afiseaza lungimea maxima
    if (lungime_maxima != -1) 
    {
        for (int i = 0; i <= lungime_maxima; i++) 
        {
            fprintf(out, "(%d,%d) ", drum_maxim[i].linie, drum_maxim[i].coloana); // afiseaza drumul
        }
        fprintf(out, "\n");
    }
}

// Parcurge arborele si pentru fiecare nod determina lantul Hamiltonian maxim
void parcurgere_task4(NodArbore* nod, FILE* out, int n, int m) 
{
    if (!nod) return;
    determina_lant_hamiltonian(out, nod->matrice, n, m); // determina si afiseaza pentru nodul curent
    parcurgere_task4(nod->stanga, out, n, m); // recursiv pe stanga
    parcurgere_task4(nod->dreapta, out, n, m); // recursiv pe dreapta
}

// Functia pentru task 4 (citeste datele, construieste arborele si parcurge pentru lanturi Hamiltoniene)
void task4(FILE* in, FILE* out) 
{
    int n, m, k, t;
    fscanf(in, "%d", &t);
    fscanf(in, "%d %d", &n, &m);
    fscanf(in, "%d", &k);
    char matrice[MAX_LINII][MAX_COLOANE];
    for (int i = 0; i < n; i++) fscanf(in, "%s", matrice[i]);
    NodArbore* radacina = creeaza_nod(matrice, n, m);
    construire_arbore(radacina, k, 0, n, m);
    parcurgere_task4(radacina, out, n, m);
}

// Bonus Task 2

void aplica_invers_modificari(char matrice[MAX_LINII][MAX_COLOANE], Lista* modificari) {
    for (Celula* c = modificari->inceput; c; c = c->urmator) {
        matrice[c->linie][c->coloana] = (matrice[c->linie][c->coloana] == 'X') ? '+' : 'X';
    }
}

void undo(char matrice[MAX_LINII][MAX_COLOANE], NodStiva** stiva) {
    if (!*stiva) return;
    aplica_invers_modificari(matrice, (*stiva)->lista);
    NodStiva* sters = *stiva;
    *stiva = (*stiva)->urmator;
    free(sters);
}

void bonus(FILE* in, FILE* out) {
    int n, m, k, t;
    fscanf(in, "%d", &t);
    fscanf(in, "%d %d", &n, &m);
    fscanf(in, "%d", &k);

    char matrice[MAX_LINII][MAX_COLOANE], urmatoarea_matrice[MAX_LINII][MAX_COLOANE];
    for (int i = 0; i < n; i++) fscanf(in, "%s", matrice[i]);
    NodStiva* stiva = NULL;

    for (int g = 1; g <= k; g++) {
        for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) {
            int v = numara_vecini_vii(matrice, i, j, n, m);
            urmatoarea_matrice[i][j] = (matrice[i][j] == 'X' ? (v < 2 || v > 3 ? '+' : 'X') : (v == 3 ? 'X' : '+'));
        }
        Lista* schimbari = diferenta_matrici(matrice, urmatoarea_matrice, n, m);
        push_stiva(&stiva, schimbari);
        copie_matrice(urmatoarea_matrice, matrice, n, m);
    }

    // Undo complet pentru a reveni la generatia 0
    while (stiva) {
        undo(matrice, &stiva);
    }

    // Afiseaza generatia 0
    fprintf(out, "Generatia 0:\n");
    afiseaza_matrice(out, matrice, n, m);
} 


// Functia principala (main): alege taskul in functie de argumentul din linia de comanda
int main(int argc, const char* argv[]) 
{
    if (argc < 4) 
    {
        printf("Utilizare: ./program input.txt output.txt task\n");
        return 1;
    }

    FILE* fisier_input = fopen(argv[1], "r");
    FILE* fisier_output = fopen(argv[2], "w");
    int task = atoi(argv[3]);
    if (task == 1) task1(fisier_input, fisier_output);
    else if (task == 2) task2(fisier_input, fisier_output);
    else if (task == 3) task3(fisier_input, fisier_output);
    else if (task == 4) task4(fisier_input, fisier_output);
    else if (task == 5) bonus(fisier_input, fisier_output);
    else fprintf(stderr, "Taskul %d nu este complet implementat.\n", task);
    fclose(fisier_input);
    fclose(fisier_output);
    return 0;
}