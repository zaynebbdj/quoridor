#include "quoridor.h"

// Fonction pour afficher le plateau avec barrières
void AffPlateau(Plateau *plateau, Joueur *joueurs, int nbJoueurs) {
    printf("\nPlateau de jeu :\n");
    //numéros de colonnes
    printf("     ");
    for(int j = 0; j < TAILLE_PLAT; j++) {
        printf("%d   ", j);
    }
    printf("\n");

    for (int i = 0; i < TAILLE_PLAT; i++) {
        //Afficher les cases et barrières horizontales
        printf("   +");
        for (int j = 0; j < TAILLE_PLAT; j++) {
            if (plateau->barrieres_h[i][j]) {
                Color(12, 0);
                printf("---");
                Color(15, 0);
            } else {
                printf("---");
            }
            printf("+");
        }
        printf("\n");

        //Afficher les cases et les barrières avec la lettre de la ligne
        printf(" %c ", 'A' + i);
        for (int j = 0; j < TAILLE_PLAT; j++) {
            if (plateau->barrieres_v[i][j]) {
                Color(12, 0);
                printf("|");
                Color(15, 0);
            } else {
                printf("|");
            }

            //Afficher le pion des joueurs
            bool pion_trouve = false;
            for (int k = 0; k < nbJoueurs; k++) {
                if (joueurs[k].x == j && joueurs[k].y == i) {
                    Color(joueurs[k].couleur, 0);
                    printf(" %c ", joueurs[k].pion);
                    Color(15, 0);
                    pion_trouve = true;
                    break;
                }
            }
            if (!pion_trouve) {
                printf("   ");
            }
        }
        printf("|\n");
    }

    printf("   +");
    for (int j = 0; j < TAILLE_PLAT; j++) {
        printf("---+");
    }
    printf("\n");

    //afficher les barrières restantes pour chaque joueur
    printf("\nBarrieres restantes :\n");
    for (int i = 0; i < nbJoueurs; i++) {
        Color(joueurs[i].couleur, 0);
        printf("%s : %d barrieres", joueurs[i].nom, joueurs[i].barrieres);
        Color(15, 0);
        printf("\n");
    }
}

void MajScore(ScoreJ *scores, int *nbScores, const char *nom, int points) {
    int trouve = 0;
    //Chercher si le joueur existe déjà
    //ajouter les points
    for (int i = 0; i < *nbScores; i++) {
        if (strcmp(scores[i].nom, nom) == 0) {
            scores[i].score += points;
            trouve = 1;
            break;
        }
    }

    //Cas nouveau joueur
    if (!trouve && *nbScores < MAX_JOUEURS*10) {
        strncpy(scores[*nbScores].nom, nom, MAX_NOM - 1);
        scores[*nbScores].nom[MAX_NOM - 1] = '\0';
        scores[*nbScores].score = points;
        (*nbScores)++;
    }

    //sauvgr les scores mis à jour
    sauvgrScores(scores, *nbScores);
}

//afficher les scores
void AffScores(ScoreJ *scores, int nbScores) {
    Color(14, 0);
    printf("\n=== TABLEAU DES SCORES ===\n");
    Color(15, 0);
    //affiche des scores des différents joueurs enregistrés s'il y en a
    if (nbScores == 0) {
        printf("Aucun score enregistre.\n");
    } else {
        printf("%-20s | %s\n", "Nom", "Points");
        printf("----------------------------------------\n");
        for (int i = 0; i < nbScores; i++) {
            printf("%-20s | %d\n", scores[i].nom, scores[i].score);
        }
    }
    printf("=======================\n\n");
}

