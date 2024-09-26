#include <stdexcept>
#include <regex>
#include <boost/algorithm/string.hpp>

#include "UnitSpecies.hpp"


namespace ecell4
{

void UnitSpecies::clear()
{
    name_ = "";
    sites_.clear();
}

void UnitSpecies::deserialize(const UnitSpecies::serial_type& serial)
{
    clear();
    if (serial == "")
    {
        return;
    }

    using namespace std;

    regex r1(
        "^\\s*(\\w+)\\s*(\\(\\s*([\\w\\s\\^=,]*)\\))?\\s*$");
    smatch results1;
    if (regex_match(serial, results1, r1))
    {
        name_ = std::string(results1.str(1).c_str());
        if (results1.str(3).size() > 0)
        {
            regex r2(
                "\\s*(\\w+)(\\s*=\\s*(\\w+))?(\\s*\\^\\s*(\\w+))?\\s*");
            // match_results<std::string::const_iterator> results2;
            smatch results2;
            std::vector<std::string> sites;
            boost::split(
                sites, static_cast<const std::string>(results1.str(3)),
                boost::is_any_of(","));
            bool order(false);
            for (std::vector<std::string>::const_iterator i(sites.begin());
                i != sites.end(); ++i)
            {
                if (regex_match(*i, results2, r2))
                {
                    if (results2.str(3).size() > 0)
                    {
                        order = true;
                    }
                    else if (order)
                    {
                        throw std::invalid_argument(
                            "non-keyword arg after keyword arg [" +
                            (*i) + "]"); //XXX:
                    }

                    add_site(
                        results2.str(1), results2.str(3), results2.str(5));
                }
                else
                {
                    throw std::invalid_argument(
                        "a wrong site specification was given [" +
                        (*i) + "]"); //XXX:
                }
            }
        }
    }
    else
    {
        throw std::invalid_argument(
            "a wrong serial was given to UnitSpecies [" + serial + "]"); //XXX:
    }
}

UnitSpecies::serial_type UnitSpecies::serial() const
{
    if (sites_.size() == 0)
    {
        return name_;
    }

    std::vector<std::string> unstated, stated;
    for (container_type::const_iterator i(sites_.begin());
        i != sites_.end(); ++i)
    {
        const std::string&
            state((*i).second.first), bond((*i).second.second);
        if (state.size() > 0)
        {
            stated.push_back((*i).first + "="
                + (bond.size() > 0? state + "^" + bond : state));
        }
        else
        {
            unstated.push_back(
                bond.size() > 0? (*i).first + "^" + bond : (*i).first);
        }
    }
    return name_ + "(" + boost::algorithm::join(unstated, ",")
        + (unstated.size() > 0 && stated.size() > 0? "," : "")
        + boost::algorithm::join(stated, ",") + ")";

    // std::stringstream unstated, stated;
    // bool is_unstated_empty(true), is_stated_empty(true);
    // unstated << name_ << "(";
    // for (container_type::const_iterator i(sites_.begin());
    //     i != sites_.end(); ++i)
    // {
    //     const std::string& state((*i).second.first);
    //     const std::string& bond((*i).second.second);

    //     if (state.size() > 0)
    //     {
    //         if (is_stated_empty)
    //         {
    //             is_stated_empty = false;
    //         }
    //         else
    //         {
    //             stated << ",";
    //         }
    //         stated << (*i).first << "=" << state;
    //         if (bond.size() > 0)
    //         {
    //             stated << "^" << bond;
    //         }
    //     }
    //     else
    //     {
    //         if (is_unstated_empty)
    //         {
    //             is_unstated_empty = false;
    //         }
    //         else
    //         {
    //             unstated << ",";
    //         }
    //         unstated << (*i).first;
    //         if (bond.size() > 0)
    //         {
    //             unstated << "^" << bond;
    //         }
    //     }
    // }
    // if (!is_unstated_empty && !is_stated_empty)
    // {
    //     unstated << ",";
    // }
    // unstated << stated.str() << ")";
    // return unstated.str();
}

} // ecell4
