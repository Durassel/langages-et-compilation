#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

typedef struct {
    int nbTerminaux; // Nombre de terminaux
    int nbNonTerminaux; // Nombre de non terminaux
    vector<string> terminaux; // Terminaux
    vector<string> nonTerminaux; // Non terminaux
    vector<vector<string> > regles; // R�gles de production de chaque �tat
    vector<vector<string> > premiers; // Premiers de chaque �tat
    vector<vector<string> > suivants; // Suivants de chaque �tat
    vector<vector<vector<string> > > table; // Table d'analyse
} data;

// Renvoie l'indice d'un caract�re dans une chaine
int indiceCaractere(string chaine, char caractere) {
    for (int i = 0; i < chaine.size(); i++) {
          if (caractere == chaine[i])
                return i;
    }
    return -1;
}

// Renvoie le nombre de r�gles d'un �tat
int longueurRegle(string chaine, char delimiteur, int debut) {
    int i = 0;
    char caractere = chaine.at(debut);

    while (caractere != delimiteur) {
        i++;
        if (debut + i <= chaine.size())
            caractere = chaine.at(debut + i);
        else
            return 0;
    }

    return i;
}

// D�coupe une chaine selon un caract�re
string hachage(string chaine, char delimiteur, int debut) {
    int i = 0;
    char caractere = chaine.at(debut);

    while (caractere != delimiteur) {
        i++;
        if (debut + i <= chaine.size())
            caractere = chaine.at(debut + i);
        else
            return 0;
    }

    return chaine.substr(debut, i);
}

// Affichage des terminaux, non terminaux et des r�gles de production de chaque �tat
void affichage(vector<string> terminaux, vector<string> nonTerminaux, vector<vector<string> > regles) {
    cout << endl << "Non terminaux : " << endl;
    for (int i = 0; i < nonTerminaux.size(); i++) {
        cout << nonTerminaux[i] << " ";
    }
    cout << endl << "Terminaux : " << endl;
    for (int i = 0; i < terminaux.size(); i++) {
        cout << terminaux[i] << " ";
    }
    cout << endl << "Regles de production : " << endl;
    for (int i = 0; i < regles.size(); i++) {
        for (int j = 0; j < regles[i].size(); j++) {
            //cout << regles[i][j];
            cout << " " << regles[i][j] << " ";
        }
        cout << endl;
    }
}

// Affichage des premiers ou des suivants de chaque �tat
void afficherPremiersSuivants(vector<string> nonTerminaux, vector<vector<string> > tab) {
    cout << endl;
    for (int i = 0; i < tab.size(); i++) {
        cout << nonTerminaux[i] << " => ";
        for (int j = 0; j < tab[i].size(); j++) {
            cout << tab[i][j] << " ";
        }
        cout << endl;
    }
}

// Suppression des | en fin de production
void supprSeparateur(vector<vector<string> >  &regles) {
    for (int i = 0; i < regles.size(); i++) {
        for (int j = 0; j < regles[i].size(); j++) {
            if (j == (regles[i].size() - 1) && regles[i][j] == "|")
                regles[i].pop_back();
        }
    }
}

// Elimine la r�cursivit� des �tats
void eliminationRecursiviteGauche(data &d, int ligne) {
    // Cr�ation des nouveaux �tats
    char caractere;
    vector<vector<string> > nouveauxEtats;
    nouveauxEtats.resize(2);

    // nouvel Etat : d�tecter les productions sans E au d�but
    bool nouveauxEtatsEgaux = false;
    bool regle = false;

    // S�lection de la lettre du nouvel �tat
    caractere = 'A';
    bool condition = false;
    do {
        int cpt = 0;

        for (int i = 0; i < d.regles.size(); i++) {
            if (caractere == d.regles[i][0].at(0)) {
                cpt++;
            }
        }

        if (cpt == 0) {
            condition = true;
        } else {
            caractere += 1;
        }
    } while (condition == false);


    string nouvelleEtat = "";
    nouvelleEtat += caractere;

    // Regroupement des lettres d'une r�gle en un string
    vector<string> tmp;
    tmp.push_back(d.regles[ligne][0]);
    tmp.push_back(d.regles[ligne][1]);
    int nbRegles = 1;
    for (int i = 0; i < d.regles[ligne].size(); i++) {
        if (d.regles[ligne][i].at(0) == '|')
            nbRegles++;
    }

    // Ajout des r�gles ne comprenant pas la lettre de l'�tat
    int cpt = 2;
    for (int i = 0; i < nbRegles; i++) {
        if (i == nbRegles - 1) {
            string ensemble = "";
            for (int i = cpt; i < d.regles[ligne].size(); i++) {
                ensemble += d.regles[ligne][cpt].at(0);
            }
            tmp.push_back(ensemble);
        } else {
            string ensemble = "";
            caractere = d.regles[ligne][cpt].at(0);
            while (caractere != '|') {
                ensemble += d.regles[ligne][cpt].at(0);
                cpt++;
                caractere = d.regles[ligne][cpt].at(0);
            }
            cpt++;
            tmp.push_back(ensemble);
            tmp.push_back("|");
        }
    }

    // Ajout des r�gles dans nouveauxEtats
    for (int j = 2; j < tmp.size(); j++) {
        if (tmp[0].at(0) != tmp[j].at(0) && tmp[j].at(0) != '|') {
            regle = true;
        }

        if (regle == true) {
            if (nouveauxEtats[0].size() == 0 && tmp[j].at(0) == '#') {
                nouveauxEtatsEgaux = true;
            } else if (tmp[j].at(0) != '#') {
                nouveauxEtats[0].push_back(tmp[j] + nouvelleEtat);
                nouveauxEtats[0].push_back("|");
            } else {
                nouveauxEtats[0].push_back("#");
            }
        }
        regle = false;
    }

    // Nouvel Etat sans r�cursivit� : d�tetcter les productions avec E
    for (int j = 1; j < tmp.size(); j++) {
        if (tmp[0].at(0) == tmp[j].at(0)) {
            string tmp2;
            for (int k = 1; k < tmp[j].size(); k++) {
                tmp2 += tmp[j].at(k);
            }
            tmp2 += nouvelleEtat;
            nouveauxEtats[1].push_back(tmp2);
            nouveauxEtats[1].push_back("|");
        }
    }
    nouveauxEtats[1].push_back("#");

    // Modification de l'�tat dans le tableau r�gles
    string sauvegardeEtat = tmp[0];
    string sauvegardeEgal = tmp[1];
    while (!d.regles[ligne].empty()) {
        d.regles[ligne].pop_back();
    }
    d.regles[ligne].push_back(sauvegardeEtat);
    d.regles[ligne].push_back(sauvegardeEgal);

    if (!nouveauxEtatsEgaux) {
        for (int i = 0; i < nouveauxEtats[0].size(); i++) {
            for (int j = 0; j < nouveauxEtats[0][i].size(); j++) {
                string tmp2 = "";
                tmp2 += nouveauxEtats[0][i].at(j);
                d.regles[ligne].push_back(tmp2);
            }

        }
    } else {
        for (int i = 0; i < nouveauxEtats[1].size(); i++) {
            for (int j = 0; j < nouveauxEtats[1][i].size(); j++) {
                string tmp2 = "";
                tmp2 += nouveauxEtats[1][i].at(j);
                d.regles[ligne].push_back(tmp2);
            }
        }
    }

    // Ajout du nouvel �tat dans le tableau r�gles
    for (unsigned int i = 0; i < d.regles.size(); i++) {
        if (d.regles[i][0].at(0) == sauvegardeEtat.at(0)) {
            vector<string> tmp2;
            tmp2.push_back(nouvelleEtat);
            tmp2.push_back("=");
            for (int j = 0; j < nouveauxEtats[1].size(); j++) {
                for (int k = 0; k < nouveauxEtats[1][j].size(); k++) {
                    string tmp3 = "";
                    tmp3 += nouveauxEtats[1][j].at(k);
                    tmp2.push_back(tmp3);
                }
            }
            d.regles.insert(d.regles.begin() + i + 1, tmp2);
            d.nonTerminaux.insert(d.nonTerminaux.begin() + i + 1, nouvelleEtat);
            break;
        }
    }

    // Incr�mentation du nombre de non terminaux
    d.nbNonTerminaux++;
}

// Test de la r�cursivit� gauche d'un �tat
void verifDerivationGauche(data &d) {
    bool condition = false;

    for (int i = 0; i < d.regles.size(); i++) {
        // Production avec E en premier
        for (int j = 2; j < d.regles[i].size(); j++) {
            if (d.regles[i][0].at(0) == d.regles[i][j].at(0)) {
                condition = true;
            }

            string caractere = "";
            caractere += d.regles[i][j].at(0);
            while (caractere != "|") {
                if (j < d.regles[i].size() - 1) {
                    j++;
                    caractere = d.regles[i][j].at(0);
                } else {
                    caractere = "|";
                }
            }
        }

        if (condition == true) {
            eliminationRecursiviteGauche(d, i);
        }
        condition = false;
    }
}

// Calcul des premiers d'un �tat
void premier(vector<vector<string> > regles, vector<string> &premiers, int ligne) {
    // Parcours des caract�res des r�gles de production de l'�tat ligne
    for (int i = 2; i < regles[ligne].size(); i++) {
        // Si c'est une lettre ou un |
        if ((i == 2 || regles[ligne][i - 1] == "|") && regles[ligne][i].at(0) >= 'A' && regles[ligne][i].at(0) <= 'Z') {
            int j = 0;
            // Parcours des autres �tats
            while (j < regles.size() && regles[ligne][i].at(0) != regles[j][0].at(0)) {
                j++;
            }

            // Trouv� dans le tableau d'�tats
            if (j != regles.size()) {
                premier(regles, premiers, j); // On calcul � nouveau le premier, mais pour la ligne j
            } else {
                cout << "Erreur : cas inconnu";
                return ;
            }
        } else if (regles[ligne][i] != "|" && (i == 2 || regles[ligne][i - 1] == "|") && regles[ligne][i] != "#") {
            // Sinon, on ajoute le premier
            premiers.push_back(regles[ligne][i]);
        }
    }
}

// Calcul des suivants
void suivant(vector <vector <string> > regles, vector< vector <string> > premiers, vector< vector <string> > &suivants) {
    // Boucle permettant de trouver un terminal derri�re le non terminal
    for (int ligne = 0; ligne < regles.size(); ligne++) { // Parcours de chaque r�gle
        for (int i = 0; i < regles.size(); i++) { // Deuxi�me parcours pour effectuer la comparaison
            if (ligne != i) { // Si les �tats sont diff�rents
                for (int j = 2; j < regles[i].size(); j++) { // Parcours des r�gles de l'�tat
                    if (regles[ligne][0] == regles[i][j]) { // Si non terminal � gauche pr�sent � droite dans un autre �tat
                        if (j + 1 != regles[i].size()) { // Et si l'�tat n'est ni le premier ni le dernier
                            if ((regles[i][j + 1].at(0) < 'A' || regles[i][j + 1].at(0) > 'Z') && regles[i][j + 1] != "#" && regles[i][j + 1] != "|" ) { // Si l'�tat est terminal
                                suivants[ligne].push_back(regles[i][j + 1]); // Ajout du suivant que l'on vient de trouver
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < regles.size(); i++) { // Parcours des �tats
        for (int j = regles[i].size() - 1; j > 1; j--) { // Parcours des prochains �tats
            for (int ligne = 0; ligne < regles.size(); ligne++) { // Parcours de la premi�re valeur
                if (ligne != i) { // Si les �tats sont diff�rents
                    if (regles[ligne][0] == regles[i][j]) { // Si non terminal � gauche pr�sent � droite dans un autre �tat
                        if (j + 1 != (regles[i].size())) {// Et si l'�tat n'est pas le dernier
                            if ((regles[i][j + 1].at(0) >= 'A' && regles[i][j + 1].at(0) <= 'Z')) { // Etat non terminal
                                int cpt = j + 1;
                                while (cpt < regles[i].size() && regles[i][cpt] != "|" && (regles[i][cpt].at(0) < 'A' || regles[i][cpt].at(0) > 'Z')) { // Alors recherche du non terminal
                                    cpt++;
                                }

                                if (regles[i][cpt].at(0) >= 'A' && regles[i][cpt].at(0) <= 'Z') { // Etat non terminal
                                    int k = 0;
                                    while (k < regles.size() && regles[i][cpt].at(0) != regles[k][0].at(0)) { // Recherche de l'index de l'�tat dans r�gles
                                        k++;
                                    }

                                    if(k != regles.size()) {
                                        int l = 0;
                                        while (l < premiers[k].size()) {
                                            if (premiers[k][l] != "#") { // Cours : si pr�sence de # dans l'�tat X, alors suivant Y = premiers X
                                                suivants[ligne].push_back(premiers[k][l]);
                                            } else {
                                                for (int m = 0; m < suivants[k].size(); m++) { // Suivant Y = suivant X
                                                    suivants[ligne].push_back(suivants[k][m]);
                                                }
                                            }
                                            l++;
                                        }
                                    } else {
                                        cout << "Erreur : cas inconnu";
                                        return ;
                                    }
                                }
                            }
                        } else if ((j + 1 == (regles[i].size()) || regles[i][j + 1] == "|") && (j != 2 && regles[i][j - 1] != "|")) { // Si != premier
                            for (int k = 0; k < suivants[i].size(); k++) {
                                suivants[ligne].push_back(suivants[i][k]);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Recherche de la r�gle correspondant � l'�tat terminal et non terminal
int rechercheRegle(vector<vector<string> > regles, int ligne, string terminaux) {
    int debutRegle = 2;
    int cpt = 0;
    for (int i = 2; i < regles[ligne].size(); i++) { // Observation des �tats � droite
        if (regles[ligne][i - 1] == "|") { // Si r�gle diff�rente
            debutRegle = i; // Sauvegarde de l'indice de la r�gle
            cpt = 0;
        }

        if (cpt == 0 && (regles[ligne][i].at(0) >= 'A' && regles[ligne][i].at(0) <= 'Z')) { // Si �tat non terminal
            int j = 0;
            while (j < regles.size() && regles[j][0] != regles[ligne][i]) { // Recherche de l'indice de l'�tat
                j++;
            }

            int indice = rechercheRegle(regles, j, terminaux); // Nouvel appel de la recherche pour trouver le bon �tat
            if (indice != 0) {
                return debutRegle; // Retourne l'indice trouv� de l'�tat
            } else {
                int k = 2;
                while (k < regles[j].size() && regles[j][k] != "#") {
                    k++;
                }

                if (k == regles[j].size()) {
                    cpt = 1;
                }
            }
        } else if (cpt == 0 && regles[ligne][i] == terminaux) { // Si d�tection d'un terminal en tout premier, retourne l'indice de la r�gle
            return debutRegle;
        } else if (regles[ligne][i] != "|") {
            cpt = 1;
        }
    }

    return 0;
}

// Remplissage de la table d'analyse
void creationTable(vector<vector<string> > regles, int ligne, vector<string> premiers, vector<string> suivants, vector<string> terminaux, vector<vector<string> > &table) {
    int cpt = 0;
    for (int i = 0; i < terminaux.size(); i++) { // Parcours des terminaux pour les comparer aux premiers
        for (int j = 0; j < premiers.size(); j++) { // Parcours des premiers
            if (terminaux[i] == premiers[j]) { // Si �tats identiques
                int indice = rechercheRegle(regles, ligne, terminaux[i]); // D�termination de la r�gle de production
                if (indice != 0) { // Si existance de la r�gle
                    while (indice < regles[ligne].size() && regles[ligne][indice] != "|") {
                        table[i].push_back(regles[ligne][indice]); // Ajout dans la table
                        indice++;
                    }
                }
            }
            if (cpt == 0 && premiers[j] == "#") { // Si pr�sence du mot vide, parcours des suivants
                cpt = 1;
            }
        }
    }

    if (cpt == 1) {
        for (int i = 0; i < terminaux.size(); i++) { // Parcours des terminaux � comparer aux suivants
            for (int j = 0; j < suivants.size(); j++) { // Parcours des suivants
                if (terminaux[i] == suivants[j]) { // Ajout du mot vide aux suivants de l'�tat
                    table[i].push_back("#");
                }
            }
        }
    }
}

// Affichage de la table d'analyse
void afficherTable(vector<vector<vector<string> > > table, vector<string> terminaux, vector<vector<string> >regles) {
    // Affichage des terminaux
    cout << "  |";
    for (int i = 0; i < terminaux.size(); i++) {
        cout << " " << terminaux[i] << " |";
    }
    cout << endl;

    for (int i = 0; i < table.size(); i++) {
        // Affichage des non terminaux en d�but de ligne
        cout << regles[i][0] << " |";
        // Affichage des cases du tableau
        for (int j = 0; j < table[i].size(); j++) {
            for (int k = 0; k < 3; k++) {
                if (k < table[i][j].size()) {
                    cout << table[i][j][k];
                } else {
                    cout << " ";
                }
            }
            cout << "|";
        }
        cout << endl;
    }
    cout << endl;
}

// Permet de trouver si une chaine est reconnue par la grammaire
void analyseChaine(vector<vector<vector<string> > > table, vector<vector<string> > regles, vector<string> terminaux, string chaine) {
    vector<string> pile;

    // Initialisation
    pile.push_back("$");
    chaine += '$'; // Symbole de fin en bout de chaine
    pile.push_back(regles[0][0]);// Ajout de l'axiome (premier �tat non terminal)

    cout << "Pile          Entree      Sortie" << endl;
    cout << "               " << chaine << endl;

    // Tant que la pile n'est pas vide, on continue le traitement
    do {
        int i = 0;
        while (i < regles.size() && pile[pile.size() - 1].at(0) != regles[i][0].at(0)) { // Recherche des �tats intervenant dans la pile
            i++;
        }

        int j = 0;
        while (j < terminaux.size() && chaine.at(0) != terminaux[j].at(0)) { // Recherche de l'�tat terminal de la chaine
            j++;
        }

        // Affichage de l'analyse
        for (int k = 0; k < 15; k++) {
            if (k < pile.size()) {
                cout << pile[k];
            } else {
                cout << " ";
            }
        }

        for (int k = 0; k < 15; k++) {
            if (k < chaine.size()) {
                cout << chaine.at(k);
            } else {
                cout << " ";
            }
        }

        if (j != terminaux.size()) { // Si l'�tat fait parti de la grammaire
            if (pile[pile.size() - 1].at(0) == chaine.at(0)) { // Suppression des �tats terminaux pr�sents dans la chaine et la pile
                cout << endl;
                pile.pop_back();
                chaine.erase(0, 1); // Suppression de la lettre de l'�tat
            } else if (pile[pile.size() - 1].at(0) < 'A' || pile[pile.size() - 1].at(0) > 'Z') { // Sinon si l'�tat terminal de la pile n'est pas pr�sent dans la chaine
                cout << endl << "Erreur : la chaine saisie en entree n'est pas complete. Voulez-vous la compl�ter avec " << pile[pile.size() -1 ].at(0) << " ? (Oui : 1 / Non : 0) ";

                string ajout;
                cin >> ajout;
                if (ajout == "1") {
                    chaine.insert(0, pile[pile.size()-1]); // Ajout du reste dans la chaine
                } else { // Fin du programme
                    return;
                }
            } else {
                if (table[i][j].empty()) { // Si l'�tat non terminal est introuvable depuis l'�tat actuel
                    cout << endl << "Erreur : votre chaine est incorrecte. L'etat terminal " << terminaux[j] << " ne peut pas etre lu depuis " << pile.at(pile.size() - 1) << endl;
                    cout << "Souhaitez-vous modifier la chaine saisie en entree ? (Oui : 1 / Non : 0) ";

                    string ajout;
                    cin >> ajout;
                    if (ajout == "1") {
                        cout << "Choisissez un caractere pouvant etre lu depuis " << pile.at(pile.size() -1 ) << endl;
                        cout << "Veuillez saisir la chaine a ajouter : ";
                        cin >> ajout;
                        chaine.insert(0, ajout);
                    } else { // Fin du programme
                        return;
                    }
                } else { // Affichage
                    cout << regles[i][0] << "->";
                    for (int k = 0; k < table[i][j].size(); k++) {
                        cout << table[i][j][k];
                    }
                    cout << endl;

                    pile.pop_back();
                    for (int k = table[i][j].size() - 1; k >= 0; k--) {
                        if (table[i][j][k] != "#") {
                            pile.push_back(table[i][j][k]);
                        }
                    }
                }
            }
        } else { // Si l'�tat n'est pas dans la grammaire
            cout << endl << "Erreur : le caractere " << chaine.at(0) << " est inconnu" << endl;
            cout << "Souhaitez-vous modifier ce caractere ? (Oui : 1 / Non : 0) ";

            string ajout;
            cin >> ajout;
            if (ajout == "1") {
                chaine.erase(0, 1);
                cout << "Veuillez saisir le caractere a ajouter : ";
                cin >> ajout;
                chaine.insert(0, ajout);
            } else { // Fin du programme
                return;
            }
        }
    } while (!pile.empty());

    if (pile.empty()) {
        cout << "La chaine a ete reconnue !" << endl;
    }
}

// Suppression des caract�res en double
void suppressionDoublons(vector<string> &tab) {
    for (int i = 0; i < tab.size(); i++) {
        for (int j = tab.size() - 1; j > i; j--) {
            if (tab[i] == tab[j]) {
                tab.erase(tab.begin() + j);
            }
        }
    }
}

int main(void) {
    // D�claration d'une structure de donn�es
    data d;

    // Lecture du fichier
    cout << "----------------------------------------------------------" << endl;
    cout << "------------------- Lecture du fichier -------------------" << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "Veuillez saisir le nom du fichier a ouvrir (sans extension, sans groupe) : ";
    string nom;
    cin >> nom;
    nom = "G1-" + nom + ".txt";
    ifstream F(nom.c_str());
    if (F.fail()) {
		cout << "Erreur lors de l'ouverture du fichier." << endl;
		return(0);
	}

    // Non termianux
    string chaine;
    getline(F, chaine);
    d.nbNonTerminaux = chaine.size();
    for (int i = 0; i < chaine.size(); i++) {
        string caractere = chaine.substr(i,1);
        d.nonTerminaux.push_back(caractere);
    }

    // Terminaux
    getline(F, chaine);
    d.nbTerminaux = chaine.size();
    for (int i = 0; i < chaine.size(); i++) {
        string caractere = chaine.substr(i,1);
        d.terminaux.push_back(caractere);
    }

    // Lecture des r�gles
    d.regles.resize(d.nbNonTerminaux);
    unsigned int indice = 0;
    while(!F.eof()) {
        getline(F, chaine);

        // Lecture du premier caract�re
        string caractere = chaine.substr(0,1);
        d.regles[indice].push_back(caractere);
        caractere = chaine.substr(1,1); // =
        d.regles[indice].push_back(caractere);

        // Compter le nombre de productions par ligne
        int nbRegles = 1;
        for (int i = 0; i < chaine.size(); i++) {
            if (chaine[i] == '|')
                nbRegles++;
        }

        // D�coupage de chaque r�gle
        int debut = indiceCaractere(chaine, '=') + 1;
        for (int i = 0; i < nbRegles; i++) {
            // Derni�re r�gle de production de la ligne
            if (i == nbRegles - 1) {
                string tmp = chaine.substr(debut);
                for (unsigned int j = 0; j < tmp.size(); j++) {
                    caractere = "";
                    caractere += tmp[j];
                    d.regles[indice].push_back(caractere);
                }
            } else {
                string tmp = hachage(chaine, '|', debut);
                for (unsigned int j = 0; j < tmp.size(); j++) {
                    caractere = "";
                    caractere += tmp[j];
                    d.regles[indice].push_back(caractere);
                }
                d.regles[indice].push_back("|");
                debut += longueurRegle(chaine, '|', debut) + 1;
            }
        }
        indice++;
    }

    // Affichage des donn�es de la lecture
    affichage(d.terminaux, d.nonTerminaux, d.regles);

    cout << endl << endl << "----------------------------------------------------------" << endl;
    cout << "--------- Elimination de la recursivite a gauche ---------" << endl;
    cout << "----------------------------------------------------------" << endl;
    // V�rification de la r�cursivit� � gauche
    verifDerivationGauche(d);
    // Suppression des | en fin de chaine
    supprSeparateur(d.regles);

    // Affichage du tableau apr�s �limination
    affichage(d.terminaux, d.nonTerminaux, d.regles);

    // Calcul des premiers de chaque �tat
    cout << endl << endl << "----------------------------------------------------------" << endl;
    cout << "------------------ Calcul des premiers -------------------" << endl;
    cout << "----------------------------------------------------------" << endl;

    d.premiers.resize(d.regles.size());
    d.suivants.resize(d.regles.size());

    // Calcul des premiers de chaque �tat
    for (int i = 0; i < d.premiers.size(); i++) {
        premier(d.regles, d.premiers[i], i); // Remplissage du tableau de premiers

        int j = 2;
        while (j < d.regles[i].size() && d.regles[i][j] != "#") {
            j++;
        }

        if (j != d.regles[i].size()) {
            d.premiers[i].push_back("#"); // Ajout du mot vide
        }
    }
    // Autre r�gle des premiers : si X => ##Y, premier Y = premier # (de chacun des �tats #)
    for (int i = 0; i < d.premiers.size(); i++) { // Parcours des premiers
        int cpt = 0;

        for (int j = 2; j < d.regles[i].size(); j++) { // Parcours des lettres de l'�tat
            if (d.regles[i][j] == "|") {
                cpt = 0;
            } else if (cpt == 0 && j != 2 && d.regles[i][j].at(0) >= 'A' && d.regles[i][j].at(0) <= 'Z') { // Si �tat non terminal
                int k = 0;
                while (d.regles[i][j] != d.regles[k][0]) { // Recherche du premier
                    k++;
                }

                int l = 0;
                while ( l < d.premiers[k].size() && d.premiers[k][l] != "#") { // Si pr�sence de l'�tat vide
                    l++;
                }

                if (l != d.premiers[k].size()) {
                    k = 0;
                    while (d.regles[i][j] != d.regles[k][0]) { // Recherche de l'�tat
                        k++;
                    }

                    for (int m = 0; m < d.premiers[k].size(); m++) {
                        int n = 0;
                        while (n < d.premiers[i].size() && d.premiers[i][n] != d.premiers[k][m]) {
                            n++;
                        }

                        if (n == d.premiers[i].size() && d.premiers[k][m] != "#") { // D�tection du premier
                             d.premiers[i].push_back(d.premiers[k][m]); // Ajout
                        }
                    }
                } else {
                    cpt = 1;
                }
            } else if (d.regles[i][j].at(0) < 'A' || d.regles[i][j].at(0) > 'Z') {
                cpt = 1;
            }
        }
    }
    afficherPremiersSuivants(d.nonTerminaux, d.premiers);

    cout << endl << endl << "----------------------------------------------------------" << endl;
    cout << "------------------ Calcul des suivants -------------------" << endl;
    cout << "----------------------------------------------------------" << endl;
    d.suivants[0].push_back("$"); // R�gle du cours : ajout $ dans l'axiome (premier �tat non terminal)
    // Protophrase
    for (int i = 1; i < d.regles.size(); i++) {
        if (d.regles[i][d.regles[i].size() - 1] == "#") { // Ajout $ pour la fin d'une protophrase
            d.suivants[i].push_back("$");
        }
    }
    suivant(d.regles, d.premiers, d.suivants); // Remplissage du tableau de suivants

    for (int i = 0; i < d.premiers.size();i++) {
        suppressionDoublons(d.suivants[i]);
    }
    afficherPremiersSuivants(d.nonTerminaux, d.suivants);

    // Ajout du $ dans le tableau des terminaux
    d.terminaux.push_back("$");

    cout << endl << endl << "----------------------------------------------------------" << endl;
    cout << "-------------------- Table d'analyse ---------------------" << endl;
    cout << "----------------------------------------------------------" << endl;
    // Redimensionnement du tableau
    d.table.resize(d.regles.size());
    for (int i = 0; i < d.table.size(); i++) {
        d.table[i].resize(d.terminaux.size());
    }
    // Remplissage de la tble d'analyse
    for (int i = 0; i < d.table.size(); i++) {
        creationTable(d.regles, i, d.premiers[i], d.suivants[i], d.terminaux, d.table[i]);
    }
    afficherTable(d.table, d.terminaux, d.regles);

    cout << endl << endl << "----------------------------------------------------------" << endl;
    cout << "------------------ analyse d'une chaine ------------------" << endl;
    cout << "----------------------------------------------------------" << endl;
    chaine = "";
    cout << "Entrez une expression a analyser : " << endl;
    cin >> chaine;
    analyseChaine(d.table, d.regles, d.terminaux, chaine);

    return 0;
}
