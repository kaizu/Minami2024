#ifndef ECELL4_REACTION_RULE_DESCRIPTOR_HPP
#define ECELL4_REACTION_RULE_DESCRIPTOR_HPP

#include <stdexcept>
#include <memory>
#include <cmath>

#include "types.hpp"
#include "Species.hpp"

#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_io.hpp"


namespace ecell4
{

class ReactionRuleDescriptor
{
public:

    typedef std::vector<Species> reactant_container_type;
    typedef std::vector<Species> product_container_type;
    typedef std::vector<Real> coefficient_container_type;

    typedef std::vector<Real> state_container_type;

    // The following stores propensity_func, number of reactant coefficients, number of product_coefficients
    typedef boost::tuple<bool, int, int> descriptor_attribute;

public:

    ReactionRuleDescriptor(const coefficient_container_type &reactant_coefficients, const coefficient_container_type &product_coefficients)
        : reactant_coefficients_(reactant_coefficients), product_coefficients_(product_coefficients)
    {
        ;
    }

    ReactionRuleDescriptor()
    {
        ;
    }

    virtual ~ReactionRuleDescriptor()
    {
        ;
    }

    virtual bool is_available() const
    {
        return true;
    }

    descriptor_attribute attribute(void) const
    {
        return boost::make_tuple(
            this->is_available(), reactant_coefficients_.size(), product_coefficients_.size());
    }

    virtual Real propensity(const state_container_type& r, const state_container_type& p, Real volume, Real time) const = 0;

    virtual ReactionRuleDescriptor* clone() const
    {
        return 0;
    }

    // Accessor of coefficients;
    const coefficient_container_type &reactant_coefficients(void) const
    {
        return this->reactant_coefficients_;
    }

    const coefficient_container_type &product_coefficients(void) const
    {
        return this->product_coefficients_;
    }

    void resize_reactants(const std::size_t size)
    {
        this->reactant_coefficients_.resize(size, 1.0);
    }

    void resize_products(const std::size_t size)
    {
        this->product_coefficients_.resize(size, 1.0);
    }

    void set_reactant_coefficient(const std::size_t num, const Real new_coeff)
    {
        if (num >= reactant_coefficients_.size())
        {
            this->resize_reactants(num + 1);
        }
        this->reactant_coefficients_[num] = new_coeff;
    }

    void set_product_coefficient(const std::size_t num, const Real new_coeff)
    {
        if (num >= product_coefficients_.size())
        {
            this->resize_products(num + 1);
        }
        this->product_coefficients_[num] = new_coeff;
    }

    void set_reactant_coefficients(const coefficient_container_type &new_reactant_coefficients)
    {
        this->reactant_coefficients_.clear();
        for (coefficient_container_type::size_type i = 0; i < new_reactant_coefficients.size(); i++)
        {
            this->reactant_coefficients_.push_back(new_reactant_coefficients[i]);
        }
    }

    void set_product_coefficients(const coefficient_container_type &new_product_coefficients)
    {
        this->product_coefficients_.clear();
        for (coefficient_container_type::size_type i = 0; i < new_product_coefficients.size(); i++)
        {
            this->product_coefficients_.push_back(new_product_coefficients[i]);
        }
    }

    bool has_coefficients(void) const
    {
        return !(this->reactant_coefficients_.empty() && this->product_coefficients_.empty());
    }

private:

    coefficient_container_type reactant_coefficients_;
    coefficient_container_type product_coefficients_;
};

class ReactionRuleDescriptorConstant
    : public ReactionRuleDescriptor
{
public:

    typedef ReactionRuleDescriptor base_type;
    typedef base_type::state_container_type state_container_type;

public:

    ReactionRuleDescriptorConstant(const Quantity<Real>& k)
        : base_type(), k_(k)
    {
        ;
    }

    ReactionRuleDescriptorConstant(const Quantity<Real>& k, const coefficient_container_type &reactant_coefficients, const coefficient_container_type &product_coefficients)
        : base_type(reactant_coefficients, product_coefficients), k_(k)
    {
        ;
    }

    ReactionRuleDescriptorConstant(const Real k)
        : base_type(), k_(k)
    {
        ;
    }

    ReactionRuleDescriptorConstant(const Real k, const coefficient_container_type &reactant_coefficients, const coefficient_container_type &product_coefficients)
        : base_type(reactant_coefficients, product_coefficients), k_(k)
    {
        ;
    }

    virtual ReactionRuleDescriptor* clone() const
    {
        return new ReactionRuleDescriptorConstant(k_, reactant_coefficients(), product_coefficients());
    }

    const Real k() const
    {
        return k_.magnitude;
    }

    Quantity<Real> get_k() const
    {
        return k_;
    }

    void set_k(const Real k)
    {
        k_ = Quantity<Real>(k);
    }

    void set_k(const Quantity<Real>& k)
    {
        k_ = k;
    }

    virtual Real propensity(const state_container_type& reactants, const state_container_type& products, Real volume, Real t) const
    {
        return k_.magnitude;
    }

private:

    Quantity<Real> k_;
};

class ReactionRuleDescriptorMassAction
    : public ReactionRuleDescriptor
{
public:

    typedef ReactionRuleDescriptor base_type;
    typedef base_type::state_container_type state_container_type;

public:

    ReactionRuleDescriptorMassAction(const Quantity<Real>& k)
        : base_type(), k_(k)
    {
        ;
    }

    ReactionRuleDescriptorMassAction(const Quantity<Real>& k, const coefficient_container_type &reactant_coefficients, const coefficient_container_type &product_coefficients)
        : base_type(reactant_coefficients, product_coefficients), k_(k)
    {
        ;
    }

    ReactionRuleDescriptorMassAction(const Real k)
        : base_type(), k_(k)
    {
        ;
    }

    ReactionRuleDescriptorMassAction(const Real k, const coefficient_container_type &reactant_coefficients, const coefficient_container_type &product_coefficients)
        : base_type(reactant_coefficients, product_coefficients), k_(k)
    {
        ;
    }

    virtual ReactionRuleDescriptor* clone() const
    {
        return new ReactionRuleDescriptorMassAction(k_, reactant_coefficients(), product_coefficients());
    }

    const Real k() const
    {
        return k_.magnitude;
    }

    Quantity<Real> get_k() const
    {
        return k_;
    }

    void set_k(const Real k)
    {
        k_ = Quantity<Real>(k);
    }

    void set_k(const Quantity<Real>& k)
    {
        k_ = k;
    }

    virtual Real propensity(const state_container_type& reactants, const state_container_type& products, Real volume, Real t) const
    {
        Real ret = k_.magnitude * volume;
        state_container_type::const_iterator i(reactants.begin());
        coefficient_container_type::const_iterator j(reactant_coefficients().begin());
        for (; i != reactants.end() && j != reactant_coefficients().end(); ++i, ++j)
        {
            ret *= std::pow((*i) / volume, (*j));
        }
        return ret;
    }

private:

    Quantity<Real> k_;
};

class ReactionRuleDescriptorCPPfunc
    : public ReactionRuleDescriptor
{
public:

    typedef ReactionRuleDescriptor base_type;
    typedef base_type::state_container_type state_container_type;
    typedef Real (*func_type)(const state_container_type& r, const state_container_type& p, Real volume, Real t, const ReactionRuleDescriptorCPPfunc& rd);

public:

    ReactionRuleDescriptorCPPfunc(func_type pf)
        : base_type(), pf_(pf)
    {
        ;
    }

    ReactionRuleDescriptorCPPfunc(func_type pf, const coefficient_container_type &reactant_coefficients, const coefficient_container_type &product_coefficients)
        : base_type(reactant_coefficients, product_coefficients), pf_(pf)
    {
        ;
    }

    virtual ReactionRuleDescriptor* clone() const
    {
        return new ReactionRuleDescriptorCPPfunc(pf_, reactant_coefficients(), product_coefficients());
    }

    bool is_available() const
    {
        return (this->pf_ != 0);
    }

    func_type get() const
    {
        return this->pf_;
    }

    virtual Real propensity(const state_container_type& reactants, const state_container_type& products, Real volume, Real time) const
    {
        if (this->is_available())
        {
            return (this->pf_)(reactants, products, volume, time, *this);
        }
        else
        {
            // throw IllegalState("Pointer to the user-defined propensity function is NULL");
            return 0.0;
        }
    }

private:

    func_type pf_;
};

} // ecell4

#endif /* ECELL4_REACTION_RULE_DESCRIPTOR_HPP */
