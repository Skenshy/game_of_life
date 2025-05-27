README – Game of Life în C

1. Descriere generală
Acest proiect este o implementare completă în limbajul C a automatului celular "Conway's Game of Life".
Game of Life este un joc matematic care simulează evoluția unei populații pe o grilă bidimensională,
în funcție de vecinătatea celulelor. Detalii suplimentare despre joc se pot găsi la:
https://site-pa.netlify.app/proiecte/game_of_life/

Programul este structurat pe patru taskuri principale și un bonus, fiecare tratând o funcționalitate 
diferită și crescând gradual în complexitate.

2. Cum se utilizează
Pentru rularea programului este necesar:
- Un compilator C (recomandat: gcc)
- Sistem compatibil POSIX (Linux, macOS sau Windows cu WSL/Cygwin)

Pentru a compila programul:
    gcc GofL.c -o game 

Pentru a rula programul:
    ./game input.txt output.txt <task>
Unde <task> este un număr de la 1 la 5 corespunzător funcționalității dorite:
- 1: simulare Game of Life clasic
- 2: afișare diferențe între generații
- 3: construirea arborelui binar cu două reguli
- 4: determinare lanțuri Hamiltoniene
- 5: reconstrucție generație 0 din generația K folosind undo

3. Structura și organizare
Fișierul `GofL.c` conține întregul cod sursă. Acesta este organizat în mai multe secțiuni:

- Structuri de date:
    - `Celula`, `Lista`, `NodStiva`, `NodArbore`, `Varf` – structuri esențiale pentru stocarea stării jocului.
- Funcții generale:
    - `numara_vecini_vii()` – calculează vecinii vii pentru fiecare celulă.
    - `copie_matrice()` – copiază o matrice în alta.
    - `afiseaza_matrice()` – afișează conținutul unei matrici într-un fișier.
- Taskuri:
    - `task1()` – implementează simularea Game of Life pe K generații.
    - `task2()` – salvează modificările dintre generații într-o stivă.
    - `task3()` – construiește un arbore binar cu regula B (pentru stânga) și regula standard (pentru dreapta).
    - `task4()` – caută cel mai lung lanț Hamiltonian în fiecare nod al arborelui.
    - `bonus()` – revine de la generația K la generația 0 folosind stiva de modificări.
- Funcția `main()` selectează taskul dorit pe baza argumentului trimis în linia de comandă.

Toate funcțiile sunt scrise modular, astfel încât pot fi testate și extinse ușor. Codul este comentat extensiv
pentru a ajuta în înțelegerea logicii implementate.
