#ifndef QUORIDOR_H
#define QUORIDOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//définitions
#define TAILLE_PLAT 9
#define MAX_NOM 50
#define MAX_JOUEURS 4

//Structure d'un joueur
typedef struct {
    char nom[MAX_NOM];
    int x, y;
    int objectif;
    int objectifx;
    int barrieres;
    char pion;
    int couleur;
} Joueur;

//Structure pour la grille de jeu incluant les barrières
typedef struct {
    char cases[TAILLE_PLAT][TAILLE_PLAT];
    bool barrieres_h[TAILLE_PLAT+1][TAILLE_PLAT];
    bool barrieres_v[TAILLE_PLAT][TAILLE_PLAT+1];
} Plateau;

//Structure pour sauvgr l'état précédent nécessaire aux annulations
typedef struct {
    Plateau plateau;
    Joueur joueurs[MAX_JOUEURS];
} Etat;

//Structure pour gérer les scores des joueurs
typedef struct {
    char nom[MAX_NOM];
    int score;
} ScoreJ;

// Structure pour sauvgr une partie
typedef struct {
    Plateau plateau;
    Joueur joueurs[MAX_JOUEURS];
    int nbJoueurs;
    int joueurActuel;
} sauvgPartie;

//Fonctions d'affichage et d'initialisation
void Color(int couleurDuTexte, int couleurDeFond);
void Initplateau(Plateau *plateau);
void AffPlateau(Plateau *plateau, Joueur *joueurs, int nbJoueurs);
void InitJoueur(Joueur *joueur, int index);
void InitJoueurs(Joueur joueurs[], int nbJoueurs, Plateau *plateau);

//Fonctions de déplacement
bool peutDeplacer(Plateau *plateau, int x, int y, char direction);
int deplacerPion(Joueur *joueur, char direction, Plateau *plateau);

//fonction barrières
int Posebarriere(Joueur *joueur, int x1, int y1, int x2, int y2, char direction, Plateau *plateau);

//Fonctions pour garde états temporairement
void CopyEtat(Etat *destination, Etat *source, int nbJoueurs);

//fonctions de sauvg et chargement de partie
void sauvgrScores(ScoreJ *score, int nbJoueurs);
int chargerS(ScoreJ *scores);
void sauverPartie(sauvgPartie *sauvg);
int chargerPartie(sauvgPartie *sauvg);

//Fonctions de menu et interface
void Annuler(Etat *etatActuel, Etat *etatPrecedent, Plateau *plateau, Joueur joueurs[], int nbJoueurs);
void AffScores(ScoreJ *scores, int nbScores);
void MajScore(ScoreJ *scores, int *nbScores, const char *nom, int points);
void AffAide(void);

#endif