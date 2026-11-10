#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <format>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr double noteMin{0.0};
constexpr double noteMax{20.0};
constexpr double seuilAdmission{10.0};
constexpr int effectifMin{1};
constexpr int effectifMax{200};
constexpr std::size_t nbMentions{5};

enum class Mention { Ajourne, Passable, AssezBien, Bien, TresBien };

constexpr bool estNoteValide(double note)
{
    return note >= noteMin && note <= noteMax;
}

Mention mentionPour(double note)
{
    if (note < seuilAdmission) {
        return Mention::Ajourne;
    }
    if (note < 12.0) {
        return Mention::Passable;
    }
    if (note < 14.0) {
        return Mention::AssezBien;
    }
    if (note < 16.0) {
        return Mention::Bien;
    }
    return Mention::TresBien;
}

std::string_view libelle(Mention mention)
{
    switch (mention) {
    case Mention::Ajourne:
        return "Ajourné";
    case Mention::Passable:
        return "Passable";
    case Mention::AssezBien:
        return "Assez bien";
    case Mention::Bien:
        return "Bien";
    case Mention::TresBien:
        return "Très bien";
    }
    return "Inconnue";
}

std::string lireLigne(std::string_view invite)
{
    std::cout << invite;
    std::string ligne;
    std::getline(std::cin, ligne);
    return ligne;
}

bool estEntierStrict(const std::string& texte, int& valeur)
{
    std::istringstream flux{texte};
    char reste{};
    return (flux >> valeur) && !(flux >> reste);
}

bool estReelStrict(std::string texte, double& valeur)
{
    for (char& c : texte) {
        if (c == ',') {
            c = '.';
        }
    }
    std::istringstream flux{texte};
    char reste{};
    return (flux >> valeur) && !(flux >> reste);
}

std::string lireTexte(std::string_view invite)
{
    return lireLigne(invite);
}

int lireEntier(std::string_view invite, int min, int max)
{
    while (std::cin) {
        const std::string ligne = lireLigne(invite);
        int valeur{};
        if (!estEntierStrict(ligne, valeur)) {
            std::cout << "   Saisie invalide : entrez un nombre entier.\n";
        } else if (valeur < min || valeur > max) {
            std::cout << "   La valeur doit être comprise entre " << min << " et " << max << ".\n";
        } else {
            return valeur;
        }
    }
    return min;
}

double lireNote(std::string_view invite)
{
    while (std::cin) {
        const std::string ligne = lireLigne(invite);
        double note{};
        if (!estReelStrict(ligne, note)) {
            std::cout << "   Saisie invalide : entrez un nombre (ex. 12.5 ou 12,5).\n";
        } else if (!estNoteValide(note)) {
            std::cout << "   La note doit être comprise entre " << noteMin << " et " << noteMax
                      << ".\n";
        } else {
            return note;
        }
    }
    return noteMin;
}

} // namespace

int main()
{
    const std::string nomUE = lireTexte("Nom de l'UE : ");
    const int effectif = lireEntier("Nombre d'étudiants (1 à 200) : ", effectifMin, effectifMax);

    std::vector<double> notes;
    notes.reserve(static_cast<std::size_t>(effectif));
    for (int i{1}; i <= effectif; ++i) {
        notes.push_back(lireNote(std::format("Note de l'étudiant {} : ", i)));
    }

    const double minimum = *std::min_element(notes.begin(), notes.end());
    const double maximum = *std::max_element(notes.begin(), notes.end());
    const double taille = static_cast<double>(notes.size());

    double somme{0.0};
    for (double n : notes) {
        somme += n;
    }
    const double moyenne = somme / taille;

    double sommeCarres{0.0};
    for (double n : notes) {
        sommeCarres += (n - moyenne) * (n - moyenne);
    }
    const double ecartType = std::sqrt(sommeCarres / taille);

    int admis{0};
    std::array<int, nbMentions> repartition{};
    for (double n : notes) {
        if (n >= seuilAdmission) {
            ++admis;
        }
        ++repartition[static_cast<std::size_t>(mentionPour(n))];
    }
    const double pourcentage = 100.0 * static_cast<double>(admis) / taille;

    std::println("\nStatistiques — {} ({} étudiants)", nomUE, effectif);
    std::println("{:-<45}", "");
    std::println("{:<20}: {:>8.2f}", "Note minimale", minimum);
    std::println("{:<20}: {:>8.2f}", "Note maximale", maximum);
    std::println("{:<20}: {:>8.2f}", "Moyenne", moyenne);
    std::println("{:<20}: {:>8.2f}", "Écart-type", ecartType);
    std::println("{:<20}: {:>8} ({:.1f} %)", "Admis", admis, pourcentage);

    std::println("\nRépartition par mention");
    for (std::size_t i{0}; i < nbMentions; ++i) {
        const auto mention = static_cast<Mention>(i);
        std::println("{:<12} {} ({})", libelle(mention),
                     std::string(static_cast<std::size_t>(repartition[i]), '*'), repartition[i]);
    }
}
