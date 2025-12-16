#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#include <stdio.h>

// ================= STRUCTURE UTILISATEUR =================
typedef struct {
    char id[100];
    char nom[100];
    char prenom[100];
    char email[100];
    char mot_de_passe[100];
    char telephone[100];
    char adresse[100];
    int jour_naissance;
    int mois_naissance;
    int annee_naissance;
    int sexe;           // Radio button: 0=Femme, 1=Homme
    char poids[20];     // Champ poids
} User;
// ================= PROTOTYPES DES FONCTIONS =================

// Fonctions utilisateurs
int ajouter_user(char *filename, User u);
int modifier_user(char *filename, char *id, User nouv);
int supprimer_user(char *filename, char *id);
User chercher_user(char *filename, char *id);
int id_existe_dans_fichier(const char *fichier, const char *id);
// ================= STRUCTURE DEMANDE COACH =================
typedef struct {
    char id_membre[100];  // ID du membre (chaîne de caractères)
    int objectifs[5];      // Check buttons (0 ou 1)
    int disponibilite;     // Radio button: 0=Matin, 1=Midi, 2=Soir
    int jours[5];         // Check buttons (0 ou 1)
} DemandeCoach;

// ================= PROTOTYPES DES FONCTIONS =================


// Fonctions demande coach
void saisir_demande_coach(DemandeCoach *demande);
int enregistrer_demande_coach(DemandeCoach demande);
User chercher_user(char *filename, char *id);
// Fonctions de réservation
DemandeCoach* lire_toutes_demandes(const char *filename, int *count);
int supprimer_reservation_par_ligne(const char *filename, int ligne_a_supprimer);
int texte_vers_disponibilite(const char *texte);
int mettre_a_jour_reservation(DemandeCoach nouvelle_demande, int ligne_index);
DemandeCoach lire_reservation_par_index(const char *filename, int index);
int envoyer_email_inscription(User nouveau_membre);
// Nouvelle structure pour les statistiques
typedef struct {
    int total_membres;
    int hommes;
    int femmes;
    int moins_20_ans;
    int entre_20_30;
    int entre_30_40;
    int entre_40_50;
    int plus_50_ans;
    float age_moyen;
    int age_min;
    int age_max;
} StatistiquesDemographiques;

// Prototypes
StatistiquesDemographiques calculer_statistiques_demographiques();
void generer_rapport_statistiques(const char* filename);

// Structure pour les données de l'histogramme
typedef struct {
    int tranches_age[5];        // [<20, 20-29, 30-39, 40-49, 50+]
    int hommes;
    int femmes;
    int categories_poids[5];    // [<60, 60-69, 70-79, 80-89, 90+]
} HistogrammeData;


// Fonctions histogramme
HistogrammeData calculer_donnees_histogramme();
void dessiner_histogramme(void *cr_ptr, int width, int height, HistogrammeData data);
#endif
