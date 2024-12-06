#include "quoridor.h"

//sauvergarde des scores dans un fichier txt
void sauvgrScores(ScoreJ *scores, int nbScores) {
    FILE *fichier = fopen("scores.txt", "w");
    if (fichier != NULL) {
        fprintf(fichier, "SCORES_QUORIDOR\n");
        fprintf(fichier, "%d\n", nbScores);
        // Écriture des scores
        for (int i = 0; i < nbScores; i++) {
            fprintf(fichier, "%s %d\n", scores[i].nom, scores[i].score);
        }
        fclose(fichier);
    } else {
        printf("Erreur lors de la sauvg des scores\n");
    }
}

//charger les scores précédemment enregistrés
int chargerS(ScoreJ *scores) {
    FILE *fichier = fopen("scores.txt", "r");
    int nbScores = 0;

    if (fichier != NULL) {
        char entete[20];
        if (fgets(entete, sizeof(entete), fichier) && strcmp(entete, "SCORES_QUORIDOR\n") == 0) {
            fscanf(fichier, "%d\n", &nbScores);
            //lire chaque score
            for (int i = 0; i < nbScores && i < MAX_JOUEURS * 10; i++) {
                if (fscanf(fichier, "%s %d\n", scores[i].nom, &scores[i].score) != 2) {
                    nbScores = i;
                    break;
                }
            }
        }
        fclose(fichier);
    }
    return nbScores;
}

//Mettre la sauvergarde dans fichier pour la charger ultérieurement
void sauverPartie(sauvgPartie *sauvg) {
    FILE *fichier = fopen("partie.txt", "wb");
    if (fichier != NULL) {
        fwrite(sauvg, sizeof(sauvgPartie), 1, fichier);
        fclose(fichier);
        printf("Partie sauvge avec succes\n");
    } else {
        printf("Erreur lors de la sauvg de la partie.\n");
    }
}

//charger la partie sauvergarder
int chargerPartie(sauvgPartie *sauvg) {
    FILE *fichier = fopen("partie.txt", "rb");
    if (fichier != NULL) {
        fread(sauvg, sizeof(sauvgPartie), 1, fichier);
        fclose(fichier);
        printf("Partie chargee avec succes\n");
        return 1;
    }
    printf("Aucune partie sauvge trouvee\n");
    return 0;
}