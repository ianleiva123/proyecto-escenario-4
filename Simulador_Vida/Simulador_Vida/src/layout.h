#pragma once
#include "settings.h"
#include <SFML/Graphics/Rect.hpp>
namespace layout
{
constexpr int SPECIES_BUTTONS = MAX_SPECIES;
constexpr int NEIGHBOR_BUTTONS = RULE_COUNT;
constexpr float WIDTH = 1280.0f;
constexpr float HEIGHT = 900.0f;
constexpr float GRID_X = 30.0f;
constexpr float GRID_Y = 210.0f;
constexpr float GRID_WIDTH = 760.0f;
constexpr float GRID_HEIGHT = 520.0f;
constexpr float BUTTON_HEIGHT = 32.0f;
constexpr float RULE_X = 830.0f;
constexpr float RULE_SPACING = 45.0f;
constexpr float RULE_WIDTH = 39.0f;
constexpr float SPECIES_SPACING = 62.0f;
constexpr float SPECIES_WIDTH = 53.0f;
constexpr unsigned int FRAME_LIMIT = 60;
constexpr unsigned int CIRCLE_POINTS = 24;
constexpr unsigned int TRIANGLE_POINTS = 3;
constexpr unsigned int DIAMOND_POINTS = 4;
enum class Control
{
    Create,
    Duplicate,
    Delete,
    PreviousWorld,
    NextWorld,
    NewLab,
    Save,
    Load,
    Backward,
    Forward,
    Play,
    Clear,
    Random,
    Pattern,
    Neighborhood,
    Border,
    SpeedMinus,
    SpeedPlus,
    Mutation,
    MutationMinus,
    MutationPlus,
    DensityMinus,
    DensityPlus,
    Confirm,
    Cancel,
    FileField,
    SpeciesFirst,
    SpeciesLast = SpeciesFirst + 3,
    BirthFirst,
    BirthLast = BirthFirst + 8,
    SurvivalFirst,
    SurvivalLast = SurvivalFirst + 8
};
inline Control offset(Control first, int index)
{
    return static_cast<Control>(static_cast<int>(first) + index);
}
inline sf::FloatRect bounds(Control control)
{
    int id = static_cast<int>(control);
    int species = id - static_cast<int>(Control::SpeciesFirst);
    if (species >= 0 && species < SPECIES_BUTTONS)
    {
        return sf::FloatRect(RULE_X + species * SPECIES_SPACING, 173, SPECIES_WIDTH, BUTTON_HEIGHT);
    }
    int birth = id - static_cast<int>(Control::BirthFirst);
    if (birth >= 0 && birth < NEIGHBOR_BUTTONS)
    {
        return sf::FloatRect(RULE_X + birth * RULE_SPACING, 337, RULE_WIDTH, BUTTON_HEIGHT);
    }
    int survival = id - static_cast<int>(Control::SurvivalFirst);
    if (survival >= 0 && survival < NEIGHBOR_BUTTONS)
    {
        return sf::FloatRect(RULE_X + survival * RULE_SPACING, 409, RULE_WIDTH, BUTTON_HEIGHT);
    }
    switch (control)
    {
    case Control::Create:
        return sf::FloatRect(30, 83, 95, BUTTON_HEIGHT);
    case Control::Duplicate:
        return sf::FloatRect(135, 83, 100, BUTTON_HEIGHT);
    case Control::Delete:
        return sf::FloatRect(245, 83, 100, BUTTON_HEIGHT);
    case Control::PreviousWorld:
        return sf::FloatRect(355, 83, 110, BUTTON_HEIGHT);
    case Control::NextWorld:
        return sf::FloatRect(475, 83, 110, BUTTON_HEIGHT);
    case Control::NewLab:
        return sf::FloatRect(30, 125, 145, BUTTON_HEIGHT);
    case Control::Save:
        return sf::FloatRect(185, 125, 95, BUTTON_HEIGHT);
    case Control::Load:
        return sf::FloatRect(290, 125, 95, BUTTON_HEIGHT);
    case Control::Backward:
        return sf::FloatRect(30, 780, 100, BUTTON_HEIGHT);
    case Control::Forward:
        return sf::FloatRect(140, 780, 120, BUTTON_HEIGHT);
    case Control::Play:
        return sf::FloatRect(270, 780, 120, BUTTON_HEIGHT);
    case Control::Clear:
        return sf::FloatRect(400, 780, 100, BUTTON_HEIGHT);
    case Control::Random:
        return sf::FloatRect(510, 780, 110, BUTTON_HEIGHT);
    case Control::Pattern:
        return sf::FloatRect(630, 780, 150, BUTTON_HEIGHT);
    case Control::Neighborhood:
        return sf::FloatRect(830, 477, 200, BUTTON_HEIGHT);
    case Control::Border:
        return sf::FloatRect(1040, 477, 190, BUTTON_HEIGHT);
    case Control::SpeedMinus:
        return sf::FloatRect(1130, 519, 42, BUTTON_HEIGHT);
    case Control::SpeedPlus:
        return sf::FloatRect(1184, 519, 42, BUTTON_HEIGHT);
    case Control::Mutation:
        return sf::FloatRect(830, 566, 170, BUTTON_HEIGHT);
    case Control::MutationMinus:
        return sf::FloatRect(1130, 566, 42, BUTTON_HEIGHT);
    case Control::MutationPlus:
        return sf::FloatRect(1184, 566, 42, BUTTON_HEIGHT);
    case Control::DensityMinus:
        return sf::FloatRect(1130, 614, 42, BUTTON_HEIGHT);
    case Control::DensityPlus:
        return sf::FloatRect(1184, 614, 42, BUTTON_HEIGHT);
    case Control::Confirm:
        return sf::FloatRect(420, 530, 140, BUTTON_HEIGHT);
    case Control::Cancel:
        return sf::FloatRect(580, 530, 140, BUTTON_HEIGHT);
    case Control::FileField:
        return sf::FloatRect(420, 370, 435, 38);
    default:
        return sf::FloatRect();
    }
}
inline sf::FloatRect fieldBounds(int index)
{
    constexpr float FIELD_X = 665.0f;
    constexpr float FIELD_Y = 280.0f;
    constexpr float FIELD_GAP = 55.0f;
    return sf::FloatRect(FIELD_X, FIELD_Y + index * FIELD_GAP, 190, 34);
}
} // namespace layout
