#ifndef ECELL4_SHAPE_OPERATORS
#define ECELL4_SHAPE_OPERATORS

#include "exceptions.hpp"
#include "Shape.hpp"

namespace ecell4
{

struct Surface
    : public Shape
{
public:

    Surface()
    {
        ;
    }

    Surface(const std::shared_ptr<Shape>& root)
        : root_(root)
    {
        ;
    }

    Surface(const Surface& other)
        : root_(other.root_)
    {
        ;
    }

    ~Surface()
    {
        ; // do nothing
    }

    virtual dimension_kind dimension() const
    {
        return TWO;
    }

    virtual Real is_inside(const Real3& coord) const
    {
        return root_->is_inside(coord);
    }

    virtual Real3 draw_position(
        std::shared_ptr<RandomNumberGenerator>& rng) const
    {
        throw NotSupported("draw_position is not supported.");
    }

    virtual bool test_AABB(const Real3& l, const Real3& u) const
    {
        throw NotSupported("test_AABB is not supported.");
    }

    virtual void bounding_box(
        const Real3& edge_lengths, Real3& lower, Real3& upper) const
    {
        root_->bounding_box(edge_lengths, lower, upper);
    }

    const std::shared_ptr<Shape>& root() const
    {
        return root_;
    }

protected:

    std::shared_ptr<Shape> root_;
};

struct Inversion
    : public Shape
{
public:

    Inversion()
    {
        ;
    }

    Inversion(const std::shared_ptr<Shape>& root)
        : root_(root)
    {
        ;
    }

    Inversion(const Inversion& other)
        : root_(other.root_)
    {
        ;
    }

    ~Inversion()
    {
        ; // do nothing
    }

    virtual dimension_kind dimension() const
    {
        return root_->dimension();
    }

    virtual Real is_inside(const Real3& coord) const
    {
        return -1 * root_->is_inside(coord);
    }

    virtual Real3 draw_position(
        std::shared_ptr<RandomNumberGenerator>& rng) const
    {
        throw NotSupported("draw_position is not supported.");
    }

    virtual bool test_AABB(const Real3& l, const Real3& u) const
    {
        throw NotSupported("test_AABB is not supported.");
    }

    virtual void bounding_box(
        const Real3& edge_lengths, Real3& lower, Real3& upper) const
    {
        throw NotSupported("bounding_box is not supported.");
    }

    const std::shared_ptr<Shape>& root() const
    {
        return root_;
    }

protected:

    std::shared_ptr<Shape> root_;
};

struct Union
    : public Shape
{
public:

    Union(const std::shared_ptr<Shape>& a,
          const std::shared_ptr<Shape>& b)
        : a_(a), b_(b)
    {
        ;
    }

    Union(const Union& other)
        : a_(other.a_), b_(other.b_)
    {
        ;
    }

    ~Union()
    {
        ; // do nothing
    }

    virtual dimension_kind dimension() const
    {
        return a_->dimension();
    }

    virtual Real is_inside(const Real3& coord) const
    {
        const Real retval1 = a_->is_inside(coord);
        const Real retval2 = b_->is_inside(coord);
        return std::min(retval1, retval2);
    }

    virtual Real3 draw_position(
        std::shared_ptr<RandomNumberGenerator>& rng) const
    {
        throw NotImplemented("not implemented yet");
    }

    virtual bool test_AABB(const Real3& l, const Real3& u) const
    {
        return (a_->test_AABB(l, u) || b_->test_AABB(l, u));
    }

    virtual void bounding_box(
        const Real3& edge_lengths, Real3& lower, Real3& upper) const
    {
        a_->bounding_box(edge_lengths, lower, upper);

        Real3 l, u;
        b_->bounding_box(edge_lengths, l, u);
        for (unsigned int dim(0); dim < 3; ++dim)
        {
            lower[dim] = std::min(lower[dim], l[dim]);
            upper[dim] = std::max(upper[dim], u[dim]);
        }
    }

    Surface surface() const
    {
        if (dimension() == TWO)
        {
            throw NotSupported("This union object is two-dimensional");
        }
        return Surface(std::shared_ptr<Shape>(new Union(*this)));
    }

    const std::shared_ptr<Shape>& one() const
    {
        return a_;
    }

    const std::shared_ptr<Shape>& another() const
    {
        return b_;
    }

protected:

    std::shared_ptr<Shape> a_;
    std::shared_ptr<Shape> b_;
};

struct Complement
    : public Shape
{
public:

    Complement(const std::shared_ptr<Shape>& a,
               const std::shared_ptr<Shape>& b)
        : a_(a), b_(b)
    {
        ;
    }

    Complement(const Complement& other)
        : a_(other.a_), b_(other.b_)
    {
        ;
    }

    ~Complement()
    {
        ; // do nothing
    }

    virtual dimension_kind dimension() const
    {
        return a_->dimension();
    }

    virtual Real is_inside(const Real3& coord) const
    {
        if (b_->is_inside(coord) > 0)
        {
            return a_->is_inside(coord);
        }
        else
        {
            return std::numeric_limits<Real>::infinity();
        }
    }

    virtual Real3 draw_position(
        std::shared_ptr<RandomNumberGenerator>& rng) const
    {
        throw NotImplemented("not implemented yet");
    }

    virtual bool test_AABB(const Real3& l, const Real3& u) const
    {
        return (a_->test_AABB(l, u) && !b_->test_AABB(l, u));
    }

    virtual void bounding_box(
        const Real3& edge_lengths, Real3& lower, Real3& upper) const
    {
        return a_->bounding_box(edge_lengths, lower, upper);
    }

    Surface surface() const
    {
        if (dimension() == TWO)
        {
            throw NotSupported("This complement object is two-dimensional");
        }
        return Surface(std::shared_ptr<Shape>(new Complement(*this)));
    }

    const std::shared_ptr<Shape>& one() const
    {
        return a_;
    }

    const std::shared_ptr<Shape>& another() const
    {
        return b_;
    }

protected:

    std::shared_ptr<Shape> a_;
    std::shared_ptr<Shape> b_;
};

struct AffineTransformation
    : public Shape
{
public:

    AffineTransformation()
        : root_(), a0_(1, 0, 0), a1_(0, 1, 0), a2_(0, 0, 1), b_()
    {
        ;
    }

    AffineTransformation(const std::shared_ptr<Shape>& root)
        : root_(root), a0_(1, 0, 0), a1_(0, 1, 0), a2_(0, 0, 1), b_()
    {
        ;
    }

    AffineTransformation(const std::shared_ptr<Shape>& root, const Real3& first, const Real3& second, const Real3& third, const Real3& shift)
        : root_(root), a0_(first), a1_(second), a2_(third), b_(shift)
    {
        ;
    }

    AffineTransformation(const AffineTransformation& other)
        : root_(other.root_),
        a0_(other.a0_), a1_(other.a1_), a2_(other.a2_), b_(other.b_)
    {
        ;
    }

    ~AffineTransformation()
    {
        ; // do nothing
    }

    virtual dimension_kind dimension() const
    {
        return root_->dimension();
    }

    virtual Real is_inside(const Real3& pos) const
    {
        Real3 p(pos);
        invmap(p);
        return root_->is_inside(p);
    }

    virtual Real3 draw_position(
        std::shared_ptr<RandomNumberGenerator>& rng) const
    {
        Real3 pos = root_->draw_position(rng);
        map(pos);
        return pos;
    }

    virtual bool test_AABB(const Real3& l, const Real3& u) const
    {
        Real3 lower(l), upper(u);
        invmap(lower);
        invmap(upper);
        return root_->test_AABB(lower, upper);
    }

    virtual void bounding_box(
        const Real3& edge_lengths, Real3& lower, Real3& upper) const
    {
        root_->bounding_box(edge_lengths, lower, upper);
        map(lower);
        map(upper);
    }

    void translate(const Real3& b)
    {
        b_ += b;
    }

    void rescale(const Real3& a)
    {
        if (a[0] == 0 || a[1] == 0 || a[2] == 0)
        {
            throw std::invalid_argument(
                "rescaling factors must be non-zero.");
        }

        a0_[0] *= a[0];
        a0_[1] *= a[1];
        a0_[2] *= a[2];
        a1_[0] *= a[0];
        a1_[1] *= a[1];
        a1_[2] *= a[2];
        a2_[0] *= a[0];
        a2_[1] *= a[1];
        a2_[2] *= a[2];
        b_[0] *= a[0];
        b_[1] *= a[1];
        b_[2] *= a[2];
    }

    void xroll(const Real& theta)
    {
        const double c = cos(theta);
        const double s = sin(theta);

        double tmp;

        tmp = a1_[0] * c - a2_[0] * s;
        a2_[0] = a1_[0] * s + a2_[0] * c;
        a1_[0] = tmp;

        tmp = a1_[1] * c - a2_[1] * s;
        a2_[1] = a1_[1] * s + a2_[1] * c;
        a1_[1] = tmp;

        tmp = a1_[2] * c - a2_[2] * s;
        a2_[2] = a1_[2] * s + a2_[2] * c;
        a1_[2] = tmp;

        tmp = b_[1] * c - b_[2] * s;
        b_[2] = b_[1] * s + b_[2] * c;
        b_[1] = tmp;
    }

    void yroll(const Real& theta)
    {
        const double c = cos(theta);
        const double s = sin(theta);

        double tmp;

        tmp = a0_[0] * c + a2_[0] * s;
        a2_[0] = a0_[0] * -s + a2_[0] * c;
        a0_[0] = tmp;

        tmp = a0_[1] * c + a2_[1] * s;
        a2_[1] = a0_[1] * -s + a2_[1] * c;
        a0_[1] = tmp;

        tmp = a0_[2] * c + a2_[2] * s;
        a2_[2] = a0_[2] * -s + a2_[2] * c;
        a0_[2] = tmp;

        tmp = b_[0] * c + b_[2] * s;
        b_[2] = b_[0] * -s + b_[2] * c;
        b_[0] = tmp;
    }

    void zroll(const Real& theta)
    {
        const double c = cos(theta);
        const double s = sin(theta);

        double tmp;

        tmp = a0_[0] * c - a1_[0] * s;
        a1_[0] = a0_[0] * s + a1_[0] * c;
        a0_[0] = tmp;

        tmp = a0_[1] * c - a1_[1] * s;
        a1_[1] = a0_[1] * s + a1_[1] * c;
        a0_[1] = tmp;

        tmp = a0_[2] * c - a1_[2] * s;
        a1_[2] = a0_[2] * s + a1_[2] * c;
        a0_[2] = tmp;

        tmp = b_[0] * c - b_[1] * s;
        b_[1] = b_[0] * s + b_[1] * c;
        b_[0] = tmp;
    }

    Surface surface() const
    {
        if (dimension() == TWO)
        {
            throw NotSupported("This affine object is two-dimensional");
        }
        return Surface(
            std::shared_ptr<Shape>(new AffineTransformation(*this)));
    }

    const Real3 first() const
    {
        return a0_;
    }

    const Real3 second() const
    {
        return a1_;
    }

    const Real3 third() const
    {
        return a2_;
    }

    const Real3 shift() const
    {
        return b_;
    }

    const std::shared_ptr<Shape>& root() const
    {
        return root_;
    }

protected:

    inline void map(Real3& p) const
    {
        p[0] = dot_product(p, a0_) + b_[0];
        p[1] = dot_product(p, a1_) + b_[1];
        p[2] = dot_product(p, a2_) + b_[2];
    }

    inline void invmap(Real3& p) const
    {
        double det = 0.0;
        det += a0_[0] * a1_[1] * a2_[2];
        det += a1_[0] * a2_[1] * a0_[2];
        det += a2_[0] * a0_[1] * a1_[2];
        det -= a2_[0] * a1_[1] * a0_[2];
        det -= a1_[0] * a0_[1] * a2_[2];
        det -= a0_[0] * a2_[1] * a1_[2];

        if (det == 0)
        {
            throw IllegalState(
                "The determinant of an Affine matrix is equal to zero.");
        }

        const Real3 inva0(a1_[1] * a2_[2] - a1_[2] * a2_[1],
                          a0_[2] * a2_[1] - a0_[1] * a2_[2],
                          a0_[1] * a1_[2] - a0_[2] * a1_[1]);
        const Real3 inva1(a1_[2] * a2_[0] - a1_[0] * a2_[2],
                          a0_[0] * a2_[2] - a0_[2] * a2_[0],
                          a0_[2] * a1_[0] - a0_[0] * a1_[2]);
        const Real3 inva2(a1_[0] * a2_[1] - a1_[1] * a2_[0],
                          a0_[1] * a2_[0] - a0_[0] * a2_[1],
                          a0_[0] * a1_[1] - a0_[1] * a1_[0]);

        Real3 tmp = p - b_;
        p[0] = dot_product(tmp, inva0) / det;
        p[1] = dot_product(tmp, inva1) / det;
        p[2] = dot_product(tmp, inva2) / det;
    }

protected:

    std::shared_ptr<Shape> root_;

    Real3 a0_, a1_, a2_;
    Real3 b_;
};

}

#endif /* ECELL4_SHAPE_OPERATORS */
