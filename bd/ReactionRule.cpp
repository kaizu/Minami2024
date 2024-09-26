#include <sstream>
#include <boost/algorithm/string.hpp>

#include "ReactionRule.hpp"
// #include "Context.hpp"

namespace ecell4
{

ReactionRule::ReactionRule()
    : k_(0), reactants_(), products_(),
    policy_(POLICY_STRICT), attributes_() // , rr_descriptor_()
{
    ;
}

ReactionRule::ReactionRule(
    const reactant_container_type& reactants,
    const product_container_type& products)
    : k_(0), reactants_(reactants), products_(products),
    policy_(POLICY_STRICT), attributes_() // , rr_descriptor_()
{
    ;
}

ReactionRule::ReactionRule(
    const reactant_container_type& reactants,
    const product_container_type& products,
    const Real& k)
    : k_(k), reactants_(reactants), products_(products),
    policy_(POLICY_STRICT), attributes_() // , rr_descriptor_()
{
    ;
}

ReactionRule::ReactionRule(
    const reactant_container_type& reactants,
    const product_container_type& products,
    const Quantity<Real>& k)
    : k_(k), reactants_(reactants), products_(products),
    policy_(POLICY_STRICT), attributes_() // , rr_descriptor_()
{
    ;
}

ReactionRule::ReactionRule(const ReactionRule& rr)
    : k_(rr.k_), reactants_(rr.reactants_), products_(rr.products_),
    policy_(rr.policy_), attributes_(rr.attributes_) // , rr_descriptor_()
{
    // if (rr.has_descriptor())
    // {
    //     set_descriptor(std::shared_ptr<ReactionRuleDescriptor>(rr.get_descriptor()->clone()));
    // }
}

Real ReactionRule::k() const
{
    return k_.magnitude;
}

void ReactionRule::set_k(const Real& k)
{
    set_k(Quantity<Real>(k));
}

void ReactionRule::set_k(const Quantity<Real>& k)
{
    if (k.magnitude < 0)
    {
        throw std::invalid_argument("a kinetic rate must be positive.");
    }
    k_ = k;
}

Quantity<Real> ReactionRule::get_k() const
{
    return k_;
}

const ReactionRule::reactant_container_type& ReactionRule::reactants() const
{
    return reactants_;
}

const ReactionRule::product_container_type& ReactionRule::products() const
{
    return products_;
}

void ReactionRule::add_reactant(const Species& sp)
{
    reactants_.push_back(sp);
}

void ReactionRule::add_product(const Species& sp)
{
    products_.push_back(sp);
}

const ReactionRule::policy_type ReactionRule::policy() const
{
    return policy_;
}

void ReactionRule::set_policy(const ReactionRule::policy_type policy)
{
    policy_ = policy;
}


const std::string ReactionRule::as_string() const
{
    std::stringstream oss;
    std::vector<std::string> tmp;
    // if (!has_descriptor())
    {
        for (reactant_container_type::const_iterator i(reactants_.begin());
            i != reactants_.end(); ++i)
        {
            tmp.push_back((*i).serial());
        }
        oss << boost::algorithm::join(tmp, "+") << ">";
        tmp.clear();
        for (product_container_type::const_iterator i(products_.begin());
            i != products_.end(); ++i)
        {
            tmp.push_back((*i).serial());
        }
        oss << boost::algorithm::join(tmp, "+") << "|" << k();
    }
    // else
    // {
    //     {
    //         reactant_container_type::const_iterator i(reactants_.begin());
    //         ReactionRuleDescriptor::coefficient_container_type::const_iterator
    //             j(rr_descriptor_->reactant_coefficients().begin());
    //         for (; i != reactants_.end() && j != rr_descriptor_->reactant_coefficients().end(); ++i, ++j)
    //         {
    //             std::stringstream oss_;
    //             oss_ << (*j) << "*" << (*i).serial();
    //             tmp.push_back(oss_.str());
    //         }
    //         oss << boost::algorithm::join(tmp, "+") << ">";
    //     }
    //     tmp.clear();
    //     {
    //         product_container_type::const_iterator i(products_.begin());
    //         ReactionRuleDescriptor::coefficient_container_type::const_iterator
    //             j(rr_descriptor_->product_coefficients().begin());
    //         for (; i != products_.end() && j != rr_descriptor_->product_coefficients().end(); ++i, ++j)
    //         {
    //             std::stringstream oss_;
    //             oss_ << (*j) << "*" << (*i).serial();
    //             tmp.push_back(oss_.str());
    //         }
    //         oss << boost::algorithm::join(tmp, "+") << "|" << k();
    //     }
    // }
    return oss.str();
}

// Integer ReactionRule::count(const ReactionRule::reactant_container_type& reactants) const
// {
//     return this->generate(reactants).size();
// }
// 
// std::vector<ReactionRule> ReactionRule::generate(const reactant_container_type& reactants) const
// {
//     return generate_reaction_rules(*this, reactants);
// }

// bool ReactionRule::has_descriptor() const
// {
//     return (rr_descriptor_.get() != NULL);
// }
// 
// void ReactionRule::set_descriptor(const std::shared_ptr<ReactionRuleDescriptor>& descriptor)
// {
//     rr_descriptor_ = descriptor;
// }
// 
// const std::shared_ptr<ReactionRuleDescriptor>& ReactionRule::get_descriptor() const
// {
//     return rr_descriptor_;
// }
// 
// void ReactionRule::reset_descriptor()
// {
//     std::shared_ptr<ReactionRuleDescriptor> tmp;
//     rr_descriptor_.swap(tmp);
// }

// ReactionRule format_reaction_rule_with_nosort(const ReactionRule& rr)
// {
//     ReactionRule::reactant_container_type reactants;
//     reactants.reserve(rr.reactants().size());
//     for (ReactionRule::reactant_container_type::const_iterator i(rr.reactants().begin());
//         i != rr.reactants().end(); ++i)
//     {
//         reactants.push_back(format_species(*i));
//     }
// 
//     ReactionRule::product_container_type products;
//     products.reserve(rr.products().size());
//     for (ReactionRule::product_container_type::const_iterator i(rr.products().begin());
//         i != rr.products().end(); ++i)
//     {
//         products.push_back(format_species(*i));
//     }
// 
//     return ReactionRule(reactants, products, rr.k());
// }
// 
// ReactionRule format_reaction_rule(const ReactionRule& rr)
// {
//     ReactionRule::reactant_container_type reactants;
//     reactants.reserve(rr.reactants().size());
//     for (ReactionRule::reactant_container_type::const_iterator i(rr.reactants().begin());
//         i != rr.reactants().end(); ++i)
//     {
//         reactants.push_back(format_species(*i));
//     }
// 
//     ReactionRule::product_container_type products;
//     products.reserve(rr.products().size());
//     for (ReactionRule::product_container_type::const_iterator i(rr.products().begin());
//         i != rr.products().end(); ++i)
//     {
//         products.push_back(format_species(*i));
//     }
// 
//     std::sort(reactants.begin(), reactants.end());
//     std::sort(products.begin(), products.end());
//     return ReactionRule(reactants, products, rr.k());
//     // ReactionRule::reactant_container_type reactants(rr.reactants());
//     // ReactionRule::product_container_type products(rr.products());
//     // std::sort(reactants.begin(), reactants.end());
//     // std::sort(products.begin(), products.end());
//     // return ReactionRule(reactants, products, rr.k());
//     // return rr;
// }

ReactionRule::attribute_type ReactionRule::get_attribute(const std::string& key) const
{
    return attributes_.get(key);
}

std::vector<std::pair<std::string, ReactionRule::attribute_type> > ReactionRule::list_attributes() const
{
    return attributes_.values();
}

void ReactionRule::set_attributes(const Attribute& attr)
{
    attributes_ = attr;
}

void ReactionRule::remove_attribute(const std::string& key)
{
    attributes_.remove(key);
}

bool ReactionRule::has_attribute(const std::string& key) const
{
    return attributes_.has_key(key);
}

const Attribute& ReactionRule::attributes() const
{
    return attributes_;
}

ReactionRule create_degradation_reaction_rule(
    const Species& reactant1, const Real& k)
{
    ReactionRule rr;
    rr.set_k(k);
    rr.add_reactant(reactant1);
    return rr;
}

ReactionRule create_synthesis_reaction_rule(
    const Species& product1, const Real& k)
{
    ReactionRule rr;
    rr.set_k(k);
    rr.add_product(product1);
    return rr;
}

ReactionRule create_unimolecular_reaction_rule(
    const Species& reactant1, const Species& product1, const Real& k)
{
    ReactionRule rr;
    rr.set_k(k);
    rr.add_reactant(reactant1);
    rr.add_product(product1);
    return rr;
}

ReactionRule create_binding_reaction_rule(
    const Species& reactant1, const Species& reactant2, const Species& product1,
    const Real& k)
{
    ReactionRule rr;
    rr.set_k(k);
    rr.add_reactant(reactant1);
    rr.add_reactant(reactant2);
    rr.add_product(product1);
    return rr;
}

ReactionRule create_unbinding_reaction_rule(
    const Species& reactant1, const Species& product1, const Species& product2,
    const Real& k)
{
    ReactionRule rr;
    rr.set_k(k);
    rr.add_reactant(reactant1);
    rr.add_product(product1);
    rr.add_product(product2);
    return rr;
}

// ReactionRule create_repulsive_reaction_rule(
//     const Species& reactant1, const Species& reactant2)
// {
//     ReactionRule rr;
//     rr.set_k(0.0);
//     rr.add_reactant(reactant1);
//     rr.add_reactant(reactant2);
//     return rr;
// }

}// ecell4
