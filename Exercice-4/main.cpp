
#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// --- Énumération pour la catégorie de salle ---
enum class CategorieSalle {
    COURS,
    TD,
    SOUTENANCE
};

string categorieToString(CategorieSalle cat) {
    switch (cat) {
        case CategorieSalle::COURS: return "Cours";
        case CategorieSalle::TD: return "TD";
        case CategorieSalle::SOUTENANCE: return "Soutenance";
        default: return "Inconnue";
    }
}

// --- Classe Salle ---
class Salle {
private:
    string numero;
    int capacite;
    CategorieSalle categorie;

public:
    Salle(string num, int cap, CategorieSalle cat)
        : numero(num), capacite(cap), categorie(cat) {}

    string getNumero() const { return numero; }
    int getCapacite() const { return capacite; }
    CategorieSalle getCategorie() const { return categorie; }

    void afficher() const {
        cout << "Salle " << numero << " (" << categorieToString(categorie) 
             << ", " << capacite << " places)";
    }
};

// --- Classe Enseignant ---
class Enseignant {
private:
    int id;
    string nom;
    string prenom;

public:
    Enseignant(int idEns, string n, string p)
        : id(idEns), nom(n), prenom(p) {}

    int getId() const { return id; }
    string getNomComplet() const { return prenom + " " + nom; }
};

// --- Classe Reservation ---
class Reservation {
private:
    int idReservation;
    string date;
    int heureDebut;
    int heureFin;
    shared_ptr<Enseignant> enseignant; // Association (1)
    shared_ptr<Salle> salle;           // Association (1)

public:
    Reservation(int id, string d, int hDeb, int hFin, 
                shared_ptr<Enseignant> ens, shared_ptr<Salle> s)
        : idReservation(id), date(d), heureDebut(hDeb), heureFin(hFin), 
          enseignant(ens), salle(s) {}

    int getId() const { return idReservation; }
    string getDate() const { return date; }
    int getHeureDebut() const { return heureDebut; }
    int getHeureFin() const { return heureFin; }
    shared_ptr<Salle> getSalle() const { return salle; }
    shared_ptr<Enseignant> getEnseignant() const { return enseignant; }

    void afficher() const {
        cout << "Reservation #" << idReservation << " | Le " << date 
             << " de " << heureDebut << "h a " << heureFin << "h | ";
        salle->afficher();
        cout << " | Par : " << enseignant->getNomComplet() << endl;
    }
};

// --- Classe GestionReservations (Contrôleur métier) ---
class GestionReservations {
private:
    vector<shared_ptr<Salle>> salles;
    vector<shared_ptr<Enseignant>> enseignants;
    vector<Reservation> reservations;
    int prochainId = 1;

    shared_ptr<Salle> rechercherSalle(const string& numero) const {
        for (const auto& s : salles) {
            if (s->getNumero() == numero) return s;
        }
        return nullptr;
    }

    shared_ptr<Enseignant> rechercherEnseignant(int id) const {
        for (const auto& e : enseignants) {
            if (e->getId() == id) return e;
        }
        return nullptr;
    }

public:
    void ajouterSalle(string numero, int capacite, CategorieSalle cat) {
        salles.push_back(make_shared<Salle>(numero, capacite, cat));
    }

    void ajouterEnseignant(int id, string nom, string prenom) {
        enseignants.push_back(make_shared<Enseignant>(id, nom, prenom));
    }

    // 1. Vérifier la disponibilité d'une salle
    bool estDisponible(const string& numSalle, const string& date, int hDeb, int hFin) const {
        for (const auto& res : reservations) {
            if (res.getSalle()->getNumero() == numSalle && res.getDate() == date) {
                // Règle de chevauchement d'intervalles : [hDeb, hFin[ et [resDeb, resFin[
                if (hDeb < res.getHeureFin() && hFin > res.getHeureDebut()) {
                    return false; // Conflit détecté
                }
            }
        }
        return true;
    }

    // 2. Enregistrer une réservation en empêchant les conflits
    bool reserverSalle(int idEnseignant, const string& numSalle, const string& date, int hDeb, int hFin) {
        if (hDeb >= hFin || hDeb < 0 || hFin > 24) {
            cout << "[Erreur] Creneau horaire invalide (" << hDeb << "h-" << hFin << "h)." << endl;
            return false;
        }

        auto ens = rechercherEnseignant(idEnseignant);
        auto salle = rechercherSalle(numSalle);

        if (!ens || !salle) {
            cout << "[Erreur] Enseignant ou salle introuvable." << endl;
            return false;
        }

        // Contrainte métier : interdiction des réservations conflictuelles
        if (!estDisponible(numSalle, date, hDeb, hFin)) {
            cout << "[Refus] Conflit : La salle " << numSalle << " est deja occupee le " 
                 << date << " sur le creneau " << hDeb << "h-" << hFin << "h." << endl;
            return false;
        }

        Reservation nouvelleRes(prochainId++, date, hDeb, hFin, ens, salle);
        reservations.push_back(nouvelleRes);
        cout << "[Succes] Reservation #" << nouvelleRes.getId() << " confirmee." << endl;
        return true;
    }

    // 3. Annuler une réservation
    bool annulerReservation(int idReservation) {
        for (auto it = reservations.begin(); it != reservations.end(); ++it) {
            if (it->getId() == idReservation) {
                reservations.erase(it);
                cout << "[Succes] Reservation #" << idReservation << " annulee." << endl;
                return true;
            }
        }
        cout << "[Erreur] Reservation #" << idReservation << " introuvable." << endl;
        return false;
    }

    void afficherToutesLesReservations() const {
        cout << "\n=== Planning des reservations ===" << endl;
        if (reservations.empty()) {
            cout << "Aucune reservation enregistree." << endl;
        } else {
            for (const auto& res : reservations) {
                res.afficher();
            }
        }
        cout << "=================================\n" << endl;
    }
};

// --- Programme de test ---
int main() {
    GestionReservations systeme;

    // Initialisation des salles et enseignants
    systeme.ajouterSalle("Amphi A", 200, CategorieSalle::COURS);
    systeme.ajouterSalle("TD-12", 30, CategorieSalle::TD);
    systeme.ajouterSalle("S-Soutenance", 15, CategorieSalle::SOUTENANCE);

    systeme.ajouterEnseignant(101, "Koffi", "Amina");
    systeme.ajouterEnseignant(102, "Mensah", "Paul");

    // Réservations valides
    systeme.reserverSalle(101, "TD-12", "20/11/2026", 8, 10);
    systeme.reserverSalle(102, "Amphi A", "20/11/2026", 10, 12);

    // Tentative de réservation conflictuelle (chevauchement 9h-11h avec 8h-10h)
    systeme.reserverSalle(102, "TD-12", "20/11/2026", 9, 11);

    // Réservation consécutive autorisée (10h-12h juste après 8h-10h)
    systeme.reserverSalle(102, "TD-12", "20/11/2026", 10, 12);

    systeme.afficherToutesLesReservations();

    // Annulation et reprise du créneau libéré
    systeme.annulerReservation(1);
    systeme.reserverSalle(102, "TD-12", "20/11/2026", 9, 11);

    systeme.afficherToutesLesReservations();

    return 0;
}