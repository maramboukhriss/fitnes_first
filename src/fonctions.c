#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h> // Pour obtenir l'année actuelle
#include <cairo.h>

// Fonction pour calculer l'âge à partir de la date de naissance
int calculer_age(int annee_naissance, int mois_naissance, int jour_naissance) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    int annee_actuelle = local->tm_year + 1900;
    int mois_actuel = local->tm_mon + 1;
    int jour_actuel = local->tm_mday;
    
    int age = annee_actuelle - annee_naissance;
    
    // Ajuster si l'anniversaire n'est pas encore passé cette année
    if (mois_actuel < mois_naissance || 
        (mois_actuel == mois_naissance && jour_actuel < jour_naissance)) {
        age--;
    }
    
    return age;
}

// Fonction principale pour calculer les statistiques
StatistiquesDemographiques calculer_statistiques_demographiques() {
    StatistiquesDemographiques stats = {0};
    FILE *f = fopen("users.txt", "r");
    
    if (f == NULL) {
        printf("Fichier users.txt non trouvé\n");
        return stats;
    }
    
    User user;
    int somme_ages = 0;
    stats.age_min = 100;
    stats.age_max = 0;
    
    while (fscanf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                 user.id, user.nom, user.prenom, user.email, user.mot_de_passe,
                 user.telephone, user.adresse, &user.jour_naissance, &user.mois_naissance,
                 &user.annee_naissance, &user.sexe, user.poids) != EOF) {
        
        stats.total_membres++;
        
        // Compter hommes/femmes
        if (user.sexe == 0) {
            stats.femmes++;
        } else {
            stats.hommes++;
        }
        
        // Calculer l'âge
        int age = calculer_age(user.annee_naissance, user.mois_naissance, user.jour_naissance);
        somme_ages += age;
        
        // Mettre à jour min/max
        if (age < stats.age_min) stats.age_min = age;
        if (age > stats.age_max) stats.age_max = age;
        
        // Répartition par tranche d'âge
        if (age < 20) stats.moins_20_ans++;
        else if (age >= 20 && age < 30) stats.entre_20_30++;
        else if (age >= 30 && age < 40) stats.entre_30_40++;
        else if (age >= 40 && age < 50) stats.entre_40_50++;
        else stats.plus_50_ans++;
    }
    
    fclose(f);
    
    // Calculer l'âge moyen
    if (stats.total_membres > 0) {
        stats.age_moyen = (float)somme_ages / stats.total_membres;
    }
    
    return stats;
}

// Fonction pour générer un rapport texte
void generer_rapport_statistiques(const char* filename) {
    StatistiquesDemographiques stats = calculer_statistiques_demographiques();
    
    FILE *rapport = fopen(filename, "w");
    if (rapport == NULL) {
        printf("Erreur création du rapport\n");
        return;
    }
    
    fprintf(rapport, "=== RAPPORT STATISTIQUES - FITNESS CLUB ===\n\n");
    fprintf(rapport, "Date de génération : %s\n\n", __DATE__);
    
    fprintf(rapport, "📊 STATISTIQUES DÉMOGRAPHIQUES\n");
    fprintf(rapport, "--------------------------------\n");
    fprintf(rapport, "Total membres : %d\n", stats.total_membres);
    fprintf(rapport, "Hommes : %d (%.1f%%)\n", stats.hommes, 
            stats.total_membres > 0 ? (float)stats.hommes/stats.total_membres*100 : 0);
    fprintf(rapport, "Femmes : %d (%.1f%%)\n", stats.femmes,
            stats.total_membres > 0 ? (float)stats.femmes/stats.total_membres*100 : 0);
    fprintf(rapport, "Âge moyen : %.1f ans\n", stats.age_moyen);
    fprintf(rapport, "Âge minimum : %d ans\n", stats.age_min);
    fprintf(rapport, "Âge maximum : %d ans\n\n", stats.age_max);
    
    fprintf(rapport, "📈 RÉPARTITION PAR ÂGE\n");
    fprintf(rapport, "----------------------\n");
    fprintf(rapport, "Moins de 20 ans : %d (%.1f%%)\n", stats.moins_20_ans,
            stats.total_membres > 0 ? (float)stats.moins_20_ans/stats.total_membres*100 : 0);
    fprintf(rapport, "20-29 ans : %d (%.1f%%)\n", stats.entre_20_30,
            stats.total_membres > 0 ? (float)stats.entre_20_30/stats.total_membres*100 : 0);
    fprintf(rapport, "30-39 ans : %d (%.1f%%)\n", stats.entre_30_40,
            stats.total_membres > 0 ? (float)stats.entre_30_40/stats.total_membres*100 : 0);
    fprintf(rapport, "40-49 ans : %d (%.1f%%)\n", stats.entre_40_50,
            stats.total_membres > 0 ? (float)stats.entre_40_50/stats.total_membres*100 : 0);
    fprintf(rapport, "50 ans et plus : %d (%.1f%%)\n", stats.plus_50_ans,
            stats.total_membres > 0 ? (float)stats.plus_50_ans/stats.total_membres*100 : 0);
    
    fclose(rapport);
    printf("Rapport généré : %s\n", filename);
}

// ================= FONCTIONS UTILISATEURS =================
int id_existe_dans_fichier(const char *fichier, const char *id) {
    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        return 0; // Fichier n'existe pas encore
    }
    
    User u;
    int id_existe = 0;
    
    while (fscanf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                 u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                 u.telephone, u.adresse, &u.jour_naissance, &u.mois_naissance,
                 &u.annee_naissance, &u.sexe, u.poids) != EOF) {
        
        if (strcmp(u.id, id) == 0) {
            id_existe = 1;
            break;
        }
    }
    
    fclose(f);
    return id_existe;
}
int ajouter_user(char *filename, User u) {
    FILE *f = fopen(filename, "a");
    if (f != NULL) {
        fprintf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                u.telephone, u.adresse, u.jour_naissance, u.mois_naissance,
                u.annee_naissance, u.sexe, u.poids);
        fclose(f);
        return 1;
    }
    return 0;
}

int modifier_user(char *filename, char *id, User nouv) {
    int tr = 0;
    User u;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("temp_user.txt", "w");
    if (f != NULL && f2 != NULL) {
        while (fscanf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                     u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                     u.telephone, u.adresse, &u.jour_naissance, &u.mois_naissance,
                     &u.annee_naissance, &u.sexe, u.poids) != EOF) {
            if (strcmp(u.id, id) == 0) {
                fprintf(f2, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                        nouv.id, nouv.nom, nouv.prenom, nouv.email, nouv.mot_de_passe,
                        nouv.telephone, nouv.adresse, nouv.jour_naissance, nouv.mois_naissance,
                        nouv.annee_naissance, nouv.sexe, nouv.poids);
                tr = 1;
            } else {
                fprintf(f2, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                        u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                        u.telephone, u.adresse, u.jour_naissance, u.mois_naissance,
                        u.annee_naissance, u.sexe, u.poids);
            }
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("temp_user.txt", filename);
    }
    return tr;
}

int supprimer_user(char *filename, char *id) {
    int tr = 0;
    User u;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("temp_user.txt", "w");
    if (f != NULL && f2 != NULL) {
        while (fscanf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                     u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                     u.telephone, u.adresse, &u.jour_naissance, &u.mois_naissance,
                     &u.annee_naissance, &u.sexe, u.poids) != EOF) {
            if (strcmp(u.id, id) == 0) {
                tr = 1;
            } else {
                fprintf(f2, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                        u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                        u.telephone, u.adresse, u.jour_naissance, u.mois_naissance,
                        u.annee_naissance, u.sexe, u.poids);
            }
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("temp_user.txt", filename);
    }
    return tr;
}


// Fonction pour nettoyer une chaîne (supprimer espaces et retours à la ligne)
static void clean_string(char *str) {
    if (str == NULL) return;
    
    // Supprimer les espaces/retours en début
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        start++;
    }
    
    // Si la chaîne commence par des espaces, décaler
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    // Supprimer les espaces/retours en fin
    char *end = str + strlen(str) - 1;
    while (end >= str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

User chercher_user(char *filename, char *id) {
    User u;
    int tr = 0;
    
    // Nettoyer l'ID recherché
    char id_clean[100];
    strncpy(id_clean, id, sizeof(id_clean) - 1);
    id_clean[sizeof(id_clean) - 1] = '\0';
    clean_string(id_clean);
    
    printf("DEBUG chercher_user: Recherche de l'ID '%s' dans %s\n", id_clean, filename);
    
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("ERREUR: Impossible d'ouvrir le fichier %s\n", filename);
        strcpy(u.id, "-1");
        return u;
    }
    
    char line[1024];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), f)) {
        line_num++;
        
        // Nettoyer la ligne
        clean_string(line);
        
        // Si la ligne est vide, passer à la suivante
        if (strlen(line) == 0) continue;
        
        printf("DEBUG chercher_user: Ligne %d: '%s'\n", line_num, line);
        
        // Essayer de parser la ligne avec sscanf
        // Note: nous utilisons %99s pour éviter le débordement
        int result = sscanf(line, "%99s %99s %99s %99s %99s %99s %99[^\n] %d %d %d %d %99s",
                          u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                          u.telephone, u.adresse, &u.jour_naissance, &u.mois_naissance,
                          &u.annee_naissance, &u.sexe, u.poids);
        
        printf("DEBUG chercher_user: Résultat sscanf: %d champs\n", result);
        
        // Si nous avons lu 12 champs, comparer les IDs
        if (result == 12) {
            // Nettoyer l'ID lu du fichier
            clean_string(u.id);
            
            printf("DEBUG chercher_user: ID lu: '%s', ID recherché: '%s'\n", u.id, id_clean);
            
            if (strcmp(u.id, id_clean) == 0) {
                tr = 1;
                printf("DEBUG chercher_user: ID TROUVÉ à la ligne %d\n", line_num);
                break;
            }
        } else if (result >= 7) {
            // Tentative alternative: peut-être que l'adresse contient des espaces
            // Dans ce cas, nous réessayons avec une adresse simple (un seul mot)
            result = sscanf(line, "%99s %99s %99s %99s %99s %99s %99s %d %d %d %d %99s",
                          u.id, u.nom, u.prenom, u.email, u.mot_de_passe,
                          u.telephone, u.adresse, &u.jour_naissance, &u.mois_naissance,
                          &u.annee_naissance, &u.sexe, u.poids);
            
            if (result == 12) {
                clean_string(u.id);
                printf("DEBUG chercher_user (2e essai): ID lu: '%s', ID recherché: '%s'\n", u.id, id_clean);
                
                if (strcmp(u.id, id_clean) == 0) {
                    tr = 1;
                    printf("DEBUG chercher_user: ID TROUVÉ (2e essai) à la ligne %d\n", line_num);
                    break;
                }
            }
        }
    }
    
    fclose(f);
    
    if (!tr) {
        printf("DEBUG chercher_user: ID '%s' NON TROUVÉ\n", id_clean);
        strcpy(u.id, "-1");
    } else {
        printf("DEBUG chercher_user: Succès - Membre trouvé: %s %s\n", u.prenom, u.nom);
    }
    
    return u;
}

// Fonction utilitaire pour vider le buffer d'entrée
static void vider_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void saisir_demande_coach(DemandeCoach *demande) {
    int i;
    int result;
    
    printf("\n=== DEMANDE DE COACH PRIVE ===\n");
    
    // Initialiser toutes les cases à 0
    for(i = 0; i < 5; i++) demande->objectifs[i] = 0;
    for(i = 0; i < 5; i++) demande->jours[i] = 0;
    
    // Saisie de l'ID du membre (vérification d'abord)
    printf("ID du membre: ");
    result = scanf("%s", demande->id_membre);
    if (result != 1) {
        printf("❌ ERREUR: Saisie de l'ID invalide!\n");
        vider_buffer();
        strcpy(demande->id_membre, "-1");
        return;
    }
    
    // Vérifier si le membre existe
    User membre = chercher_user("users.txt", demande->id_membre);
    if (strcmp(membre.id, "-1") == 0) {
        printf("❌ ERREUR: Aucun membre trouvé avec cet ID!\n");
        strcpy(demande->id_membre, "-1");
        return;
    }
    printf("✅ Membre trouvé: %s %s\n", membre.prenom, membre.nom);
    
    // Saisie des objectifs (check buttons)
    printf("\n=== OBJECTIFS DE COACHING (cocher 1 pour OUI, 0 pour NON) ===\n");
    
    printf("[0] Perte de poids: ");
    result = scanf("%d", &demande->objectifs[0]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour l'objectif 0!\n");
        vider_buffer();
        return;
    }
    
    printf("[1] Prise de muscle: ");
    result = scanf("%d", &demande->objectifs[1]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour l'objectif 1!\n");
        vider_buffer();
        return;
    }
    
    printf("[2] Cardio: ");
    result = scanf("%d", &demande->objectifs[2]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour l'objectif 2!\n");
        vider_buffer();
        return;
    }
    
    printf("[3] Amélioration des performances: ");
    result = scanf("%d", &demande->objectifs[3]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour l'objectif 3!\n");
        vider_buffer();
        return;
    }
    
    printf("[4] Gymnastique: ");
    result = scanf("%d", &demande->objectifs[4]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour l'objectif 4!\n");
        vider_buffer();
        return;
    }
    
    // Saisie de la disponibilité (radio button)
    printf("\n=== DISPONIBILITE (choisir 0, 1 ou 2) ===\n");
    printf("[0] Matin\n");
    printf("[1] Midi\n");
    printf("[2] Soir\n");
    printf("Votre choix: ");
    result = scanf("%d", &demande->disponibilite);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour la disponibilité!\n");
        vider_buffer();
        return;
    }
    
    // Validation de la disponibilité
    if (demande->disponibilite < 0 || demande->disponibilite > 2) {
        printf("❌ ERREUR: Disponibilité doit être 0, 1 ou 2!\n");
        return;
    }
    
    // Saisie des jours (check buttons)
    printf("\n=== JOURS D'ENTRAINEMENT (cocher 1 pour OUI, 0 pour NON) ===\n");
    
    printf("[0] LUNDI: ");
    result = scanf("%d", &demande->jours[0]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour le jour 0!\n");
        vider_buffer();
        return;
    }
    
    printf("[1] MARDI: ");
    result = scanf("%d", &demande->jours[1]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour le jour 1!\n");
        vider_buffer();
        return;
    }
    
    printf("[2] MERCREDI: ");
    result = scanf("%d", &demande->jours[2]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour le jour 2!\n");
        vider_buffer();
        return;
    }
    
    printf("[3] JEUDI: ");
    result = scanf("%d", &demande->jours[3]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour le jour 3!\n");
        vider_buffer();
        return;
    }
    
    printf("[4] VENDREDI: ");
    result = scanf("%d", &demande->jours[4]);
    if (result != 1) {
        printf("❌ ERREUR: Saisie invalide pour le jour 4!\n");
        vider_buffer();
        return;
    }
    
    printf("✅ Saisie terminée avec succès!\n");
}

int enregistrer_demande_coach(DemandeCoach demande) {
    FILE *f = fopen("reservation.txt", "a");
    if (f == NULL) {
        printf("❌ Erreur: Impossible de créer/ouvrir le fichier reservation.txt\n");
        return 0;
    }
    
    // Convertir les tableaux en chaînes pour l'enregistrement
    char objectifs_str[100] = "";
    char jours_str[100] = "";
    
    int i;
    for(i = 0; i < 5; i++) {
        char temp[10];
        snprintf(temp, sizeof(temp), "%d", demande.objectifs[i]);
        strncat(objectifs_str, temp, sizeof(objectifs_str) - strlen(objectifs_str) - 1);
        if(i < 4) strncat(objectifs_str, ",", sizeof(objectifs_str) - strlen(objectifs_str) - 1);
        
        snprintf(temp, sizeof(temp), "%d", demande.jours[i]);
        strncat(jours_str, temp, sizeof(jours_str) - strlen(jours_str) - 1);
        if(i < 4) strncat(jours_str, ",", sizeof(jours_str) - strlen(jours_str) - 1);
    }
    
    // Convertir la disponibilité en texte
    char *dispo_str;
    switch(demande.disponibilite) {
        case 0: dispo_str = "Matin"; break;
        case 1: dispo_str = "Midi"; break;
        case 2: dispo_str = "Soir"; break;
        default: dispo_str = "Inconnu";
    }
    
    // Enregistrer dans le fichier
    fprintf(f, "Membre:%s | Objectifs:%s | Disponibilite:%s | Jours:%s\n",
            demande.id_membre, objectifs_str, dispo_str, jours_str);
    fclose(f);
    
    printf("\n✅ Demande de coach enregistrée avec succès!\n");
    printf("📁 Fichier: reservation.txt\n");
    
    return 1; // Succès
}

// Fonction pour supprimer une réservation par index de ligne
int supprimer_reservation_par_ligne(const char *filename, int ligne_a_supprimer) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        return 0;
    }
    
    FILE *temp = fopen("temp_reservation.txt", "w");
    if (temp == NULL) {
        fclose(f);
        return 0;
    }
    
    char line[1024];
    int ligne_courante = 0;
    int supprime = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (ligne_courante != ligne_a_supprimer) {
            fputs(line, temp);
        } else {
            supprime = 1;
        }
        ligne_courante++;
    }
    
    fclose(f);
    fclose(temp);
    
    if (supprime) {
        remove(filename);
        rename("temp_reservation.txt", filename);
        return 1;
    } else {
        remove("temp_reservation.txt");
        return 0;
    }
}
DemandeCoach* lire_toutes_demandes(const char *filename, int *count) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        *count = 0;
        return NULL;
    }
    
    // Première passe pour compter le nombre de lignes
    char line[1024];
    int line_count = 0;
    while (fgets(line, sizeof(line), f)) {
        line_count++;
    }
    rewind(f);
    
    // Allouer mémoire
    DemandeCoach *demandes = malloc(line_count * sizeof(DemandeCoach));
    if (demandes == NULL) {
        fclose(f);
        *count = 0;
        return NULL;
    }
    
    int index = 0;
    while (fgets(line, sizeof(line), f)) {
        // Nettoyer la ligne
        line[strcspn(line, "\n")] = 0;
        line[strcspn(line, "\r")] = 0;
        
        // DEBUG: Afficher la ligne lue
        printf("DEBUG: Parsing line: %s\n", line);
        
        // Extraire les parties avec sscanf
        char id[100] = "";
        char objectifs_str[100] = "";
        char disponibilite_str[50] = "";
        char jours_str[100] = "";
        
        // Utiliser un format qui correspond exactement à votre fichier
        // Le format: Membre:ID | Objectifs:0,1,0,0,0 | Disponibilite:Matin | Jours:0,0,0,1,0
        if (sscanf(line, "Membre:%99[^|] | Objectifs:%99[^|] | Disponibilite:%99[^|] | Jours:%99[^\n]", 
                   id, objectifs_str, disponibilite_str, jours_str) == 4) {
            
            // Nettoyer les espaces autour des chaînes
            char *clean_id = id;
            while (*clean_id == ' ') clean_id++;
            char *end_id = clean_id + strlen(clean_id) - 1;
            while (end_id > clean_id && *end_id == ' ') end_id--;
            *(end_id + 1) = '\0';
            
            char *clean_disp = disponibilite_str;
            while (*clean_disp == ' ') clean_disp++;
            char *end_disp = clean_disp + strlen(clean_disp) - 1;
            while (end_disp > clean_disp && *end_disp == ' ') end_disp--;
            *(end_disp + 1) = '\0';
            
            // DEBUG
            printf("DEBUG: Extracted - ID:'%s', Disp:'%s'\n", clean_id, clean_disp);
            
            // Copier l'ID
            strcpy(demandes[index].id_membre, clean_id);
            
            // Convertir disponibilité
            if (strcmp(clean_disp, "Matin") == 0) {
                demandes[index].disponibilite = 0;
            } else if (strcmp(clean_disp, "Midi") == 0) {
                demandes[index].disponibilite = 1;
            } else if (strcmp(clean_disp, "Soir") == 0) {
                demandes[index].disponibilite = 2;
            } else {
                printf("WARNING: Disponibilite inconnue: '%s'\n", clean_disp);
                demandes[index].disponibilite = -1;
            }
            
            // Convertir objectifs
            int obj_values[5] = {0};
            int obj_count = 0;
            char *token = strtok(objectifs_str, ",");
            while (token != NULL && obj_count < 5) {
                // Nettoyer le token
                char *clean_token = token;
                while (*clean_token == ' ') clean_token++;
                char *end_token = clean_token + strlen(clean_token) - 1;
                while (end_token > clean_token && *end_token == ' ') end_token--;
                *(end_token + 1) = '\0';
                
                obj_values[obj_count] = atoi(clean_token);
                obj_count++;
                token = strtok(NULL, ",");
            }
            for (int i = 0; i < 5; i++) {
                demandes[index].objectifs[i] = obj_values[i];
            }
            
            // Convertir jours
            int jours_values[5] = {0};
            int jours_count = 0;
            token = strtok(jours_str, ",");
            while (token != NULL && jours_count < 5) {
                char *clean_token = token;
                while (*clean_token == ' ') clean_token++;
                char *end_token = clean_token + strlen(clean_token) - 1;
                while (end_token > clean_token && *end_token == ' ') end_token--;
                *(end_token + 1) = '\0';
                
                jours_values[jours_count] = atoi(clean_token);
                jours_count++;
                token = strtok(NULL, ",");
            }
            for (int i = 0; i < 5; i++) {
                demandes[index].jours[i] = jours_values[i];
            }
            
            index++;
        } else {
            printf("WARNING: Failed to parse line: %s\n", line);
            printf("DEBUG: sscanf returned %d items\n", 
                   sscanf(line, "Membre:%99[^|] | Objectifs:%99[^|] | Disponibilite:%99[^|] | Jours:%99[^\n]", 
                          id, objectifs_str, disponibilite_str, jours_str));
        }
    }
    
    fclose(f);
    *count = index;
    
    printf("DEBUG: Total reservations read: %d\n", index);
    return demandes;
}
// Fonction pour mettre à jour une réservation par index de ligne
int mettre_a_jour_reservation(DemandeCoach nouvelle_demande, int ligne_index) {
    FILE *f = fopen("reservation.txt", "r");
    if (f == NULL) {
        printf("ERREUR: Impossible d'ouvrir reservation.txt pour lecture\n");
        return 0;
    }
    
    FILE *temp = fopen("temp_reservation.txt", "w");
    if (temp == NULL) {
        fclose(f);
        printf("ERREUR: Impossible de créer fichier temporaire\n");
        return 0;
    }
    
    char line[1024];
    int ligne_courante = 0;
    int modifie = 0;
    
    while (fgets(line, sizeof(line), f)) {
        // Supprimer le saut de ligne
        line[strcspn(line, "\n")] = 0;
        line[strcspn(line, "\r")] = 0;
        
        if (ligne_courante == ligne_index) {
            // Écrire la nouvelle réservation
            // Convertir les tableaux en chaînes
            char objectifs_str[100] = "";
            char jours_str[100] = "";
            
            for (int i = 0; i < 5; i++) {
                char temp_str[10];
                snprintf(temp_str, sizeof(temp_str), "%d", nouvelle_demande.objectifs[i]);
                strncat(objectifs_str, temp_str, sizeof(objectifs_str) - strlen(objectifs_str) - 1);
                if (i < 4) strncat(objectifs_str, ",", sizeof(objectifs_str) - strlen(objectifs_str) - 1);
                
                snprintf(temp_str, sizeof(temp_str), "%d", nouvelle_demande.jours[i]);
                strncat(jours_str, temp_str, sizeof(jours_str) - strlen(jours_str) - 1);
                if (i < 4) strncat(jours_str, ",", sizeof(jours_str) - strlen(jours_str) - 1);
            }
            
            // Convertir la disponibilité en texte
            const char *dispo_str;
            switch(nouvelle_demande.disponibilite) {
                case 0: dispo_str = "Matin"; break;
                case 1: dispo_str = "Midi"; break;
                case 2: dispo_str = "Soir"; break;
                default: dispo_str = "Inconnu";
            }
            
            fprintf(temp, "Membre:%s | Objectifs:%s | Disponibilite:%s | Jours:%s\n",
                    nouvelle_demande.id_membre, objectifs_str, dispo_str, jours_str);
            modifie = 1;
        } else {
            // Recopier la ligne telle quelle
            fprintf(temp, "%s\n", line);
        }
        
        ligne_courante++;
    }
    
    fclose(f);
    fclose(temp);
    
    if (modifie) {
        // Remplacer l'ancien fichier par le nouveau
        remove("reservation.txt");
        rename("temp_reservation.txt", "reservation.txt");
        return 1;
    } else {
        remove("temp_reservation.txt");
        return 0;
    }
}

// Fonction pour convertir le texte en disponibilité
int texte_vers_disponibilite(const char *texte) {
    if (strcmp(texte, "Matin") == 0) return 0;
    if (strcmp(texte, "Midi") == 0) return 1;
    if (strcmp(texte, "Soir") == 0) return 2;
    return -1;
}

// Fonction pour lire une réservation spécifique par index
DemandeCoach lire_reservation_par_index(const char *filename, int index) {
    DemandeCoach demande;
    int count = 0;
    DemandeCoach *demandes = lire_toutes_demandes(filename, &count);
    
    if (demandes == NULL || index < 0 || index >= count) {
        strcpy(demande.id_membre, "-1");
        return demande;
    }
    
    demande = demandes[index];
    free(demandes);
    return demande;
}
// Fonction pour envoyer un email d'inscription
int envoyer_email_inscription(User nouveau_membre) {
    // Créer un dossier pour les confirmations
    mkdir("confirmations", 0755);
    
    // Nom du fichier
    char filename[256];
    snprintf(filename, sizeof(filename), 
             "confirmations/%s_inscription.txt", 
             nouveau_membre.id);
    
    // Ouvrir le fichier
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Erreur: impossible de créer le fichier de confirmation\n");
        return 0;
    }
    
    // Écrire la confirmation
    fprintf(f, "=== CONFIRMATION D'INSCRIPTION ===\n\n");
    fprintf(f, "Date: %s\n", __DATE__);
    fprintf(f, "Heure: %s\n", __TIME__);
    fprintf(f, "\n");
    fprintf(f, "DESTINATAIRE:\n");
    fprintf(f, "  %s %s\n", nouveau_membre.prenom, nouveau_membre.nom);
    fprintf(f, "  Email: %s\n", nouveau_membre.email);
    fprintf(f, "  Téléphone: %s\n", nouveau_membre.telephone);
    fprintf(f, "\n");
    fprintf(f, "DÉTAILS DE L'INSCRIPTION:\n");
    fprintf(f, "  ID Membre: %s\n", nouveau_membre.id);
    fprintf(f, "  Statut: CONFIRMÉ\n");
    fprintf(f, "\n");
    fprintf(f, "En production, cet email serait envoyé à l'adresse ci-dessus.\n");
    
    fclose(f);
    
    // Afficher confirmation dans le terminal
    printf("\n");
    printf("=========================================\n");
    printf("✅ INSCRIPTION CONFIRMÉE\n");
    printf("=========================================\n");
    printf("Membre: %s %s\n", nouveau_membre.prenom, nouveau_membre.nom);
    printf("ID: %s\n", nouveau_membre.id);
    printf("Email: %s\n", nouveau_membre.email);
    printf("Fichier de confirmation: %s\n", filename);
    printf("=========================================\n\n");
    
    return 1;
}
// Fonction pour calculer les données de l'histogramme
HistogrammeData calculer_donnees_histogramme() {
    HistogrammeData data = {0};
    
    FILE *f = fopen("users.txt", "r");
    if (f == NULL) {
        printf("Fichier users.txt non trouvé\n");
        return data;
    }
    
    User user;
    while (fscanf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                 user.id, user.nom, user.prenom, user.email, user.mot_de_passe,
                 user.telephone, user.adresse, &user.jour_naissance, &user.mois_naissance,
                 &user.annee_naissance, &user.sexe, user.poids) != EOF) {
        
        // Calculer l'âge
        int age = calculer_age(user.annee_naissance, user.mois_naissance, user.jour_naissance);
        
        // Classer par tranche d'âge
        if (age < 20) data.tranches_age[0]++;
        else if (age >= 20 && age < 30) data.tranches_age[1]++;
        else if (age >= 30 && age < 40) data.tranches_age[2]++;
        else if (age >= 40 && age < 50) data.tranches_age[3]++;
        else data.tranches_age[4]++;
        
        // Par sexe
        if (user.sexe == 0) data.femmes++;
        else data.hommes++;
        
        // Par poids
        float poids = atof(user.poids);
        if (poids > 0) {
            if (poids < 60) data.categories_poids[0]++;
            else if (poids < 70) data.categories_poids[1]++;
            else if (poids < 80) data.categories_poids[2]++;
            else if (poids < 90) data.categories_poids[3]++;
            else data.categories_poids[4]++;
        }
    }
    
    fclose(f);
    return data;
}

// Fonction pour dessiner un histogramme avec Cairo (GTK2 compatible)
// Fonction pour dessiner un histogramme avec Cairo (GTK2 compatible)
void dessiner_histogramme(void *cr_ptr, int width, int height, HistogrammeData data) {
    // Convertir le pointeur void en cairo_t
    cairo_t *cr = (cairo_t *)cr_ptr;
    
    // Fond blanc
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    // Titre
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, width/2 - 100, 30);
    cairo_show_text(cr, "Histogramme des Membres");
    
    // Sous-titre
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, width/2 - 80, 50);
    cairo_show_text(cr, "Fitness First Club");
    
    // ===== HISTOGRAMME DES ÂGES =====
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, 50, 80);
    cairo_show_text(cr, "Repartition par age");
    
    // Coordonnées de l'histogramme
    int start_x = 50;
    int start_y = 120;
    int graph_width = width - 100;
    int graph_height = 200;
    int bar_width = (graph_width - 60) / 5;
    
    // Trouver la valeur maximale pour l'échelle
    int max_value = 0;
    for (int i = 0; i < 5; i++) {
        if (data.tranches_age[i] > max_value) {
            max_value = data.tranches_age[i];
        }
    }
    if (max_value == 0) max_value = 1; // Éviter la division par zéro
    
    // Dessiner les axes
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);
    
    // Axe vertical (Y)
    cairo_move_to(cr, start_x, start_y);
    cairo_line_to(cr, start_x, start_y + graph_height);
    
    // Axe horizontal (X)
    cairo_move_to(cr, start_x, start_y + graph_height);
    cairo_line_to(cr, start_x + graph_width, start_y + graph_height);
    
    cairo_stroke(cr);
    
    // Échelle sur l'axe Y
    cairo_set_font_size(cr, 10);
    for (int i = 0; i <= 10; i++) {
        int y = start_y + graph_height - (i * graph_height / 10);
        cairo_move_to(cr, start_x - 30, y);
        
        // CORRECTION ICI : Utiliser snprintf au lieu de sprintf
        char label[12]; // Augmenter la taille pour éviter le débordement
        int value = (max_value * i) / 10;
        snprintf(label, sizeof(label), "%d", value);
        cairo_show_text(cr, label);
    }
    
    // Labels des catégories d'âge
    const char *labels_age[] = {"<20", "20-29", "30-39", "40-49", "50+"};
    
    // Dessiner les barres
    for (int i = 0; i < 5; i++) {
        int bar_height = (int)((float)data.tranches_age[i] / max_value * graph_height);
        int x = start_x + 30 + i * (bar_width + 10);
        int y = start_y + graph_height - bar_height;
        
        // Couleur de la barre (dégradé de bleu)
        cairo_pattern_t *pat = cairo_pattern_create_linear(x, y, x, y + bar_height);
        cairo_pattern_add_color_stop_rgba(pat, 0, 0.2, 0.4, 0.8, 0.8);
        cairo_pattern_add_color_stop_rgba(pat, 1, 0.1, 0.2, 0.6, 0.8);
        cairo_set_source(cr, pat);
        
        // Dessiner la barre
        cairo_rectangle(cr, x, y, bar_width, bar_height);
        cairo_fill(cr);
        cairo_pattern_destroy(pat);
        
        // Contour de la barre
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 0.5);
        cairo_rectangle(cr, x, y, bar_width, bar_height);
        cairo_stroke(cr);
        
        // Label sous la barre
        cairo_move_to(cr, x + bar_width/2 - 10, start_y + graph_height + 15);
        cairo_show_text(cr, labels_age[i]);
        
        // Valeur au-dessus de la barre
        if (data.tranches_age[i] > 0) {
            // CORRECTION ICI : Utiliser snprintf au lieu de sprintf
            char valeur[12]; // Augmenter la taille
            snprintf(valeur, sizeof(valeur), "%d", data.tranches_age[i]);
            cairo_move_to(cr, x + bar_width/2 - 5, y - 5);
            cairo_show_text(cr, valeur);
        }
    }
    
    // ===== HISTOGRAMME DES SEXES =====
    int start_y2 = start_y + graph_height + 50;
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, 50, start_y2);
    cairo_show_text(cr, "Repartition par sexe");
    
    // Positions pour les barres hommes/femmes
    int bar_width2 = 50;
    int x_hommes = 100;
    int x_femmes = 200;
    int total_sexes = data.hommes + data.femmes;
    
    if (total_sexes > 0) {
        // Barre hommes (bleu)
        int bar_height_hommes = (int)((float)data.hommes / total_sexes * 100);
        cairo_set_source_rgb(cr, 0.1, 0.3, 0.8);
        cairo_rectangle(cr, x_hommes, start_y2 + 30, bar_width2, bar_height_hommes);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x_hommes, start_y2 + 30, bar_width2, bar_height_hommes);
        cairo_stroke(cr);
        cairo_move_to(cr, x_hommes + 10, start_y2 + bar_height_hommes + 45);
        cairo_show_text(cr, "Hommes");
        
        // Barre femmes (rose)
        int bar_height_femmes = (int)((float)data.femmes / total_sexes * 100);
        cairo_set_source_rgb(cr, 0.9, 0.3, 0.6);
        cairo_rectangle(cr, x_femmes, start_y2 + 30, bar_width2, bar_height_femmes);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x_femmes, start_y2 + 30, bar_width2, bar_height_femmes);
        cairo_stroke(cr);
        cairo_move_to(cr, x_femmes + 10, start_y2 + bar_height_femmes + 45);
        cairo_show_text(cr, "Femmes");
    }
    
    // Légende
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, width - 200, start_y2 + 60);
    
    // CORRECTION ICI : Utiliser snprintf au lieu de sprintf
    char legende[100];
    snprintf(legende, sizeof(legende), "Total: %d membres", data.hommes + data.femmes);
    cairo_show_text(cr, legende);
    
    // Date en bas
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_set_font_size(cr, 10);
    
    // CORRECTION ICI : Utiliser snprintf au lieu de sprintf
    char date[50];
    snprintf(date, sizeof(date), "Genere le %s", __DATE__);
    cairo_move_to(cr, width - 150, height - 20);
    cairo_show_text(cr, date);
}
