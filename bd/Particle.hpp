#ifndef ECELL4_PARTICLE_HPP
#define ECELL4_PARTICLE_HPP

#include <map>
#include <functional>

#include "config.h"

#include "types.hpp"
#include "Real3.hpp"
#include "Species.hpp"
#include "Identifier.hpp"


namespace ecell4
{

class Particle;

template<typename Tstrm_, typename Ttraits_>
inline std::basic_ostream<Tstrm_, Ttraits_>& operator<<(std::basic_ostream<Tstrm_, Ttraits_>& strm, const Particle& p);

class Particle
{
public:

    typedef Real3 position_type;
    typedef Real length_type;
    typedef Real D_type;
    typedef Species species_type;
    typedef species_type::serial_type species_serial_type;
    typedef std::string Location;
    typedef Location location_type;

public:

    Particle()
    {
        ;
    }

    explicit Particle(
        const Species& sp, const Real3& pos,
        const Real& radius, const Real& D, const Real& constraint_radius,
        const Real3& stride = Real3())
        : species_(sp), position_(pos), stride_(stride), radius_(radius), D_(D), constraint_radius_(constraint_radius), original_position_(add(pos, stride)), location_("")
    {
        ;
    }

    explicit Particle(
        const Species& sp, const Real3& pos,
        const Real& radius, const Real& D, const Real& constraint_radius,
        const Real3& stride, const Real3& original_position)
        : species_(sp), position_(pos), stride_(stride), radius_(radius), D_(D), constraint_radius_(constraint_radius), original_position_(original_position), location_("")
    {
        ;
    }

    // explicit Particle(
    //     const Species& sp, const Real3& pos,
    //     const Real& radius, const Real& D, const Real& constraint_radius,
    //     const Real3& stride = Real3(), const Location& loc = "")
    //     : species_(sp), position_(pos), stride_(stride), radius_(radius), D_(D), constraint_radius_(constraint_radius), location_(loc)
    // {
    //     ;
    // }

    Real3& position()
    {
        return position_;
    }

    const Real3& position() const
    {
        return position_;
    }

    Real3& stride()
    {
        return stride_;
    }

    const Real3& stride() const
    {
        return stride_;
    }

    Real3& original_position()
    {
        return original_position_;
    }

    const Real3& original_position() const
    {
        return original_position_;
    }

    Real& radius()
    {
        return radius_;
    }

    const Real& radius() const
    {
        return radius_;
    }

    Real& D()
    {
        return D_;
    }

    const Real& D() const
    {
        return D_;
    }

    Real& constraint_radius()
    {
        return constraint_radius_;
    }

    const Real& constraint_radius() const
    {
        return constraint_radius_;
    }

    Species& species()
    {
        return species_;
    }

    const Species& species() const
    {
        return species_;
    }

    Location& location()
    {
        return location_;
    }

    const Location& location() const
    {
        return location_;
    }

    Species::serial_type species_serial()
    {
        return species_.serial();
    }

    const Species::serial_type species_serial() const
    {
        return species_.serial();
    }

    inline Species::serial_type sid()
    {
        return species_serial();
    }

    inline const Species::serial_type sid() const
    {
        return species_serial();
    }

    bool operator==(Particle const& rhs) const
    {
        return (this->sid() == rhs.sid() &&
                this->radius() == rhs.radius() &&
                this->position() == rhs.position() &&
                this->location() == rhs.location());
    }

    bool operator!=(Particle const& rhs) const
    {
        return !operator==(rhs);
    }

    std::string show(int precision)
    {
        std::ostringstream strm;
        strm.precision(precision);
        strm << *this;
        return strm.str();
    }

private:

    Species species_;
    Real3 position_, stride_;
    Real radius_, D_, constraint_radius_;
    Real3 original_position_;
    Location location_;
};

template<typename Tstrm_, typename Ttraits_>
inline std::basic_ostream<Tstrm_, Ttraits_>& operator<<(std::basic_ostream<Tstrm_, Ttraits_>& strm, const Particle& p)
{
    strm << "Particle(" << "{ " << p.position() << ", " << p.radius() << "}, " << ", D=" << p.D() << ", " << p.sid() << ", " << p.location() << ")";
    return strm;
}

} // ecell4

namespace std {

template<>
struct hash<ecell4::Particle>
{
    typedef ecell4::Particle argument_type;

    std::size_t operator()(argument_type const& val)
    {
        return hash<argument_type::position_type>()(val.position()) ^
            hash<argument_type::length_type>()(val.radius()) ^
            hash<argument_type::D_type>()(val.D()) ^
            // hash<argument_type::species_type>()(val.species());
            hash<argument_type::species_serial_type>()(val.sid());
    }
};
} // std

#endif /* ECELL4_PARTICLE_HPP */
