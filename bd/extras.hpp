#ifndef ECELL4_EXTRAS_HPP
#define ECELL4_EXTRAS_HPP

#include <memory>
#include "config.h"

#ifdef WITH_HDF5
#include <hdf5.h>
#include <H5Cpp.h>
#endif

#include "types.hpp"
#include "Real3.hpp"
#include "Species.hpp"
#include "Particle.hpp"
#include "AABB.hpp"
#include "Model.hpp"
#include "Shape.hpp"


namespace ecell4
{

namespace extras
{

template<typename Tworld_, typename Trng_>
void throw_in_particles(
    Tworld_& world, const Species& sp, const Integer& N,
    const std::shared_ptr<Shape> shape,
    std::shared_ptr<Trng_>& rng)
{
    typedef typename Tworld_::molecule_info_type molecule_info_type;
    std::shared_ptr<RandomNumberGenerator>
        myrng(static_cast<std::shared_ptr<RandomNumberGenerator> >(rng));

    if (N < 0)
    {
        throw std::invalid_argument("the number of particles must be positive.");
    }

    // const Real3 edge_lengths(world.edge_lengths());
    const molecule_info_type info(world.get_molecule_info(sp));

    for (int i(0); i < N; ++i)
    {
        while (true)
        {
            // const Real3 pos(
            //     rng.uniform(0.0, edge_lengths[0]),
            //     rng.uniform(0.0, edge_lengths[1]),
            //     rng.uniform(0.0, edge_lengths[2]));
            // if (world.list_particles_within_radius(pos, info.radius).size()
            //     == 0)
            // {
            //     world.new_particle(Particle(sp, pos, info.radius, info.D));
            //     break;
            // }
            const Real3 pos(shape->draw_position(myrng));
            if (world.new_particle(Particle(sp, pos, info.radius, info.D, info.constraint_radius)).second)
            {
                break;
            }
        }
    }
}

template<typename Tworld_, typename Trng_>
void throw_in_particles_wo_draw_position(
    Tworld_& world, const Species& sp, const Integer& N,
    const std::shared_ptr<Shape> shape,
    std::shared_ptr<Trng_>& rng)
{
    typedef typename Tworld_::molecule_info_type molecule_info_type;
    std::shared_ptr<RandomNumberGenerator>
        myrng(static_cast<std::shared_ptr<RandomNumberGenerator> >(rng));

    if (N < 0)
    {
        throw std::invalid_argument("the number of particles must be positive.");
    }

    const Real3 edge_lengths(world.edge_lengths());
    const molecule_info_type info(world.get_molecule_info(sp));

    for (int i(0); i < N; ++i)
    {
        while (true)
        {
            const Real3 pos(
                rng->uniform(0.0, edge_lengths[0]),
                rng->uniform(0.0, edge_lengths[1]),
                rng->uniform(0.0, edge_lengths[2]));
            if (shape->is_inside(pos) <= 0)
            {
                if (world.new_particle(Particle(sp, pos, info.radius, info.D, info.constraint_radius)).second)
                {
                    break;
                }
            }
        }
    }
}

template<typename Tworld_, typename Trng_>
void throw_in_particles(
    Tworld_& world, const Species& sp, const Integer& N, std::shared_ptr<Trng_>& rng)
{
    std::shared_ptr<Shape> shape(new AABB(Real3(0, 0, 0), world.edge_lengths()));
    throw_in_particles(world, sp, N, shape, rng);
}

template<typename Tfactory_>
typename Tfactory_::world_type* generate_world_from_model(
    const Tfactory_& f, const std::shared_ptr<Model>& m)
{
    typename Tfactory_::world_type* w = f.world();
    w->bind_to(m);
    return w;
}

Shape::dimension_kind
get_dimension_from_model(const Species& species, const std::shared_ptr<Model>& model);

struct VersionInformation
{
    typedef enum PreRelease
    {
        NONE = 0,
        ALPHA = 1,
        BETA = 2,
        RC = 3,
        FINAL = 4
    } prerelease_type;

    std::string header;
    unsigned int majorno, minorno, patchno;
    prerelease_type pre;
    unsigned int preno;
    int devno;

    VersionInformation(
        const std::string& header, const unsigned int majorno, const unsigned int minorno, const unsigned int patchno,
        const prerelease_type pre, const unsigned int preno, const int devno)
        : header(header), majorno(majorno), minorno(minorno), patchno(patchno),
        pre(pre), preno(preno), devno(devno)
    {
        ;
    }
};

VersionInformation parse_version_information(const std::string& version);
bool check_version_information(const std::string& version, const std::string& required);

#ifdef WITH_HDF5
void save_version_information(H5::H5Location* root, const std::string& version);
std::string load_version_information(const H5::H5Location& root);
#endif
std::string load_version_information(const std::string& filename);

std::vector<std::vector<Real> > get_stoichiometry(
    const std::vector<Species>& species_list, const std::vector<ReactionRule>& reaction_rules);

} // extras

} // ecell4

#endif // ECELL4_EXTRAS_HPP
