/*
 *
 *
 *      Projet d'algorithmique 2 - Cartographier un labyrinthe
 *      Le but consiste à explorer tout le donjon sans s'épuiser.
 *
 *
 */

#include <stdlib.h>  // null, rand
#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf

#include "dedalus_explorer.h"

const char *    monome = "Daniel Zhu";
const bool   debugMode = false ; // Mettre à TRUE si on veut voir le fil d'Ariane
                                 // et la détection de boucle en mode debug.





/******************************************************************************

    Ensemble de modules relatifs au fil d'Ariane

 *****************************************************************************/

/* --- INSERTION D'UN ÉLÉMENT DANS LE FIL D'ARIANE ----------------------------

 DESCRIPTION :
    Procédure "insérant" un élément dans la fil d'Ariane, en première position.
    
    Pour ce faire, on alloue un espace mémoire que l'on positionne en deuxième
    position de la liste, en décalant l'ancien "premier élément".
    
    Par exemple, on veut insérer West dans le fil. On a avant l'insertion :
    
        (South, @2) -> (East, @1) -> (None, NULL)
        @3             @2            @1
    
    On duplique les membres `m` et `next` du premier élément du fil à l'adresse
    mémoire @4 que l'on crée, ainsi, deux éléments pointent désormais vers
    le deuxième élément du fil :
    
        (South, @2) -+-> (East, @1) -> (None, NULL)     +- (South, @2)
        @3           |  @2            @1                |  @4
                     +----------------------------------+
    
    Puis on remplace les membres du premier élément par les valeurs que l'on
    veut ajouter à la fil d'Ariane et on le pointe vers le doublon :
    
        (West, @4)  -> (South, @2) -> (East, @1) -> (None, NULL)
        @3             @4             @2            @1
    
    D'où l'insertion (il s'agit plus d'un écrasement de valeurs du premier
    élément et d'une insertion de l'ancienne première valeur en 2de position).

    On procède ainsi car on ne peut pas modifier l'adresse du premier élément.
    
    Si le mouvement enregistré est None, on ne l'enregistre pas dans le fil.
    
    
 PARAMÈTRES :
    thread (string) : fil d'Ariane auquel on veut insérer un élément ;
    move (Move)     : mouvement que l'on veut ajouter.
    
 --------------------------------------------------------------------------- */

void ariane_insert(
    string const thread,
      Move const move
) {
    
    // On ne souhaite pas enregistrer un mouvement None dans le fil d'Ariane
    if ( move != None ) {
    
        // Copie des valeurs du premier élément dans un link auxiliaire
        string const tmp = malloc( sizeof(struct link) );
        tmp->m           = thread->m;
        tmp->next        = thread->next;
        
        // Insertion du nouvel élément (en écrasant les valeurs du premier élément)
        // puis lier le premier élément au string auxiliaire
        thread->m    = move;
        thread->next = tmp;
        
    }
}



/* --- SUPPRESSION D'UN ÉLÉMENT DANS LA FIL D'ARIANE --------------------------

 DESCRIPTION :
    Procédure supprimant le premier élément de la fil d'Ariane.
    
    Pour ce faire, on enregistre la valeur des membres `m` et `next` du second
    élément que l'on affecte au premier élément de la liste.
    Puis, l'espace mémoire allouée au second élément est libérée.
    
    Par exemple, on veut retirer le premier élément de la liste suivante :
    
        (South, @2) -> (East, @1) -> (None, NULL)
        @3             @2            @1
    
    On écrase les valeurs de @3 par celles de @2, ce qui donne :
    
        (East, @1) -+ (East, @1) -+-> (None, NULL)
        @3          | @2          |   @1
                    +--------------

    Puis on libère @2 qui n'est plus qu'un doublon de @3 (premier élément)
    
    
 PARAMÈTRE :
    thread (string) : fil d'Ariane auquel on veut retirer le premier élément.
    
 --------------------------------------------------------------------------- */

void ariane_remove(
    string const thread
) {
    
    string const tmp = thread->next;
    
    thread->m    = tmp->m;
    thread->next = tmp->next;
    
    free( tmp );
    
}



/* --- CRÉER LE FIL D'ARIANE --------------------------------------------------

 DESCRIPTION :
    Procédure qui génère le fil d'Ariane à partir de l'arbre d'exploration et de
    la position actuelle de Thésée.
    
    La fonction est récursive et explore chaque sous-arbre jusqu'à retrouver
    la position actuelle de Thésée.
 
 
 PARAMÈTRES :
    thread (string) : fil d'Ariane à modifier ;
    tree (ExpTree)  : arbre d'exploration ;
    pos (ExpTree)   : le noeud contenant la position actuelle de Thésée dans
                      la carte d'exploration ;
    found (bool *)  : booléen transmis par référence, permettant d'annuler
                      toute recherche inutile si Thésée venait à être retrouvé
                      avant la fin du parcours de la totalité de l'arbre.
 
 --------------------------------------------------------------------------- */

void ariane_generate(
     string const         thread,
    ExpTree const         tree,
    ExpTree const         pos,
       bool       * const found // c'est le pointeur que l'on veut constant
) {                             // et non le booléen.
    
    // Insertion du mouvement stocké dans ce noeud
    ariane_insert( thread, tree->m );
    
    // ------------------------------------------------------------------
    // Thésée a été trouvé, Arrêt de la recherche
    
    if ( tree == pos ) {
        
        *found = true;
        
    }
    
    
    // ------------------------------------------------------------------
    // Parcours simpliste de l'arbre. La recherche récursive se fait à condition
    // que le sous-noeud existe et que Thésée n'a pas été retrouvé
    
    else {
        
        // Recherche au Nord
        if ( !(*found) && tree->north ) {
            ariane_generate( thread, tree->north, pos, found );
        }
        
        // Recherche à l'Est
        if ( !(*found) && tree->east ) {
            ariane_generate( thread, tree->east, pos, found );
        }
        
        // Recherche au Sud
        if ( !(*found) && tree->south ) {
            ariane_generate( thread, tree->south, pos, found );
        }
        
        // Recherche à l'Ouest
        if ( !(*found) && tree->west ) {
            ariane_generate( thread, tree->west, pos, found );
        }
        
        // Si malgré la visite de chaque sous-arbre, Thésée demeure introuvable,
        // on supprime le dernier mouvement de la fil.
        if ( !(*found) ) {
            ariane_remove( thread );
        }
        
    }
}



/* --- AFFICHER FIL D'ARIANE --------------------------------------------------

 DESCRIPTION :
    Affiche le fil d'Ariane. Notez que cette procédure est appelée uniquement
    lorsque `debugMode` (défini au début du fichier) vaut TRUE.
 
 
 PARAMÈTRES :
    thread (string) : fil d'Ariane à afficher.
 
 --------------------------------------------------------------------------- */

void ariane_print(
     string const thread
) {
    bool   firstMove = true; // Permet de ne pas afficher de flèche au début
    string       tmp = thread;
    
    printf( "Fil d'Ariane : " );
    
    while ( tmp ) {
        if ( !firstMove ) {
            printf( " < " );
        } else {
            firstMove = false;
        }
        
        switch ( tmp->m ) {
            case North:
                printf( "N" );
                break;
            
            case East:
                printf( "E" );
                break;
            
            case South:
                printf( "S" );
                break;
            
            case West:
                printf( "W" );
                break;
            
            case None:
                printf( "START" );
                break;
            
            default:
                printf( "?" );
                break;
        }
        
        tmp = tmp->next;
    }
    
    printf( "\n" );
}



/* --- INITIALISATION DU FIL D'ARIANE -----------------------------------------

 DESCRIPTION :
    Procédure qui initialise le fil d'Ariane à partir de l'arbre d'exploration
    et de la position actuelle de Thésée.
    
    L'ordre d'enregistrement des positions est chronologiquement décroissant,
    c'est-à-dire que le dernier mouvement effectué par Thésée est le premier
    élément de la liste tandis que le tout premier mouvement effectué est le
    dernier élément de la liste.
    
    Le booléen `found` permet d'arrêter la recherche dans l'arbre si Thésée
    a été retrouvé dans la procédure `ariane_generate`.
 
 
 PARAMÈTRES :
    thread (string) : fil d'Ariane à initialiser et à remplir
    tree (ExpTree)  : arbre d'exploration
    pos (ExpTree)   : le noeud contenant la position actuelle de Thésée dans
                      la carte d'exploration.
 
 --------------------------------------------------------------------------- */

void ariane_init(
     string const thread,
    ExpTree const tree,
    ExpTree const pos
) {
    
    bool found = false;
    
    // Première position enregistrée manuellement (ne sera pas traitée dans
    // la reconstitution)
    thread->m    = None;
    thread->next = NULL;
    
    // Reconstitution du fil d'Ariane à l'aide de l'arbre
    ariane_generate( thread, tree, pos, &found );
    
    // Affichage (si en mode débug)
    if ( debugMode ) {
        ariane_print( thread );
    }
}



/* --- DÉTECTION DE BOUCLE DANS LA FIL D'ARIANE - PROCÉDURE ANTIBOUCLE --------

 DESCRIPTION - PROCÉDURE ANTIBOUCLE
    Fonction indique si une boucle a été détectée à partir du fil d'Ariane
    et de la position actuelle de Thésée.
    
    La fonction compte le nombre de mouvements qui s'opposent, en remontant le
    fil d'Ariane du mouvement le plus récent jusqu'au mouvement le plus ancien.
    
    Si au cours du défilement, on compte un nombre égal de mouvements qui 
    s'opposent, alors on a une boucle et on renvoie TRUE.
    
    La fonction a la possibilité de prendre en compte un mouvement en plus de
    ceux enregistrés dans le fil d'Ariane, ce qui permet d'anticiper une boucle.
 
 
 PARAMÈTRES :
    thread (string) : fil d'Ariane dont on cherche une boucle ;
    nextMove (Move) : si spécifié autrement que `None`, la fonction tentera de 
                      prévoir une boucle si Thésée effectuait ce mouvement 
                      en plus de tous ceux qu'il a fait dans le fil d'Ariane.
 
 RETOUR :
             (bool) : booléen valant TRUE si une boucle a été détectée
                      et FALSE sinon.
 
 --------------------------------------------------------------------------- */

bool ariane_looped(
    string const thread,
      Move const move
) {
    
    // Compteurs des mouvements cardinaux
    int n_counter = 0
      , e_counter = 0
      , s_counter = 0
      , w_counter = 0;
    
    // Parcoureur du fil d'Ariane
    string tmp = thread;
    
    // Si on effectue une prévision de boucle, on ajoute temporaire le
    // mouvement hypothétique au fil d'Ariane
    if ( move != None ) {
        ariane_insert( thread, move );
    }
    
    // Exécuter la boucle tant que le fil d'Ariane est défilable
    // (tmp->next!=NULL). Si une boucle est détectée durant l'itération, sortir
    // du while.
    // Un demi-tour ne comptant pas pour une boucle, on considère que la
    // plus petite boucle possible nécessite à chaque compteur 1 ou plus
    // et que toute boucle compte autant de mouvement vers le Nord que vers
    // le Sud et autant de mouvement vers l'Est que vers l'Ouest.
    while (   tmp->next
           && !(   n_counter == s_counter
                && e_counter == w_counter
                && n_counter >  0
                && e_counter >  0
                && s_counter >  0
                && w_counter >  0
               )
          ) {
        
        // Incrémentation du compteur cardinal concerné
        switch ( tmp->m ) {
            case North:
                n_counter++;
                break;
            
            case East:
                e_counter++;
                break;
            
            case South:
                s_counter++;
                break;
            
            case West:
                w_counter++;
                break;
            
            default:
                break;
        }
        
        tmp = tmp->next;
    }
    
    // Si une prévision a été demandée, on a ajouté un mouvement hypothétique
    // dans le fil d'Ariane, on doit donc le retirer à la fin puisque ce 
    // mouvement n'est pas réel pour l'instant.
    if ( move != None ) {
        ariane_remove( thread );
    }
    
    // Retour - une boucle est détectée si, et seulement si, en remontant le fil
    // d'Ariane vers le plus ancien mouvement, on compte autant de direction N
    // que de S et autant de E que de W.
    return    n_counter == s_counter
           && e_counter == w_counter;
}



/* --- RETOUR AU POINT DE DÉPART - PROCÉDURE EMBUSCADE ------------------------

 DESCRIPTION :
    Fonction qui indique si l'enchaînement de la totalité des mouvements d'un
    fil d'Ariane donné ramène sur place.
    
    Exemple : soit le fil d'Ariane (W -> S -> E -> N). Alors étant donné 
    qu'à l'issu du parcours, on revient à sa position de départ (de manière
    évidente), la fonction renverra TRUE.
    
    Exemple : soit le fil d'Ariane (W -> S -> E -> N -> N). À l'issu du parcours
    de ce fil, on ne revient pas à sa position de départ, donc la fonction
    renvoie FALSE.
    
    Si le paramètre `nextMove` est indiqué est différent de None, alors, tout
    comme la fonction `ariane_looped`, la fonction effectue une prévision en
    ajoutant temporairement `nextMove` dans `thread`.

    À la différence de `ariane_looped`, on ne s'arrête pas dès la détection
    d'une boucle mais dès que tout le fil donné en paramètre a été parcouru,
    afin d'être certain de ne pas renvoyer TRUE par erreur si une boucle voisine
    existe mais ne concerne pas le mouvement actuel.

    Pour changer de `ariane_looped`, on utilise ici un système de coordonnées.
 
 
 PARAMÈTRES :
    thread (string) : fil d'Ariane que l'on va parcourir en entier ;
    nextMove (Move) : si spécifié autrement que `None`, la fonction tentera de 
                      prévoir si on revient à sa position d'origine en ajoutant
                      temporairement ce mouvement dans le fil.
 
 RETOUR :
             (bool) : booléen valant TRUE si on est retourné à sa position
                      d'origine, FALSE sinon.
 
 --------------------------------------------------------------------------- */

bool ariane_back_to_square_one(
    string const thread,
      Move const move
) {
    
    // Coordonnées
    int xPos = 0
      , yPos = 0;
    
    // Compteur de mouvement
    int c = 0;
    
    // Si on effectue une prévision de boucle, on ajoute temporaire le
    // mouvement hypothétique au fil d'Ariane
    if ( move != None ) {
        ariane_insert( thread, move );
    }
    
    // Parcoureur du fil d'Ariane
    string tmp = thread;
    
    // Parcours de la totalité du fil
    while ( tmp->next ) {
        
        // Incrémente le compteur
        c++;
        
        // Incrémentation du compteur cardinal concerné
        switch ( tmp->m ) {
            case North:
                yPos++;
                break;
            
            case East:
                xPos--;
                break;
            
            case South:
                yPos--;
                break;
            
            case West:
                xPos++;
                break;
            
            default:
                break;
        }
        
        // Prochain mouvement
        tmp = tmp->next;
    }
    
    // Si une prévision a été demandée, on a ajouté un mouvement hypothétique
    // dans le fil d'Ariane, on doit donc le retirer à la fin puisque ce 
    // mouvement n'est pas réel.
    if ( move != None ) {
        ariane_remove( thread );
    }
    
    // Retour
    return    xPos == 0
           && yPos == 0
           &&    c >  1; // Le fil d'Ariane doit contenir au moins 2 éléments
    
}





/******************************************************************************

    Ensemble de modules relatifs aux mouvements

 *****************************************************************************/

/* --- MOUVEMENT OPPOSÉ -------------------------------------------------------

 DESCRIPTION :
    Fonction qui renvoie le mouvement opposé à celui indiqué en paramètre.
 
 PARAMÈTRE :
    direction (Move) : le mouvement dont on veut obtenir l'opposé.
 
 RETOUR :
    oppDir (Move)    : mouvement opposé obtenu à partir du paramètre.
 
 --------------------------------------------------------------------------- */

Move move_opposite( Move const direction ) {
    Move oppDir;
    
    switch ( direction ) {
        case North:
            oppDir = South;
            break;
        
        case East:
            oppDir = West;
            break;
        
        case South:
            oppDir = North;
            break;
        
        case West:
            oppDir = East;
            break;
        
        default:
            oppDir = None;
            break;
    }
    
    return oppDir;
}



/* --- MOUVEMENT ANTI EMBUSCADE - PROCÉDURE AMBUSCADE -------------------------

 DESCRIPTION :
    Justifions la légitimité de cette procédure à l'appelation barbare.
    
    Admettons que Thésée arrive à l'intersection (X) depuis le couloir (0) et
    qu'il prenne le chemin (1). Grâce à la fonction `ariane_looped`, on peut
    éviter à Thésée de revenir vers (X) et de faire une boucle : il préférera
    faire demi-tour.
    
    Mais une fois revenu sur (X) en faisant demi-tour (donc via (1)), Thésée
    cherchera à emprunter le chemin (2) puisque par construction, l'arbre ne 
    présente pas d'enfant vers le Sud. Or, ce chemin a déjà été visité mais il
    est inconnu de l'arbre.
    
    Cette procédure permet donc, lorsque Thésée revient vers (X), d'emprunter
    le chemin (0), c'est-à-dire celui par lequel il est arrivé dans cette
    portion bouclée.
    
         (1)
          +------+
          |      |
   (0) --(X)     |
          |      |
          +------+
         (2)
    
    Cette procédure prend en paramètre le noeud position et le mouvement que
    Thésée s'apprête à prendre. Elle explore récursivement chaque enfant du 
    noeud position et vérifie que chaque feuille ne coïncidera pas
    géographiquement avec le prochain mouvement que Thésée s'apprête à faire.
    
    Si c'est le cas ("embuscade"), alors on modifiera le mouvement de sorte à
    ce qu'il quitte le noeud (direction opposée à `pos->m`).
    
    Sinon, on laisse le mouvement envisagé inchangé.
    
    
 PARAMÈTRE :
    tree (ExpTree)           : pointeur vers un noeud (au premier appel, doit
                               être paramétré sur `pos`) ;
    thread (string)          : pointeur vers un lien de fil d'Ariane hypothétique
                               (différent du vrai fil d'Ariane toutefois) ;
    move (Move)              : prochain mouvement que Thésée s'apprête à faire ;
    isNextMoveATrap (bool *) : booléen indiquant si finalement, il existe une
                               embuscade.

 --------------------------------------------------------------------------- */

void move_prevent_ambush(
    ExpTree   const tree,
    string    const thread,
    Move      const move,
    bool    * const isNextMoveATrap
) {
    
    // On est arrivé sur une feuille
    if (   !(tree->north)
        && !(tree->east )
        && !(tree->south)
        && !(tree->west )) {
        
        // En cas de détection d'embuscade, l'indiquer dans le booléen passé par
        // référence `isNextMoveATrap` (qui sera traité en dehors de cette
        // procédure)
        if ( ariane_back_to_square_one( thread, move_opposite( move ) ) ) {
            if ( debugMode ) {
                printf( "/!\\ PROCÉDURE EMBUSCADE ACTIVÉE -- chemin initialement envisagé : %d\n", move );
                printf( " EMBUSCADE - " );
                ariane_print( thread );
            }

            *isNextMoveATrap = true;
        } else {
            if ( debugMode ) {
                printf("           - ");
                ariane_print( thread );
            }
        }
    }
    
    // Itérer sur chaque enfant -- Pour chaque enfant, ajouter son mouvement
    // dans la fil puis le supprimer
    else {
        
        if ( tree->north ) {
            ariane_insert( thread, tree->north->m );
            move_prevent_ambush( tree->north, thread, move, isNextMoveATrap );
            ariane_remove( thread );
        }
        
        if ( tree->east  ) {
            ariane_insert( thread, tree->east->m );
            move_prevent_ambush( tree->east , thread, move, isNextMoveATrap );
            ariane_remove( thread );
        }
        
        if ( tree->south ) {
            ariane_insert( thread, tree->south->m );
            move_prevent_ambush( tree->south, thread, move, isNextMoveATrap );
            ariane_remove( thread );
        }
        
        if ( tree->west  ) {
            ariane_insert( thread, tree->west->m );
            move_prevent_ambush( tree->west , thread, move, isNextMoveATrap );
            ariane_remove( thread );
        }
        
    }
}



/******************************************************************************

    Fonction principale pour le choix du prochain mouvement

 *****************************************************************************/

Move theseus(
    ExpTree const map,      // current exploration tree
    ExpTree const pos,      // current position in the map exploration tree
       bool       north,    // can i go North?
       bool       east,     // can i go East?
       bool       south,    // can i go South?
       bool       west      // can i go West?
) {
    
    /* ------------------------------------------------------------------------
     * Initialisation et déclaration des variables utiles
     */
    
    // Booléens indiquant (i) si le chemin que la fonction s'apprête à renvoyer
    // ne mène pas vers un chemin déjà visité/une boucle cachée (voir la
    // procédure `move_prevent_loop`) ; (ii) si on a choisi de faire demi-tour
    // (voir la suite du programme)
    bool nextMoveIsATrap = false
       , hasTurnedAround = false;
    
    
    // Réservation de l'espace mémoire pour (i) le fil d'Ariane ; (ii) un fil
    // antiboucle (voir procédure `move_prevent_loop`)
    string const thread     = malloc( sizeof(string) )
               , loopKiller = malloc( sizeof(string) );
    
    
    // Prochain mouvement envisagé
    Move move;
    
    
    // Génération du fil d'Ariane
    ariane_init( thread, map, pos );
    
    
    // Initialisation du fil d'Ariane temporaire anti-bouclage
    loopKiller->m    = None; // Le `None` sera remplacé ultérieurement
    loopKiller->next = NULL;
    
    
    /* ------------------------------------------------------------------------
     * L'exploration de la totalité du donjon accessible nécessite que l'on 
     * parcourt tout l'arbre et ses enfants. On choisit donc comme direction,
     * dès que possible, le Nord, l'Est, le Sud ou (exclusif) l'Ouest.
     * 
     * Pour chaque direction cardinale, on vérifie que l'on peut y accéder (à
     * l'aide des paramètres booléens north, east, south et west) et qu'il ne
     * s'agit pas de la direction d'où l'on vient.
     *
     * Si Thésée arrive à un cul-de-sac (= aucune direction cardinale accessible
     * sauf celle d'où l'on vient), on lui fait faire demi-tour.
     *
     * Une fois la direction choisie, il faut encore vérifier que celle-ci ne
     * débouche pas vers une embuscade (boucle déjà entâmée depuis un autre
     * sens), on ferme donc la direction choisie si, après une vérification
     * ultérieure, on doit rappeler la fonction theseus.
     */
    
    if (        !(pos->north) && north && pos->m != South ) {
        
        move = North;
        move_prevent_ambush( pos, loopKiller, move, &nextMoveIsATrap );
        north = false; // on ferme l'accès au Nord
        
    } else if ( !(pos->east)  && east  && pos->m != West ) {
    
        move = East;
        move_prevent_ambush( pos, loopKiller, move, &nextMoveIsATrap );
        east = false; // on ferme l'accès à l'Est
    
    } else if ( !(pos->south) && south && pos->m != North ) {
    
        move = South;
        move_prevent_ambush( pos, loopKiller, move, &nextMoveIsATrap );
        south = false; // on ferme l'accès au Sud
    
    } else if ( !(pos->west)  && west  && pos->m != East ) {
    
        move = West;
        move_prevent_ambush( pos, loopKiller, move, &nextMoveIsATrap );
        west = false; // on ferme l'accès à l'Ouest
        
    } else {
        
        // Dans ce cas là, tous les enfants ont été explorés, donc on remonte
        // toujours vers le parent (demi-tour)
        move = move_opposite( pos->m );
        hasTurnedAround = true;
        
    }
    
    
    /* ------------------------------------------------------------------------
     * Si on n'a pas fait demi-tour, on peut alors déclencher les vérifications
     * contre les boucles et les embuscades
     */
    
    if ( !hasTurnedAround ) {
        
        // Si on détecte qu'on aura parcouru une boucle au prochain mouvement,
        // imposer à Thésée de faire demi-tour.
        if (   move != None
            && ariane_looped( thread, move )
           ) {
            
            if ( debugMode ) {
                printf( "\n/!\\ Le prochain mouvement (%d) ramène vers une position déjà\n", move );
                printf( "    visitée, demi-tour imposé (%d) !\n\n", move_opposite( pos->m ) );
            }
            
            move = move_opposite( pos->m );
        }
        
        // Si on détecte que le chemin que l'on s'apprête à prendre a déjà été
        // pris via un autre chemin (EMBUSCADE !), que l'on s'apprête en fait
        // à s'engager dans une boucle, alors il convient d'appeler de nouveau
        // la fonction actuelle mais en bloquant l'accès vers le chemin 
        // normalement choisi
        if ( nextMoveIsATrap ) {
            move = theseus( map, pos, north, east, south, west );
            
            if ( debugMode ) {
                printf( "Chemins possibles --\n Nord: %d\n  Est: %d\n  Sud: %d\nOuest: %d\n\n", north, east, south, west );
            }
        }
        
    }
    
    
    /* ------------------------------------------------------------------------
     * Libération des fils d'Ariane puis retour du prochain mouvement choisi.
     */
    
    free( loopKiller );
    free( thread );
    return move;
    
}





