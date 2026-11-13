# TP 4 : Système Intégré de Gestion d'un Département Universitaire

## 1. Présentation du Projet
Ce projet consiste à concevoir et développer une application centralisée permettant de gérer les activités administratives et pédagogiques d'un département universitaire :
- Gestion des étudiants et des enseignants.
- Gestion des filières, niveaux, unités d'enseignement (UE) et matières.
- Planification des emplois du temps et gestion des salles (avec détection de conflits).
- Saisie des notes, calcul des moyennes, validation des crédits et génération de relevés de notes.

## 2. Architecture du Projet
```text
GestionDepartement/
├── README.md                 # Documentation complète et modélisation UML
├── docs/
│   └── images/               # Images générées des diagrammes UML (.png)
│       ├── use_case.png
│       ├── class_diagram.png
│       ├── sequence_notes.png
│       ├── activity_deliberation.png
│       └── component_deployment.png
├── include/                  # Fichiers d'en-tête (.h)
│   ├── Models.h
│   └── SystemeGestion.h
├── src/                      # Fichiers sources (.cpp)
│   └── main.cpp
```

## 3. Modélisation UML

> **Note :** Exportez les scripts PlantUML ci-dessous au format PNG dans le dossier `images/` pour afficher les rendus visuels directement dans ce fichier.

### A. Diagramme Global de Cas d'Utilisation
![Diagramme de cas d'utilisation](images\cas-utilisation.png)

<details>
<summary>Voir le code source PlantUML</summary>

```plantuml
@startuml
left to right direction
actor "Administrateur" as Admin
actor "Enseignant" as Prof
actor "Étudiant" as Etud

rectangle "Système de Gestion du Département" {
  usecase "Gérer filières, niveaux et UE" as UC1
  usecase "Gérer étudiants et enseignants" as UC2
  usecase "Gérer salles et emplois du temps" as UC3
  usecase "Saisir les notes" as UC4
  usecase "Lancer la délibération (Moyennes/Crédits)" as UC5
  usecase "Générer les relevés de notes" as UC6
  usecase "Consulter emploi du temps" as UC7
  usecase "Consulter notes et relevés" as UC8
}

Admin --> UC1
Admin --> UC2
Admin --> UC3
Admin --> UC5
Admin --> UC6

Prof --> UC4
Prof --> UC7

Etud --> UC7
Etud --> UC8
@enduml
```
</details>

### B. Diagramme de Classes Complet
![Diagramme de classes](images\classes.png)

<details>
<summary>Voir le code source PlantUML</summary>

```plantuml
@startuml
class Departement {
  - id: int
  - nom: string
  + ajouterFiliere(f: Filiere): void
}

class Filiere {
  - code: string
  - intitule: string
}

class Niveau {
  - code: string
  - libelle: string
}

class UE {
  - code: string
  - nom: string
  - credits: int
  + calculerMoyenneUE(matricule: string): float
}

class Matiere {
  - code: string
  - nom: string
  - coefficient: float
}

class Etudiant {
  - matricule: string
  - nom: string
  - prenom: string
  + calculerMoyenneGenerale(): float
  + getCreditsValides(): int
}

class Enseignant {
  - id: int
  - nom: string
  - specialite: string
}

class Note {
  - valeurCC: float
  - valeurExamen: float
  + calculerNoteFinale(): float
}

class Salle {
  - numero: string
  - capacite: int
  - type: string
}

class Seance {
  - jour: string
  - heureDebut: string
  - heureFin: string
}

class EmploiDuTemps {
  - semestre: string
  + verifierConflit(s: Seance): bool
  + ajouterSeance(s: Seance): void
}

Departement "1" *-- "*" Filiere
Filiere "1" *-- "*" Niveau
Niveau "1" *-- "*" UE
UE "1" *-- "*" Matiere

Niveau "1" -- "*" Etudiant
Enseignant "1" -- "*" Matiere : enseigne >

Etudiant "1" -- "*" Note
Matiere "1" -- "*" Note

EmploiDuTemps "1" o-- "*" Seance
Seance "*" -- "1" Matiere
Seance "*" -- "1" Salle
Seance "*" -- "1" Enseignant
@enduml
```
</details>

### C. Diagramme de Séquence (Saisie des Notes)
![Diagramme de séquence saisie des notes](images\sequence-note.png)
![Diagramme de séquence saisie des notes](images\sequence-releve.png)
![Diagramme de séquence saisie des notes](images\sequence-seance.png)
<details>
<summary>Voir le code source PlantUML</summary>

```plantuml
@startuml
actor Enseignant
participant "InterfaceConsole" as UI
participant "SystemeGestion" as SG
participant "Etudiant" as Etud
participant "FichierCSV" as CSV

Enseignant -> UI : Demande saisie notes(codeMatiere)
UI -> SG : chargerEtudiants(codeMatiere)
SG --> UI : Liste des étudiants

loop Pour chaque étudiant
  Enseignant -> UI : Saisit note CC et Examen
  UI -> SG : enregistrerNote(matricule, codeMatiere, cc, exam)
  create participant "Note" as N
  SG -> N : new Note(cc, exam)
  SG -> Etud : ajouterNote(N)
end

SG -> CSV : sauvegarderNotes()
CSV --> SG : Confirmation
SG --> UI : Saisie terminée avec succès
@enduml
```
</details>

### D. Diagramme d'Activités (Processus de Délibération)
![Diagramme d'activités délibération](images\activite-deliberation.png)

<details>
<summary>Voir le code source PlantUML</summary>

```plantuml
@startuml
start
:Récupérer la liste des étudiants du niveau;
while (Reste-t-il un étudiant à traiter ?) is (Oui)
  :Récupérer les UE du niveau;
  while (Reste-t-il une UE ?) is (Oui)
    :Calculer la moyenne pondérée des matières de l'UE;
    if (Moyenne UE >= 10.0 ?) then (Oui)
      :Marquer l'UE comme Validée;
      :Ajouter les crédits de l'UE au total validé;
    else (Non)
      :Marquer l'UE comme Non Validée;
    endif
  endwhile (Non)
  :Calculer la moyenne générale annuelle;
  if (Moyenne Générale >= 10.0 ET Crédits >= Seuil ?) then (Oui)
    :Décision = Admis;
  else (Non)
    :Décision = Ajourné;
  endif
  :Générer le relevé de notes (CSV/TXT);
endwhile (Non)
:Archiver le procès-verbal de délibération;
stop
@enduml
```
</details>

### E. Diagrammes de Composants et de Déploiement
![Diagramme de composants et déploiement](images\composants.png)

![Diagramme de composants et déploiement](images\deploiement.png)

<details>
<summary>Voir le code source PlantUML</summary>

```plantuml
@startuml
package "Diagramme de Composants" {
  [Interface Console (CLI)] ..> [Module Pédagogique (UE/Notes)]
  [Interface Console (CLI)] ..> [Module Planification (EDT/Salles)]
  [Module Pédagogique (UE/Notes)] ..> [Gestionnaire Persistance CSV]
  [Module Planification (EDT/Salles)] ..> [Gestionnaire Persistance CSV]
  [Gestionnaire Persistance CSV] --> (Fichiers .csv / .txt)
}

node "Poste Administratif (PC Windows/Linux)" {
  artifact "GestionDepartement.exe" as App
  folder "Repertoire_Data" {
    file "etudiants.csv"
    file "notes.csv"
    file "edt.csv"
  }
  App --> Repertoire_Data : Lecture / Écriture locale
}
@enduml
```
</details>

## 4. Compilation et Exécution (C++)

### Prérequis
- Compilateur C++ supportant la norme **C++17** (`g++`, `clang++` ou MSVC).

### Commande de compilation
```bash
g++ -std=c++17 -Iinclude src/main.cpp -o GestionDepartement
```

### Exécution
```bash
./GestionDepartement
```
