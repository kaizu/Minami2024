#include <iostream>

#include "./bd/NetworkModel.hpp"
#include "./bd/BDSimulator.hpp"

using namespace ecell4;
using namespace ecell4::bd;
using namespace ecell4::extras;

void dump_positions(BDSimulator const& sim, bool const dump_all = false)
{
    BDWorld const& w(*sim.world());

    typedef std::vector<std::pair<ParticleID, Particle> > container_type;
    container_type const particles = w.list_particles();
    for (container_type::const_iterator i(particles.begin()); i != particles.end(); ++i)
    {
        ParticleID const& pid = (*i).first;
        Real3 const& pos = add((*i).second.position(), (*i).second.stride());
        Species const& sp = (*i).second.species();

        if (dump_all || sp.serial() == "X")
        {
            std::cout << w.t() << "," << sp.serial() << "," << pid.serial()
                << "," << pos[0] << "," << pos[1] << "," << pos[2] << std::endl;
        }
    }
}

/*
    https://doi.org/10.1091%2Fmbc.E17-06-0359
*/
int main(int argc, char* argv[])
{
    // nm
    const unsigned long int seed(argc > 1 ? std::stoi(argv[1]) : 0);;
    const Real tracer_diameter(argc > 2 ? std::stod(argv[2]) : 10.0);  // nm
    const Real crowder_constraint_diameter(
        argc > 3 ? std::stod(argv[3]) : std::numeric_limits<Real>::infinity());  // nm
    const Real D_crowder(argc > 4 ? std::stod(argv[4]) : 9.0);  // um2/s
    const Real crowder_diameter(argc > 5 ? std::stod(argv[5]) : 9.6);  // nm
    const Integer N_crowder_right(argc > 6 ? std::stoi(argv[6]) : 96);
    const Real dt(argc > 7 ? std::stod(argv[7]) : 1e-9);  // sec

    std::cout
        << "#seed=" << seed
        << ",tracer_diameter=" << tracer_diameter
        << ",crowder_constraint_diameter=" << crowder_constraint_diameter
        << ",D_crowder=" << D_crowder
        << ",crowder_diameter=" << crowder_diameter
        << ",N_crowder_right=" << N_crowder_right
        << ",dt=" << dt << std::endl;

    const Real L(0.149);  // um
    const Real3 edge_lengths(L * 2, L, L);
    const Integer3 matrix_sizes(
        std::max(3, static_cast<int>(
            edge_lengths[0] / (std::max(tracer_diameter, crowder_diameter) * 1e-3))),
        std::max(3, static_cast<int>(
            edge_lengths[1] / (std::max(tracer_diameter, crowder_diameter) * 1e-3))),
        std::max(3, static_cast<int>(
            edge_lengths[2] / (std::max(tracer_diameter, crowder_diameter) * 1e-3))));

    const Integer N_crowder_left(96);
    const Integer N_tracer(10);

    const Real D_tracer(90.0 / tracer_diameter);  // um2/s

    std::cout
        << "#L=" << L
        << ",N_crowder_left=" << N_crowder_left
        << ",N_tracer=" << N_tracer
        << ",D_tracer=" << D_tracer << std::endl;

    std::shared_ptr<NetworkModel> m(new NetworkModel());
    Species sp_tracer("X", tracer_diameter * 1e-3 * 0.5, D_tracer);
    (*m).add_species_attribute(sp_tracer);
    Species sp_crowder1("C1", crowder_diameter * 1e-3 * 0.5, D_crowder);
    sp_crowder1.set_attribute("constraint_radius", crowder_constraint_diameter * 1e-3 * 0.5);
    (*m).add_species_attribute(sp_crowder1);
    Species sp_crowder2("C2", crowder_diameter * 1e-3 * 0.5, D_crowder);
    sp_crowder2.set_attribute("constraint_radius", crowder_constraint_diameter * 1e-3 * 0.5);
    (*m).add_species_attribute(sp_crowder2);

    std::shared_ptr<RandomNumberGenerator> rng(new GSLRandomNumberGenerator(seed));
    std::shared_ptr<BDWorld> w(new BDWorld(edge_lengths, matrix_sizes, rng));
    w->bind_to(m);

    (*w).add_molecules(sp_crowder1, N_crowder_left,
        std::shared_ptr<Shape>(new AABB(Real3(L * 0, 0, 0), Real3(L * 1, L, L))));  // sparse region
    (*w).add_molecules(sp_crowder2, N_crowder_right,
        std::shared_ptr<Shape>(new AABB(Real3(L * 1, 0, 0), Real3(L * 2, L, L))));  // dense region
    (*w).add_molecules(sp_tracer, 10,
        std::shared_ptr<Shape>(new AABB(Real3(L * 0, 0, 0), Real3(L * 1, L, L))));  // sparse region

    BDSimulator sim(w, m);
    sim.set_dt(dt);
    sim.initialize();

    const Real interval(10e-6);
    const Real duration(100e-3);
    // const Real duration(1e-3);

    dump_positions(sim, true);
    for (unsigned int i(1); i <= duration / interval; ++i)
    {
        while (sim.step(interval * i))
        {
            ; // do nothing
        }

        dump_positions(sim);
        // dump_positions(sim, true);
    }
}
