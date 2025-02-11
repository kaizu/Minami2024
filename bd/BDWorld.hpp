#ifndef ECELL4_BD_BD_WORLD_HPP
#define ECELL4_BD_BD_WORLD_HPP

#include <memory>
#include <sstream>

#include "./exceptions.hpp"
#include "./extras.hpp"
#include "./RandomNumberGenerator.hpp"
#include "./SerialIDGenerator.hpp"
#include "./ParticleSpace.hpp"
#include "./ParticleSpaceCellListImpl.hpp"
#include "./Model.hpp"
// #include "./WorldInterface.hpp"


namespace ecell4
{

namespace bd
{

struct MoleculeInfo
{
    const Real radius;
    const Real D;
    const Real constraint_radius;
};

class BDWorld
//     : public WorldInterface
{
public:

    typedef MoleculeInfo molecule_info_type;
    typedef ParticleSpaceCellListImpl particle_space_type;
    // typedef ParticleSpaceVectorImpl particle_space_type;
    typedef particle_space_type::particle_container_type particle_container_type;

public:

    BDWorld(const Real3& edge_lengths = Real3(1, 1, 1),
        const Integer3& matrix_sizes = Integer3(3, 3, 3))
        : ps_(new particle_space_type(edge_lengths, matrix_sizes))
    {
        rng_ = std::shared_ptr<RandomNumberGenerator>(
            new GSLRandomNumberGenerator());
        (*rng_).seed();
    }

    BDWorld(
        const Real3& edge_lengths, const Integer3& matrix_sizes,
        std::shared_ptr<RandomNumberGenerator> rng)
        : ps_(new particle_space_type(edge_lengths, matrix_sizes)), rng_(rng)
    {
        ;
    }

    BDWorld(const std::string& filename)
        : ps_(new particle_space_type(Real3(1, 1, 1)))
    {
        rng_ = std::shared_ptr<RandomNumberGenerator>(
            new GSLRandomNumberGenerator());
        this->load(filename);
    }

    /**
     * create and add a new particle
     * @param p a particle
     * @return a pair of a pair of pid (a particle id) and p (a particle)
     * and bool (if it's succeeded or not)
     */
    std::pair<std::pair<ParticleID, Particle>, bool>
    new_particle(const Particle& p)
    {
        ParticleID pid(pidgen_());
        // if (has_particle(pid))
        // {
        //     throw AlreadyExists("particle already exists");
        // }
        if (list_particles_within_radius(p.position(), p.radius()).size() == 0)
        {
            (*ps_).update_particle(pid, p); //XXX: DONOT call this->update_particle
            return std::make_pair(std::make_pair(pid, p), true);
        }
        else
        {
            return std::make_pair(std::make_pair(pid, p), false);
        }
    }

    std::pair<std::pair<ParticleID, Particle>, bool>
    new_particle(const Species& sp, const Real3& pos)
    {
        const MoleculeInfo info(get_molecule_info(sp));
        return new_particle(Particle(sp, pos, info.radius, info.D, info.constraint_radius));
    }

    /**
     * draw attributes of species and return it as a molecule info.
     * @param sp a species
     * @return info a molecule info
     */
    MoleculeInfo get_molecule_info(const Species& sp) const
    {
        Real radius(0.0), D(0.0), constraint_radius(std::numeric_limits<Real>::infinity());

        if (std::shared_ptr<Model> bound_model = lock_model())
        {
            Species newsp(bound_model->apply_species_attributes(sp));

            if (newsp.has_attribute("radius"))
            {
                radius = newsp.get_attribute_as<Real>("radius");
            }
            if (newsp.has_attribute("D"))
            {
                D = newsp.get_attribute_as<Real>("D");
            }
            if (newsp.has_attribute("constraint_radius"))
            {
                constraint_radius = newsp.get_attribute_as<Real>("constraint_radius");
            }
        }
        else
        {
            if (sp.has_attribute("radius"))
            {
                radius = sp.get_attribute_as<Real>("radius");
            }
            if (sp.has_attribute("D"))
            {
                D = sp.get_attribute_as<Real>("D");
            }
            if (sp.has_attribute("constraint_radius"))
            {
                constraint_radius = sp.get_attribute_as<Real>("constraint_radius");
            }
        }

        if (radius <= 0.0)
        {
            std::stringstream msg;
            msg << "A particle with invalid size [" << radius << "] was given.";
            throw IllegalArgument(msg.str());
        }

        MoleculeInfo info = {radius, D, constraint_radius};
        return info;
    }

    const Real t() const
    {
        return (*ps_).t();
    }

    void set_t(const Real& t)
    {
        (*ps_).set_t(t);
    }

    const Real3& edge_lengths() const
    {
        return (*ps_).edge_lengths();
    }

    Integer num_particles() const
    {
        return (*ps_).num_particles();
    }

    Integer num_particles(const Species& species) const
    {
        return (*ps_).num_particles(species);
    }

    Integer num_particles_exact(const Species& species) const
    {
        return (*ps_).num_particles_exact(species);
    }

    bool has_particle(const ParticleID& pid) const
    {
        return (*ps_).has_particle(pid);
    }

    std::vector<std::pair<ParticleID, Particle> > list_particles() const
    {
        return (*ps_).list_particles();
    }

    std::vector<std::pair<ParticleID, Particle> >
    list_particles(const Species& sp) const
    {
        return (*ps_).list_particles(sp);
    }

    std::vector<std::pair<ParticleID, Particle> >
    list_particles_exact(const Species& sp) const
    {
        return (*ps_).list_particles_exact(sp);
    }

    std::vector<Species> list_species() const
    {
        return (*ps_).list_species();
    }

    virtual Real get_value(const Species& sp) const
    {
        return static_cast<Real>(num_molecules(sp));
    }

    virtual Real get_value_exact(const Species& sp) const
    {
        return static_cast<Real>(num_molecules_exact(sp));
    }

    bool update_particle_without_checking(const ParticleID& pid, const Particle& p)
    {
        return (*ps_).update_particle(pid, p);
    }

    bool update_particle(const ParticleID& pid, const Particle& p)
    {
        if (list_particles_within_radius(p.position(), p.radius(), pid).size()
            == 0)
        {
            return (*ps_).update_particle(pid, p);
        }
        else
        {
            return true;
        }
    }

    std::pair<ParticleID, Particle> const&
    _get_particle(const size_t idx) const
    {
        return (*ps_)._get_particle(idx);
    }

    std::pair<ParticleID, Particle>
    get_particle(const ParticleID& pid) const
    {
        return (*ps_).get_particle(pid);
    }

    void remove_particle(const ParticleID& pid)
    {
        (*ps_).remove_particle(pid);
    }

    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
    list_particles_within_radius(
        const Real3& pos, const Real& radius) const
    {
        return (*ps_).list_particles_within_radius(pos, radius);
    }

    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
    list_particles_within_radius(
        const Real3& pos, const Real& radius, const ParticleID& ignore) const
    {
        return (*ps_).list_particles_within_radius(pos, radius, ignore);
    }

    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
    list_particles_within_radius(
        const Real3& pos, const Real& radius,
        const ParticleID& ignore1, const ParticleID& ignore2) const
    {
        return (*ps_).list_particles_within_radius(pos, radius, ignore1, ignore2);
    }

    bool _check_particles_within_radius(
        const Real3& pos, const Real& radius, const ParticleID& ignore) const
    {
        return (*ps_)._check_particles_within_radius(pos, radius, ignore);
    }

    inline Real3 periodic_transpose(
        const Real3& pos1, const Real3& pos2) const
    {
        return (*ps_).periodic_transpose(pos1, pos2);
    }

    inline Real3 apply_boundary(const Real3& pos) const
    {
        return (*ps_).apply_boundary(pos);
    }

    inline Real distance_sq(const Real3& pos1, const Real3& pos2) const
    {
        return (*ps_).distance_sq(pos1, pos2);
    }

    inline Real distance(const Real3& pos1, const Real3& pos2) const
    {
        return (*ps_).distance(pos1, pos2);
    }

    Integer num_molecules(const Species& sp) const
    {
        return (*ps_).num_molecules(sp);
    }

    Integer num_molecules_exact(const Species& sp) const
    {
        return (*ps_).num_molecules_exact(sp);
    }

    void add_molecules(const Species& sp, const Integer& num)
    {
        extras::throw_in_particles(*this, sp, num, rng());
    }

    void add_molecules(const Species& sp, const Integer& num, const std::shared_ptr<Shape> shape)
    {
        // extras::throw_in_particles(*this, sp, num, shape, rng());
        extras::throw_in_particles_wo_draw_position(*this, sp, num, shape, rng());
    }

    void remove_molecules(const Species& sp, const Integer& num)
    {
        if (num < 0)
        {
            throw std::invalid_argument(
                "The number of molecules must be positive.");
        }

        std::vector<std::pair<ParticleID, Particle> >
            particles(list_particles(sp));
        const Integer num_particles(particles.size());
        if (num_particles < num)
        {
            throw std::invalid_argument(
                "The number of molecules cannot be negative.");
        }

        shuffle((*rng_), particles);
        for (std::vector<std::pair<ParticleID, Particle> >::const_iterator
            i(particles.begin()); i != particles.begin() + num; ++i)
        {
            remove_particle((*i).first);
        }
    }

    const Real volume() const
    {
        const Real3& lengths(edge_lengths());
        return lengths[0] * lengths[1] * lengths[2];
    }

    inline std::shared_ptr<RandomNumberGenerator>& rng()
    {
        return rng_;
    }

    const particle_container_type& particles() const
    {
        return (*ps_).particles();
    }

    void save(const std::string& filename) const
    {
#ifdef WITH_HDF5
        std::unique_ptr<H5::H5File>
            fout(new H5::H5File(filename.c_str(), H5F_ACC_TRUNC));
        rng_->save(fout.get());
        pidgen_.save(fout.get());
        std::unique_ptr<H5::Group>
            group(new H5::Group(fout->createGroup("ParticleSpace")));
        ps_->save_hdf5(group.get());
        extras::save_version_information(fout.get(), std::string("ecell4-bd-") + std::string(VERSION_INFO));
#else
        throw NotSupported(
            "This method requires HDF5. The HDF5 support is turned off.");
#endif
    }

    void load(const std::string& filename)
    {
#ifdef WITH_HDF5
        std::unique_ptr<H5::H5File>
            fin(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));

        const std::string required = "ecell4-bd-0.0";
        try
        {
            const std::string version = extras::load_version_information(*fin);
            if (!extras::check_version_information(version, required))
            {
                std::stringstream ss;
                ss << "The version of the given file [" << version
                    << "] is too old. [" << required << "] or later is required.";
                throw NotSupported(ss.str());
            }
        }
        catch(H5::GroupIException not_found_error)
        {
            throw NotFound("No version information was found.");
        }

        const H5::Group group(fin->openGroup("ParticleSpace"));
        ps_->load_hdf5(group);
        pidgen_.load(*fin);
        rng_->load(*fin);
#else
        throw NotSupported(
            "This method requires HDF5. The HDF5 support is turned off.");
#endif
    }

    void bind_to(std::shared_ptr<Model> model)
    {
        if (std::shared_ptr<Model> bound_model = lock_model())
        {
            if (bound_model.get() != model.get())
            {
                std::cerr << "Warning: Model already bound to BDWorld"
                    << std::endl;
            }
        }

        model_ = model;
    }

    std::shared_ptr<Model> lock_model() const
    {
        return model_.lock();
    }

protected:

    std::unique_ptr<ParticleSpace> ps_;
    std::shared_ptr<RandomNumberGenerator> rng_;
    SerialIDGenerator<ParticleID> pidgen_;

    std::weak_ptr<Model> model_;
};

} // bd

} // ecell4

#endif /* ECELL4_BD_BD_WORLD_HPP */
