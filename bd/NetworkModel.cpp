#include <algorithm>

#include "exceptions.hpp"
#include "NetworkModel.hpp"


namespace ecell4
{

bool NetworkModel::update_species_attribute(const Species& sp)
{
    species_container_type::iterator i(std::find(species_attributes_.begin(), species_attributes_.end(), sp));
    if (i == species_attributes_.end())
    {
        add_species_attribute(sp);
        return true;
    }
    (*i).overwrite_attributes(sp);
    return false;
}

void NetworkModel::add_species_attribute(const Species& sp, const bool proceed)
{
    species_attributes_.push_back(sp);
    species_attributes_proceed_.push_back(proceed);
}

void NetworkModel::remove_species_attribute(const Species& sp)
{
    species_container_type::iterator i(std::find(species_attributes_.begin(), species_attributes_.end(), sp));
    if (i == species_attributes_.end())
    {
        throw_exception<NotFound>("The given Speices [", sp.serial(), "] was not found");
    }
    species_attributes_proceed_.erase(
        species_attributes_proceed_.begin() + std::distance(species_attributes_.begin(), i));
    species_attributes_.erase(i);
}

bool NetworkModel::has_species_attribute(const Species& sp) const
{
    species_container_type::const_iterator i(
        std::find(species_attributes_.begin(), species_attributes_.end(), sp));
    return (i != species_attributes_.end());
}

Species NetworkModel::apply_species_attributes(const Species& sp) const
{
    Species ret(sp);
    species_container_type::const_iterator i(species_attributes_.begin());
    std::vector<bool>::const_iterator j(species_attributes_proceed_.begin());
    for (; i != species_attributes_.end() && j != species_attributes_proceed_.end(); ++i, ++j)
    {
        if ((*i).serial() != "_" && sp != (*i))
        {
            continue;
        }
        ret.overwrite_attributes(*i);
        if (!(*j))
        {
            break;
        }
    }
    return ret;
}

Integer NetworkModel::apply(const Species& pttrn, const Species& sp) const
{
    return (pttrn == sp ? 1 : 0);
}

std::vector<ReactionRule> NetworkModel::query_reaction_rules(
    const Species& sp) const
{
    first_order_reaction_rules_map_type::const_iterator
        i(first_order_reaction_rules_map_.find(sp.serial()));

    std::vector<ReactionRule> retval;
    if (i != first_order_reaction_rules_map_.end())
    {
        retval.reserve((*i).second.size());
        for (first_order_reaction_rules_map_type::mapped_type::const_iterator
                 j((*i).second.begin()); j != (*i).second.end(); ++j)
        {
            retval.push_back(reaction_rules_[*j]);
        }
    }
    return retval;
}

std::vector<ReactionRule> NetworkModel::query_reaction_rules(
    const Species& sp1, const Species& sp2) const
{
    std::vector<ReactionRule> retval;
    const std::pair<Species::serial_type, Species::serial_type>
        key(sp1.serial() < sp2.serial()?
            std::make_pair(sp1.serial(), sp2.serial()):
            std::make_pair(sp2.serial(), sp1.serial()));

    second_order_reaction_rules_map_type::const_iterator
        i(second_order_reaction_rules_map_.find(key));
    if (i != second_order_reaction_rules_map_.end())
    {
        retval.reserve((*i).second.size());
        for (second_order_reaction_rules_map_type::mapped_type::const_iterator
                 j((*i).second.begin()); j != (*i).second.end(); ++j)
        {
            retval.push_back(reaction_rules_[*j]);
        }
    }
    return retval;
}

std::vector<ReactionRule> NetworkModel::apply(
    const ReactionRule& rr, const ReactionRule::reactant_container_type& reactants) const
{
    if (rr.reactants().size() != reactants.size())
    {
        return std::vector<ReactionRule>();
    }

    ReactionRule::reactant_container_type::const_iterator
        i(rr.reactants().begin()), j(reactants.begin());
    for (; i != rr.reactants().end(); ++i, ++j)
    {
        if (*i != *j)
        {
            return std::vector<ReactionRule>();
        }
    }
    return std::vector<ReactionRule>(1, rr);
}

void NetworkModel::add_reaction_rule(const ReactionRule& rr)
{
    // if (rr.has_descriptor())
    // {
    //     reaction_rules_.push_back(rr);
    //     return;
    // }

    for (reaction_rule_container_type::iterator i(reaction_rules_.begin());
        i != reaction_rules_.end(); ++i)
    {
        // if ((*i) == rr && !(*i).has_descriptor())
        if ((*i) == rr)
        {
            (*i).set_k((*i).k() + rr.k());  // Merging
            return;
        }
    }

    const reaction_rule_container_type::size_type idx(reaction_rules_.size());
    reaction_rules_.push_back(rr);

    if (rr.reactants().size() == 1)
    {
        const Species::serial_type key = rr.reactants()[0].serial();
        first_order_reaction_rules_map_[key].push_back(idx);
    }
    else if (rr.reactants().size() == 2)
    {
        const Species::serial_type
            serial1(rr.reactants()[0].serial()),
            serial2(rr.reactants()[1].serial());
        const std::pair<Species::serial_type, Species::serial_type>
            key(serial1 < serial2?
                std::make_pair(serial1, serial2):
                std::make_pair(serial2, serial1));
        second_order_reaction_rules_map_[key].push_back(idx);
    }
    else
    {
        ; // do nothing
    }
}

void NetworkModel::remove_reaction_rule(const ReactionRule& rr)
{
    reaction_rule_container_type::size_type removed = 0;
    while (true)
    {
        reaction_rule_container_type::iterator
            i(std::find(reaction_rules_.begin(), reaction_rules_.end(), rr));
        if (i == reaction_rules_.end())
        {
            break;
        }
        remove_reaction_rule(i);
        ++removed;
    }

    if (removed == 0)
    {
        throw NotFound("The given reaction rule was not found.");
    }
}

void NetworkModel::remove_reaction_rule(const NetworkModel::reaction_rule_container_type::iterator i)
{
    assert(reaction_rules_.size() > 0);
    const reaction_rule_container_type::size_type idx = i - reaction_rules_.begin();
    assert(idx < reaction_rules_.size());
    const reaction_rule_container_type::size_type last_idx(reaction_rules_.size() - 1);

    {
        const ReactionRule& rr = reaction_rules_[idx];

        // if (rr.has_descriptor())
        // {
        //     ; // do nothing
        // }
        // else if (rr.reactants().size() == 1)
        if (rr.reactants().size() == 1)
        {
            first_order_reaction_rules_map_type::iterator
                j(first_order_reaction_rules_map_.find(rr.reactants()[0].serial()));
            assert(j != first_order_reaction_rules_map_.end());

            first_order_reaction_rules_map_type::mapped_type::iterator
                k(std::remove((*j).second.begin(), (*j).second.end(), idx));
            assert(k != (*j).second.end());

            (*j).second.erase(k, (*j).second.end());
        }
        else if (rr.reactants().size() == 2)
        {
            second_order_reaction_rules_map_type::iterator
                j(second_order_reaction_rules_map_.find(std::make_pair(
                    rr.reactants()[0].serial(), rr.reactants()[1].serial())));
            assert(j != second_order_reaction_rules_map_.end());

            second_order_reaction_rules_map_type::mapped_type::iterator
                k(std::remove((*j).second.begin(), (*j).second.end(), idx));
            assert(k != (*j).second.end());

            (*j).second.erase(k, (*j).second.end());
        }
    }

    if (idx < last_idx)
    {
        reaction_rule_container_type::value_type const
            rrlast(reaction_rules_[last_idx]);
        (*i) = rrlast;

        // if (rrlast.has_descriptor())
        // {
        //     ; // do nothing
        // }
        // else if (rrlast.reactants().size() == 1)
        if (rrlast.reactants().size() == 1)
        {
            first_order_reaction_rules_map_type::iterator
                j(first_order_reaction_rules_map_.find(
                    rrlast.reactants()[0].serial()));
            assert(j != first_order_reaction_rules_map_.end());

            first_order_reaction_rules_map_type::mapped_type::iterator
                k(std::remove((*j).second.begin(), (*j).second.end(), last_idx));
            assert(k != (*j).second.end());

            (*j).second.erase(k, (*j).second.end());
            (*j).second.push_back(idx);
        }
        else if (rrlast.reactants().size() == 2)
        {
            second_order_reaction_rules_map_type::iterator
                j(second_order_reaction_rules_map_.find(std::make_pair(
                    rrlast.reactants()[0].serial(),
                    rrlast.reactants()[1].serial())));
            assert(j != second_order_reaction_rules_map_.end());

            second_order_reaction_rules_map_type::mapped_type::iterator
                k(std::remove((*j).second.begin(), (*j).second.end(), last_idx));
            assert(k != (*j).second.end());

            (*j).second.erase(k, (*j).second.end());
            (*j).second.push_back(idx);
        }
    }

    reaction_rules_.pop_back();
}

bool NetworkModel::has_reaction_rule(const ReactionRule& rr) const
{
    reaction_rule_container_type::const_iterator
        i(std::find(reaction_rules_.begin(), reaction_rules_.end(), rr));
    return (i != reaction_rules_.end());
}

} // ecell4
