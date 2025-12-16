#include <gtk/gtk.h>

extern int sexe_ajout;  // 0 = Femme, 1 = Homme

void on_radio_femme_ajout_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_radio_homme_ajout_toggled(GtkToggleButton *togglebutton, gpointer user_data);

void
on_ajouter_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data);


void
on_annuler_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_chercher_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_modifier_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_annulern_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data);


void
on_recherche_clicked                   (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
void
on_cherche_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_treeview4_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
on_afficher_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data);


void
on_reserver_clicked                    (GtkWidget       *objet_graphique,
                                        gpointer         user_data);



void
on_voir_clicked                        (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_retour_clicked                      (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_treeview5_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
on_voir_clicked                        (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_affiche_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
void initialiser_treeview_reservation(GtkWidget *treeview5);

void
on_valider_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
void
on_demande_clicked                     (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
void
on_membre_clicked                      (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
const char* get_utilisateur_connecte_id(void);
const char* get_utilisateur_type(void);
int est_connecte(void);
int verifier_admin(const char *id, const char *mdp);
int verifier_membre(const char *id, const char *mdp);
void initialiser_treeview_reservation_editable(GtkWidget *treeview5);
void on_objectif_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer user_data);
void on_jour_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer user_data);
void on_disponibilite_edited(GtkCellRendererCombo *cell, gchar *path_str, gchar *new_text, gpointer user_data);

void
on_home_clicked                        (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_home2_clicked                       (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_home3_clicked                       (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_home4_clicked                       (GtkWidget       *objet_graphique,
                                        gpointer         user_data);

void
on_statistique_clicked                 (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
void on_generer_rapport_clicked(GtkWidget *objet_graphique, gpointer user_data);

void
on_eye_auth_clicked                    (GtkWidget       *button,
                                        gpointer         user_data);

void
on_histogramme_clicked                 (GtkWidget       *objet_graphique,
                                        gpointer         user_data);
gboolean on_draw_histogramme(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void on_export_histogramme_clicked(GtkWidget *button, gpointer user_data);
void on_histogramme_window_destroy(GtkWidget *window, gpointer user_data);
