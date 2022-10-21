#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <vector>

class Point
{
    double m_x = 0.0;
    double m_y = 0.0;

public:
    Point() = default;
    ~Point() = default;
    Point(double x, double y)
        : m_x(x)
        , m_y(y){};

    double x() const;
    double y() const;
    double distance(const Point &) const;

    bool operator<(const Point &) const;
    bool operator>(const Point &) const;
    bool operator<=(const Point &) const;
    bool operator>=(const Point &) const;
    bool operator==(const Point &) const;
    bool operator!=(const Point &) const;

    std::string toString() const { return "(" + std::to_string(m_x) + "; " + std::to_string(m_y) + ")"; };

    friend std::ostream & operator<<(std::ostream &, const Point &);
};

class Rect
{
    const Point m_left_bottom;
    const Point m_right_top;

public:
    Rect(const Point & left_bottom, const Point & right_top)
        : m_left_bottom(left_bottom)
        , m_right_top(right_top){};

    double xmin() const { return m_left_bottom.x(); };
    double ymin() const { return m_left_bottom.y(); };
    double xmax() const { return m_right_top.x(); };
    double ymax() const { return m_right_top.y(); };
    double distance(const Point & p) const;

    bool contains(const Point & p) const
    {
        return m_left_bottom.x() <= p.x() && m_left_bottom.y() <= p.y() &&
                p.x() <= m_right_top.x() && p.y() <= m_right_top.y();
    };

    bool notIntersects(const Rect & rect) const
    {
        return rect.m_right_top.y() < m_left_bottom.y() || m_right_top.y() < rect.m_left_bottom.y() ||
                rect.m_right_top.x() < m_left_bottom.x() || m_right_top.x() < rect.m_left_bottom.x();
    }
    bool intersects(const Rect & rect) const
    {
        return !this->notIntersects(rect);
    }
};

using pair_data = std::pair<double, const Point *>;
struct myComp
{
    bool operator()(const pair_data & a, const pair_data & b)
    {
        return a.first < b.first;
    }
};

namespace rbtree {

class PointSet
{
    std::set<Point> m_setOfPoints;

public:
    class iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Point;
        using pointer = const value_type *;
        using reference = const value_type &;
        using iterator_category = std::forward_iterator_tag;

        iterator() = default;

        std::size_t size() const { return m_set.size(); };

        friend bool operator==(const iterator & it1, const iterator & it2) { return it1.m_set == it2.m_set; };
        friend bool operator!=(const iterator & it1, const iterator & it2) { return !(it1 == it2); };
        reference operator*() const { return *m_set.front(); };
        pointer operator->() { return m_set.front(); };
        iterator & operator++()
        {
            m_set.pop();
            return *this;
        };
        iterator operator++(int)
        {
            iterator tmp = *this;
            operator++();
            return tmp;
        }

    private:
        friend class rbtree::PointSet;

        iterator(std::queue<const Point *> && points)
            : m_set(std::move(points))
        {
        }

        std::queue<const Point *> m_set;
    };

    PointSet(const std::string & filename = {});

    bool empty() const;
    std::size_t size() const;
    void put(const Point &);
    bool contains(const Point &) const;

    // second iterator points to an element out of range
    std::pair<iterator, iterator> range(const Rect & rect) const
    {
        std::queue<const Point *> storage;
        for (auto & i : m_setOfPoints) {
            if (rect.contains(i)) {
                storage.push(&i);
            }
        }
        return std::pair<iterator, iterator>(std::move(storage), {});
    }
    iterator begin() const
    {
        std::queue<const Point *> storage;
        for (auto & i : m_setOfPoints) {
            storage.push(&i);
        }
        return {std::move(storage)};
    };
    iterator end() const { return {}; };

    std::optional<Point> nearest(const Point &) const;
    // second iterator points to an element out of range
    std::pair<iterator, iterator> nearest(const Point & point, std::size_t len) const
    {
        std::priority_queue<pair_data, std::vector<pair_data>, myComp> line;
        std::queue<const Point *> storage;
        for (auto & i : m_setOfPoints) {
            line.push(std::pair<double, const Point *>(point.distance(i), &i));
            if (line.size() > len) {
                line.pop();
            }
        }
        std::size_t lineSize = line.size();
        for (std::size_t i = 0; i < lineSize; i++) {
            storage.push(line.top().second);
            line.pop();
        }
        return std::pair<iterator, iterator>(std::move(storage), {});
    }

    friend std::ostream & operator<<(std::ostream & out, const PointSet & pointSet)
    {
        for (const auto i : pointSet) {
            out << i << std::endl;
        }
        return out;
    }
};

} // namespace rbtree

namespace kdtree {

class PointSet
{

public:
    struct Node
    {
        Node() = default;
        Node(const Point & point)
            : value(point)
        {
        }

        Point value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };
    class iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Point;
        using pointer = const value_type *;
        using reference = const value_type &;
        using iterator_category = std::forward_iterator_tag;

        iterator() = default;

        std::size_t size() const
        {
            return m_set.size();
        };

        friend bool operator==(const iterator & it1, const iterator & it2) { return it1.m_set == it2.m_set; };
        friend bool operator!=(const iterator & it1, const iterator & it2) { return !(it1 == it2); };
        reference operator*() const { return *m_set.front(); };
        pointer operator->() { return m_set.front(); };
        iterator & operator++()
        {
            m_set.pop();
            return *this;
        };
        iterator operator++(int)
        {
            iterator tmp = *this;
            operator++();
            return tmp;
        }

    private:
        friend class kdtree::PointSet;

        std::queue<const Point *> m_set;

        iterator(std::queue<const Point *> && points)
            : m_set(std::move(points))
        {
        }
    };

    PointSet(const std::string & filename = {});
    PointSet(const kdtree::PointSet & pointSet)
    {
        for (const auto i : pointSet) {
            put(i);
        }
    }

    bool empty() const { return !m_root; };
    std::size_t size() const;
    void put(const Point &);
    bool contains(const Point &) const;

    std::pair<iterator, iterator> range(const Rect &) const;
    iterator begin() const
    {
        std::queue<const Point *> ans;
        treeTraversal(m_root, ans);
        return {std::move(ans)};
    };
    iterator end() const { return {}; };

    std::optional<Point> nearest(const Point &) const;
    std::pair<iterator, iterator> nearest(const Point &, std::size_t) const;
    friend std::ostream & operator<<(std::ostream & out, const PointSet & set)
    {
        dumpTree(set.m_root, 0, out, true);
        return out;
    }

private:
    using NodePTR = std::unique_ptr<Node>;
    NodePTR m_root;
    std::size_t m_treeSize = 0;
    size_t subTreeSize(const NodePTR & node) const;
    void treeTraversal(const NodePTR & node, std::queue<const Point *> & result) const;
    std::pair<bool, kdtree::PointSet::Node *> findParent(const Point & point) const;
    kdtree::PointSet::Node * nearestNeighbor(Node * subtreeRoot, const Point & targetPoint, bool parity) const;

    static void dumpTree(const NodePTR & subtree, std::size_t depth, std::ostream & out, bool left)
    {
        if (subtree == nullptr) {
            return;
        }
        dumpTree(subtree->left, depth + 1, out, false);
        for (std::size_t i = 1; i < depth; ++i) {
            out << "\t\t\t";
        }
        if (depth != 0) {
            if (left) {
                out << "\\---";
            }
            else {
                out << "/---";
            }
        }
        out << subtree->value.toString() << "\n";
        dumpTree(subtree->right, depth + 1, out, true);
    }
    void branchSelecter(const Node * subtreeRoot, bool flag, Node *& next, Node *& other) const;
};

} // namespace kdtree
