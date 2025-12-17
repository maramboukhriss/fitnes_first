#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "fonctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <math.h>
#include <time.h>
int oeil_auth_visible = 0;
// Déclarations des fonctions utilitaires
char* objectifs_vers_texte(int objectifs[5]);
char* jours_vers_texte(int jours[5]);
const char* disponibilite_vers_texte(int disponibilite);
// Variables globales pour les boutons radio (au début du fichier callbacks.c)
int sexe_ajout = 0; // 0 = Femme, 1 = Homme (par défaut: Femme)
int sexe_modif = 0; // 0 = Femme, 1 = Homme (par défaut: Femme)

// Callback pour les boutons radio (ajout)
void on_radio_femme_ajout_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        sexe_ajout = 0; // 0 = Femme
    }
}

void on_radio_homme_ajout_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        sexe_ajout = 1; // 1 = Homme
    }
}


// Callback pour les boutons radio (modification)
void on_radio_femme_modif_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        sexe_modif = 0; // 0 = Femme
    }
}

void on_radio_homme_modif_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if (gtk_toggle_button_get_active(togglebutton)) {
        sexe_modif = 1; // 1 = Homme
    }
}

void
on_ajouter_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    // Déclarer deux pointeurs sur GtkWidget
    GtkWidget* input;
    GtkWidget* output;
    
    // Déclarer deux chaines de caractères
    char message_text[200];
    
    // Liez les pointeurs *input et *output aux objets graphiques correspondants
    input = lookup_widget(objet_graphique, "entrynom");
    output = lookup_widget(objet_graphique, "message");
    
    // Vérifier que le nom n'est pas vide
    const char *nom = gtk_entry_get_text(GTK_ENTRY(input));
    if (strlen(nom) == 0) {
        strcpy(message_text, "Le nom est obligatoire");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    // Récupérer tous les autres champs
    User nouvel_user;
    
    // Récupérer le nom
    input = lookup_widget(objet_graphique, "entrynom");
    strcpy(nouvel_user.nom, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer le prénom
    input = lookup_widget(objet_graphique, "entryprenom");
    strcpy(nouvel_user.prenom, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer l'ID
    input = lookup_widget(objet_graphique, "entryid");
    strcpy(nouvel_user.id, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Vérifier si l'ID est vide
    if (strlen(nouvel_user.id) == 0) {
        strcpy(message_text, "L'ID est obligatoire");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    // Vérifier la longueur de l'ID
    if (strlen(nouvel_user.id) > 19) {
        strcpy(message_text, "L'ID est trop long (max 19 caractères)");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    // Vérifier si l'ID existe déjà dans le fichier
    if (id_existe_dans_fichier("users.txt", nouvel_user.id)) {
        strcpy(message_text, "Cet ID existe déjà");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    // Récupérer l'email
    input = lookup_widget(objet_graphique, "entryemail");
    strcpy(nouvel_user.email, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Validation de l'email (format simple)
    if (strlen(nouvel_user.email) > 0 && strchr(nouvel_user.email, '@') == NULL) {
        strcpy(message_text, "Format d'email invalide");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    // Récupérer le mot de passe
    input = lookup_widget(objet_graphique, "entrymdp");
    strcpy(nouvel_user.mot_de_passe, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer le téléphone
    input = lookup_widget(objet_graphique, "entrytel");
    strcpy(nouvel_user.telephone, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer l'adresse
    input = lookup_widget(objet_graphique, "entryadressee");
    strcpy(nouvel_user.adresse, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer le poids (entry122 dans votre interface)
    input = lookup_widget(objet_graphique, "entry122");
    strcpy(nouvel_user.poids, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer les spins pour la date de naissance
    input = lookup_widget(objet_graphique, "jour1");
    nouvel_user.jour_naissance = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(input));
    
    input = lookup_widget(objet_graphique, "mois1");
    nouvel_user.mois_naissance = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(input));
    
    input = lookup_widget(objet_graphique, "annee1");
    nouvel_user.annee_naissance = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(input));
    
    // Validation de la date de naissance
    if (nouvel_user.annee_naissance < 1900 || nouvel_user.annee_naissance > 2024) {
        strcpy(message_text, "Année de naissance invalide");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    if (nouvel_user.mois_naissance < 1 || nouvel_user.mois_naissance > 12) {
        strcpy(message_text, "Mois de naissance invalide");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    if (nouvel_user.jour_naissance < 1 || nouvel_user.jour_naissance > 31) {
        strcpy(message_text, "Jour de naissance invalide");
        gtk_label_set_text(GTK_LABEL(output), message_text);
        return;
    }
    
    // Déterminer le sexe
    GtkWidget* femme_widget = lookup_widget(objet_graphique, "femme");
    GtkWidget* homme_widget = lookup_widget(objet_graphique, "homme");
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(homme_widget))) {
        nouvel_user.sexe = 1; // Homme
    } else {
        nouvel_user.sexe = 0; // Femme (par défaut)
    }
    
    // Ajouter l'utilisateur dans le fichier
    if (ajouter_user("users.txt", nouvel_user)) {
        // SUCCÈS : Envoyer l'email d'inscription
        if (envoyer_email_inscription(nouvel_user)) {
            char success_msg[512];
            snprintf(success_msg, sizeof(success_msg),
                    "✅ Membre %s %s ajouté avec succès!\n"
                    "ID: %s\n"
                    "Email: %s\n"
                    "📧 Email de confirmation envoyé!",
                    nouvel_user.prenom, nouvel_user.nom, 
                    nouvel_user.id, nouvel_user.email);
            
            gtk_label_set_text(GTK_LABEL(output), success_msg);
            printf("INFO: Email d'inscription envoyé à %s\n", nouvel_user.email);
        } else {
            char warning_msg[512];
            snprintf(warning_msg, sizeof(warning_msg),
                    "✅ Membre %s %s ajouté avec succès!\n"
                    "⚠️ L'inscription est réussie mais l'email n'a pas pu être envoyé.",
                    nouvel_user.prenom, nouvel_user.nom);
            
            gtk_label_set_text(GTK_LABEL(output), warning_msg);
            printf("ATTENTION: Échec de l'envoi de l'email à %s\n", nouvel_user.email);
        }
        
        // Vider les champs après l'ajout réussi
        // Vider tous les champs de texte
        input = lookup_widget(objet_graphique, "entrynom");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entryprenom");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entryid");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entryemail");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entrymdp");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entrytel");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entryadressee");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        input = lookup_widget(objet_graphique, "entry122");
        gtk_entry_set_text(GTK_ENTRY(input), "");
        
        // Réinitialiser les spins
        input = lookup_widget(objet_graphique, "jour1");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 1);
        
        input = lookup_widget(objet_graphique, "mois1");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 1);
        
        input = lookup_widget(objet_graphique, "annee1");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 2004);
        
        // Réinitialiser les boutons radio à "Femme" par défaut
        input = lookup_widget(objet_graphique, "femme");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(input), TRUE);
        
        input = lookup_widget(objet_graphique, "homme");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(input), FALSE);
        
        // Mettre le focus sur le premier champ
        gtk_widget_grab_focus(lookup_widget(objet_graphique, "entrynom"));
        
    } else {
        strcpy(message_text, "❌ Erreur lors de l'ajout du membre");
        gtk_label_set_text(GTK_LABEL(output), message_text);
    }
}
void
on_annuler_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
// Déclarer deux pointeurs sur GtkWidget
    GtkWidget* input;
    GtkWidget* output;
    
    // Déclarer une chaine de caractères vide
    char vide[2] = "";
    
    // Liez les pointeurs *input et *output aux objets graphiques correspondants
    input = lookup_widget(objet_graphique, "entrynom");
    output = lookup_widget(objet_graphique, "message");
    
    // Vider tous les champs de texte
    input = lookup_widget(objet_graphique, "entrynom");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryprenom");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryid");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryemail");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entrymdp");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entrytel");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryadressee");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entry122");
    gtk_entry_set_text(GTK_ENTRY(input), "");
    
    // Réinitialiser les spins
    input = lookup_widget(objet_graphique, "jour1");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 1);
    
    input = lookup_widget(objet_graphique, "mois1");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 1);
    
    input = lookup_widget(objet_graphique, "annee1");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 2000);
    
    // Réinitialiser les boutons radio
    input = lookup_widget(objet_graphique, "femme");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(input), TRUE);
    
    input = lookup_widget(objet_graphique, "homme");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(input), FALSE);
    sexe_ajout = 0;
    
    // Modifiez le texte de labelMessage
    gtk_label_set_text(GTK_LABEL(output), vide); 
}


void
on_chercher_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
   // Déclarer deux pointeurs sur GtkWidget
    GtkWidget* input;
    GtkWidget* output;
    
    // Déclarer deux chaines de caractères
    char id_recherche[20];
    char message[300];
    
    // Liez les pointeurs *input et *output aux objets graphiques correspondants
    input = lookup_widget(objet_graphique, "entryId");
    output = lookup_widget(objet_graphique, "message2");
    
    // Récupérer le texte saisi
    strcpy(id_recherche, gtk_entry_get_text(GTK_ENTRY(input)));
    
    if (strlen(id_recherche) == 0) {
        strcpy(message, "Veuillez entrer un ID à rechercher");
        gtk_label_set_text(GTK_LABEL(output), message);
        return;
    }
    
    User user_trouve = chercher_user("users.txt", id_recherche);
    
    if (strcmp(user_trouve.id, "-1") == 0) {
        snprintf(message, sizeof(message), "Aucun utilisateur trouvé avec l'ID %s", id_recherche);
        gtk_label_set_text(GTK_LABEL(output), message);
        return;
    }
    
    // Remplir les champs avec les données de l'utilisateur
    input = lookup_widget(objet_graphique, "entrynomn");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.nom);
    
    input = lookup_widget(objet_graphique, "entryprenomn");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.prenom);
    
    input = lookup_widget(objet_graphique, "entryidn");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.id);
    
    input = lookup_widget(objet_graphique, "entryemailn");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.email);
    
    input = lookup_widget(objet_graphique, "entrymdpn");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.mot_de_passe);
    
    input = lookup_widget(objet_graphique, "entryteln");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.telephone);
    
    input = lookup_widget(objet_graphique, "entryadressen");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.adresse);
    
    input = lookup_widget(objet_graphique, "entrypoidsn");
    gtk_entry_set_text(GTK_ENTRY(input), user_trouve.poids);
    
    // Remplir les spins pour la date de naissance
    input = lookup_widget(objet_graphique, "nouvjour1");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), user_trouve.jour_naissance);
    
    input = lookup_widget(objet_graphique, "nouvmois1");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), user_trouve.mois_naissance);
    
    input = lookup_widget(objet_graphique, "nouvannee1");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), user_trouve.annee_naissance);
    
    // Définir le sexe
    input = lookup_widget(objet_graphique, "radiobutton46"); // Femme
    input = lookup_widget(objet_graphique, "radiobutton47"); // Homme
    
    if (user_trouve.sexe == 0) { // Femme
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(objet_graphique, "radiobutton46")), TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(objet_graphique, "radiobutton47")), FALSE);
        sexe_modif = 0;
    } else { // Homme
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(objet_graphique, "radiobutton46")), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(objet_graphique, "radiobutton47")), TRUE);
        sexe_modif = 1;
    }
    
    snprintf(message, sizeof(message), "Utilisateur %s %s trouvé", user_trouve.prenom, user_trouve.nom);
    gtk_label_set_text(GTK_LABEL(output), message);
}


void
on_modifier_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
 // Déclarer deux pointeurs sur GtkWidget
    GtkWidget* input;
    GtkWidget* output;
    
    // Déclarer deux chaines de caractères
    char message[200];
    char succes[200] = "Utilisateur modifié avec succès!";
    
    // Liez les pointeurs *input et *output aux objets graphiques correspondants
    input = lookup_widget(objet_graphique, "entryidn");
    output = lookup_widget(objet_graphique, "message2");
    
    // Récupérer l'ID et vérifier qu'il existe
    const char *id = gtk_entry_get_text(GTK_ENTRY(input));
    User user_existant = chercher_user("users.txt", (char*)id);
    
    if (strcmp(user_existant.id, "-1") == 0) {
        strcpy(message, "Aucun utilisateur trouvé avec cet ID");
        gtk_label_set_text(GTK_LABEL(output), message);
        return;
    }
    
    // Créer la structure user avec les nouvelles données
    User user_modifie;
    strcpy(user_modifie.id, id);
    
    // Récupérer les autres champs
    input = lookup_widget(objet_graphique, "entrynomn");
    strcpy(user_modifie.nom, gtk_entry_get_text(GTK_ENTRY(input)));
    
    input = lookup_widget(objet_graphique, "entryprenomn");
    strcpy(user_modifie.prenom, gtk_entry_get_text(GTK_ENTRY(input)));
    
    input = lookup_widget(objet_graphique, "entryemailn");
    strcpy(user_modifie.email, gtk_entry_get_text(GTK_ENTRY(input)));
    
    input = lookup_widget(objet_graphique, "entrymdpn");
    strcpy(user_modifie.mot_de_passe, gtk_entry_get_text(GTK_ENTRY(input)));
    
    input = lookup_widget(objet_graphique, "entryteln");
    strcpy(user_modifie.telephone, gtk_entry_get_text(GTK_ENTRY(input)));
    
    input = lookup_widget(objet_graphique, "entryadressen");
    strcpy(user_modifie.adresse, gtk_entry_get_text(GTK_ENTRY(input)));
    
    input = lookup_widget(objet_graphique, "entrypoidsn");
    strcpy(user_modifie.poids, gtk_entry_get_text(GTK_ENTRY(input)));
    
    // Récupérer les spins pour la date de naissance
    input = lookup_widget(objet_graphique, "nouvjour1");
    user_modifie.jour_naissance = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(input));
    
    input = lookup_widget(objet_graphique, "nouvmois1");
    user_modifie.mois_naissance = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(input));
    
    input = lookup_widget(objet_graphique, "nouvannee1");
    user_modifie.annee_naissance = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(input));
    
    // Déterminer le sexe à partir de la variable globale
    user_modifie.sexe = sexe_modif;
    
    // Sauvegarder les modifications
    if (modifier_user("users.txt", (char*)id, user_modifie)) {
        // Modifier le texte du label
        gtk_label_set_text(GTK_LABEL(output), succes);
    } else {
        strcpy(message, "Erreur lors de la modification de l'utilisateur");
        gtk_label_set_text(GTK_LABEL(output), message);
    }

}


void
on_annulern_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    // Déclarer deux pointeurs sur GtkWidget
    GtkWidget* input;
    GtkWidget* output;
    
    // Déclarer une chaine de caractères vide
    char vide[2] = "";
    
    // Liez le pointeur *output au label de message
    output = lookup_widget(objet_graphique, "message2");
    
    // Vider tous les champs de texte de la section modification
    input = lookup_widget(objet_graphique, "entryidn");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entrynomn");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryprenomn");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryemailn");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entrymdpn");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryteln");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entryadressen");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    input = lookup_widget(objet_graphique, "entrypoidsn");
    if (input) gtk_entry_set_text(GTK_ENTRY(input), "");
    
    // Réinitialiser les spins pour la date de naissance
    input = lookup_widget(objet_graphique, "nouvjour1");
    if (input) gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 1);
    
    input = lookup_widget(objet_graphique, "nouvmois1");
    if (input) gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 1);
    
    input = lookup_widget(objet_graphique, "nouvannee1");
    if (input) gtk_spin_button_set_value(GTK_SPIN_BUTTON(input), 2000);
    
    // Réinitialiser les boutons radio pour le sexe
    input = lookup_widget(objet_graphique, "nouvfemme");
    if (input) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(input), TRUE);
    
    input = lookup_widget(objet_graphique, "nouvhomme");
    if (input) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(input), FALSE);
    
    // Réinitialiser la variable globale pour le sexe (si vous l'utilisez)
    // sexe_modif = 0; // Décommentez si vous avez cette variable globale
    
    // Modifiez le texte du label de message
    if (output) gtk_label_set_text(GTK_LABEL(output), vide);
    
    // Optionnel: Remettre le focus sur le premier champ
    input = lookup_widget(objet_graphique, "entryidn");
    if (input) gtk_widget_grab_focus(input);
}
void
on_cherche_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
GtkWidget* input = lookup_widget(objet_graphique, "entryIDD");
    GtkWidget* output = lookup_widget(objet_graphique, "message3");
    
    char id[20];
    char message[500];
    
    strcpy(id, gtk_entry_get_text(GTK_ENTRY(input)));
    
    if (strlen(id) == 0) {
        strcpy(message, "Veuillez entrer un ID à rechercher");
        gtk_label_set_text(GTK_LABEL(output), message);
        return;
    }
    
    User user_trouve = chercher_user("users.txt", id);
    
    if (strcmp(user_trouve.id, "-1") == 0) {
        snprintf(message, sizeof(message), "Aucun utilisateur trouvé avec l'ID %s", id);
        gtk_label_set_text(GTK_LABEL(output), message);
    } else {
        snprintf(message, sizeof(message), 
                "Utilisateur trouvé: %s %s\nEmail: %s\nTéléphone: %s",
                user_trouve.prenom, user_trouve.nom, user_trouve.email, user_trouve.telephone);
        gtk_label_set_text(GTK_LABEL(output), message);
    }
}


void
on_recherche_clicked                   (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{

    // Récupérer les widgets
    GtkWidget *entry = lookup_widget(objet_graphique, "entryidd");
    GtkWidget *treeview = lookup_widget(objet_graphique, "treeview4");
    
    // Variables pour le TreeView
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    GtkTreeIter iter;
    
    // Variables pour la recherche
    char id[100];
    char message[500];
    char date_naissance[50];
    char sexe_str[10];
    char *id_ptr;  // Pointeur non-const pour appeler chercher_user
    
    // Récupérer l'ID saisi
    strcpy(id, gtk_entry_get_text(GTK_ENTRY(entry)));
    id_ptr = id;  // Conversion implicite de char[] à char*
    
    if (strlen(id) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(objet_graphique)),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Veuillez entrer un ID à rechercher");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Chercher l'utilisateur (id_ptr est char*, pas const char*)
    User user_trouve = chercher_user("users.txt", id_ptr);
    
    if (strcmp(user_trouve.id, "-1") == 0) {
        char msg_dialog[200];
        snprintf(msg_dialog, sizeof(msg_dialog), 
                 "Aucun utilisateur trouvé avec l'ID %s", id);
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(objet_graphique)),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", msg_dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Formater la date de naissance
    snprintf(date_naissance, sizeof(date_naissance), "%02d/%02d/%04d", 
             user_trouve.jour_naissance, user_trouve.mois_naissance, user_trouve.annee_naissance);
    
    // Formater le sexe
    strcpy(sexe_str, (user_trouve.sexe == 0) ? "Femme" : "Homme");
    
    // Créer le modèle de données (ListStore)
    store = gtk_list_store_new(8, 
        G_TYPE_STRING,  // ID
        G_TYPE_STRING,  // Nom
        G_TYPE_STRING,  // Prénom
        G_TYPE_STRING,  // Email
        G_TYPE_STRING,  // Téléphone
        G_TYPE_STRING,  // Date de naissance
        G_TYPE_STRING,  // Sexe
        G_TYPE_STRING   // Poids
    );
    
    // Ajouter l'utilisateur au modèle
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
        0, user_trouve.id,
        1, user_trouve.nom,
        2, user_trouve.prenom,
        3, user_trouve.email,
        4, user_trouve.telephone,
        5, date_naissance,
        6, sexe_str,
        7, user_trouve.poids,
        -1);
    
    // Vérifier si le TreeView a déjà des colonnes
    // On utilise gtk_tree_view_get_columns et on compte le nombre
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    int num_columns = 0;
    GList *iter_list = columns;
    while (iter_list != NULL) {
        num_columns++;
        iter_list = iter_list->next;
    }
    g_list_free(columns);
    
    if (num_columns == 0) {
        // Colonne ID
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Nom
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Prénom
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Prénom", renderer, "text", 2, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Email
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Email", renderer, "text", 3, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Téléphone
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Téléphone", renderer, "text", 4, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Date de naissance
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Date Naiss.", renderer, "text", 5, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Sexe
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Sexe", renderer, "text", 6, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Poids
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Poids", renderer, "text", 7, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    }
    
    // Associer le modèle au TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    
    // Libérer la référence du modèle (le TreeView en garde une)
    g_object_unref(store);
    
    // Afficher un message de confirmation
    snprintf(message, sizeof(message), 
             "Membre trouvé : %s %s (ID: %s) affiché dans la liste",
             user_trouve.prenom, user_trouve.nom, user_trouve.id);
    
    // Optionnel : afficher un message dans une boîte de dialogue
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(gtk_widget_get_toplevel(objet_graphique)),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void
on_treeview4_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{

    GtkTreeIter iter;
    gchar *id;
    gchar *nom;
    gchar *prenom;
    gchar *email;
    gchar *telephone;
    gchar *date_naissance;
    gchar *sexe;
    gchar *poids;
    
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        // Obtenir les valeurs de la ligne sélectionnée
        gtk_tree_model_get(model, &iter,
            0, &id,
            1, &nom,
            2, &prenom,
            3, &email,
            4, &telephone,
            5, &date_naissance,
            6, &sexe,
            7, &poids,
            -1);
        
        // Demander confirmation avant suppression
        char confirmation[500];
        snprintf(confirmation, sizeof(confirmation),
                 "Voulez-vous vraiment supprimer ce membre ?\n\n"
                 "ID : %s\n"
                 "Nom : %s\n"
                 "Prénom : %s\n"
                 "Email : %s\n"
                 "Téléphone : %s\n"
                 "Date de naissance : %s\n"
                 "Sexe : %s\n"
                 "Poids : %s",
                 id, nom, prenom, email, telephone, 
                 date_naissance, sexe, poids);
        
        GtkWidget *confirm_dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(treeview))),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "%s", confirmation);
        
        gtk_window_set_title(GTK_WINDOW(confirm_dialog), "Confirmer la suppression");
        
        gint response = gtk_dialog_run(GTK_DIALOG(confirm_dialog));
        gtk_widget_destroy(confirm_dialog);
        
        if (response == GTK_RESPONSE_YES) {
            // Supprimer le membre du fichier
            if (supprimer_user("users.txt", id)) {
                // Supprimer la ligne du TreeView
                GtkListStore *store = GTK_LIST_STORE(model);
                gtk_list_store_remove(store, &iter);
                
                // Afficher message de succès
                char success_msg[300];
                snprintf(success_msg, sizeof(success_msg),
                         "✅ Membre %s %s supprimé avec succès !\n"
                         "ID : %s\n"
                         "Nom : %s %s",
                         prenom, nom, id, prenom, nom);
                
                GtkWidget *success_dialog = gtk_message_dialog_new(
                    GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(treeview))),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "%s", success_msg);
                
                gtk_window_set_title(GTK_WINDOW(success_dialog), "Suppression réussie");
                gtk_dialog_run(GTK_DIALOG(success_dialog));
                gtk_widget_destroy(success_dialog);
            } else {
                // Erreur lors de la suppression
                char error_msg[200];
                snprintf(error_msg, sizeof(error_msg),
                         "❌ Erreur lors de la suppression du membre %s %s",
                         prenom, nom);
                
                GtkWidget *error_dialog = gtk_message_dialog_new(
                    GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(treeview))),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "%s", error_msg);
                
                gtk_window_set_title(GTK_WINDOW(error_dialog), "Erreur de suppression");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }
        } else {
            // Annulation de la suppression
            char cancel_msg[200];
            snprintf(cancel_msg, sizeof(cancel_msg),
                     "Suppression annulée pour %s %s",
                     prenom, nom);
            
            GtkWidget *cancel_dialog = gtk_message_dialog_new(
                GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(treeview))),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "%s", cancel_msg);
            
            gtk_window_set_title(GTK_WINDOW(cancel_dialog), "Suppression annulée");
            gtk_dialog_run(GTK_DIALOG(cancel_dialog));
            gtk_widget_destroy(cancel_dialog);
        }
        
        // Libérer la mémoire allouée pour les chaînes
        g_free(id);
        g_free(nom);
        g_free(prenom);
        g_free(email);
        g_free(telephone);
        g_free(date_naissance);
        g_free(sexe);
        g_free(poids);
    }
}


void
on_afficher_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    // Récupérer le widget TreeView
    GtkWidget *treeview = lookup_widget(objet_graphique, "treeview4");
    GtkWidget *output = lookup_widget(objet_graphique, "message3");
    
    // Variables pour le TreeView
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    GtkTreeIter iter;
    
    char message[500];
    
    // Vérifier si le TreeView a déjà des colonnes
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    int num_columns = 0;
    GList *iter_list = columns;
    while (iter_list != NULL) {
        num_columns++;
        iter_list = iter_list->next;
    }
    g_list_free(columns);
    
    // Si pas de colonnes, les créer
    if (num_columns == 0) {
        // Colonne ID
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 50);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Nom
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 100);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Prénom
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Prénom", renderer, "text", 2, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 100);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Email
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Email", renderer, "text", 3, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 150);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Téléphone
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Téléphone", renderer, "text", 4, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 100);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Date de naissance
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Date Naiss.", renderer, "text", 5, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 100);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Sexe
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Sexe", renderer, "text", 6, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 70);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        // Colonne Poids
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Poids", renderer, "text", 7, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 70);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    }
    
    // Créer le modèle de données (ListStore)
    store = gtk_list_store_new(8, 
        G_TYPE_STRING,  // ID
        G_TYPE_STRING,  // Nom
        G_TYPE_STRING,  // Prénom
        G_TYPE_STRING,  // Email
        G_TYPE_STRING,  // Téléphone
        G_TYPE_STRING,  // Date de naissance
        G_TYPE_STRING,  // Sexe
        G_TYPE_STRING   // Poids
    );
    
    // Lire le fichier users.txt
    FILE *f = fopen("users.txt", "r");
    if (f == NULL) {
        snprintf(message, sizeof(message), "❌ Fichier users.txt introuvable!");
        gtk_label_set_text(GTK_LABEL(output), message);
        g_object_unref(store);
        return;
    }
    
    User user;
    int count = 0;
    
    while (fscanf(f, "%s %s %s %s %s %s %s %d %d %d %d %s\n",
                 user.id, user.nom, user.prenom, user.email, user.mot_de_passe,
                 user.telephone, user.adresse, &user.jour_naissance, &user.mois_naissance,
                 &user.annee_naissance, &user.sexe, user.poids) != EOF) {
        
        // Formater la date de naissance
        char date_naissance[50];
        snprintf(date_naissance, sizeof(date_naissance), "%02d/%02d/%04d", 
                 user.jour_naissance, user.mois_naissance, user.annee_naissance);
        
        // Formater le sexe
        char sexe_str[10];
        strcpy(sexe_str, (user.sexe == 0) ? "Femme" : "Homme");
        
        // Ajouter l'utilisateur au modèle
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, user.id,
            1, user.nom,
            2, user.prenom,
            3, user.email,
            4, user.telephone,
            5, date_naissance,
            6, sexe_str,
            7, user.poids,
            -1);
        
        count++;
    }
    fclose(f);
    
    // Associer le modèle au TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    
    // Libérer la référence du modèle (le TreeView en garde une)
    g_object_unref(store);
    
    // Afficher un message de confirmation
    if (count > 0) {
        snprintf(message, sizeof(message), "✅ %d membre(s) affiché(s) dans la liste", count);
    } else {
        snprintf(message, sizeof(message), "ℹ️ Aucun membre dans la base de données");
    }
    
    gtk_label_set_text(GTK_LABEL(output), message);
    
    // Optionnel : Afficher aussi dans une boîte de dialogue
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(gtk_widget_get_toplevel(objet_graphique)),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void
on_reserver_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{

    // Déclarer les pointeurs sur GtkWidget
    GtkWidget* output;
    
    // Liez le pointeur *output au label sortie
    output = lookup_widget(objet_graphique, "sortie");
    
    // Récupérer tous les widgets nécessaires
    GtkWidget *entryidcoach = lookup_widget(objet_graphique, "entryidcoach");
    GtkWidget *checkbuttonperte = lookup_widget(objet_graphique, "checkbuttonperte");
    GtkWidget *checkbuttonprise = lookup_widget(objet_graphique, "checkbuttonprise");
    GtkWidget *checkbuttonreeduction = lookup_widget(objet_graphique, "checkbuttonreeduction");
    GtkWidget *checkbuttonamelioration = lookup_widget(objet_graphique, "checkbuttonamelioration");
    GtkWidget *checkbuttongymnastique = lookup_widget(objet_graphique, "checkbuttongymnastique");
    
    // REMPLACEMENT des radio boutons par un ComboBox
    GtkWidget *combobox_disponibilite = lookup_widget(objet_graphique, "combobox_disponibilite");
    
    GtkWidget *checkbuttonlundi = lookup_widget(objet_graphique, "checkbuttonlundi");
    GtkWidget *checkbuttonmardi = lookup_widget(objet_graphique, "checkbuttonmardi");
    GtkWidget *checkbuttonmercredi = lookup_widget(objet_graphique, "checkbuttonmercredi");
    GtkWidget *checkbuttonjeudi = lookup_widget(objet_graphique, "checkbuttonjeudi");
    GtkWidget *checkbuttonvendredi = lookup_widget(objet_graphique, "checkbuttonvendredi");

    // Créer et remplir la structure DemandeCoach
    DemandeCoach demande;
    
    // Initialiser les tableaux
    int i;
    for(i = 0; i < 5; i++) {
        demande.objectifs[i] = 0;
        demande.jours[i] = 0;
    }
    
    // Récupérer l'ID du membre
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entryidcoach));
    
    // Debug: Afficher l'ID saisi
    printf("DEBUG: ID saisi: '%s' (longueur: %ld)\n", id_text, strlen(id_text));
    
    if(strlen(id_text) == 0) {
        gtk_label_set_text(GTK_LABEL(output), "❌ Veuillez entrer un ID de membre!");
        return;
    }
    
    // Copier l'ID dans la structure
    strncpy(demande.id_membre, id_text, sizeof(demande.id_membre) - 1);
    demande.id_membre[sizeof(demande.id_membre) - 1] = '\0';
    
    // Nettoyer l'ID (supprimer les espaces)
    char id_clean[100];
    strncpy(id_clean, demande.id_membre, sizeof(id_clean) - 1);
    id_clean[sizeof(id_clean) - 1] = '\0';
    
    // Supprimer les espaces de début et fin
    int start = 0, end = strlen(id_clean) - 1;
    while (start <= end && (id_clean[start] == ' ' || id_clean[start] == '\t')) start++;
    while (end >= start && (id_clean[end] == ' ' || id_clean[end] == '\t' || id_clean[end] == '\n')) end--;
    id_clean[end + 1] = '\0';
    if (start > 0) memmove(id_clean, id_clean + start, strlen(id_clean) - start + 1);
    
    strcpy(demande.id_membre, id_clean);
    printf("DEBUG: ID nettoyé: '%s'\n", demande.id_membre);
    
    // Vérifier si le membre existe dans users.txt
    User membre = chercher_user("users.txt", demande.id_membre);
    
    // Debug: Afficher le résultat de la recherche
    printf("DEBUG: Résultat recherche - ID retourné: '%s'\n", membre.id);
    
    if(strcmp(membre.id, "-1") == 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "❌ Aucun membre trouvé avec l'ID '%s'!\n", demande.id_membre);
        gtk_label_set_text(GTK_LABEL(output), error_msg);
        return;
    }
    
    // Afficher les infos du membre trouvé (debug)
    printf("DEBUG: Membre trouvé:\n");
    printf("  ID: %s\n", membre.id);
    printf("  Nom: %s\n", membre.nom);
    printf("  Prénom: %s\n", membre.prenom);
    
    // Récupérer les objectifs (checkbuttons)
    demande.objectifs[0] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonperte)) ? 1 : 0;
    demande.objectifs[1] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonprise)) ? 1 : 0;
    demande.objectifs[2] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonreeduction)) ? 1 : 0;
    demande.objectifs[3] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonamelioration)) ? 1 : 0;
    demande.objectifs[4] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttongymnastique)) ? 1 : 0;
    
    // Récupérer la disponibilité depuis le ComboBox
    gint selected_index = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox_disponibilite));
    
    if(selected_index == -1) {
        gtk_label_set_text(GTK_LABEL(output), "❌ Veuillez sélectionner une disponibilité!");
        return;
    }
    
    // Affecter la valeur correspondante
    // 0: Matin, 1: Midi, 2: Soir
    demande.disponibilite = selected_index;
    
    // Récupérer les jours (checkbuttons)
    demande.jours[0] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonlundi)) ? 1 : 0;
    demande.jours[1] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonmardi)) ? 1 : 0;
    demande.jours[2] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonmercredi)) ? 1 : 0;
    demande.jours[3] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonjeudi)) ? 1 : 0;
    demande.jours[4] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonvendredi)) ? 1 : 0;
    
    // Vérifier qu'au moins un jour est sélectionné
    int jours_selectionnes = 0;
    for(i = 0; i < 5; i++) {
        if(demande.jours[i]) jours_selectionnes = 1;
    }
    
    if(!jours_selectionnes) {
        gtk_label_set_text(GTK_LABEL(output), "❌ Veuillez sélectionner au moins un jour d'entraînement!");
        return;
    }
    
    // Vérifier qu'au moins un objectif est sélectionné
    int objectifs_selectionnes = 0;
    for(i = 0; i < 5; i++) {
        if(demande.objectifs[i]) objectifs_selectionnes = 1;
    }
    
    if(!objectifs_selectionnes) {
        gtk_label_set_text(GTK_LABEL(output), "❌ Veuillez sélectionner au moins un objectif!");
        return;
    }
    
    // Enregistrer la demande dans reservation.txt
    if(enregistrer_demande_coach(demande)) {
        char success_msg[256];
        snprintf(success_msg, sizeof(success_msg), 
                "✅ Demande enregistrée pour %s %s!\n", 
                membre.prenom, membre.nom);
        gtk_label_set_text(GTK_LABEL(output), success_msg);
        
        // Réinitialiser le formulaire
        gtk_entry_set_text(GTK_ENTRY(entryidcoach), "");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonperte), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonprise), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonreeduction), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonamelioration), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttongymnastique), FALSE);
        
        // Réinitialiser le ComboBox (sélectionner le premier élément ou aucun)
        gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_disponibilite), -1); // Aucune sélection
        // OU gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_disponibilite), 0); // Sélectionner "Matin"
        
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonlundi), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonmardi), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonmercredi), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonjeudi), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonvendredi), FALSE);
    } else {
        gtk_label_set_text(GTK_LABEL(output), "❌ Erreur lors de l'enregistrement de la demande!");
    }

}





void
on_retour_clicked                      (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{

    g_print("DEBUG: Retour à la fenêtre demande coach\n");
    
    // Récupérer la fenêtre actuelle (réservation)
    GtkWidget *reservation_window = gtk_widget_get_toplevel(objet_graphique);
    
    if (reservation_window == NULL) {
        g_print("ERREUR: Impossible de récupérer la fenêtre réservation\n");
        return;
    }
    
    // Fermer/détruire la fenêtre réservation
    gtk_widget_destroy(reservation_window);
    
    // Créer et afficher une nouvelle fenêtre demande coach
    GtkWidget *demande_coach_window = create_demande_coach();
    
    if (demande_coach_window != NULL) {
        // Positionner la fenêtre au centre
        gtk_window_set_position(GTK_WINDOW(demande_coach_window), GTK_WIN_POS_CENTER);
        
        // Afficher la fenêtre
        gtk_widget_show_all(demande_coach_window);
        
        // Amener la fenêtre au premier plan
        gtk_window_present(GTK_WINDOW(demande_coach_window));
    } else {
        g_print("ERREUR: Impossible de créer la fenêtre demande_coach\n");
    }
}
void
on_treeview5_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)

{

    // Obtenir le modèle
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    if (model == NULL) return;
    
    // Obtenir l'itérateur pour la ligne sélectionnée
    GtkTreeIter iter;
    if (!gtk_tree_model_get_iter(model, &iter, path)) return;
    
    // Obtenir l'index de ligne (stocké dans la colonne 4)
    gint ligne_index;
    gtk_tree_model_get(model, &iter, 4, &ligne_index, -1);
    
    // Si c'est -1, c'est la ligne "Aucune réservation"
    if (ligne_index == -1) {
        g_print("Aucune réservation à supprimer.\n");
        return;
    }
    
    // Demander confirmation
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Voulez-vous vraiment supprimer cette réservation ?");
    
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirmation de suppression");
    
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        // Supprimer la réservation
        if (supprimer_reservation_par_ligne("reservation.txt", ligne_index)) {
            g_print("Réservation supprimée avec succès.\n");
            
            // Réafficher les réservations
            // On obtient la fenêtre parente pour pouvoir rappeler on_affiche_clicked
            GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(treeview));
            if (window != NULL) {
                on_affiche_clicked(window, NULL);
            }
        } else {
            g_print("Erreur lors de la suppression de la réservation.\n");
            
            // Afficher un message d'erreur
            GtkWidget *error_dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Erreur lors de la suppression de la réservation.");
            gtk_window_set_title(GTK_WINDOW(error_dialog), "Erreur");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    } else {
        g_print("Suppression annulée.\n");
    }
}
// Fonction pour convertir les objectifs en chaîne lisible
char* objectifs_vers_texte(int objectifs[5]) {
    static char texte[256];
    texte[0] = '\0';
    
    const char *noms_objectifs[] = {"Cardio", "Dance", "Musculation", "Kick boxing", "Gymnastique"};
    
    int premier = 1;
    for (int i = 0; i < 5; i++) {
        if (objectifs[i]) {
            if (!premier) strcat(texte, ", ");
            strcat(texte, noms_objectifs[i]);
            premier = 0;
        }
    }
    
    if (texte[0] == '\0') {
        strcpy(texte, "Aucun");
    }
    
    return texte;
}

// Fonction pour convertir les jours en chaîne lisible
char* jours_vers_texte(int jours[5]) {
    static char texte[256];
    texte[0] = '\0';
    
    const char *noms_jours[] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi"};
    
    int premier = 1;
    for (int i = 0; i < 5; i++) {
        if (jours[i]) {
            if (!premier) strcat(texte, ", ");
            strcat(texte, noms_jours[i]);
            premier = 0;
        }
    }
    
    if (texte[0] == '\0') {
        strcpy(texte, "Aucun");
    }
    
    return texte;
}

// Fonction pour convertir la disponibilité en texte
const char* disponibilite_vers_texte(int disponibilite) {
    switch(disponibilite) {
        case 0: return "Matin";
        case 1: return "Midi";
        case 2: return "Soir";
        default: return "Inconnue";
    }
}


void on_voir_clicked(GtkWidget *objet_graphique, gpointer user_data)
{
    g_print("DEBUG: Ouverture de la fenêtre réservation (mode éditable)\n");
    
    // Obtenir la fenêtre parente du bouton
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(objet_graphique));
    
    // Cacher la fenêtre actuelle
    gtk_widget_hide(parent_window);
    
    // Créer et afficher la fenêtre de réservation
    GtkWidget *reservation_window = create_reservation();
    
    // Récupérer le treeview5
    GtkWidget *treeview5 = lookup_widget(reservation_window, "treeview5");
    
    if (treeview5 != NULL) {
        // Initialiser et remplir le treeview avec les réservations (mode éditable)
        initialiser_treeview_reservation_editable(treeview5);
    } else {
        g_print("ERREUR: treeview5 non trouvé dans la fenêtre!\n");
    }
    
    gtk_widget_show_all(reservation_window);
}
void
on_affiche_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    // Récupérer le treeview
    GtkWidget *treeview5 = lookup_widget(objet_graphique, "treeview5");
    
    if (treeview5 == NULL) {
        g_print("ERREUR: treeview5 non trouvé!\n");
        return;
    }
    
    // Vider le treeview existant
    GtkTreeModel *old_model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview5));
    if (old_model != NULL) {
        g_object_unref(old_model);
    }
    
    // Créer un nouveau modèle de liste
    GtkListStore *liststore = gtk_list_store_new(5, 
        G_TYPE_STRING,    // ID membre
        G_TYPE_STRING,    // Objectifs
        G_TYPE_STRING,    // Disponibilité
        G_TYPE_STRING,    // Jours
        G_TYPE_INT        // Index ligne (caché)
    );
    
    // Lire les demandes depuis le fichier
    int count = 0;
    DemandeCoach *demandes = lire_toutes_demandes("reservation.txt", &count);
    
    if (demandes == NULL || count == 0) {
        g_print("Aucune réservation trouvée dans le fichier.\n");
        
        // Ajouter une ligne "vide" pour indiquer qu'il n'y a pas de réservations
        GtkTreeIter iter;
        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter,
            0, "Aucune réservation",
            1, "",
            2, "",
            3, "",
            4, -1,
            -1);
    } else {
        // Ajouter chaque demande au modèle
        for (int i = 0; i < count; i++) {
            GtkTreeIter iter;
            gtk_list_store_append(liststore, &iter);
            
            // Chercher les infos du membre
            User membre = chercher_user("users.txt", demandes[i].id_membre);
            char membre_info[256];
            
            if (strcmp(membre.id, "-1") == 0) {
                snprintf(membre_info, sizeof(membre_info), "ID: %s (Membre inconnu)", demandes[i].id_membre);
            } else {
                snprintf(membre_info, sizeof(membre_info), "ID: %s - %s %s", 
                        demandes[i].id_membre, membre.prenom, membre.nom);
            }
            
            gtk_list_store_set(liststore, &iter,
                0, membre_info,
                1, objectifs_vers_texte(demandes[i].objectifs),
                2, disponibilite_vers_texte(demandes[i].disponibilite),
                3, jours_vers_texte(demandes[i].jours),
                4, i,  // Stocker l'index pour la suppression
                -1);
        }
        
        free(demandes);
    }
    
    // Configurer le treeview avec le modèle
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview5), GTK_TREE_MODEL(liststore));
    
    // Ajouter les colonnes (si elles n'existent pas déjà)
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview5)) == NULL) {
        // Colonne 1: Membre
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            "Membre", renderer, "text", 0, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 200);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
        
        // Colonne 2: Objectifs
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(
            "Objectifs", renderer, "text", 1, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 150);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
        
        // Colonne 3: Disponibilité
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(
            "Disponibilité", renderer, "text", 2, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 100);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
        
        // Colonne 4: Jours
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(
            "Jours", renderer, "text", 3, NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_min_width(column, 150);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    }
    
    g_print("Affichage des réservations terminé (%d réservations)\n", count);

}

void
on_demande_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    
    // Récupérer la fenêtre actuelle (acceuilmembre)
    GtkWidget *acceuilmembre_window = gtk_widget_get_toplevel(objet_graphique);
    
    // Cacher la fenêtre actuelle
    gtk_widget_hide(acceuilmembre_window);
    
    // Créer la fenêtre coach
    GtkWidget *demande_coach_window = create_demande_coach();
    
    // Stocker la référence à la fenêtre précédente dans la nouvelle fenêtre
    g_object_set_data(G_OBJECT(demande_coach_window), "acceuilmembre_window", acceuilmembre_window);
    
    // Afficher la fenêtre coach
    gtk_widget_show_all(demande_coach_window);

}
void
on_membre_clicked                      (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{

    // Obtenir la fenêtre parente du bouton
    GtkWidget *parent_window = gtk_widget_get_toplevel(objet_graphique);
    
    // Cacher la fenêtre actuelle
    gtk_widget_hide(parent_window);
    
    // Créer et afficher la fenêtre de gestion des membres
    GtkWidget *Gestion_membres = create_Gestion_membres();
    gtk_widget_show_all(Gestion_membres);
}

// Variables d'authentification
char utilisateur_connecte_id[50] = "";
char utilisateur_type[20] = ""; // "admin" ou "membre"
int verifier_admin(const char *id, const char *mdp) {
    FILE *f = fopen("admin.txt", "r");
    if (f == NULL) {
        printf("Erreur: fichier admin.txt non trouvé\n");
        return 0;
    }
    
    char ligne[200];
    char id_fichier[50], mdp_fichier[100];
    int trouve = 0;
    
    while (fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\n")] = 0;
        
        if (sscanf(ligne, "%s %s", id_fichier, mdp_fichier) == 2) {
            if (strcmp(id_fichier, id) == 0 && strcmp(mdp_fichier, mdp) == 0) {
                trouve = 1;
                break;
            }
        }
    }
    
    fclose(f);
    return trouve;
}
int verifier_entraineur(const char *id, const char *mdp) {
    FILE *fichier = fopen("entraineurs.txt", "r");
    if (!fichier) {
        printf("Erreur: fichier entraineurs.txt introuvable\n");
        return 0;
    }
    
    char ligne[256];
    int trouve = 0;
    
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Supprimer le saut de ligne
        ligne[strcspn(ligne, "\n")] = 0;
        
        // Séparer l'ID et le mot de passe (format: id:mdp)
        char *token_id = strtok(ligne, ":");
        char *token_mdp = strtok(NULL, ":");
        
        if (token_id && token_mdp) {
            if (strcmp(token_id, id) == 0 && strcmp(token_mdp, mdp) == 0) {
                trouve = 1;
                break;
            }
        }
    }
    
    fclose(fichier);
    return trouve;
}
int verifier_membre(const char *id, const char *mdp) {
    FILE *f = fopen("users.txt", "r");
    if (f == NULL) {
        printf("Erreur: fichier users.txt non trouvé\n");
        return 0;
    }
    
    // Nettoyer les chaînes d'entrée
    char id_clean[100];
    char mdp_clean[100];
    strncpy(id_clean, id, sizeof(id_clean) - 1);
    strncpy(mdp_clean, mdp, sizeof(mdp_clean) - 1);
    id_clean[sizeof(id_clean) - 1] = '\0';
    mdp_clean[sizeof(mdp_clean) - 1] = '\0';
    
    // Supprimer les espaces
    char *start = id_clean;
    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = '\0';
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n')) start++;
    if (start != id_clean) memmove(id_clean, start, strlen(start) + 1);
    
    start = mdp_clean;
    end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = '\0';
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n')) start++;
    if (start != mdp_clean) memmove(mdp_clean, start, strlen(start) + 1);
    
    printf("DEBUG verifier_membre: Recherche ID='%s', MDP='%s'\n", id_clean, mdp_clean);
    
    char ligne[1024];
    int trouve = 0;
    int ligne_num = 0;
    
    while (fgets(ligne, sizeof(ligne), f)) {
        ligne_num++;
        ligne[strcspn(ligne, "\n")] = 0;
        ligne[strcspn(ligne, "\r")] = 0;
        
        // Nettoyer la ligne
        char *line_start = ligne;
        while (*line_start == ' ' || *line_start == '\t') line_start++;
        char *line_end = line_start + strlen(line_start) - 1;
        while (line_end > line_start && (*line_end == ' ' || *line_end == '\t')) line_end--;
        *(line_end + 1) = '\0';
        
        printf("DEBUG verifier_membre: Ligne %d: %s\n", ligne_num, line_start);
        
        // Utiliser un parsing flexible
        char id_fichier[100] = "";
        char mdp_fichier[100] = "";
        
        // Essayer de lire au moins l'ID et le mot de passe
        // Le format est: ID Nom Prenom Email MotDePasse ...
        if (sscanf(line_start, "%s %*s %*s %*s %s", id_fichier, mdp_fichier) >= 2) {
            printf("DEBUG verifier_membre: ID lu='%s', MDP lu='%s'\n", id_fichier, mdp_fichier);
            
            if (strcmp(id_fichier, id_clean) == 0 && strcmp(mdp_fichier, mdp_clean) == 0) {
                trouve = 1;
                printf("DEBUG verifier_membre: Correspondance trouvée!\n");
                break;
            }
        } else {
            printf("DEBUG verifier_membre: Échec du parsing de la ligne\n");
        }
    }
    
    fclose(f);
    
    if (!trouve) {
        printf("DEBUG verifier_membre: Aucune correspondance trouvée\n");
    }
    
    return trouve;
}
void
on_valider_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(objet_graphique);
    // Récupérer les champs d'authentification
    GtkWidget *entry_id = lookup_widget(window, "entry21");
    GtkWidget *entry_mdp = lookup_widget(window, "entry20");
    GtkWidget *label_message = lookup_widget(window, "label55");
    GtkWidget *eye_button = lookup_widget(window, "eye_button_auth");
    
    if (!entry_id || !entry_mdp || !label_message) {
        printf("Erreur: widgets non trouvés dans l'authentification\n");
        return;
    }
    
    // Récupérer les valeurs
    const char *id = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *mdp = gtk_entry_get_text(GTK_ENTRY(entry_mdp));
    
    // Validation
    if (strlen(id) == 0 || strlen(mdp) == 0) {
        gtk_label_set_text(GTK_LABEL(label_message), "Veuillez remplir tous les champs");
        return;
    }
    
    // Vérifier si c'est un admin
    if (verifier_admin(id, mdp)) {
        strcpy(utilisateur_connecte_id, id);
        strcpy(utilisateur_type, "admin");
        printf("Connexion réussie en tant qu'admin: %s\n", id);
        
        // Réinitialiser l'œil avant de fermer
        oeil_auth_visible = 0;
        
        // Fermer la fenêtre d'authentification
        gtk_widget_destroy(window);
        
        // Ouvrir l'interface admin
        GtkWidget *acceuiladmin = create_acceuiladmin();
        gtk_widget_show_all(acceuiladmin);
        return;
    }
    
    // VÉRIFICATION AJOUTÉE : Vérifier si c'est un entraîneur
    if (verifier_entraineur(id, mdp)) {
        strcpy(utilisateur_connecte_id, id);
        strcpy(utilisateur_type, "entraineur");
        
        printf("Connexion réussie en tant qu'entraîneur: %s\n", id);
        
        // Réinitialiser l'œil avant de fermer
        oeil_auth_visible = 0;
        
        // Fermer la fenêtre d'authentification
        gtk_widget_destroy(window);
        
        // Ouvrir l'interface entraîneur
        GtkWidget *acceuilentraineur = create_acceuilentraineur();
        gtk_widget_show_all(acceuilentraineur);
        return;
    }
    
    // Vérifier si c'est un membre
    if (verifier_membre(id, mdp)) {
        strcpy(utilisateur_connecte_id, id);
        strcpy(utilisateur_type, "membre");
        
        printf("Connexion réussie en tant que membre: %s\n", id);
        
        // Réinitialiser l'œil avant de fermer
        oeil_auth_visible = 0;
        
        // Fermer la fenêtre d'authentification
        gtk_widget_destroy(window);
        
        // Ouvrir l'interface membre
        GtkWidget *acceuilmembre = create_acceuilmembre();
        gtk_widget_show_all(acceuilmembre);
        return;
    }
    
    // Si on arrive ici, les identifiants sont incorrects
    gtk_label_set_text(GTK_LABEL(label_message), "ID ou mot de passe incorrect");
    
    // Vider le champ mot de passe et remettre le focus sur ID
    gtk_entry_set_text(GTK_ENTRY(entry_mdp), "");
    gtk_widget_grab_focus(entry_id);
}

// Fonctions utilitaires d'authentification
const char* get_utilisateur_connecte_id(void) {
    return utilisateur_connecte_id;
}

const char* get_utilisateur_type(void) {
    return utilisateur_type;
}

int est_connecte(void) {
    return strlen(utilisateur_connecte_id) > 0;
}
// Fonction pour initialiser le TreeView avec édition directe
void initialiser_treeview_reservation_editable(GtkWidget *treeview5)
{
    if (treeview5 == NULL) {
        g_print("ERREUR: treeview5 est NULL!\n");
        return;
    }
    
    // Vider le treeview existant
    GtkTreeModel *old_model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview5));
    if (old_model != NULL) {
        g_object_unref(old_model);
    }
    
    // Créer un nouveau modèle de liste avec TOUTES les colonnes
    // Colonnes: 0:ID, 1:Nom, 2:Prénom, 3-7:Objectifs, 8:Disponibilité, 9-13:Jours, 14:Index
    GtkListStore *liststore = gtk_list_store_new(15,
        G_TYPE_STRING,    // 0: ID membre
        G_TYPE_STRING,    // 1: Nom
        G_TYPE_STRING,    // 2: Prénom
        G_TYPE_BOOLEAN,   // 3: Objectif Cardio
        G_TYPE_BOOLEAN,   // 4: Objectif Dance
        G_TYPE_BOOLEAN,   // 5: Objectif Musculation
        G_TYPE_BOOLEAN,   // 6: Objectif Kick boxing
        G_TYPE_BOOLEAN,   // 7: Objectif Gymnastique
        G_TYPE_STRING,    // 8: Disponibilité
        G_TYPE_BOOLEAN,   // 9: Jour Lundi
        G_TYPE_BOOLEAN,   // 10: Jour Mardi
        G_TYPE_BOOLEAN,   // 11: Jour Mercredi
        G_TYPE_BOOLEAN,   // 12: Jour Jeudi
        G_TYPE_BOOLEAN,   // 13: Jour Vendredi
        G_TYPE_INT        // 14: Index ligne (caché)
    );
    
    // Remplir le modèle
    int count = 0;
    DemandeCoach *demandes = lire_toutes_demandes("reservation.txt", &count);
    
    if (demandes == NULL || count == 0) {
        g_print("Aucune réservation trouvée dans le fichier.\n");
        
        // Ajouter une ligne "vide"
        GtkTreeIter iter;
        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter,
            0, "Aucune réservation",
            14, -1,
            -1);
    } else {
        // Ajouter chaque demande au modèle
        for (int i = 0; i < count; i++) {
            GtkTreeIter iter;
            gtk_list_store_append(liststore, &iter);
            
            // Chercher les infos du membre
            User membre = chercher_user("users.txt", demandes[i].id_membre);
            
            // Remplir toutes les colonnes
            gtk_list_store_set(liststore, &iter,
                0, demandes[i].id_membre,
                1, (strcmp(membre.id, "-1") == 0) ? "Inconnu" : membre.nom,
                2, (strcmp(membre.id, "-1") == 0) ? "Inconnu" : membre.prenom,
                3, demandes[i].objectifs[0] ? TRUE : FALSE,
                4, demandes[i].objectifs[1] ? TRUE : FALSE,
                5, demandes[i].objectifs[2] ? TRUE : FALSE,
                6, demandes[i].objectifs[3] ? TRUE : FALSE,
                7, demandes[i].objectifs[4] ? TRUE : FALSE,
                8, disponibilite_vers_texte(demandes[i].disponibilite),
                9, demandes[i].jours[0] ? TRUE : FALSE,
                10, demandes[i].jours[1] ? TRUE : FALSE,
                11, demandes[i].jours[2] ? TRUE : FALSE,
                12, demandes[i].jours[3] ? TRUE : FALSE,
                13, demandes[i].jours[4] ? TRUE : FALSE,
                14, i,
                -1);
        }
        
        free(demandes);
    }
    
    // Configurer le treeview avec le modèle
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview5), GTK_TREE_MODEL(liststore));
    
    // Supprimer toutes les anciennes colonnes
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview5));
    while (columns) {
        gtk_tree_view_remove_column(GTK_TREE_VIEW(treeview5), GTK_TREE_VIEW_COLUMN(columns->data));
        columns = columns->next;
    }
    
    // Créer les nouvelles colonnes
    
    // Colonne 1: ID Membre (non éditable)
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "ID Membre", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    
    // Colonne 2: Nom (non éditable)
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Nom", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    
    // Colonne 3: Prénom (non éditable)
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Prénom", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    
    // Colonnes pour les objectifs (toggle)
    const char *obj_labels[] = {"Cardio", "Dance", "Musculation", "Kick boxing", "Gymnastique"};
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *toggle_renderer = gtk_cell_renderer_toggle_new();
        
        // Connecter le signal toggled
        g_object_set_data(G_OBJECT(toggle_renderer), "column_index", GINT_TO_POINTER(i));
        g_signal_connect(toggle_renderer, "toggled", 
                        G_CALLBACK(on_objectif_toggled), treeview5);
        
        column = gtk_tree_view_column_new_with_attributes(
            obj_labels[i], toggle_renderer, "active", 3 + i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    }
    
    // Colonne pour la disponibilité (combobox)
    GtkCellRenderer *combo_renderer = gtk_cell_renderer_combo_new();
    
    // Créer le modèle pour le combobox
    GtkListStore *combo_model = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter combo_iter;
    
    gtk_list_store_append(combo_model, &combo_iter);
    gtk_list_store_set(combo_model, &combo_iter, 0, "Matin", -1);
    
    gtk_list_store_append(combo_model, &combo_iter);
    gtk_list_store_set(combo_model, &combo_iter, 0, "Midi", -1);
    
    gtk_list_store_append(combo_model, &combo_iter);
    gtk_list_store_set(combo_model, &combo_iter, 0, "Soir", -1);
    
    g_object_set(combo_renderer,
                 "model", combo_model,
                 "text-column", 0,
                 "editable", TRUE,
                 "has-entry", FALSE,
                 NULL);
    
    column = gtk_tree_view_column_new_with_attributes(
        "Disponibilité", combo_renderer, "text", 8, NULL);
    g_signal_connect(combo_renderer, "edited", 
                     G_CALLBACK(on_disponibilite_edited), treeview5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    
    // Colonnes pour les jours (toggle)
    const char *jour_labels[] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi"};
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *toggle_renderer = gtk_cell_renderer_toggle_new();
        
        // Connecter le signal toggled
        g_object_set_data(G_OBJECT(toggle_renderer), "column_index", GINT_TO_POINTER(i));
        g_signal_connect(toggle_renderer, "toggled", 
                        G_CALLBACK(on_jour_toggled), treeview5);
        
        column = gtk_tree_view_column_new_with_attributes(
            jour_labels[i], toggle_renderer, "active", 9 + i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview5), column);
    }
    
    // Optionnel: ajuster la largeur des colonnes
    gtk_tree_view_columns_autosize(GTK_TREE_VIEW(treeview5));
    
    g_print("Treeview éditable initialisé avec %d réservations\n", count);
}

// Callback pour les objectifs (checkbuttons)
void on_objectif_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer user_data)
{
    GtkWidget *treeview = GTK_WIDGET(user_data);
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    
    // Convertir le chemin en itérateur
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    
    if (!gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_path_free(path);
        return;
    }
    
    // Obtenir la valeur actuelle et l'index de la colonne
    gboolean active;
    int col_index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column_index"));
    gtk_tree_model_get(model, &iter, 3 + col_index, &active, -1);
    
    // Inverser la valeur
    active = !active;
    
    // Mettre à jour le modèle
    GtkListStore *store = GTK_LIST_STORE(model);
    gtk_list_store_set(store, &iter, 3 + col_index, active, -1);
    
    // Mettre à jour le fichier
    gint ligne_index;
    gtk_tree_model_get(model, &iter, 14, &ligne_index, -1);
    
    if (ligne_index != -1) {
        // Lire la réservation actuelle
        DemandeCoach *demandes = NULL;
        int count = 0;
        demandes = lire_toutes_demandes("reservation.txt", &count);
        
        if (demandes != NULL && ligne_index < count) {
            // Mettre à jour l'objectif
            demandes[ligne_index].objectifs[col_index] = active ? 1 : 0;
            
            // Mettre à jour dans le fichier
            mettre_a_jour_reservation(demandes[ligne_index], ligne_index);
            
            free(demandes);
            g_print("Objectif %d mis à jour pour la ligne %d: %s\n", 
                   col_index, ligne_index, active ? "activé" : "désactivé");
        }
    }
    
    gtk_tree_path_free(path);
}

// Callback pour les jours (checkbuttons)
void on_jour_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer user_data)
{
    GtkWidget *treeview = GTK_WIDGET(user_data);
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    
    if (!gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_path_free(path);
        return;
    }
    
    // Obtenir la valeur actuelle et l'index de la colonne
    gboolean active;
    int col_index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column_index"));
    gtk_tree_model_get(model, &iter, 9 + col_index, &active, -1);
    
    // Inverser la valeur
    active = !active;
    
    // Mettre à jour le modèle
    GtkListStore *store = GTK_LIST_STORE(model);
    gtk_list_store_set(store, &iter, 9 + col_index, active, -1);
    
    // Mettre à jour le fichier
    gint ligne_index;
    gtk_tree_model_get(model, &iter, 14, &ligne_index, -1);
    
    if (ligne_index != -1) {
        // Lire la réservation actuelle
        DemandeCoach *demandes = NULL;
        int count = 0;
        demandes = lire_toutes_demandes("reservation.txt", &count);
        
        if (demandes != NULL && ligne_index < count) {
            // Mettre à jour le jour
            demandes[ligne_index].jours[col_index] = active ? 1 : 0;
            
            // Mettre à jour dans le fichier
            mettre_a_jour_reservation(demandes[ligne_index], ligne_index);
            
            free(demandes);
            g_print("Jour %d mis à jour pour la ligne %d: %s\n", 
                   col_index, ligne_index, active ? "activé" : "désactivé");
        }
    }
    
    gtk_tree_path_free(path);
}

// Callback pour la disponibilité (combobox)
void on_disponibilite_edited(GtkCellRendererCombo *cell, gchar *path_str, gchar *new_text, gpointer user_data)
{
    GtkWidget *treeview = GTK_WIDGET(user_data);
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    
    if (!gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_path_free(path);
        return;
    }
    
    // Valider le texte
    if (strcmp(new_text, "Matin") != 0 && 
        strcmp(new_text, "Midi") != 0 && 
        strcmp(new_text, "Soir") != 0) {
        
        g_print("ERREUR: Disponibilité invalide: %s\n", new_text);
        gtk_tree_path_free(path);
        return;
    }
    
    // Mettre à jour le modèle
    GtkListStore *store = GTK_LIST_STORE(model);
    gtk_list_store_set(store, &iter, 8, new_text, -1);
    
    // Mettre à jour le fichier
    gint ligne_index;
    gtk_tree_model_get(model, &iter, 14, &ligne_index, -1);
    
    if (ligne_index != -1) {
        // Lire la réservation actuelle
        DemandeCoach *demandes = NULL;
        int count = 0;
        demandes = lire_toutes_demandes("reservation.txt", &count);
        
        if (demandes != NULL && ligne_index < count) {
            // Mettre à jour la disponibilité
            if (strcmp(new_text, "Matin") == 0) {
                demandes[ligne_index].disponibilite = 0;
            } else if (strcmp(new_text, "Midi") == 0) {
                demandes[ligne_index].disponibilite = 1;
            } else if (strcmp(new_text, "Soir") == 0) {
                demandes[ligne_index].disponibilite = 2;
            }
            
            // Mettre à jour dans le fichier
            mettre_a_jour_reservation(demandes[ligne_index], ligne_index);
            
            free(demandes);
            g_print("Disponibilité mise à jour pour la ligne %d: %s\n", 
                   ligne_index, new_text);
        }
    }
    
    gtk_tree_path_free(path);
}



void
on_home_clicked                        (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(objet_graphique);
    GtkWidget *acceuiladmin = create_acceuiladmin();
    gtk_widget_show_all(acceuiladmin);
    gtk_widget_destroy(window);
}


void
on_home2_clicked                       (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(objet_graphique);
    GtkWidget *acceuilmembre = create_acceuilmembre();
    gtk_widget_show_all(acceuilmembre);
    gtk_widget_destroy(window);
}


void
on_home3_clicked                       (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(objet_graphique);
    GtkWidget *authentification = create_authentification();
    gtk_widget_show_all(authentification);
    gtk_widget_destroy(window);
}


void
on_home4_clicked                       (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(objet_graphique);
    GtkWidget *authentification = create_authentification();
    gtk_widget_show_all(authentification);
    gtk_widget_destroy(window);
}


void
on_statistique_clicked                 (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{

    // Créer la fenêtre de dialogue
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "📊 Statistiques du Fitness First",
        GTK_WINDOW(gtk_widget_get_toplevel(objet_graphique)),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_STOCK_CLOSE,
        GTK_RESPONSE_CLOSE,
        NULL);
    
    // Utiliser gtk_vbox_new() pour GTK2
    GtkWidget *vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Calculer les statistiques
    StatistiquesDemographiques stats = calculer_statistiques_demographiques();
    
    // Créer un texte formaté avec les statistiques
    char buffer[2000];
    snprintf(buffer, sizeof(buffer),
        "<span size='x-large' weight='bold' foreground='#2E86C1'>📊 STATISTIQUES DU FITNESS FIRST</span>\n\n"
        "<span size='large' weight='bold'>👥 Vue d'ensemble</span>\n"
        "• <b>Total membres</b> : %d\n"
        "• <b>Hommes</b> : %d (%.1f%%)\n"
        "• <b>Femmes</b> : %d (%.1f%%)\n"
        "• <b>Âge moyen</b> : %.1f ans\n"
        "• <b>Âge minimum</b> : %d ans\n"
        "• <b>Âge maximum</b> : %d ans\n\n"
        
        "<span size='large' weight='bold'>📈 Répartition par âge</span>\n"
        "• <b>&lt; 20 ans</b> : %d membres (%.1f%%)\n"
        "• <b>20-29 ans</b> : %d membres (%.1f%%)\n"
        "• <b>30-39 ans</b> : %d membres (%.1f%%)\n"
        "• <b>40-49 ans</b> : %d membres (%.1f%%)\n"
        "• <b>50+ ans</b> : %d membres (%.1f%%)\n\n"
        
        "<span size='large' weight='bold'>📅 Dernière mise à jour</span>\n"
        "Généré le %s à %s",
        
        stats.total_membres,
        stats.hommes, stats.total_membres > 0 ? (float)stats.hommes/stats.total_membres*100 : 0,
        stats.femmes, stats.total_membres > 0 ? (float)stats.femmes/stats.total_membres*100 : 0,
        stats.age_moyen, stats.age_min, stats.age_max,
        
        stats.moins_20_ans, stats.total_membres > 0 ? (float)stats.moins_20_ans/stats.total_membres*100 : 0,
        stats.entre_20_30, stats.total_membres > 0 ? (float)stats.entre_20_30/stats.total_membres*100 : 0,
        stats.entre_30_40, stats.total_membres > 0 ? (float)stats.entre_30_40/stats.total_membres*100 : 0,
        stats.entre_40_50, stats.total_membres > 0 ? (float)stats.entre_40_50/stats.total_membres*100 : 0,
        stats.plus_50_ans, stats.total_membres > 0 ? (float)stats.plus_50_ans/stats.total_membres*100 : 0,
        
        __DATE__, __TIME__);
    
    // Créer un label avec mise en forme
    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    
    // Scroll si le contenu est long - VERSION CORRIGÉE GTK2
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
    
    // CORRECTION ICI : Utiliser gtk_widget_set_size_request au lieu de gtk_scrolled_window_set_min_content_height
    gtk_widget_set_size_request(scrolled, 500, 300);  // Largeur 500, Hauteur 300
    
    gtk_container_add(GTK_CONTAINER(scrolled), label);
    
    // Bouton pour générer un rapport
    GtkWidget *btn_rapport = gtk_button_new_with_label("📄 Générer un rapport complet");
    g_signal_connect(btn_rapport, "clicked", 
                    G_CALLBACK(on_generer_rapport_clicked), dialog);
    
    // Ajouter tout au conteneur
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_rapport, FALSE, FALSE, 5);
    
    // Ajouter à la boîte de dialogue - Syntaxe GTK2
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), vbox);
    
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Ajoutez cette fonction (simple, sans graphique)
void on_generer_rapport_clicked(GtkWidget *objet_graphique, gpointer user_data) {
    // Créer un nom de fichier avec date/heure
    char filename[256];
    time_t maintenant = time(NULL);
    struct tm *temps = localtime(&maintenant);
    strftime(filename, sizeof(filename), 
             "statistiques_%Y%m%d_%H%M.txt", temps);
    
    // Générer le rapport
    generer_rapport_statistiques(filename);
    
    // Afficher confirmation
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(gtk_widget_get_toplevel(objet_graphique)),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "✅ Rapport généré avec succès !\n\nFichier : %s", filename);
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void
on_eye_auth_clicked                    (GtkWidget       *button,
                                        gpointer         user_data)
{

    GtkWidget *window = gtk_widget_get_toplevel(button);
    GtkWidget *entry_mdp = lookup_widget(window, "entry20");
    
    if (entry_mdp) {
        oeil_auth_visible = !oeil_auth_visible;
        gtk_entry_set_visibility(GTK_ENTRY(entry_mdp), oeil_auth_visible);
        
        // Changer l'icône du bouton
        if (oeil_auth_visible) {
            gtk_button_set_label(GTK_BUTTON(button), "🙈");
            gtk_widget_set_tooltip_text(button, "Cacher le mot de passe");
        } else {
            gtk_button_set_label(GTK_BUTTON(button), "👁");
            gtk_widget_set_tooltip_text(button, "Afficher le mot de passe");
        }
    }
}


void
on_histogramme_clicked                 (GtkWidget       *objet_graphique,
                                        gpointer         user_data)
{
// ==================== FONCTIONS HISTOGRAMME ====================

// Callback pour le bouton histogramme
    // Créer une fenêtre pour l'histogramme
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Histogramme des Membres");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    // Créer un VBox pour organiser les widgets
    GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Créer un DrawingArea pour dessiner l'histogramme
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 800, 500);
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);
    
    // Créer une HBox pour les boutons
    GtkWidget *hbox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    
    // Bouton d'exportation
    GtkWidget *btn_export = gtk_button_new_with_label("Exporter en PNG");
    gtk_box_pack_start(GTK_BOX(hbox), btn_export, FALSE, FALSE, 0);
    
    // Bouton fermer
    GtkWidget *btn_fermer = gtk_button_new_with_label("Fermer");
    gtk_box_pack_start(GTK_BOX(hbox), btn_fermer, FALSE, FALSE, 0);
    
    // Calculer les données
    HistogrammeData data = calculer_donnees_histogramme();
    
    // Stocker les données dans l'objet drawing_area
    HistogrammeData *data_ptr = malloc(sizeof(HistogrammeData));
    *data_ptr = data;
    g_object_set_data(G_OBJECT(drawing_area), "histogramme_data", data_ptr);
    
    // Connecter le signal expose-event pour GTK2
    g_signal_connect(G_OBJECT(drawing_area), "expose-event", 
                     G_CALLBACK(on_draw_histogramme), NULL);
    
    // Connecter les signaux des boutons
    g_signal_connect(btn_export, "clicked", 
                     G_CALLBACK(on_export_histogramme_clicked), drawing_area);
    g_signal_connect(btn_fermer, "clicked", 
                     G_CALLBACK(gtk_widget_destroy), window);
    
    // Connecter le signal destroy pour libérer la mémoire
    g_signal_connect(window, "destroy", 
                     G_CALLBACK(on_histogramme_window_destroy), drawing_area);
    
    gtk_widget_show_all(window);
}

// Fonction pour libérer la mémoire quand la fenêtre est détruite
void on_histogramme_window_destroy(GtkWidget *window, gpointer user_data) {
    GtkWidget *drawing_area = GTK_WIDGET(user_data);
    HistogrammeData *data = (HistogrammeData *)g_object_get_data(G_OBJECT(drawing_area), "histogramme_data");
    if (data) {
        free(data);
    }
}

// Fonction de dessin de l'histogramme (GTK2 avec expose-event)
gboolean on_draw_histogramme(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
    // Obtenir le contexte graphique
    cairo_t *cr = gdk_cairo_create(widget->window);
    
    // Obtenir les dimensions avec gtk_widget_get_allocation (GTK2)
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    int width = allocation.width;
    int height = allocation.height;
    
    // Récupérer les données
    HistogrammeData *data = (HistogrammeData *)g_object_get_data(G_OBJECT(widget), "histogramme_data");
    
    if (data != NULL) {
        dessiner_histogramme(cr, width, height, *data);
    } else {
        // Message si pas de données
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_paint(cr);
        
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 24);
        cairo_move_to(cr, width/2 - 150, height/2);
        cairo_show_text(cr, "Aucune donnee disponible");
    }
    
    // Libérer le contexte Cairo
    cairo_destroy(cr);
    
    return FALSE;
}

// Fonction pour exporter l'histogramme en PNG
void on_export_histogramme_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget *drawing_area = GTK_WIDGET(user_data);
    
    // Créer un nom de fichier avec date/heure
    char filename[256];
    time_t maintenant = time(NULL);
    struct tm *temps = localtime(&maintenant);
    strftime(filename, sizeof(filename), 
             "histogramme_%Y%m%d_%H%M.png", temps);
    
    // Créer une surface Cairo pour l'export
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
    cairo_t *cr = cairo_create(surface);
    
    // Récupérer les données
    HistogrammeData *data = (HistogrammeData *)g_object_get_data(G_OBJECT(drawing_area), "histogramme_data");
    
    if (data != NULL) {
        // Dessiner sur la surface
        dessiner_histogramme(cr, 800, 600, *data);
        
        // Sauvegarder en PNG
        cairo_surface_write_to_png(surface, filename);
        
        // Afficher confirmation
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(button)),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Histogramme exporte avec succes !\n\nFichier : %s", filename);
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    
    // Nettoyer
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}




void
on_button_afficher_tous_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonannuler1_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonAjouter_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonRechercherMod_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_radioDisponible_mod_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_buttonannuler2_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonModifier_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_on_button_recherche_clicked_clicked (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_afficher_liste_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_retour_acceuil_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_rech_equip_dis_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_refresh_reserver_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_rech_reserver_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_radiobutton_reserver1_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_radiobutton_reserver2_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_radiobutton_reserver3_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button_reserver_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_mes_reservation_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_retour_acceuil_reservation_clicked  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_refresh_mes_reservation_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data)
{

}

