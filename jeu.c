#include "quoridor.h"

void Color(int couleurDuTexte, int couleurDeFond) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond*16 + couleurDuTexte);
}
//pour vérifier déplcament valide
bool peutDeplacer(Plateau *plateau, int x, int y, char direction) {
    switch(direction) {
        case 'H':
            return (y > 0 && !plateau->barrieres_h[y][x]);
        case 'B':
            return (y < TAILLE_PLAT-1 && !plateau->barrieres_h[y+1][x]);
        case 'G':
            return (x > 0 && !plateau->barrieres_v[y][x]);
        case 'D':
            return (x < TAILLE_PLAT-1 && !plateau->barrieres_v[y][x+1]);
        default:
            return false;
    }
}

//Fonction pour initialiser le plateau
void Initplateau(Plateau *plateau) {
    //Initialiser les cases
    for (int i = 0; i < TAILLE_PLAT; i++) {
        for (int j = 0; j < TAILLE_PLAT; j++) {
            plateau->cases[i][j] = ' ';
        }
    }
    //Initialiser les barrières horizontales
    for (int i = 0; i <= TAILLE_PLAT; i++) {
        for (int j = 0; j < TAILLE_PLAT; j++) {
            plateau->barrieres_h[i][j] = false;
        }
    }
    //Initialiser les barrières verticales
    for (int i = 0; i < TAILLE_PLAT; i++) {
        for (int j = 0; j <= TAILLE_PLAT; j++) {
            plateau->barrieres_v[i][j] = false;
        }
    }
}

//initialisation joueur
void InitJoueur(Joueur *joueur, int index) {
    printf("Joueur %d, entrez votre nom : ", index+1);
    scanf("%s", joueur->nom);
    printf("Choisissez votre pion (un caractere ou code ANSI, ex: 0x03 ) : ");
    char input[10];
    scanf("%s", input);
    if (strncmp(input, "0x", 2) == 0) {
        // Conversion du code hexadécimal en caractère
        int code;
        sscanf(input + 2, "%x", &code);
        joueur->pion = (char)code;
    } else {
        joueur->pion = input[0];
    }
    printf("Choisissez la couleur du pion (1-15): ");
    scanf("%d", &joueur->couleur);
}

//Affichage de l'aide
void AffAide() {

    Color(14, 0);
    printf("\nREGLES DU JEU:\n\n");
    Color(15, 0);
    printf("A votre tour, vous pouvez soit :\n\n");
    Color(11, 0);
    printf("1. DEPLACER votre pion :\n");
    Color(15, 0);  // Blanc
    printf("   Une case a la fois : verticalement ou horizontalement\n");
    printf("   Si un pion adverse est adjacent, vous pouvez le sauter \n");
    printf("   s'il n'y a pas de barriere\n");  // Séparé en deux lignes
    printf("   En partie a 4 joueurs, impossible de sauter plus \n");
    printf("   d'un pion a la fois\n\n");
    Color(11, 0);
    printf("2. POSER UNE BARRIERE :\n");
    Color(15, 0);
    printf("   Entre deux blocs de cases adjacentes\n");
    printf("   Pour bloquer ou rallonger le chemin des adversaires\n");
    printf("   Sans jamais fermer totalement l'acces a la ligne d'arrivee\n");
    Color(11, 0);
    printf("COMMANDES DE JEU :\n");
    Color(15, 0);
    printf("D - Deplacer (H:haut, B:bas, G:gauche, D:droite)\n");
    printf("B - Poser une barriere (indiquer 2 cases adjacentes puis la position)\n");
    printf("P - Passer son tour\n");
    printf("A - Annuler le dernier coup\n");
    printf("S - sauvgr la partie\n\n");
    Color(13, 0);
    printf("SCORES :\n");
    Color(15, 0);
    printf("Victoire : 5 points\n");
    printf("Match nul (blocage) : 0 point\n\n");
    printf("Appuyez sur Entree pour revenir au menu");
    getchar();
    getchar();
}

//fonction pour initialiser les joueurs
void InitJoueurs(Joueur joueurs[], int nbJoueurs, Plateau *plateau) {
    printf("initialisation desjoueurs\n");
    int barrieresInitiales = (nbJoueurs == 2) ? 10 : 5;
    int positions[4][2] = {
        {4, 0},  // Joueur 1
        {4, 8},  // Joueur 2
        {0, 4},  // Joueur 3
        {8, 4}   // Joueur 4
    };
    printf("nombre de joueur : %d\n", nbJoueurs);
    for (int i = 0; i < nbJoueurs; i++) {
        snprintf(joueurs[i].nom, MAX_NOM, "Joueur %d", i + 1);
        joueurs[i].x = positions[i][0];
        joueurs[i].y = positions[i][1];
        joueurs[i].barrieres = barrieresInitiales;
        plateau->cases[joueurs[i].y][joueurs[i].x] = 'P' + i;
    }
}

//fonction pour déplacer un pion
int deplacerPion(Joueur *joueur, char direction, Plateau *plateau) {
    int newX = joueur->x;
    int newY = joueur->y;
    if (!peutDeplacer(plateau, joueur->x, joueur->y, direction)) {
        printf("Deplacement impossible : barriere ou limite du plateau.\n");
        return 0;
    }
    switch (direction) {
        case 'H': newY--; break;
        case 'B': newY++; break;
        case 'G': newX--; break;
        case 'D': newX++; break;
        default:
            printf("Direction invalide. Utilisez H, B, G, ou D.\n");
            return 0;
    }

    //Vérifie si un pion est présent
    if (plateau->cases[newY][newX] != ' ') {
        //Calculer la position de saut
        int sautX = newX + (newX - joueur->x);
        int sautY = newY + (newY - joueur->y);

        //vérifier si le saut est possible
        if (sautX >= 0 && sautX < TAILLE_PLAT &&
            sautY >= 0 && sautY < TAILLE_PLAT &&
            plateau->cases[sautY][sautX] == ' ') {

            //Vérifier les barrières pour le saut
            bool barriereSaut = false;
            if (newY == sautY) { //saut horizontal
                if (plateau->barrieres_v[newY][min(newX, sautX)] ||
                    plateau->barrieres_v[newY][max(newX, sautX)]) {
                    barriereSaut = true;
                }
            } else { //saut vertical
                if (plateau->barrieres_h[min(newY, sautY)][newX] ||
                    plateau->barrieres_h[max(newY, sautY)][newX]) {
                    barriereSaut = true;
                }
            }
            if (!barriereSaut) {
                //Sauter
                char pieceJoueur = plateau->cases[joueur->y][joueur->x];
                plateau->cases[joueur->y][joueur->x] = ' ';
                plateau->cases[sautY][sautX] = pieceJoueur;
                joueur->x = sautX;
                joueur->y = sautY;
                return 1;
            }
        }
        printf("Saut impossible : obstacle ou limite du plateau.\n");
        return 0;
    }

    //Déplacement normal
    char pieceJoueur = plateau->cases[joueur->y][joueur->x];
    plateau->cases[joueur->y][joueur->x] = ' ';
    plateau->cases[newY][newX] = pieceJoueur;
    joueur->x = newX;
    joueur->y = newY;

    return 1;
}

int Posebarriere(Joueur *joueur, int x1, int y1, int x2, int y2, char position, Plateau *plateau) {
    if (joueur->barrieres <= 0) {
        printf("Plus de barrieres disponibles.\n");
        return 0;
    }

    //Vérifier que les cases sont adjacentes
    bool memeligne = (y1 == y2 && abs(x1-x2) == 1);
    bool memecolonne = (x1 == x2 && abs(y1-y2) == 1);

    if (!memeligne && !memecolonne) {
        printf("Les cases doivent etre adjacentes.\n");
        return 0;
    }

    int barrierex = min(x1, x2);
    int barrierey = min(y1, y2);

    //barrière horizontale, au-dessus ou en-dessous
    if (memeligne) {
        if (position != 'H' && position != 'B') {
            printf("Position invalide. Utilisez H (haut) ou B (bas).\n");
            return 0;
        }

        //eviter coordonnées bordure plateau ou en dehors
        if (position == 'H') {
            if (y1 == 0) {
                printf("Impossible de placer une barriere ici (bord du plateau).\n");
                return 0;
            }
            barrierey = y1;
        } else {
            if (y1 == TAILLE_PLAT - 1) {
                printf("Impossible de placer une barriere ici (bord du plateau).\n");
                return 0;
            }
            barrierey = y1 + 1;
        }

        //barrière existe déjà?
        if (plateau->barrieres_h[barrierey][barrierex] ||
            plateau->barrieres_h[barrierey][barrierex + 1]) {
            printf("Il y a deja une barriere ici.\n");
            return 0;
        }

        //placement barrière horizontale
        plateau->barrieres_h[barrierey][barrierex] = true;
        plateau->barrieres_h[barrierey][barrierex + 1] = true;
    }
    //Pour une barrière verticale
    else {
        if (position != 'G' && position != 'D') {
            printf("Position invalide. Utilisez G (gauche) ou D (droite).\n");
            return 0;
        }

        if (position == 'G') {
            if (x1 == 0) {
                printf("Impossible de placer une barriere ici (bord du plateau).\n");
                return 0;
            }
            barrierex = x1;
        } else {
            if (x1 == TAILLE_PLAT - 1) {
                printf("Impossible de placer une barriere ici (bord du plateau).\n");
                return 0;
            }
            barrierex = x1 + 1;
        }

        //Vérifier si une barrière existe déjà
        if (plateau->barrieres_v[barrierey][barrierex] ||
            plateau->barrieres_v[barrierey + 1][barrierex]) {
            printf("Une barriere existe deja ici.\n");
            return 0;
        }

        //placement barrière verticale
        plateau->barrieres_v[barrierey][barrierex] = true;
        plateau->barrieres_v[barrierey + 1][barrierex] = true;
    }

    joueur->barrieres--;
    return 1;
}

// Fonction pour annuler la dernière action
void Annuler(Etat *etatActuel, Etat *etatPrecedent, Plateau *plateau, Joueur joueurs[], int nbJoueurs) {
    printf("Annulation de la derniere action...\n");
    memcpy(plateau, &etatPrecedent->plateau, sizeof(Plateau));
    memcpy(joueurs, etatPrecedent->joueurs, nbJoueurs * sizeof(Joueur));
    CopyEtat(etatActuel, etatPrecedent, nbJoueurs);
    printf("Action annulee avec succes.\n");
}

// Fonction pour copier un état
void CopyEtat(Etat *destination, Etat *source, int nbJoueurs) {
    memcpy(&destination->plateau, &source->plateau, sizeof(Plateau));
    memcpy(destination->joueurs, source->joueurs, nbJoueurs * sizeof(Joueur));
}

