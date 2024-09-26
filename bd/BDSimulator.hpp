#ifndef ECELL4_BD_BD_SIMULATOR_HPP
#define ECELL4_BD_BD_SIMULATOR_HPP

#include <stdexcept>

#include "./Model.hpp"
#include "./SimulatorBase.hpp"

#include "BDWorld.hpp"


namespace ecell4
{

namespace bd
{

class BDSimulator
    : public SimulatorBase<BDWorld>
{
public:

    typedef SimulatorBase<BDWorld> base_type;
    // typedef BDPropagator::reaction_info_type reaction_info_type;

public:

    BDSimulator(
        std::shared_ptr<BDWorld> world, std::shared_ptr<Model> model,
        Real bd_dt_factor = 1e-5)
        : base_type(world, model), dt_(0), bd_dt_factor_(bd_dt_factor), dt_set_by_user_(false),
        gamma_t_(1.0), beta_(1.0), region_radius(0.0)
    {
        initialize();
    }

    BDSimulator(std::shared_ptr<BDWorld> world, Real bd_dt_factor = 1e-5)
        : base_type(world), dt_(0), bd_dt_factor_(bd_dt_factor), dt_set_by_user_(false),
        gamma_t_(1.0), beta_(1.0), region_radius(0.0)
    {
        initialize();
    }

    // SimulatorTraits
    void initialize();

    Real determine_dt() const
    {
        constexpr Real inf = std::numeric_limits<Real>::infinity();
        Real rmin(inf), Dmax(0.0);

        for (std::vector<Species>::const_iterator i(model_->species_attributes().begin());
            i != model_->species_attributes().end(); ++i)
        {
            const BDWorld::molecule_info_type
                info(world_->get_molecule_info(*i));

            if (rmin > info.radius)
            {
                rmin = info.radius;
            }
            if (Dmax < info.D)
            {
                Dmax = info.D;
            }
        }

        const Real dt(rmin < inf && Dmax > 0.0
            ? 4.0 * rmin * rmin / (2.0 * Dmax) * bd_dt_factor_
            // ? rmin * rmin / (6.0 * Dmax) * bd_dt_factor_
            : inf);
        return dt;
    }

    Real dt() const
    {
        return dt_;
    }

    void step();
    bool step(const Real& upto);

    // Optional members

    virtual bool check_reaction() const
    {
        return false;
    }

    void set_dt(const Real& dt)
    {
        if (dt <= 0)
        {
            throw std::invalid_argument("The step size must be positive.");
        }
        dt_ = dt;
        dt_set_by_user_ = true;
    }

    inline std::shared_ptr<RandomNumberGenerator> rng()
    {
        return (*world_).rng();
    }

    Real beta() const
    {
        return beta_;
    }

    void set_beta(const Real& beta)
    {
        beta_ = beta;
    }

    Real gamma_t() const
    {
        return gamma_t_;
    }

    void set_gamma_t(const Real& gamma_t)
    {
        gamma_t_ = gamma_t;
    }

public:

    std::map<std::pair<ParticleID, ParticleID>, Real> first_encount;  // unordered_map requires hash.
    Real region_radius;

protected:

    /**
     * the protected internal state of BDSimulator.
     * they are needed to be saved/loaded with Visitor pattern.
     */
    Real dt_;
    const Real bd_dt_factor_;
    bool dt_set_by_user_;
    // std::vector<std::pair<ReactionRule, reaction_info_type> > last_reactions_;

    std::vector<size_t> queue_;
    std::vector<Real> scheduled_times_;

    Real gamma_t_, beta_;
};

} // bd

} // ecell4

#endif /* ECELL4_BD_BD_SIMULATOR_HPP */
