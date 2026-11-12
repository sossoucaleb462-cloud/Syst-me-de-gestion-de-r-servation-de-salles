# Exercice 4 : Gestion d'un système de réservation de salles

## Contexte
Un département universitaire dispose de plusieurs salles destinées aux cours, aux travaux dirigés et aux soutenances.
* Chaque salle possède un **numéro**, une **capacité** et une **catégorie**.
* Les enseignants peuvent effectuer des réservations pour des **dates** et des **créneaux horaires** précis.
* **Contrainte métier :** Une salle ne peut pas être réservée par deux enseignants pour le même créneau.

---

## 1. Identification des classes nécessaires

Pour répondre aux exigences d'encapsulation, de collections d'objets et de gestion des contraintes métier, l'architecture s'appuie sur **4 classes** et **1 énumération** :

* **`CategorieSalle` (Énumération)** : Définit les types de salles disponibles (`COURS`, `TD`, `SOUTENANCE`).
* **`Salle`** : Modélise une salle physique du département.
  * *Attributs privés* : `numero` (`string`), `capacite` (`int`), `categorie` (`CategorieSalle`).
* **`Enseignant`** : Modélise l'utilisateur qui effectue les réservations.
  * *Attributs privés* : `id` (`int`), `nom` (`string`), `prenom` (`string`).
* **`Reservation`** : Classe d'association matérialisant la réservation d'une salle par un enseignant.
  * *Attributs privés* : `idReservation` (`int`), `date` (`string`), `heureDebut` (`int`), `heureFin` (`int`), `enseignant` (`shared_ptr<Enseignant>`), `salle` (`shared_ptr<Salle>`).
* **`GestionReservations`** : Classe contrôleur qui centralise les collections d'objets (`std::vector`) et garantit le respect des règles métier.

---

## 2. Diagramme de cas d'utilisation

**Acteur principal :** Enseignant.

![Diagramme de cas d'utilisation](images\utilisation.png)

> *Note : Placez l'image de votre diagramme de cas d'utilisation dans le dossier `images/` sous le nom `diagramme_cas_utilisation.png` (ou mettez à jour le chemin ci-dessus).*

---

## 3. Diagramme de classes UML

**Associations et multiplicités :**
* Un `Enseignant` effectue `0..*` `Reservation`. Une `Reservation` appartient à `1` seul `Enseignant`.
* Une `Salle` est concernée par `0..*` `Reservation`. Une `Reservation` porte sur `1` seule `Salle`.
* `GestionReservations` agrège les collections de salles, d'enseignants et de réservations (`0..*`).

![Diagramme de classes UML](images\classes.png)

> *Note : Placez l'image de votre diagramme de classes dans le dossier `images/` sous le nom `diagramme_classes.png`.*

---

## 4. Diagramme de séquence (Scénario de réservation)

Ce diagramme illustre le processus de réservation d'une salle, incluant la vérification préalable de disponibilité afin d'empêcher tout conflit horaire.

![Diagramme de séquence](images\séquence.png)

> *Note : Placez l'image de votre diagramme de séquence dans le dossier `images/` sous le nom `diagramme_sequence.png`.*

---

## 5. Implémentation complète en C++

Ce code met en œuvre l'encapsulation (`private`/`public`), les collections d'objets (`std::vector`), la gestion sécurisée de la mémoire (`std::shared_ptr`) ainsi que la détection mathématique des chevauchements horaires.

---

## Compilation et exécution

Pour compiler et exécuter ce programme avec `g++` (norme C++11 ou supérieure) :

```bash
g++ -std=c++17 main.cpp -o gestion_salles
./gestion_salles
```
### SOSSOU Caleb