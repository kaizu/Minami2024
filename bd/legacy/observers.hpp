#ifndef ECELL4_OBSERVER_HPP
#define ECELL4_OBSERVER_HPP

#include "config.h"

#include "types.hpp"
#include "functions.hpp"
// #include "Space.hpp"
#include "Species.hpp"
#include "Real3.hpp"
#include "Model.hpp"
#include "Simulator.hpp"
#include "WorldInterface.hpp"

#include <fstream>
#include <boost/format.hpp>

#include <chrono>


namespace ecell4
{

class Observer
{
public:

    Observer(const bool e, const Integer num_steps = 0)
        : every_(e), num_steps_(num_steps)
    {
        ;
    }

    virtual ~Observer()
    {
        ; // do nothing
    }

    virtual const Real next_time() const;
    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual void finalize(const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    // virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world) = 0;

    const Integer num_steps() const;
    void set_num_steps(const Integer nsteps);

    bool every()
    {
        return every_;
    }

private:

    const bool every_;

protected:

    Integer num_steps_;
};

class FixedIntervalObserver
    : public Observer
{
public:

    typedef Observer base_type;

public:

    FixedIntervalObserver(const Real& dt)
        : base_type(false), t0_(0.0), dt_(dt), count_(0)
    {
        ;
    }

    FixedIntervalObserver(const Real& dt, const Real& t0, const Integer count)
        : base_type(false), t0_(t0), dt_(dt), count_(count)
    {
        ;
    }

    virtual ~FixedIntervalObserver()
    {
        ;
    }

    const Real next_time() const;

    const Real dt() const;
    const Real t0() const;
    const Integer count() const;
    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

protected:

    Real t0_, dt_;
    Integer count_;
};

struct NumberLogger
{
    typedef std::vector<std::vector<Real> > data_container_type;
    typedef std::vector<Species> species_container_type;

    NumberLogger()
        : all_species(true)
    {
        ;
    }

    NumberLogger(const std::vector<std::string>& species)
        : all_species(species.size() == 0)
    {
        targets.reserve(species.size());
        for (std::vector<std::string>::const_iterator i(species.begin());
            i != species.end(); ++i)
        {
            targets.push_back(Species(*i));
        }
    }

    ~NumberLogger()
    {
        ;
    }

    void initialize()
    {
        ;
    }

    void reset()
    {
        data.clear();
    }

    void log(const std::shared_ptr<WorldInterface>& world);
    void save(const std::string& filename) const;

    species_container_type targets;
    data_container_type data;
    bool all_species;
};

class FixedIntervalNumberObserver
    : public FixedIntervalObserver
{
public:

    typedef FixedIntervalObserver base_type;

public:

    FixedIntervalNumberObserver(const Real& dt, const std::vector<std::string>& species)
        : base_type(dt), logger_(species)
    {
        ;
    }

    FixedIntervalNumberObserver(const Real& dt)
        : base_type(dt), logger_()
    {
        ;
    }

    FixedIntervalNumberObserver(const Real& dt, const Real& t0, const Integer count)
        : base_type(dt, t0, count), logger_()
    {
        ;
    }

    virtual ~FixedIntervalNumberObserver()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();
    NumberLogger::data_container_type data() const;
    NumberLogger::species_container_type targets() const;

    const NumberLogger& logger() const
    {
        return logger_;
    }

    void set_logger(const NumberLogger& logger)
    {
        logger_ = logger;
    }

    void save(const std::string& filename) const
    {
        logger_.save(filename);
    }

protected:

    NumberLogger logger_;
};

class NumberObserver
    : public Observer
{
public:

    typedef Observer base_type;

public:

    NumberObserver(const std::vector<std::string>& species)
        : base_type(true), logger_(species)
    {
        ;
    }

    NumberObserver()
        : base_type(true), logger_()
    {
        ;
    }

    virtual ~NumberObserver()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual void finalize(const std::shared_ptr<WorldInterface>& world);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();
    NumberLogger::data_container_type data() const;
    NumberLogger::species_container_type targets() const;

    const NumberLogger& logger() const
    {
        return logger_;
    }

    void set_logger(const NumberLogger& logger)
    {
        logger_ = logger;
    }

    void save(const std::string& filename) const
    {
        logger_.save(filename);
    }

protected:

    NumberLogger logger_;
};

class TimingObserver
    : public Observer
{
public:

    typedef Observer base_type;

public:

    TimingObserver(const std::vector<Real>& t)
        : base_type(false), t_(t), count_(0)
    {
        ;
    }

    TimingObserver(const std::vector<Real>& t, const Integer num_steps, const Integer count)
        : base_type(false, num_steps), t_(t), count_(count)
    {
        ;
    }

    virtual ~TimingObserver()
    {
        ;
    }

    const Real next_time() const;

    const std::vector<Real>& timings() const
    {
        return t_;
    }

    const Integer count() const
    {
        return count_;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

protected:

    std::vector<Real> t_;
    Integer count_;
};

class TimingNumberObserver
    : public TimingObserver
{
public:

    typedef TimingObserver base_type;

public:

    TimingNumberObserver(
        const std::vector<Real>& t, const std::vector<std::string>& species)
        : base_type(t), logger_(species)
    {
        ;
    }

    TimingNumberObserver(const std::vector<Real>& t)
        : base_type(t), logger_()
    {
        ;
    }

    TimingNumberObserver(const std::vector<Real>& t, const Integer num_steps, const Integer count)
        : base_type(t, num_steps, count), logger_()
    {
        ;
    }

    virtual ~TimingNumberObserver()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();
    NumberLogger::data_container_type data() const;
    NumberLogger::species_container_type targets() const;

    const NumberLogger& logger() const
    {
        return logger_;
    }

    void set_logger(const NumberLogger& logger)
    {
        logger_ = logger;
    }

    void save(const std::string& filename) const
    {
        logger_.save(filename);
    }

protected:

    NumberLogger logger_;
};

class FixedIntervalHDF5Observer
    : public FixedIntervalObserver
{
public:

    typedef FixedIntervalObserver base_type;

public:

    FixedIntervalHDF5Observer(const Real& dt, const std::string& filename)
        : base_type(dt), prefix_(filename)
    {
        ;
    }

    FixedIntervalHDF5Observer(const Real& dt, const Real& t0, const Integer count, const std::string& filename)
        : base_type(dt, t0, count), prefix_(filename)
    {
        ;
    }

    virtual ~FixedIntervalHDF5Observer()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);

    inline const std::string filename() const
    {
        return filename(num_steps());
    }

    const std::string filename(const Integer idx) const;

    const std::string& prefix() const
    {
        return prefix_;
    }

protected:

    std::string prefix_;
};

struct PositionLogger
{
    typedef std::vector<std::pair<ParticleID, Particle> >
        particle_container_type;
    typedef std::unordered_map<Species::serial_type, unsigned int>
        serial_map_type;

    PositionLogger(const std::vector<std::string>& species)
        : species(species), header("x,y,z,r,sid"), formatter("%2%,%3%,%4%,%5%,%8%"), serials()
    {
        ;
    }

    PositionLogger()
        : species(), header("x,y,z,r,sid"), formatter("%2%,%3%,%4%,%5%,%8%"), serials()
    {
        ;
    }

    ~PositionLogger()
    {
        ;
    }

    void initialize()
    {
        ;
    }

    void reset()
    {
        ;
    }

    void write_particles(
        std::ofstream& ofs, const Real t, const particle_container_type& particles,
        const Species::serial_type label = "")
    {
        for(particle_container_type::const_iterator i(particles.begin());
            i != particles.end(); ++i)
        {
            const ParticleID& pid((*i).first);
            const Real3 pos((*i).second.position());
            const Real radius((*i).second.radius());
            const Species::serial_type serial(
                label == "" ? (*i).second.species_serial() : label);

            unsigned int idx;
            serial_map_type::iterator j(serials.find(serial));
            if (j == serials.end())
            {
                idx = serials.size();
                serials.insert(std::make_pair(serial, idx));
            }
            else
            {
                idx = (*j).second;
            }

            boost::format fmt(formatter);
            ofs << (fmt % t % pos[0] % pos[1] % pos[2] % radius
                    % pid.lot() % pid.serial() % idx).str() << std::endl;
        }
    }

    void save(std::ofstream& ofs, const std::shared_ptr<WorldInterface>& world)
    {
        ofs << std::setprecision(17);

        if (header.size() > 0)
        {
            ofs << header << std::endl;
        }

        if (species.size() == 0)
        {
            const particle_container_type particles(world->list_particles());
            write_particles(ofs, world->t(), particles);
        }
        else
        {
            for (std::vector<std::string>::const_iterator i(species.begin());
                i != species.end(); ++i)
            {
                const Species sp(*i);
                const particle_container_type particles(world->list_particles(sp));
                write_particles(ofs, world->t(), particles, *i);
            }
        }
    }

    std::vector<std::string> species;
    std::string header, formatter;
    serial_map_type serials;
};

class FixedIntervalCSVObserver
    : public FixedIntervalObserver
{
public:

    typedef FixedIntervalObserver base_type;

    typedef std::vector<std::pair<ParticleID, Particle> >
        particle_container_type;
    typedef std::unordered_map<Species::serial_type, unsigned int>
        serial_map_type;

public:

    FixedIntervalCSVObserver(
        const Real& dt, const std::string& filename)
        : base_type(dt), prefix_(filename), logger_()
    {
        ;
    }

    FixedIntervalCSVObserver(
        const Real& dt, const std::string& filename,
        const std::vector<std::string>& species)
        : base_type(dt), prefix_(filename), logger_(species)
    {
        ;
    }

    FixedIntervalCSVObserver(
        const Real& dt, const std::string& filename,
        const Real& t0, const Integer count)
        : base_type(dt, t0, count), prefix_(filename), logger_()
    {
        ;
    }

    virtual ~FixedIntervalCSVObserver()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    void log(const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

    void set_header(const std::string& header)
    {
        logger_.header = header;
    }

    void set_formatter(const std::string& formatter)
    {
        logger_.formatter = formatter;
    }

    const PositionLogger& logger() const
    {
        return logger_;
    }

    void set_logger(const PositionLogger& logger)
    {
        logger_ = logger;
    }

    const std::string& prefix() const
    {
        return prefix_;
    }

    inline const std::string filename() const
    {
        return filename(num_steps());
    }

    const std::string filename(const Integer idx) const;

protected:

    std::string prefix_;
    PositionLogger logger_;
};

class CSVObserver
    : public Observer
{
public:

    typedef Observer base_type;

    typedef std::vector<std::pair<ParticleID, Particle> >
        particle_container_type;
    typedef std::unordered_map<Species::serial_type, unsigned int>
        serial_map_type;

public:

    CSVObserver(
        const std::string& filename)
        : base_type(true), prefix_(filename), logger_()
    {
        ;
    }

    CSVObserver(
        const std::string& filename,
        const std::vector<std::string>& species)
        : base_type(true), prefix_(filename), logger_(species)
    {
        ;
    }

    virtual ~CSVObserver()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    void log(const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

    void set_header(const std::string& header)
    {
        logger_.header = header;
    }

    void set_formatter(const std::string& formatter)
    {
        logger_.formatter = formatter;
    }

    const PositionLogger& logger() const
    {
        return logger_;
    }

    void set_logger(const PositionLogger& logger)
    {
        logger_ = logger;
    }

    const std::string& prefix() const
    {
        return prefix_;
    }

    inline const std::string filename() const
    {
        return filename(num_steps());
    }

    const std::string filename(const Integer idx) const;

protected:

    std::string prefix_;
    PositionLogger logger_;
};

struct TimingEvent
{
    TimingEvent(const std::vector<Real>& times)
        : times(times), num_steps(0), count(0)
    {
        ;
    }

    TimingEvent()
        : times(), num_steps(0), count(0)
    {
        ;
    }

    virtual ~TimingEvent()
    {
        ;
    }

    void set_times(const std::vector<Real>& value)
    {
        times = value;
    }

    const Real next_time() const
    {
        if (count < times.size())
        {
            return times[count];
        }
        return std::numeric_limits<Real>::infinity();
    }

    void reset()
    {
        num_steps = 0;
        count = 0;
    }

    void initialize(const Real t)
    {
        while (next_time() < t)
        {
            ++count;
        }
    }

    void fire()
    {
        ++num_steps;
        ++count;
    }

public:

    std::vector<Real> times;
    size_t num_steps;
    size_t count;
};

struct FixedIntervalEvent
{
    FixedIntervalEvent(const Real& dt = std::numeric_limits<Real>::infinity())
        : t0(0.0), dt(dt), num_steps(0), count(0)
    {
        ;
    }

    virtual ~FixedIntervalEvent()
    {
        ;
    }

    void set_dt(const Real& value)
    {
        dt = value;
    }

    const Real next_time() const
    {
        return t0 + dt * count;
    }

    void reset()
    {
        num_steps = 0;
        count = 0;
        t0 = 0; //DUMMY
    }

    void initialize(const Real t)
    {
        if (dt <= 0.0)
        {
            throw std::invalid_argument(
                "A step interval must be positive.");
        }

        if (count == 0)
        {
            t0 = t;
        }
        else
        {
            while (next_time() < t)
            {
                ++count;
            }
        }
    }

    void fire()
    {
        ++num_steps;
        ++count;
    }

public:

    Real t0, dt;
    size_t num_steps;
    size_t count;
};

template <typename Tevent_>
class TrajectoryObserver
    : public Observer
{
public:

    typedef Observer base_type;
    typedef Tevent_ event_type;

public:

    TrajectoryObserver(
        const std::vector<ParticleID>& pids,
        const bool resolve_boundary = default_resolve_boundary(),
        const Real subdt = default_subdt())
        : base_type(false), event_(), subevent_(subdt > 0 ? subdt : std::numeric_limits<Real>::infinity()),
        pids_(pids), resolve_boundary_(resolve_boundary), prev_positions_(pids.size()),
        trajectories_(pids.size()), strides_(pids.size()), t_()
    {
        ;
    }

    TrajectoryObserver(
        const bool resolve_boundary = default_resolve_boundary(),
        const Real subdt = default_subdt())
        : base_type(false), event_(), subevent_(subdt > 0 ? subdt : std::numeric_limits<Real>::infinity()),
        pids_(), resolve_boundary_(resolve_boundary), prev_positions_(),
        trajectories_(), strides_(), t_()
    {
        ;
    }

    TrajectoryObserver(
        const std::vector<ParticleID>& pids,
        const bool resolve_boundary,
        const Real subdt,
        const std::vector<Real3>& prev_positions,
        const std::vector<std::vector<Real3> >& trajectories,
        const std::vector<Real3>& strides,
        const std::vector<Real>& t
        )
        : base_type(false), event_(), subevent_(subdt > 0 ? subdt : std::numeric_limits<Real>::infinity()),
        pids_(pids), resolve_boundary_(resolve_boundary), prev_positions_(prev_positions),
        trajectories_(trajectories), strides_(strides), t_(t)
    {
        assert(pids_.size() == prev_positions_.size());
        assert(pids_.size() == trajectories_.size());
        assert(pids_.size() == strides_.size());
    }

    virtual ~TrajectoryObserver()
    {
        ;
    }

    static inline bool default_resolve_boundary()
    {
        return true;
    }

    static inline const Real default_subdt()
    {
        return 0;
    }

    const Real next_time() const
    {
        return std::min(event_.next_time(), subevent_.next_time());
    }

    const Integer num_steps() const
    {
        return event_.num_steps + subevent_.num_steps;
    }

    const Integer count() const
    {
        return event_.count;
    }

    void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model)
    {
        event_.initialize(world->t());
        subevent_.initialize(world->t());

        typedef std::vector<std::pair<ParticleID, Particle> > particle_id_pairs;
        if (pids_.size() == 0)
        {
            particle_id_pairs const particles(world->list_particles());
            pids_.reserve(particles.size());
            for (particle_id_pairs::const_iterator i(particles.begin());
                i != particles.end(); ++i)
            {
                if ((*i).second.D() > 0)
                {
                    pids_.push_back((*i).first);
                }
            }
        }

        prev_positions_.resize(pids_.size());
        trajectories_.resize(pids_.size());
        strides_.resize(pids_.size());
    }

    bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world)
    {
        if (subevent_.next_time() <= event_.next_time())
        {
            fire_subevent(sim, world);
        }
        else
        {
            fire_event(sim, world);
        }
        return true;
    }

    void reset()
    {
        prev_positions_.clear();
        prev_positions_.resize(pids_.size(), Real3(0, 0, 0));
        trajectories_.clear();
        trajectories_.resize(pids_.size(), std::vector<Real3>());
        strides_.clear();
        strides_.resize(pids_.size(), Real3(0, 0, 0));
        t_.clear();
    }

    const event_type& event() const
    {
        return event_;
    }

    void set_event(const event_type& event)
    {
        event_ = event;
    }

    const FixedIntervalEvent& subevent() const
    {
        return subevent_;
    }

    void set_subevent(const FixedIntervalEvent& subevent)
    {
        subevent_ = subevent;
    }

    const std::vector<ParticleID>& pids() const
    {
        return pids_;
    }

    const bool resolve_boundary() const
    {
        return resolve_boundary_;
    }

    const std::vector<Real3>& prev_positions() const
    {
        return prev_positions_;
    }

    const std::vector<std::vector<Real3> >& data() const
    {
        return trajectories_;
    }

    const std::vector<Real3>& strides() const
    {
        return strides_;
    }

    const std::vector<Real>& t() const
    {
        return t_;
    }

    const Integer num_tracers() const
    {
        return pids_.size();
    }

protected:

    void fire_event(const Simulator* sim, const std::shared_ptr<WorldInterface>& world)
    {
        t_.push_back(world->t());

        const Real3 edge_lengths(world->edge_lengths());
        std::vector<Real3>::const_iterator j(prev_positions_.begin());
        std::vector<Real3>::const_iterator k(strides_.begin());
        std::vector<std::vector<Real3> >::iterator l(trajectories_.begin());
        for (std::vector<ParticleID>::const_iterator i(pids_.begin());
            i != pids_.end(); ++i)
        {
            if (world->has_particle(*i))
            {
                const Real3& stride(*k);
                Real3 pos(stride + world->get_particle(*i).second.position());

                if (resolve_boundary_ && subevent_.num_steps > 0)
                {
                    const Real3& prev(*j);

                    for (unsigned int dim(0); dim != 3; ++dim)
                    {
                        const Real L(edge_lengths[dim]);
                        if (pos[dim] - prev[dim] >= L * 0.5)
                        {
                            pos[dim] -= L;
                        }
                        else if (pos[dim] - prev[dim] <= L * -0.5)
                        {
                            pos[dim] += L;
                        }
                    }
                }

                (*l).push_back(pos);
            }
            ++j;
            ++k;
            ++l;
        }

        event_.fire();
    }

    void fire_subevent(const Simulator* sim, const std::shared_ptr<WorldInterface>& world)
    {
        if (resolve_boundary_)
        {
            const Real3 edge_lengths(world->edge_lengths());
            std::vector<Real3>::iterator j(prev_positions_.begin());
            std::vector<Real3>::iterator k(strides_.begin());
            for (std::vector<ParticleID>::const_iterator i(pids_.begin());
                i != pids_.end(); ++i)
            {
                if (world->has_particle(*i))
                {
                    Real3& stride(*k);
                    Real3 pos(stride + world->get_particle(*i).second.position());
                    if (subevent_.num_steps > 0)
                    {
                        const Real3& prev(*j);
                        for (unsigned int dim(0); dim != 3; ++dim)
                        {
                            const Real L(edge_lengths[dim]);
                            if (pos[dim] - prev[dim] >= L * 0.5)
                            {
                                stride[dim] -= L;
                                pos[dim] -= L;
                            }
                            else if (pos[dim] - prev[dim] <= L * -0.5)
                            {
                                stride[dim] += L;
                                pos[dim] += L;
                            }
                        }
                    }
                    (*j) = pos;
                }
                ++j;
                ++k;
            }
        }

        subevent_.fire();
    }

protected:

    event_type event_;
    FixedIntervalEvent subevent_;

    std::vector<ParticleID> pids_;
    bool resolve_boundary_;
    std::vector<Real3> prev_positions_;
    std::vector<std::vector<Real3> > trajectories_;
    std::vector<Real3> strides_;
    std::vector<Real> t_;
};

class FixedIntervalTrajectoryObserver
    : public TrajectoryObserver<FixedIntervalEvent>
{
public:

    typedef FixedIntervalEvent event_type;
    typedef TrajectoryObserver<event_type> base_type;

public:

    FixedIntervalTrajectoryObserver(
        const Real& dt, const std::vector<ParticleID>& pids,
        const bool resolve_boundary = default_resolve_boundary(),
        const Real subdt = default_subdt())
        : base_type(pids, resolve_boundary, (subdt > 0 ? subdt : dt))
    {
        event_.set_dt(dt);
    }

    FixedIntervalTrajectoryObserver(
        const Real& dt,
        const bool resolve_boundary = default_resolve_boundary(),
        const Real subdt = default_subdt())
        : base_type(resolve_boundary, (subdt > 0 ? subdt : dt))
    {
        event_.set_dt(dt);
    }

    FixedIntervalTrajectoryObserver(
        const Real& dt,
        const std::vector<ParticleID>& pids,
        const bool resolve_boundary,
        const Real subdt,
        const std::vector<Real3>& prev_positions,
        const std::vector<std::vector<Real3> >& trajectories,
        const std::vector<Real3>& strides,
        const std::vector<Real>& times
        )
        : base_type(pids, resolve_boundary, subdt, prev_positions, trajectories, strides, times)
    {
        event_.set_dt(dt);
    }

    virtual ~FixedIntervalTrajectoryObserver()
    {
        ;
    }
};

class TimingTrajectoryObserver
    : public TrajectoryObserver<TimingEvent>
{
public:

    typedef TimingEvent event_type;
    typedef TrajectoryObserver<event_type> base_type;

public:

    TimingTrajectoryObserver(
        const std::vector<Real>& t, const std::vector<ParticleID>& pids,
        const Real subdt = default_subdt())
        : base_type(pids, (subdt > 0), subdt)
    {
        event_.set_times(t);
    }

    TimingTrajectoryObserver(
        const std::vector<Real>& t,
        const Real subdt = default_subdt())
        : base_type((subdt > 0), subdt)
    {
        event_.set_times(t);
    }

    TimingTrajectoryObserver(
        const std::vector<Real>& t,
        const std::vector<ParticleID>& pids,
        const Real subdt,
        const std::vector<Real3>& prev_positions,
        const std::vector<std::vector<Real3> >& trajectories,
        const std::vector<Real3>& strides,
        const std::vector<Real>& times
        )
        : base_type(pids, (subdt > 0), subdt, prev_positions, trajectories, strides, times)
    {
        event_.set_times(t);
    }

    virtual ~TimingTrajectoryObserver()
    {
        ;
    }
};

class TimeoutObserver
    : public Observer
{
public:

    typedef Observer base_type;

public:

    TimeoutObserver()
        : base_type(true), interval_(std::numeric_limits<Real>::infinity()), duration_(0.0), acc_(0.0)
    {
        ;
    }

    TimeoutObserver(const Real interval, const Real duration=0.0, const Real acc=0.0)
        : base_type(true), interval_(interval), duration_(duration), acc_(acc)
    {
        ;
    }

    virtual ~TimeoutObserver()
    {
        ;
    }

    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual void finalize(const std::shared_ptr<WorldInterface>& world);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

    const Real interval() const
    {
        return interval_;
    }

    const Real duration() const
    {
        return duration_;
    }

    const Real accumulation() const
    {
        return acc_;
    }

    const std::chrono::system_clock::time_point& start_time_point() const
    {
        return tstart_;
    }

    void set_start_time_point(const std::chrono::system_clock::time_point& tstart)
    {
        tstart_ = tstart;
    }

protected:

    Real interval_;
    Real duration_;
    Real acc_;
    std::chrono::system_clock::time_point tstart_;
};

class FixedIntervalTrackingObserver
    : public Observer
{
public:

    typedef Observer base_type;

public:

    FixedIntervalTrackingObserver(
        const Real& dt, const std::vector<Species>& species,
        const bool& resolve_boundary = default_resolve_boundary(), const Real subdt = default_subdt(),
        const Real threshold = default_threshold())
        : base_type(false), event_(dt), subevent_(subdt > 0 ? subdt : dt),
        species_(species), resolve_boundary_(resolve_boundary),
        threshold_(threshold > 0 ? threshold : std::numeric_limits<Real>::infinity()),
        prev_positions_(), strides_(), pids_(), trajectories_(), t_()
    {
        ;
    }

    FixedIntervalTrackingObserver(
        const Real& dt,
        const std::vector<ParticleID>& pids,
        const bool& resolve_boundary,
        const Real subdt,
        const std::vector<Real3>& prev_positions,
        const std::vector<std::vector<Real3> >& trajectories,
        const std::vector<Real3>& strides,
        const std::vector<Real>& times,
        const std::vector<Species>& species,
        const Real threshold
        )
        : base_type(false), event_(dt), subevent_(subdt > 0 ? subdt : dt),
        pids_(pids), resolve_boundary_(resolve_boundary),
        prev_positions_(prev_positions), trajectories_(trajectories), strides_(strides), t_(times),
        species_(species),
        threshold_(threshold > 0 ? threshold : std::numeric_limits<Real>::infinity())
    {
        ;
    }

    virtual ~FixedIntervalTrackingObserver()
    {
        ;
    }

    static inline bool default_resolve_boundary()
    {
        return true;
    }

    static inline const Real default_subdt()
    {
        return 0;
    }

    static inline const Real default_threshold()
    {
        return 0;
    }

    const Real next_time() const;
    const Integer num_steps() const;
    const Integer count() const;
    const Integer num_tracers() const;
    virtual void initialize(const std::shared_ptr<WorldInterface>& world, const std::shared_ptr<Model>& model);
    virtual bool fire(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    virtual void reset();

    const std::vector<std::vector<Real3> >& data() const;
    const std::vector<Real>& t() const;

    Real distance_sq(
        const Real3& pos1, const Real3& pos2, const Real3& edge_lengths) const
    {
        Real retval(0);
        for (Real3::size_type dim(0); dim < 3; ++dim)
        {
            const Real edge_length(edge_lengths[dim]);
            const Real diff(pos2[dim] - pos1[dim]), half(edge_length * 0.5);

            if (diff > half)
            {
                retval += pow_2(diff - edge_length);
            }
            else if (diff < -half)
            {
                retval += pow_2(diff + edge_length);
            }
            else
            {
                retval += pow_2(diff);
            }
        }
        return retval;
    }

    inline Real distance(const Real3& pos1, const Real3& pos2, const Real3& edge_lengths) const
    {
        return std::sqrt(distance_sq(pos1, pos2, edge_lengths));
    }

    const FixedIntervalEvent& event() const
    {
        return event_;
    }

    void set_event(const FixedIntervalEvent& event)
    {
        event_ = event;
    }

    const FixedIntervalEvent& subevent() const
    {
        return subevent_;
    }

    void set_subevent(const FixedIntervalEvent& subevent)
    {
        subevent_ = subevent;
    }

    const std::vector<ParticleID>& pids() const
    {
        return pids_;
    }

    const bool resolve_boundary() const
    {
        return resolve_boundary_;
    }

    const std::vector<Real3>& prev_positions() const
    {
        return prev_positions_;
    }

    const std::vector<Real3>& strides() const
    {
        return strides_;
    }

    const std::vector<Species>& species() const
    {
        return species_;
    }

    const Real threshold() const
    {
        return threshold_;
    }

protected:

    void fire_event(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);
    void fire_subevent(const Simulator* sim, const std::shared_ptr<WorldInterface>& world);

protected:

    FixedIntervalEvent event_, subevent_;

    std::vector<Species> species_;
    bool resolve_boundary_;
    Real threshold_;

    std::vector<Real3> prev_positions_;
    std::vector<Real3> strides_;
    std::vector<ParticleID> pids_;
    std::vector<std::vector<Real3> > trajectories_;
    std::vector<Real> t_;
};

} // ecell4

#endif /* ECELL4_OBSERVER_HPP */
