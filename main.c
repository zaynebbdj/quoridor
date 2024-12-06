//sources et documentations :
//https://cplusplus.com/reference/cstdio/
//https://www.redblobgames.com/pathfinding/a-star/implementation.html (pathfinding non réussi)
//https://gameprogrammingpatterns.com/state.html
//https://stackoverflow.com/questions/15707933/how-to-serialize-a-struct-in-c

#include "quoridor.h"

//Programme principal
int main() {
    //déclaration des variables
    Plateau plateau;
    Joueur joueurs[MAX_JOUEURS];
    Etat etatActuel, etatPrecedent;
    ScoreJ scores[MAX_JOUEURS * 10];
    int nbScores = chargerS(scores);
    int nbJoueurs;
    int joueurActuel = 0;
    bool partieTerm = false;
    int choixMenu;
    char direction;
    char choix;
    int deplacementValide = 0;
    char lig1;
    int col1;
    char lig2;
    int col2;
    char position;
//boucle tant que dans le jeu
    while (1) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Nouvelle partie\n");
        printf("2. Charger une partie\n");
        printf("3. Voir les scores\n");
        printf("4. Aide\n");
        printf("5. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choixMenu);

//Choix de l'utilisateur dans le menu, déclanchant les fonctions
        switch (choixMenu) {
            //nouvelle partie
            case 1: {
                Initplateau(&plateau);
                do {
                    printf("Entrez le nombre de joueurs (2 ou 4) : ");
                    scanf("%d", &nbJoueurs);
                } while (nbJoueurs!=2 && nbJoueurs!=4);

                //création joueur avec leur propre pion
                for (int i=0; i < nbJoueurs; i++) {
                    InitJoueur(&joueurs[i], i);
                    joueurs[i].barrieres= (nbJoueurs == 2) ? 10 : 5;
                }

                //positions de départ sur la grille
                int positions[4][2] = {
                    {4, 0}, {4, 8}, {0, 4}, {8, 4}
                };

                //choix ordre de jeu
                srand(time(NULL));
                for (int i= nbJoueurs-1; i>0; i--) {
                    int j= rand() % (i+1);
                    // Échanger les joueurs i et j
                    Joueur temp = joueurs[i];
                    joueurs[i] = joueurs[j];
                    joueurs[j] = temp;
                }

                //donné objectif/position gagnante de chaque joueur
                for (int i = 0; i < nbJoueurs; i++) {
                    joueurs[i].x = positions[i][0];
                    joueurs[i].y = positions[i][1];
                    joueurs[i].objectif = (i % 2 == 0) ? 8 : 0;
                    plateau.cases[positions[i][1]][positions[i][0]] = joueurs[i].pion;
                }
                //affichage début de jeu (qui joue en premier, nom et pion)
                printf("\nOrdre des joueurs tire au hasard :\n");
                for (int i = 0; i < nbJoueurs; i++) {
                    Color(joueurs[i].couleur, 0);
                    printf("%d. %s\n", i+1, joueurs[i].nom);
                    Color(15, 0);
                }

                memcpy(&etatActuel.plateau, &plateau, sizeof(Plateau));
                memcpy(etatActuel.joueurs, joueurs, sizeof(joueurs));
                break;
            }
            //choix reprendre la partie sauvergardée
            case 2:
                sauvgPartie sauvg;
                if (chargerPartie(&sauvg)) {
                    plateau = sauvg.plateau;
                    memcpy(joueurs, sauvg.joueurs, sizeof(joueurs));
                    nbJoueurs = sauvg.nbJoueurs;
                    joueurActuel = sauvg.joueurActuel;
                } else {
                    continue;
                }
                break;
            //Choix d'afficher les scores enregistrés
            case 3:
                AffScores(scores, nbScores);
                printf("Appuyez sur Entree pour continuer");
                getchar();
                getchar();
                continue;
            //Choix d'afficher l'aide
            case 4:
                AffAide();
                continue;
            //Choix de quitter
            case 5:
                return 0;
        }

        //Boucle de jeu, avec les différents choix possibles: déplcement, barrière, passer, annuler
        //et sauvergarder la partie Tant que la partie n'est pas terminée
        partieTerm = false;
        while (!partieTerm) {

            //affichage plateau et communication à l'utilisateur de jouer
            AffPlateau(&plateau, joueurs, nbJoueurs);
            printf("\nC'est au tour de %s.\n", joueurs[joueurActuel].nom);
            printf("Que voulez-vous faire ? (D pour Deplacer, B pour Barriere, P pour Passer, A pour Annuler, S pour sauvgr) : ");
            scanf(" %c", &choix);

            //choix de sauvergarde
            if (choix == 'S' || choix == 's') {
                sauvgPartie sauvg = {
                    .plateau = plateau,
                    .nbJoueurs = nbJoueurs,
                    .joueurActuel = joueurActuel
                };
                memcpy(sauvg.joueurs, joueurs, sizeof(joueurs));
                sauverPartie(&sauvg);
                break;
            }
            //choix de se déplacer
            if (choix == 'D' || choix == 'd') {

                //blindage avec vérification que le joueur peut bien se déplacer là où il a décidé d'aller
                //Le joueur doit donc Entrer une direction jusqu'à ce qu'elle soit valide
                do {
                    printf("Entrez une direction (H pour haut, B pour bas, G pour gauche, D pour droite) : ");
                    scanf(" %c", &direction);

                    //On garde temporairement l'étape précédente si le joueur choisit d'annuler la précédente action
                    CopyEtat(&etatPrecedent, &etatActuel, nbJoueurs);
                    deplacementValide = deplacerPion(&joueurs[joueurActuel], direction, &plateau);

                    if (deplacementValide) {
                        memcpy(&etatActuel.plateau, &plateau, sizeof(Plateau));
                        memcpy(etatActuel.joueurs, joueurs, sizeof(joueurs));

                        //On vérifie si le déplacement a permis au joueur de gagner
                        if (joueurs[joueurActuel].y == joueurs[joueurActuel].objectif) {
                            AffPlateau(&plateau, joueurs, nbJoueurs);
                            Color(10, 0);
                            printf("\nFELICITATIONS ! %s a gagne !\n", joueurs[joueurActuel].nom);
                            Color(15, 0);
                            //Attribution des points du gagnant
                            MajScore(scores, &nbScores, joueurs[joueurActuel].nom, 5);

                            //Score actualisé des joueurs
                            Color(14, 0);
                            printf("\nScore de %s : ", joueurs[joueurActuel].nom);

                            //On affiche le nombre de points du gagnant
                            for (int i = 0; i < nbScores; i++) {
                                if (strcmp(scores[i].nom, joueurs[joueurActuel].nom) == 0) {
                                    printf("%d points\n", scores[i].score);
                                    break;
                                }
                            }
                            Color(15, 0);
                            //partie terminée pour sortir de la boucle
                            partieTerm = true;
                            Sleep(3000);
                            break;
                        }
                        }
                      else {
                        //Si déplacement invalide
                        Color(12, 0);
                        printf("Deplacement impossible !\n");
                        Color(15, 0);
                    }
                } while (!deplacementValide);

                //on passe au tour du joueur suivant
                if (!partieTerm) {
                    joueurActuel = (joueurActuel + 1) % nbJoueurs;
                }

            //le joueur décide de poser une barrière
            }else if (choix == 'B' || choix == 'b') {

                //on vérifie qu'il lui en reste
                if (joueurs[joueurActuel].barrieres > 0) {
                    printf("\nPremiere case: ");
                    scanf(" %c%d", &lig1, &col1);
                    printf("Deuxieme case adjacente: ");
                    scanf(" %c%d", &lig2, &col2);

                    //conversion car on prend des caractères mais utilise des chiffres dans la fonction car plus simple avec un int
                    int y1 = lig1 -'A';
                    int x1 = col1;
                    int y2 = lig2 -'A';
                    int x2 = col2;

                    //Choix position barrières par rapport aux cases différents si on prend des cases adjacentes verticalement ou horizontalement
                    if (y1 == y2) {
                        printf("Position de la barriere (H pour haut, B pour bas) : ");
                    } else if (x1 == x2) {
                        printf("Position de la barriere (G pour gauche, D pour droite) : ");
                    }
                    scanf(" %c", &position);
                    CopyEtat(&etatPrecedent, &etatActuel, nbJoueurs);
                    if (Posebarriere(&joueurs[joueurActuel], x1, y1, x2, y2, position, &plateau)) {
                        memcpy(&etatActuel.plateau, &plateau, sizeof(Plateau));
                        memcpy(etatActuel.joueurs, joueurs, sizeof(joueurs));
                        joueurActuel = (joueurActuel + 1) % nbJoueurs;
                    }
                } else {
                    printf("Plus de barrieres disponibles.\n");
                }

                //le joueur décide de passer son tour
            } else if (choix == 'P' || choix == 'p') {
                joueurActuel = (joueurActuel + 1) % nbJoueurs;
                printf("\nLe joueur a passe son tour\n");

                //le joueur décide d'annuler la dernière action
            } else if (choix == 'A' || choix == 'a') {
                Annuler(&etatActuel, &etatPrecedent, &plateau, joueurs, nbJoueurs);
            }
        }
    }

    return 0;
}