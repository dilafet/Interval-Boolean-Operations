#include <iostream>
#include <vector>

struct Interval {
    double start;
    double end;

    double operator[](size_t i) const {
        return *((&start) + i);
    }
};

struct BoundaryPoint {
    size_t index;
    bool isLeftOperand;
    bool isExit;
    bool updateInBasedOnNext;
};

enum class Operation { Union,
                       Intersection,
                       Difference };

inline std::vector<BoundaryPoint> MergeSort(const std::vector<Interval> &left, const std::vector<Interval> &right) {
    size_t leftSize = left.size() * 2;
    size_t rightSize = right.size() * 2;
    std::vector<BoundaryPoint> points(leftSize + rightSize);
    auto *p = points.data();
    auto lastP = p;
    size_t leftIndex = 0;
    size_t rightIndex = 0;
    size_t leftArrayIndex;
    size_t leftEventIndex;
    size_t rightArrayIndex;
    size_t rightEventIndex;
    int64_t lastChoosen = -1; // 0 for right, 1 for left
    double lastValue = -std::numeric_limits<double>::infinity();
    while (leftIndex < leftSize && rightIndex < rightSize) {
        leftArrayIndex = leftIndex >> 1;   //
        leftEventIndex = leftIndex & 1;    // 0 for start, 1 for end
        rightArrayIndex = rightIndex >> 1; //
        rightEventIndex = rightIndex & 1;  // 0 for start, 1 for end
        double leftValue = left[leftArrayIndex][leftEventIndex];
        double rightValue = right[rightArrayIndex][rightEventIndex];
        size_t choose = leftValue < rightValue || (leftValue == rightValue && leftEventIndex <= rightEventIndex); // 0 for right, 1 for left
        double value = choose ? leftValue : rightValue;
        p->index = choose ? leftArrayIndex : rightArrayIndex;
        p->isLeftOperand = choose;
        p->isExit = choose ? leftEventIndex : rightEventIndex;
        lastP->updateInBasedOnNext = lastChoosen != choose && lastValue == value;
        lastP = p;
        p++;
        leftIndex += choose;
        rightIndex += (1 - choose);
        lastChoosen = choose;
        lastValue = value;
    }
    // Add the remaining events
    while (leftIndex < leftSize) {
        leftArrayIndex = leftIndex >> 1;
        leftEventIndex = leftIndex & 1;
        p->index = leftArrayIndex;
        p->isLeftOperand = true;
        p->isExit = leftEventIndex;
        p++;
        leftIndex++;
    }
    while (rightIndex < rightSize) {
        rightArrayIndex = rightIndex >> 1;
        rightEventIndex = rightIndex & 1;
        p->index = rightArrayIndex;
        p->isLeftOperand = false;
        p->isExit = rightEventIndex;
        p++;
        rightIndex++;
    }
    return points;
}

std::vector<Interval> Boolean(Operation op, std::vector<Interval> &left, const std::vector<Interval> &right) {
    // Collect events (entry/exit points) in order
    std::vector<BoundaryPoint> points = MergeSort(left, right);

    // Do the unions
    std::vector<Interval> intervals;
    double currentEntry = -std::numeric_limits<double>::infinity();
    double currentExit = -std::numeric_limits<double>::infinity();
    bool leftIn = false;
    bool rightIn = false;
    bool in = false;

    for (size_t i = 0; i < points.size(); i++) {
        const auto &point = points[i];
        if (point.isLeftOperand) {
            leftIn = !point.isExit;
        } else {
            rightIn = !point.isExit;
        }

        if (point.updateInBasedOnNext) {
            const auto &nextPoint = points[i + 1];
            if (nextPoint.isLeftOperand) {
                leftIn = !nextPoint.isExit;
            } else {
                rightIn = !nextPoint.isExit;
            }
        }

        bool newIn = false;

        switch (op) {
        case Operation::Union:
            newIn = leftIn || rightIn;
            break;
        case Operation::Intersection:
            newIn = leftIn && rightIn;
            break;
        case Operation::Difference:
            newIn = leftIn && !rightIn;
            break;
        }

        bool justEntered = newIn && !in;
        bool justExited = in && !newIn;
        in = newIn;

        if (justEntered) {
            currentEntry = point.isLeftOperand ? left[point.index][point.isExit] : right[point.index][point.isExit];
        } else if (justExited) {
            currentExit = point.isLeftOperand ? left[point.index][point.isExit] : right[point.index][point.isExit];
            intervals.push_back({currentEntry, currentExit});
        }
    }

    return intervals;
}

int main() {
    // Example usage
    std::vector<Interval> left = {{1.0, 5.0}, {6.0, 9.0}};
    std::vector<Interval> right = {{1.0, 5.0}, {6.0, 9.0}};

    std::vector<Interval> Union = Boolean(Operation::Union, left, right);
    std::vector<Interval> Intersection = Boolean(Operation::Intersection, left, right);
    std::vector<Interval> Difference = Boolean(Operation::Difference, left, right);

    std::cout << "Left: ";
    for (const auto &interval : left) {
        std::cout << "[" << interval.start << ", " << interval.end << "] ";
    }
    std::cout << std::endl;

    std::cout << "Right: ";
    for (const auto &interval : right) {
        std::cout << "[" << interval.start << ", " << interval.end << "] ";
    }
    std::cout << std::endl;

    std::cout << std::endl;

    std::cout << "Union: ";
    for (const auto &interval : Union) {
        std::cout << "[" << interval.start << ", " << interval.end << "] ";
    }
    std::cout << std::endl;

    std::cout << "Intersection: ";
    for (const auto &interval : Intersection) {
        std::cout << "[" << interval.start << ", " << interval.end << "] ";
    }
    std::cout << std::endl;

    std::cout << "Difference: ";
    for (const auto &interval : Difference) {
        std::cout << "[" << interval.start << ", " << interval.end << "] ";
    }
    std::cout << std::endl;

    return 0;
}
