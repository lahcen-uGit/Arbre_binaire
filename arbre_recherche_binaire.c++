#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
using namespace std;

class Noeud
{
public:
  int val;
  Noeud *fg;
  Noeud *fd;
  Noeud(int v) : val(v), fg(nullptr), fd(nullptr) {}
};

class ArbreBinaireRecherche
{

private:
  Noeud *racine;

  // Insertion recursive
  Noeud *inserer(Noeud *node, int val)
  {
    if (!node)
      return new Noeud(val);

    if (node->val < val)
      node->fd = inserer(node->fd, val);
    else
      node->fg = inserer(node->fg, val);

    return node;
  }

  // Suppression iterative : renvoie la nouvelle racine
  Noeud *supprimer(Noeud *node, int val, bool &trouve)
  {
    trouve = false;

    // 1) Chercher le noeud a supprimer en gardant son parent
    Noeud *parent = nullptr;
    Noeud *courant = node;
    while (courant && courant->val != val)
    {
      parent = courant;
      courant = (courant->val > val) ? courant->fg : courant->fd;
    }
    if (!courant)
      return node; // valeur absente : rien a faire
    trouve = true;

    // 2Deux enfants : on copie le max du sous arbre gauche dans le noeud
    // puis on supprime ce max (qui a au plus un fils gauche)
    if (courant->fg && courant->fd)
    {
      Noeud *parentMax = courant;
      Noeud *maxNode = courant->fg;
      while (maxNode->fd)
      {
        parentMax = maxNode;
        maxNode = maxNode->fd;
      }
      courant->val = maxNode->val;
      parent = parentMax;
      courant = maxNode;
    }

    // 3) Le noeud a retirer a maintenant au plus un enfant
    Noeud *enfant = courant->fg ? courant->fg : courant->fd;

    if (!parent)
      node = enfant;
    else if (parent->fg == courant)
      parent->fg = enfant;
    else
      parent->fd = enfant;

    delete courant;
    return node;
  }

  // Hauteur recursive
  int hauteur(Noeud *node) const
  {
    if (!node)
      return 0;
    return 1 + max(hauteur(node->fg), hauteur(node->fd));
  }

  // Recherche iterative
  bool rechercher(Noeud *node, int val) const
  {
    while (node)
    {
      if (node->val == val)
        return true;
      if (node->val > val)
        node = node->fg;
      else
        node = node->fd;
    }
    return false;
  }


  void vider(Noeud *node)
  {
    if (node)
    {
      vider(node->fg);
      vider(node->fd);
      delete node;
    }
  }

public:
  ArbreBinaireRecherche() : racine(nullptr) {}

  ~ArbreBinaireRecherche() { vider(racine); }

  void inserer(int val) { racine = inserer(racine, val); }

  // Supprime une occurrence de val. Renvoie true si elle existait.
  bool supprimer(int val)
  {
    bool trouve;
    racine = supprimer(racine, val, trouve);
    return trouve;
  }

  // Insere 0, 1, 2, ..., nombre-1 DANS L ORDRE
  void insererOrdonne(int nombre)
  {
    for (int i = 0; i < nombre; i++)
      inserer(i);
  }

  // Insere 'nombre' valeurs tirees au hasard entre 0 et nombre-1

  void insererAleatoire(int nombre)
  {
    mt19937 gen(time(0));

    for (int i = 0; i < nombre; i++)
      inserer(gen() % nombre);
  }

  int hauteur() const { return hauteur(racine); }

  bool rechercher(int val) const { return rechercher(racine, val); }
};

// Mesure le temps (en ms) 
double Recherche_repeter(const ArbreBinaireRecherche &arbre, const vector<int> &valeurs, int &trouves)
{
  trouves = 0;
  auto debut = chrono::steady_clock::now();
  for (int v : valeurs)
    if (arbre.rechercher(v))
      trouves++;
  auto fin = chrono::steady_clock::now();
  return chrono::duration<double, milli>(fin - debut).count();
}

// Mesure le temps (en ms) de plusieurs suppressions ; 'supprimes' = nombre reellement supprime
double Suppression_repeter(ArbreBinaireRecherche &arbre, const vector<int> &valeurs, int &supprimes)
{
  supprimes = 0;
  auto debut = chrono::steady_clock::now();
  for (int v : valeurs)
    if (arbre.supprimer(v))
      supprimes++;
  auto fin = chrono::steady_clock::now();
  return chrono::duration<double, milli>(fin - debut).count();
}

int main()
{
  ArbreBinaireRecherche arbreOrdonne, arbreAleatoire;
  int n;

  cout << "Nombre d'elements a inserer  : ";
  cin >> n;

  // Insertion dans l'ordre : 0, 1, 2, ..., n-1
  auto debut = chrono::steady_clock::now();
  arbreOrdonne.insererOrdonne(n);
  auto fin = chrono::steady_clock::now();
  double tempsInsertionOrdonne = chrono::duration<double, milli>(fin - debut).count();

  // Insertion de valeurs tirees au hasard
  debut = chrono::steady_clock::now();
  arbreAleatoire.insererAleatoire(n);
  fin = chrono::steady_clock::now();
  double tempsInsertionAleatoire = chrono::duration<double, milli>(fin - debut).count();

  
  // 1000 valeurs reparties entre 0 et n-1 (les memes pour les deux arbres)
  vector<int> valeurs(1000);
  for (int i = 0; i < 1000; i++)
    valeurs[i] = i * n / 1000;

  // Recherche
  int nbTrouvesOrdonne, nbTrouvesAleatoire;
  double tempsRechercheOrdonne = Recherche_repeter(arbreOrdonne, valeurs, nbTrouvesOrdonne);
  double tempsRechercheAleatoire = Recherche_repeter(arbreAleatoire, valeurs, nbTrouvesAleatoire);

  //  Suppression
  int hauteurAvantOrdonne = arbreOrdonne.hauteur();
  int hauteurAvantAleatoire = arbreAleatoire.hauteur();

  int nbSupprimesOrdonne, nbSupprimesAleatoire;
  double tempsSuppressionOrdonne = Suppression_repeter(arbreOrdonne, valeurs, nbSupprimesOrdonne);
  double tempsSuppressionAleatoire = Suppression_repeter(arbreAleatoire, valeurs, nbSupprimesAleatoire);

  int hauteurApresOrdonne = arbreOrdonne.hauteur();
  int hauteurApresAleatoire = arbreAleatoire.hauteur();

  
  


  cout << "\n Comparaison pour " << n << " elements\n";

  cout << "\nInsertion ORDONNEE (0, 1, 2, ...,n-1)\n";
  cout << "  Temps d'insertion   : " << tempsInsertionOrdonne << " ms\n";
  cout << "  Hauteur avant    : " << hauteurAvantOrdonne << "\n";
  cout << "  1000 recherches   : " << tempsRechercheOrdonne << " ms ("
       << nbTrouvesOrdonne << " trouvees)\n";
  cout << "  1000 suppressions   : " << tempsSuppressionOrdonne << " ms ("
       << nbSupprimesOrdonne << " supprimees)\n";
  cout << "  Hauteur apres       : " << hauteurApresOrdonne << "\n";

  cout << "\nInsertion ALEATOIRE (valeurs tirees au hasard)\n";
  cout << "  Temps d'insertion   : " << tempsInsertionAleatoire << " ms\n";
  cout << "  Hauteur avant       : " << hauteurAvantAleatoire << "\n";
  cout << "  1000 recherches     : " << tempsRechercheAleatoire << " ms ("
       << nbTrouvesAleatoire << " trouvees)\n";
  cout << "  1000 suppressions   : " << tempsSuppressionAleatoire << " ms ("
       << nbSupprimesAleatoire << " supprimees)\n";
  cout << "  Hauteur apres       : " << hauteurApresAleatoire << "\n";

  return 0;
}