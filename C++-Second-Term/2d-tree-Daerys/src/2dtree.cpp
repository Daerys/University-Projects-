#include "primitives.h"

#include <algorithm>
#include <cmath>
#include <fstream>

double Point::x() const
{
    return m_x;
}

double Point::y() const
{
    return m_y;
}

double Point::distance(const Point & point) const
{
    return std::sqrt(std::pow(m_x - point.x(), 2) + std::pow(m_y - point.y(), 2));
}

bool Point::operator<(const Point & point) const
{
    if (m_x < point.x()) {
        return true;
    }
    else if (m_x == point.x() && m_y < point.y()) {
        return true;
    }
    return false;
}

bool Point::operator>(const Point & point) const
{
    return !(*this < point);
}

bool Point::operator<=(const Point & point) const
{
    if (m_x <= point.x()) {
        return true;
    }
    else if (m_y <= point.y()) {
        return true;
    }
    return false;
}

bool Point::operator>=(const Point & point) const
{
    return !(*this <= point);
}

bool Point::operator==(const Point & point) const
{
    return (m_x == point.x()) && (m_y == point.y());
}

bool Point::operator!=(const Point & point) const
{
    return !(*this == point);
}
std::ostream & operator<<(std::ostream & outstream, const Point & point)
{
    return outstream << "(" << std::to_string(point.x()) << "; " << std::to_string(point.y()) << ")";
}

rbtree::PointSet::PointSet(const std::string & filename)
{
    if (!filename.empty()) {
        std::ifstream fileStream(filename);
        if (fileStream.fail()) {
            return;
        }
        while (!fileStream.eof()) {
            double x, y;
            fileStream >> x >> y;
            put(Point(x, y));
        }
    }
}

void rbtree::PointSet::put(const Point & point)
{
    m_setOfPoints.insert(point);
}

bool rbtree::PointSet::empty() const
{
    return m_setOfPoints.empty();
}

std::size_t rbtree::PointSet::size() const
{
    return m_setOfPoints.size();
}

bool rbtree::PointSet::contains(const Point & point) const
{
    for (const Point i : m_setOfPoints) {
        if (i == point) {
            return true;
        }
    }
    return false;
}

std::optional<Point> rbtree::PointSet::nearest(const Point & point) const
{
    return std::optional<Point>(*std::min_element(begin(), end(), [&point](const Point & a, const Point & b) { return point.distance(a) < point.distance(b); }));
}

double Rect::distance(const Point & p) const
{
    if (contains(p)) {
        return 0;
    }
    if (m_left_bottom.x() <= p.x() && p.x() <= m_right_top.x()) {
        return std::min(std::abs(p.y() - m_right_top.y()), std::abs(m_left_bottom.y() - p.y()));
    }
    if (m_left_bottom.y() <= p.y() && p.y() <= m_right_top.y()) {
        return std::min(std::abs(p.x() - m_right_top.x()), std::abs(m_left_bottom.x() - p.x()));
    }
    return std::min(m_right_top.distance(p),
                    std::min(m_left_bottom.distance(p),
                             std::min(p.distance(Point(m_right_top.x(), m_left_bottom.y())),
                                      p.distance(Point(m_left_bottom.x(), m_right_top.y())))));
}

void kdtree::PointSet::put(const Point & point)
{
    /**
     * if parity is even then compare by x, else compare by y.
     */
    if (m_root == nullptr) {
        m_root = std::make_unique<Node>(Node(point));
        m_treeSize++;
        return;
    }
    std::pair<bool, Node *> pair = findParent(point);
    Node * curr = &*pair.second;
    if ((curr->left != nullptr && curr->left->value == point) || (curr->right != nullptr && curr->right->value == point)) {
        return;
    }
    if (pair.first) {
        ((point.x() < curr->value.x()) ? curr->left : curr->right) = std::make_unique<Node>(Node(point));
    }
    else {
        ((point.y() < curr->value.y()) ? curr->left : curr->right) = std::make_unique<Node>(Node(point));
    }
    m_treeSize++;
}

kdtree::PointSet::PointSet(const std::string & filename)
{
    if (!filename.empty()) {
        std::ifstream fileStream(filename);
        if (fileStream.fail()) {
            return;
        }
        while (!fileStream.eof()) {
            double x, y;
            fileStream >> x >> y;
            put(Point(x, y));
        }
    }
}

std::size_t kdtree::PointSet::subTreeSize(const NodePTR & node) const
{
    if (node == nullptr) {
        return 0;
    }
    return subTreeSize(node->left) + 1 + subTreeSize(node->right);
}

std::size_t kdtree::PointSet::size() const
{
    return m_treeSize;
}

bool kdtree::PointSet::contains(const Point & point) const
{
    if (m_root == nullptr) {
        return false;
    }

    Node * pair = findParent(point).second;
    return pair->value == point || (pair->left != nullptr && pair->left->value == point) || (pair->right != nullptr && pair->right->value == point);
}

void kdtree::PointSet::treeTraversal(const NodePTR & node, std::queue<const Point *> & result) const
{
    if (node != nullptr) {
        treeTraversal(node->left, result);
        result.push(&node->value);
        treeTraversal(node->right, result);
    }
}

std::pair<bool, kdtree::PointSet::Node *> kdtree::PointSet::findParent(const Point & point) const
{
    bool parity = true;
    Node * previous = &*m_root;
    Node * current = &*m_root;
    while (true) {
        if (current->value == point) {
            return std::pair<bool, Node *>(!parity, previous);
        }
        previous = current;
        if (parity) {
            if (((current->value.x() > point.x()) ? current->left : current->right) == nullptr) {
                return std::pair<bool, Node *>(parity, current);
            }
            current = (current->value.x() > point.x()) ? &*current->left : &*current->right;
        }
        else {
            if (((current->value.y() > point.y()) ? current->left : current->right) == nullptr) {
                return std::pair<bool, Node *>(parity, current);
            }
            current = (current->value.y() > point.y()) ? &*current->left : &*current->right;
        }
        parity = !parity;
    }
}

kdtree::PointSet::Node * kdtree::PointSet::nearestNeighbor(Node * subtreeRoot, const Point & targetPoint, bool parity) const
{
    /**
     * parity -> x
     * !parity -> y
     */
    if (subtreeRoot == nullptr) {
        return nullptr;
    }
    Node * next;
    Node * other;
    if (parity) {
        branchSelecter(subtreeRoot, subtreeRoot->value.x() > targetPoint.x(), next, other);
    }
    else {
        branchSelecter(subtreeRoot, subtreeRoot->value.y() > targetPoint.y(), next, other);
    }
    Node * bestChoice = nearestNeighbor(next, targetPoint, !parity);

    if (bestChoice == nullptr || targetPoint.distance(m_root->value) <= targetPoint.distance(bestChoice->value)) {
        bestChoice = subtreeRoot;
    }
    std::size_t d = (parity) ? (targetPoint.x() - m_root->value.x()) : (targetPoint.y() - m_root->value.y());

    if (targetPoint.distance(bestChoice->value) > d) {
        Node * tmp = nearestNeighbor(other, targetPoint, !parity);
        if (tmp != nullptr && targetPoint.distance(tmp->value) <= targetPoint.distance(bestChoice->value)) {
            bestChoice = tmp;
        }
    }
    return bestChoice;
}
void kdtree::PointSet::branchSelecter(const kdtree::PointSet::Node * subtreeRoot, bool flag, kdtree::PointSet::Node *& next, kdtree::PointSet::Node *& other) const
{
    if ((flag ? subtreeRoot->left : subtreeRoot->right) == nullptr) {
        next = nullptr;
    }
    else {
        next = flag ? &*subtreeRoot->left : &*subtreeRoot->right;
    }

    if ((flag ? subtreeRoot->right : subtreeRoot->left) == nullptr) {
        other = nullptr;
    }
    else {
        other = flag ? &*subtreeRoot->right : &*subtreeRoot->left;
    }
}

std::optional<Point> kdtree::PointSet::nearest(const Point & point) const
{
    return std::optional<Point>(nearestNeighbor(&*m_root, point, true)->value);
}

std::pair<kdtree::PointSet::iterator, kdtree::PointSet::iterator> kdtree::PointSet::nearest(const Point & point, std::size_t k) const
{
    std::priority_queue<pair_data, std::vector<pair_data>, myComp> line;
    std::queue<const Point *> storage;
    iterator it = begin();
    for (; it != end(); it++) {
        line.push(pair_data(point.distance(*it), &*it));
        if (line.size() > k) {
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
std::pair<kdtree::PointSet::iterator, kdtree::PointSet::iterator> kdtree::PointSet::range(const Rect & rect) const
{
    std::queue<const Point *> storage;
    iterator it = begin();
    for (; it != end(); it++) {
        if (rect.contains(*it)) {
            storage.push(&*it);
        }
    }
    return std::pair<iterator, iterator>(std::move(storage), {});
}
