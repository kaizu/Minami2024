#include "BDSimulator.hpp"

#include <cstring>

namespace ecell4
{

namespace bd
{

Real get_CTRW_timestep(RandomNumberGenerator& rng, const Real gamma_t, const Real beta)
{
    const Real u = rng.uniform(0.0, 1.0);
    const Real v = rng.uniform(0.0, 1.0);
    const Real a2 = sin(beta * M_PI) / tan(beta * M_PI * v) - cos(beta * M_PI);
    const Real tau = - gamma_t * log(u) * pow(a2, 1.0 / beta);
    return tau;
}

void BDSimulator::initialize()
{
    if (!dt_set_by_user_)
    {
        dt_ = determine_dt();
    }

    queue_.resize((*world_).num_particles());
    for (size_t i = 0; i < (*world_).num_particles(); i++)
    {
        queue_[i] = i;
    }
}

void BDSimulator::step()
{
    const Real3& edge_lengths((*world_).edge_lengths());
    // const Real L(edge_lengths[0] / 3);
    const Real Lx(edge_lengths[0]);
    const Real L(edge_lengths[1]);
    const Real L_2(L * 0.5);

    {
        // std::vector<size_t> queue_((*world_).num_particles());
        // for (size_t i = 0; i < (*world_).num_particles(); i++)
        // {
        //     queue_[i] = i;
        // }

        // BDWorld::particle_container_type queue_ = (*world_).list_particles();
        shuffle(*rng(), queue_);

        for (std::vector<size_t>::const_iterator i(queue_.begin()); i != queue_.end(); i++)
        {
            std::pair<ParticleID, Particle> const& pid_particle_pair((*world_)._get_particle(*i));
            ParticleID const& pid(pid_particle_pair.first);
            Particle const& particle(pid_particle_pair.second);
            // const ParticleID pid(queue_.back().first);
            // queue_.pop_back();
            // Particle particle((*world_).get_particle(pid).second);

            const Real D(particle.D());
            if (D == 0)
            {
                continue;
            }

            const Real sigma(std::sqrt(2 * D * dt())); //FIXME
            const Real3 newpos_(
                particle.position() + Real3(rng()->gaussian(sigma), rng()->gaussian(sigma), rng()->gaussian(sigma)));

            const Real constraint_radius(particle.constraint_radius());
            const Real distance_sq_from_original(
                length_sq(subtract(add(newpos_, particle.stride()), particle.original_position())));
            if (distance_sq_from_original > constraint_radius * constraint_radius)
            {
                continue;
            }

            const Real3 newpos((*world_).apply_boundary(newpos_));

            //HERE: For double-layered situation
            // if (constraint_radius <= std::max_element(edge_lengths.begin(), edge_lengths.end()))
            if (constraint_radius != std::numeric_limits<Real>::infinity())
            {
                // crowder
                const Real posx(particle.position()[0]);
                const Real newposx(newpos[0]);
                if (std::floor(posx / L) != std::floor(newposx / L))
                {
                    continue;
                }
            }
            else
            {
                //XXX: tracer
                //XXX: reflective boundary
                if (newpos_[0] < 0 || newpos_[0] >= (*world_).edge_lengths()[0])
                {
                    continue;
                }
            }
            //THERE:

            //HERE: For multi-layered situation
            // // if (constraint_radius != std::numeric_limits<Real>::infinity()
            // //     && (particle.position()[0] < L_2) != (newpos[0] < L_2))
            // // {
            // //     // crowder
            // //     continue;
            // // }
            // if (constraint_radius != std::numeric_limits<Real>::infinity())
            // {
            //     // crowder
            //     const Real posx(particle.position()[0]);
            //     const Real newposx(newpos[0]);
            //     if (posx < L)
            //     {
            //         if (newposx >= L) continue;
            //     }
            //     else if (posx < Lx - L)
            //     {
            //         if (newposx < L || Lx - L <= newposx) continue;
            //     }
            //     else
            //     {
            //         // assert(Lx - L <= posx);
            //         if (newposx < Lx - L) continue;
            //     }

            //     // if (std::floor(particle.position()[0] / L) != std::floor(newpos[0] / L))
            //     // {
            //     //     continue;
            //     // }
            // }
            // else
            // {
            //     //XXX: tracer
            //     //XXX: reflective boundary
            //     if (newpos_[0] < 0 || newpos_[0] >= (*world_).edge_lengths()[0])
            //     {
            //         continue;
            //     }
            // }
            //THERE:

            // HERE: For spherical situation
            // if (constraint_radius != std::numeric_limits<Real>::infinity())
            // {
            //     bool const cond1(length_sq(particle.position() - edge_lengths) > region_radius * region_radius);
            //     bool const cond2(length_sq(newpos - edge_lengths) > region_radius * region_radius);
            //     if (cond1 != cond2)
            //     {
            //         continue;
            //     }
            // }
            //THERE

            Particle particle_to_update(
                particle.species(), newpos,
                particle.radius(), particle.D(), particle.constraint_radius(),
                add(particle.stride(), subtract(newpos_, newpos)),
                particle.original_position());

            // if (!(*world_)._check_particles_within_radius(newpos, particle.radius(), pid))
            std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
                overlapped((*world_).list_particles_within_radius(
                               newpos, particle.radius(), pid));
            if (overlapped.size() == 0)
            {
                (*world_).update_particle_without_checking(pid, particle_to_update);
            }
            else
            {
                for (std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >::const_iterator j = overlapped.begin(); j != overlapped.end(); j++)
                {
                    std::pair<ParticleID, ParticleID> tracer_crowder_pair;

                    if (constraint_radius != std::numeric_limits<Real>::infinity())
                    {
                        if ((*j).first.second.constraint_radius() != std::numeric_limits<Real>::infinity())
                        {
                            continue;
                        }
                        else
                        {
                            tracer_crowder_pair = std::make_pair(pid, (*j).first.first);
                        }
                    }
                    else
                    {
                        if ((*j).first.second.constraint_radius() != std::numeric_limits<Real>::infinity())
                        {
                            tracer_crowder_pair = std::make_pair((*j).first.first, pid);
                        }
                        else
                        {
                            continue;
                        }
                    }

                    std::map<std::pair<ParticleID, ParticleID>, Real>::const_iterator it(first_encount.find(tracer_crowder_pair));
                    if (it == first_encount.end())
                    {
                        first_encount.insert(std::make_pair(tracer_crowder_pair, t()));
                        std::cout
                            << "#C,"
                            << tracer_crowder_pair.first.serial() << ","
                            << tracer_crowder_pair.second.serial() << ","
                            << t() << std::endl;
                    }

                    // if (constraint_radius != std::numeric_limits<Real>::infinity() && (*j).first.second.constraint_radius() != std::numeric_limits<Real>::infinity())
                    //     continue;

                    // ParticleID const _pid((*j).first.first);
                    // std::unordered_map<ParticleID, Real>::iterator it(first_encount.find(_pid));
                    // if (it == first_encount.end())
                    // {
                    //     first_encount.insert(std::make_pair(_pid, t()));
                    //     std::cout << "#ENCOUNT:" << _pid.serial() << "," << t() << std::endl;
                    // }
                }
            }
        }
    }

    set_t(t() + dt());
    num_steps_++;
}

bool BDSimulator::step(const Real& upto)
{
    const Real t0(t()), dt0(dt()), tnext(next_time());

    if (upto <= t0)
    {
        return false;
    }

    if (upto >= tnext)
    {
        step();
        return true;
    }
    else
    {
        dt_ = upto - t0;
        step();
        dt_ = dt0;
        return false;
    }
}

} // bd

} // ecell4
