#include <iostream>
#include <vector>
#include <set>
#include <ctype.h>

// 排序和去重

// 经典comp
// std::less
template <typename el>
struct ClassicalComparator
{
    bool operator() (const el lhs ,const el rhs)
    {
        return lhs < rhs;
    }
};

// 带分隔符的comp
// 根据分隔符将字符串分隔成段并存储到vector, 然后两个vector对比排序
// 若将纯数字字符转成数字, 则需要区分int, float (未实现)
const std::string seprator_ = "-";
template <typename el>
struct SepratedComparator
{
    bool operator() (const el lhs, const el rhs)
    {
        auto lsv = splitEl(lhs);
        auto rsv = splitEl(rhs);

        auto lit = lsv.begin();
        auto rit = rsv.begin();

        while (lit != lsv.end() || rit != rsv.end())
        {
            if (lit == lsv.end())
                return true;
            if (rit == rsv.end())
                return false;
            if (*lit == *rit)
            {
                ++lit;
                ++rit;
                continue;
            }
            else
            {
                return *lit < *rit;
            }
        }

        return lhs < rhs;
    }

    std::vector<el> splitEl(const el str)
    {
        std::vector<el> splitedVec;
        el tmpStr = "";

        for (const auto& it : str)
        {
            if (it == *(seprator_.c_str()))
            {
                splitedVec.emplace_back(tmpStr);
                tmpStr = "";
            }
            else
            {
                tmpStr += it;
            }
        }
        splitedVec.emplace_back(tmpStr);

        return splitedVec;
    }
};

// 自然comp
// 先根据字符类型把字符串切分成几段, 每段只包含字母或数字等一种类型
    // 字母(包含_), 数字, 分隔符号(- / \...) (目前只区分数字)
    // 正则式更方便, 但更低效
// 浮点数和单独"."的考虑?
// [0, 00] [1, 01] 的排序
    // <= 判断
template <typename el>
struct NativeComparator
{
    bool operator() (const el lhs ,const el rhs)
    {
        auto lsv = splitEl(lhs);
        auto rsv = splitEl(rhs);

        auto lit = lsv.begin();
        auto rit = rsv.begin();

        while (lit != lsv.end() || rit != rsv.end())
        {
            if (lit == lsv.end())
                return true;
            if (rit == rsv.end())
                return false;
            if (*lit == *rit)
            {
                ++lit;
                ++rit;
                continue;
            }
            else
            {
                if (isdigit(*(lit->c_str())) && isdigit(*(rit->c_str())))
                {
                    return stoi(*lit) <= stoi(*rit);
                }
                else{
                    return *lit < *rit;
                }
            }
        }

        return lhs < rhs;
    }

    // splitEl在comparator里面会重复运行多次, 效率较低
    // 如果能包装下, 在外面先处理会更好
    // 返回值中可包含类型 vector<pair<int, string>>
    std::vector<std::string> splitEl(const el str)
    {
        std::vector<std::string> splitedVec;
        std::string tmpStr = "";
        std::string tmpInt = "";
        int currentType;
        int lastType = -1;
        // type:
        // -1   init
        // 0    number
        // 1    others
        int i = 0;
        for (const auto& it : str)
        {
            ++i;
            if (lastType == -1)
            {
                if (isdigit(it))
                {
                    currentType = 0;
                    tmpInt += it;
                }
                else
                {
                    currentType = 1;
                    tmpStr += it;
                }
            }
            else if (lastType == 0)
            {
                if (isdigit(it))
                {
                    currentType = 0;
                    tmpInt += it;
                }
                else
                {
                    // 类型改变
                    currentType = 1;
                    tmpStr += it;
                    splitedVec.emplace_back(tmpInt);
                    tmpInt = "";
                }
            }
            else if (lastType == 1)
            {
                if (isdigit(it))
                {
                    // 类型改变
                    currentType = 0;
                    tmpInt += it;
                    splitedVec.emplace_back(tmpStr);
                    tmpStr = "";
                }
                else
                {
                    currentType = 1;
                    tmpStr += it;
                }
            }
            else  // 目前代码不会走到这步
                continue;
            lastType = currentType;

            // 循环结束时添加把剩余部分添加到 vector
            if (i == str.size())
            {
                if (tmpStr != "")
                {
                    splitedVec.emplace_back(tmpStr);
                }
                if (tmpInt != "")
                {
                    splitedVec.emplace_back(tmpInt);
                }
            }
        }
        return splitedVec;
    }
};

template <typename el, class Comparator=ClassicalComparator<el>>
class Range
{
private:
    using setRange = std::set<el, Comparator>;
    setRange range_;
public:
    using iterator = typename setRange::const_iterator;

    Range() = default;

    explicit Range(std::vector<el> range) : range_(range.begin(), range.end()) {}

    iterator begin() { return range_.begin(); }
    iterator end() { return range_.end(); }
};

int main()
{
    std::cout << __func__ << std::endl;

    // bug: 增加"-d-1"后sorted都变空
    const std::vector<std::string> v = {"a", "c", "b", "b", "e", "d+-1", "d-1", "d-1-1", "d-01", "d-10", "d-2", "d-2.1", "d10-2", "d2-2", "d_1-2", "dcba--1"};
    std::cout << "unsorted:" << std::endl;
    for (const auto& it : v)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    auto range = Range<std::string>(v);
    std::cout << "sorted classical:" << std::endl;
    for (const auto& it : range)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    auto range1 = Range<std::string, SepratedComparator<std::string>>(v);
    std::cout << "sorted with seprator:" << std::endl;
    for (const auto& it : range1)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    auto range2 = Range<std::string, NativeComparator<std::string>>(v);
    std::cout << "sorted native:" << std::endl;
    for (const auto& it : range2)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;
}
